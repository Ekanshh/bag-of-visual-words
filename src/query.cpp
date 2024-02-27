#include <fmt/core.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>

#include "bovw.h"

// Function to find the indices of the top N smallest values in a given distance matrix
std::vector<int> find_top_n_indices(const cv::Mat& distance_matrix, int n) {
    std::vector<int> indices(distance_matrix.cols);
    std::iota(indices.begin(), indices.end(), 0);  // Fill indices with 0, 1, 2, ..., N-1

    // Sort the indices based on the values in the distance matrix
    std::partial_sort(
            indices.begin(), indices.begin() + n, indices.end(), [&distance_matrix](int a, int b) {
                return distance_matrix.at<double>(0, a) < distance_matrix.at<double>(0, b);
            });

    // Resize the indices vector to keep only the top N indices
    indices.resize(n);
    return indices;
}

// Select the top N similar images based on the given distance matrix and filenames
std::vector<std::pair<std::string, double>> select_top_n_similar_images(
        const cv::Mat& distance_matrix, const std::vector<std::string>& filenames, int n) {
    std::vector<int> top_indices = find_top_n_indices(distance_matrix, n);

    // Collect the filenames and corresponding distances for the top N similar images
    std::vector<std::pair<std::string, double>> top_similar_images;
    for (int index : top_indices) {
        top_similar_images.emplace_back(filenames[index] + ".png",
                                        distance_matrix.at<double>(0, index));
    }

    return top_similar_images;
}

void generate_html_with_top_similar_images(
        const std::vector<std::pair<std::string, double>>& top_similar_images,
        const std::filesystem::path& query_path,
        const std::string& metric_name) {
    // Get scored images
    std::vector<bovw::api::image_browser::ScoredImage> scored_images;
    for (const auto& [image_name, score] : top_similar_images) {
        std::filesystem::path relative_path = "../images/";
        const std::filesystem::path& image_path = relative_path / image_name;
        scored_images.emplace_back(image_path.string(), score);
    }

    // Group images into rows of three
    std::vector<bovw::api::image_browser::ImageRow> image_rows;
    for (size_t i = 0; i < scored_images.size(); i += 3) {
        bovw::api::image_browser::ImageRow row;
        for (size_t j = i; j < i + 3 && j < scored_images.size(); ++j) {
            row[j - i] = scored_images[j];  // Assign images to the row
        }
        image_rows.push_back(row);
    }

    // Create HTML file
    std::filesystem::path html_filename = query_path / (metric_name + ".html");
    bovw::api::image_browser::CreateImageBrowser("Query Image", "style.css", image_rows,
                                                 html_filename);
}

