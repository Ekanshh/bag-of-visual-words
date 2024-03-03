#include <bovw.h>

void print_summary() {
    std::cout << "╔════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║              Bag of Visual Words                   ║" << std::endl;
    std::cout << "╟────────────────────────────────────────────────────╢" << std::endl;
    std::cout << "║ This program performs feature representation,      ║" << std::endl;
    std::cout << "║ codebook generation, and histogram computation for ║" << std::endl;
    std::cout << "║ a dataset of images.                               ║" << std::endl;
    std::cout << "╟────────────────────────────────────────────────────╢" << std::endl;
    std::cout << "║ The dataset is expected to be structured as        ║" << std::endl;
    std::cout << "║ follows:                                           ║" << std::endl;
    std::cout << "║ dataset/                                           ║" << std::endl;
    std::cout << "║  images/                                           ║" << std::endl;
    std::cout << "║   image1.png                                       ║" << std::endl;
    std::cout << "║   image2.png                                       ║" << std::endl;
    std::cout << "║   ...                                              ║" << std::endl;
    std::cout << "╟────────────────────────────────────────────────────╢" << std::endl;
    std::cout << "║ The results are saved in 'setup/' directory.       ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "\nPress Enter to begin..." << std::endl;
    std::cin.get();
}

void create_result_directories(const std::filesystem::path& result_dir,
                               const std::filesystem::path& bin_dir,
                               const std::filesystem::path& hist_dir,
                               const std::filesystem::path& tfidf_hist_dir) {
    if (!std::filesystem::exists(result_dir)) {
        std::filesystem::create_directory(result_dir);
    }
    if (!std::filesystem::exists(bin_dir)) {
        std::filesystem::create_directory(bin_dir);
    }
    if (!std::filesystem::exists(hist_dir)) {
        std::filesystem::create_directory(hist_dir);
    }
    if (!std::filesystem::exists(tfidf_hist_dir)) {
        std::filesystem::create_directory(tfidf_hist_dir);
    }
}

int main() {
    print_summary();

    const std::filesystem::path& dataset_dir = "../dataset";
    const std::filesystem::path& image_directory = dataset_dir / "images";
    if (!std::filesystem::exists(dataset_dir) || !std::filesystem::exists(image_directory)) {
        std::cerr << "Error: Dataset directories do not exist or are not in the expected format."
                  << std::endl;
        return 1;
    }

    const std::filesystem::path& result = "../setup";
    const std::filesystem::path& bin_directory = result / "bin";
    const std::filesystem::path& histogram_directory = result / "histograms";
    const std::filesystem::path& tfidf_histogram_directory = result / "tfidf_histograms";
    const std::filesystem::path& dictionary_path = result / "dictionary.yml";
    const std::filesystem::path& histogram_cosdist_path = result / "metric_histogram_cosdist.csv";
    const std::filesystem::path& tfidf_histogram_cosdist_path =
            result / "metric_tfidf_histogram_cosdist.csv";

    create_result_directories(result, bin_directory, histogram_directory,
                              tfidf_histogram_directory);

    std::cout << "Extracting features.." << std::endl;
    ipb::serialization::sifts::ConvertDataset(image_directory, bin_directory);

    std::cout << "Creating dictionary.." << std::endl;
    const auto descriptors = ipb::serialization::sifts::LoadDataset(bin_directory);
    const auto max_iterations = 10;
    const auto num_words = 1000;
    auto& dictionary = ipb::BowDictionary::GetInstance();
    dictionary.build(max_iterations, num_words, descriptors);
    dictionary.save(dictionary_path.string());

    std::cout << "Computing histograms.." << std::endl;
    std::vector<ipb::Histogram> histograms;
    std::vector<std::string> histogram_filenames;
    for (const auto& entry : std::filesystem::directory_iterator(image_directory)) {
        if (entry.path().extension() == ".png") {
            const auto& image_path = entry.path();
            const auto& bin_path = bin_directory / (image_path.stem().string() + ".bin");

            auto des_descriptors = ipb::serialization::Deserialize(bin_path.string());

            // Compute the histogram and write it to disk
            ipb::Histogram histogram(des_descriptors, dictionary);
            histogram.WriteToCSV(histogram_directory / (image_path.stem().string() + ".csv"));
            histograms.push_back(histogram);
            histogram_filenames.push_back(image_path.stem().string());
        }
    }

    auto tfidf_histograms = ipb::BatchTFIDF(histograms);

    for (std::size_t i = 0; i < tfidf_histograms.size(); ++i) {
        tfidf_histograms[i].WriteToCSV(tfidf_histogram_directory /
                                       (histogram_filenames[i] + ".csv"));
    }

    std::cout << "Computing metrics.." << std::endl;
    const auto& histogram_cosdist = ipb::metrics::BatchCosineDistance(histograms);
    const auto& tfidf_histogram_cosdist = ipb::metrics::BatchCosineDistance(tfidf_histograms);

    std::cout << "Saving metrics.." << std::endl;
    ipb::metrics::VisualizeMetrics(histogram_cosdist, "Histogram Cosine Distance");
    ipb::metrics::VisualizeMetrics(tfidf_histogram_cosdist, "TF-IDF Histogram Cosine Distance");
    ipb::metrics::WriteMetricsToCSV(histogram_cosdist, histogram_cosdist_path);
    ipb::metrics::WriteMetricsToCSV(tfidf_histogram_cosdist, tfidf_histogram_cosdist_path);

    return 0;
}