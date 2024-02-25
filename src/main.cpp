#include <bovw.h>

#include <filesystem>
#include <iostream>

int main() {
    const std::filesystem::path& root = "../dataset/test/";
    const std::filesystem::path& image_directory = root / "data";
    const std::filesystem::path& bin_directory = root / "bin";
    const std::filesystem::path& histogram_directory = root / "histograms_1000";
    const std::filesystem::path& tfidf_histogram_directory = root / "tfidf_histograms_1000";
    const std::filesystem::path& dictionary_path = root / "test_data_1000.yml";

    // Convert the dataset to SIFT descriptors
    std::cout << "Converting dataset to SIFT descriptors.." << std::endl;
    ipb::serialization::sifts::ConvertDataset(image_directory);
    std::cout << "Converted dataset to SIFT descriptors." << std::endl;

    // Create the dictionary
    std::cout << "Creating dictionary.." << std::endl;
    const auto descriptors = ipb::serialization::sifts::LoadDataset(bin_directory);
    const auto max_iterations = 50;
    const auto num_words = 1000;
    ipb::BowDictionary& dictionary = ipb::BowDictionary::GetInstance();
    dictionary.build(max_iterations, num_words, descriptors);
    dictionary.save(dictionary_path.string());
    std::cout << "Created dictionary." << std::endl;

    // Compute the histograms
    std::cout << "Computing histograms.." << std::endl;
    std::vector<ipb::Histogram> histograms;
    std::vector<ipb::TFIDFHistogram> tfidf_histograms;
    for (const auto& entry : std::filesystem::directory_iterator(image_directory)) {
        if (entry.path().extension() == ".png") {
            const std::filesystem::path& image_path = entry.path();
            const std::filesystem::path& bin_path =
                    bin_directory / (image_path.stem().string() + ".bin");

            auto des_descriptors = ipb::serialization::Deserialize(bin_path.string());

            // Compute the histogram and write it to disk
            ipb::Histogram histogram(des_descriptors, dictionary);
            histogram.WriteToCSV(histogram_directory / (image_path.stem().string() + ".csv"));
            histograms.push_back(histogram);

            // Compute the TF-IDF histogram and write it to disk
            ipb::TFIDFHistogram tfidf_histogram(histogram, dictionary);
            tfidf_histogram.WriteToCSV(tfidf_histogram_directory /
                                       (image_path.stem().string() + ".csv"));
            tfidf_histograms.push_back(tfidf_histogram);
        }
    }
    std::cout << "Computed histograms." << std::endl;

    // Metrics
    // Cosine similarity
    std::cout << "Computing metrics.." << std::endl;
    std::cout << "Computing cosine similarity.." << std::endl;
    auto histogram_cosinesim =
            ipb::metrics::CosineSimilarity(histograms, true, root / "histogram_cosinesim.csv");
    auto tfidf_histogram_cosinesim = ipb::metrics::CosineSimilarity(
            tfidf_histograms, true, root / "tfidf_histogram_cosinesim.csv");

    // Euclidean distance
    std::cout << "Computing euclidean distance.." << std::endl;
    auto histogram_euclidean = ipb::metrics::NormalizedEuclideanDistance(
            histograms, true, root / "histogram_euclidean.csv");
    auto tfidf_histogram_euclidean = ipb::metrics::NormalizedEuclideanDistance(
            tfidf_histograms, true, root / "tfidf_histogram_euclidean.csv");
    std::cout << "Computed metrics." << std::endl;

    // Visualize the metrics
    ipb::metrics::VisualizeMetrics(histogram_cosinesim, "Histogram Cosine Similarity");
    ipb::metrics::VisualizeMetrics(tfidf_histogram_cosinesim, "TF-IDF Histogram Cosine Similarity");
    ipb::metrics::VisualizeMetrics(histogram_euclidean, "Histogram Euclidean Distance");
    ipb::metrics::VisualizeMetrics(tfidf_histogram_euclidean,
                                   "TF-IDF Histogram Euclidean Distance");

    return 0;
}
