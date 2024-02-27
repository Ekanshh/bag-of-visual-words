#include <bovw.h>

#include <filesystem>
#include <iostream>

int main() {
    const std::filesystem::path& root = "../dataset/";
    const std::filesystem::path& image_directory = root / "images";
    const std::filesystem::path& bin_directory = root / "bin";
    const std::filesystem::path& histogram_directory = root / "histograms";
    const std::filesystem::path& tfidf_histogram_directory = root / "tfidf_histograms";
    const std::filesystem::path& dictionary_path = root / "dictionary.yml";

    // Convert the dataset to SIFT descriptors
    std::cout << "Converting dataset to SIFT descriptors.." << std::endl;
    const std::filesystem::path& full_path = std::filesystem::current_path();
    std::cout << "Current path: " << full_path << std::endl;
    ipb::serialization::sifts::ConvertDataset(image_directory);
    std::cout << "Converted dataset to SIFT descriptors." << std::endl;

    // Create the dictionary
    std::cout << "Creating dictionary.." << std::endl;
    const auto descriptors = ipb::serialization::sifts::LoadDataset(bin_directory);
    const auto max_iterations = 100;
    const auto num_words = 50000;
    ipb::BowDictionary& dictionary = ipb::BowDictionary::GetInstance();
    dictionary.build(max_iterations, num_words, descriptors);
    dictionary.save(dictionary_path.string());
    std::cout << "Created dictionary." << std::endl;

    // Compute the histograms
    std::cout << "Computing histograms.." << std::endl;
    std::vector<ipb::Histogram> histograms;
    std::vector<std::string> histogram_filenames;
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
            histogram_filenames.push_back(image_path.stem().string());
        }
    }
    std::cout << "Computed histograms." << std::endl;

    // Compute the TF-IDF histograms
    std::cout << "Computing TF-IDF histograms.." << std::endl;
    tfidf_histograms = ipb::BatchTFIDF(histograms);

    for (std::size_t i = 0; i < tfidf_histograms.size(); ++i) {
        tfidf_histograms[i].WriteToCSV(tfidf_histogram_directory /
                                       (histogram_filenames[i] + ".csv"));
    }

    std::cout << "Computed TF-IDF histograms." << std::endl;

    // Cosine Distance Metrics
    std::cout << "Computing cosine distance.." << std::endl;
    auto histogram_cosdist = ipb::metrics::BatchCosineDistance(histograms);
    auto tfidf_histogram_cosdist = ipb::metrics::BatchCosineDistance(tfidf_histograms);
    std::cout << "Computed metrics." << std::endl;

    // Visualize the metrics
    ipb::metrics::VisualizeMetrics(histogram_cosdist, "Histogram Cosine Distance");
    ipb::metrics::VisualizeMetrics(tfidf_histogram_cosdist, "TF-IDF Histogram Cosine Distance");
    // Save the metrics to CSV
    ipb::metrics::WriteMetricsToCSV(histogram_cosdist, root / "histogram_cosdist.csv");
    ipb::metrics::WriteMetricsToCSV(tfidf_histogram_cosdist, root / "tfidf_histogram_cosdist.csv");

    return 0;
}