int main() {
    const std::filesystem::path& root = "../dataset/";
    const std::filesystem::path& image_directory = root / "images";
    const std::filesystem::path& bin_directory = root / "bin";
    const std::filesystem::path& histogram_directory = root / "histograms";
    const std::filesystem::path& tfidf_histogram_directory = root / "tfidf_histograms";
    const std::filesystem::path& dictionary_path = root / "dictionary.yml";
    const std::filesystem::path& query_path = root / "query";

    // Pre-process pre-computed histograms
    // Load all histograms from disk
    std::vector<ipb::Histogram> histograms;
    std::vector<std::string> histogram_filenames;
    for (const auto& entry : std::filesystem::directory_iterator(histogram_directory)) {
        if (entry.path().extension() == ".csv") {
            const std::filesystem::path& histogram_path = entry.path();
            histograms.push_back(ipb::Histogram::ReadFromCSV(histogram_path.string()));
            histogram_filenames.push_back(histogram_path.stem().string());
        }
    }
    // Load all tfidf histograms from disk
    std::vector<ipb::TFIDFHistogram> tfidf_histograms;
    std::vector<std::string> tfidf_histogram_filenames;
    for (const auto& entry : std::filesystem::directory_iterator(tfidf_histogram_directory)) {
        if (entry.path().extension() == ".csv") {
            const std::filesystem::path& tfidf_histogram_path = entry.path();
            tfidf_histograms.push_back(
                    ipb::TFIDFHistogram::ReadFromCSV(tfidf_histogram_path.string()));
            tfidf_histogram_filenames.push_back(tfidf_histogram_path.stem().string());
        }
    }

    // Ask the user to select a query image
    std::cout << "Provide a query image name with extension (stored in /dataset/query.png):"
              << std::endl;
    std::string query_image_name;
    std::cin >> query_image_name;

    // Convert the query image path to a std::filesystem::path
    std::filesystem::path query_image_path(query_path / (query_image_name));
    std::string query_stem = query_image_path.stem().string();

    // Load the dictionary
    ipb::BowDictionary& dictionary = ipb::BowDictionary::GetInstance();
    dictionary.load(dictionary_path.string());

    // Compute the descriptors for the query image
    ipb::serialization::sifts::ConvertDataset(query_path / "image");
    // Load descriptors from disk
    cv::Mat query_descriptors =
            ipb::serialization::Deserialize(query_path / "bin" / (query_stem + ".bin"));
    cv::imshow("Query Image Descriptors", query_descriptors);
    cv::waitKey(0);

    // Query Histogram
    auto quer_hist = ipb::Histogram(query_descriptors, dictionary);
    quer_hist.WriteToCSV(query_path / (query_stem + ".csv"));
    auto tfidf_quer_hist = ipb::QueryTFIDF(quer_hist, histograms);
    tfidf_quer_hist.WriteToCSV(query_path / (query_stem + "_tfidf.csv"));

    // Similarity Metrics
    cv::Mat histogram_query_cosdist = ipb::metrics::BatchCosineDistance(quer_hist, histograms);
    cv::Mat tfidf_histogram_query_cosdist =
            ipb::metrics::BatchCosineDistance(tfidf_quer_hist, tfidf_histograms);
    cv::Mat histogram_query_euclidean_distance =
            ipb::metrics::BatchEuclideanDistance(quer_hist, histograms);
    cv::Mat tfidf_histogram_query_euclidean_distance =
            ipb::metrics::BatchEuclideanDistance(tfidf_quer_hist, tfidf_histograms);

    // Visualize the metrics
    // ipb::metrics::VisualizeMetrics(histogram_query_cosdist, "Histogram Cosine Distance");
    // ipb::metrics::VisualizeMetrics(tfidf_histogram_query_cosdist, "TF-IDF Cosine Distance");
    // ipb::metrics::VisualizeMetrics(histogram_query_euclidean_distance,
    //                                "Histogram Euclidean Distance");
    // ipb::metrics::VisualizeMetrics(tfidf_histogram_query_euclidean_distance,
    //                                "TF-IDF Euclidean Distance");
    // cv::waitKey(0);

    // Select top 10 similar images based on cosine distance for histogram query
    auto top_similar_images_histogram_cosine =
            select_top_n_similar_images(histogram_query_cosdist, histogram_filenames, 10);

    // Select top 10 similar images based on cosine distance for TF-IDF histogram query
    auto top_similar_images_tfidf_cosine = select_top_n_similar_images(
            tfidf_histogram_query_cosdist, tfidf_histogram_filenames, 10);

    // Select top 10 similar images based on Euclidean distance for histogram query
    auto top_similar_images_histogram_euclidean = select_top_n_similar_images(
            histogram_query_euclidean_distance, histogram_filenames, 10);

    // Select top 10 similar images based on Euclidean distance for TF-IDF histogram query
    auto top_similar_images_tfidf_euclidean = select_top_n_similar_images(
            tfidf_histogram_query_euclidean_distance, tfidf_histogram_filenames, 10);

    // Generate HTML files for top similar images based on different similarity metrics
    generate_html_with_top_similar_images(top_similar_images_histogram_cosine, query_path,
                                          "histogram_cosine");
    generate_html_with_top_similar_images(top_similar_images_tfidf_cosine, query_path,
                                          "tfidf_cosine");
    generate_html_with_top_similar_images(top_similar_images_histogram_euclidean, query_path,
                                          "histogram_euclidean");
    generate_html_with_top_similar_images(top_similar_images_tfidf_euclidean, query_path,
                                          "tfidf_euclidean");
    return 0;
}