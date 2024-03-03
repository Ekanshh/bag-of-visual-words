#include <fmt/core.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>

#include "bovw.h"

// Print the summary of the program
void print_summary() {
    std::cout << "╔════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║              Bag of Visual Words Query             ║" << std::endl;
    std::cout << "╟────────────────────────────────────────────────────╢" << std::endl;
    std::cout << "║ This program allows you to query an image using    ║" << std::endl;
    std::cout << "║ pre-computed bag of visual words. The query image  ║" << std::endl;
    std::cout << "║ The query image can be an existing image in        ║" << std::endl;
    std::cout << "║ database or a new image in `query/images` directory║" << std::endl;
    std::cout << "╟────────────────────────────────────────────────────╢" << std::endl;
    std::cout << "║ The results are saved in the 'query/' directory.   ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "\nPress Enter to begin..." << std::endl;
    std::cin.get();
}
// Function to find the indices of the top N smallest values in a given distance matrix
std::vector<int> find_top_n_indices(const cv::Mat& distance_matrix, int n) {
    std::vector<int> indices(distance_matrix.cols);
    std::iota(indices.begin(), indices.end(), 0);

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
    std::vector<std::pair<std::string, double>> top_similar_images;
    std::transform(top_indices.begin(), top_indices.end(), std::back_inserter(top_similar_images),
                   [&](int index) {
                       return std::make_pair(filenames[index] + ".png",
                                             distance_matrix.at<double>(0, index));
                   });
    return top_similar_images;
}

// Generate an HTML file with the top similar images
void generate_html_with_top_similar_images(
        const std::vector<std::pair<std::string, double>>& top_similar_images,
        const std::filesystem::path& query_path,
        const std::string& metric_name) {
    // Convert the top similar images to the format required by the image browser
    std::vector<bovw::api::image_browser::ScoredImage> scored_images;
    for (const auto& similar_image : top_similar_images) {
        auto image_name = similar_image.first;
        auto score = similar_image.second;

        std::filesystem::path relative_path = "../dataset/images/";
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

// create query directory
void create_query_directory(const std::filesystem::path& query_path) {
    if (!std::filesystem::exists(query_path)) {
        std::filesystem::create_directory(query_path);
    }
}

int main() {
    print_summary();

    const std::filesystem::path& root = "../setup/";
    const std::filesystem::path& histogram_directory = root / "histograms";
    const std::filesystem::path& tfidf_histogram_directory = root / "tfidf_histograms";
    const std::filesystem::path& dictionary_path = root / "dictionary.yml";
    const std::filesystem::path& query_root = "../query";
    create_query_directory(query_root);

    // User prompt
    std::cout << "Do you want to query from an existing database or provide a new image "
                 "(../query/images/<query_image_name>.png)?"
              << std::endl;
    std::cout << "1. Query from existing database" << std::endl;
    std::cout << "2. Provide a new image (../query/images/<query_image_name>.png)" << std::endl;
    std::cout << "Enter your choice (1 or 2): ";
    int choice;
    std::cin >> choice;
    std::filesystem::path query_path;
    if (choice == 1) {
        query_path = "../dataset/images";
        std::cout << "Enter the name of the query image (e.g. 0001.png): ";
    } else if (choice == 2) {
        query_path = query_root / "images";
        std::cout << "Enter the name of the query image (e.g. query_image.png): ";
    } else {
        std::cerr << "Invalid choice. Exiting..." << std::endl;
        return 1;
    }
    std::string query_image_name;
    std::cin >> query_image_name;

    // Complete query image path
    std::filesystem::path query_image_path(query_path / (query_image_name));
    std::string query_stem = query_image_path.stem().string();

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

    // Load the dictionary
    ipb::BowDictionary& dictionary = ipb::BowDictionary::GetInstance();
    dictionary.load(dictionary_path.string());

    // Compute the descriptors for the query image
    ipb::serialization::sifts::ConvertSingleImage(query_image_path, query_root / "bin");
    // Load descriptors from disk
    cv::Mat query_descriptors =
            ipb::serialization::Deserialize(query_root / "bin" / (query_stem + ".bin"));
    cv::imshow("Query Image Descriptors", query_descriptors);
    cv::waitKey(0);

    // Query Histogram
    auto quer_hist = ipb::Histogram(query_descriptors, dictionary);
    quer_hist.WriteToCSV(query_root / (query_stem + ".csv"));
    auto tfidf_quer_hist = ipb::QueryTFIDF(quer_hist, histograms);
    tfidf_quer_hist.WriteToCSV(query_root / (query_stem + "_tfidf.csv"));

    // Similarity Metrics
    cv::Mat histogram_query_cosdist = ipb::metrics::BatchCosineDistance(quer_hist, histograms);
    cv::Mat tfidf_histogram_query_cosdist =
            ipb::metrics::BatchCosineDistance(tfidf_quer_hist, tfidf_histograms);
    cv::Mat histogram_query_euclidean_distance =
            ipb::metrics::BatchEuclideanDistance(quer_hist, histograms);
    cv::Mat tfidf_histogram_query_euclidean_distance =
            ipb::metrics::BatchEuclideanDistance(tfidf_quer_hist, tfidf_histograms);

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
    generate_html_with_top_similar_images(top_similar_images_histogram_cosine, query_root,
                                          "histogram_cosine");
    generate_html_with_top_similar_images(top_similar_images_tfidf_cosine, query_root,
                                          "tfidf_cosine");
    generate_html_with_top_similar_images(top_similar_images_histogram_euclidean, query_root,
                                          "histogram_euclidean");
    generate_html_with_top_similar_images(top_similar_images_tfidf_euclidean, query_root,
                                          "tfidf_euclidean");

    std::cout << "Results saved in the 'query/' directory." << std::endl;
    return 0;
}