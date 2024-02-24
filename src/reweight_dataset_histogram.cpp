#include <bovw.h>

// Get all histogram files names
std::vector<std::filesystem::path> GetHistogramFiles(const std::filesystem::path& directory) {
    std::vector<std::filesystem::path> histogram_files;
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".csv") {
            histogram_files.push_back(entry.path());
        }
    }
    return histogram_files;
}

int main() {
    // Input directory
    const std::filesystem::path& input_directory = "../dataset/test/histograms_1000";

    // Get all histograms files
    const std::vector<std::filesystem::path>& histogram_files = GetHistogramFiles(input_directory);
    std::cout << "Found " << histogram_files.size() << " histogram files" << std::endl;

    // Save all histograms
    std::vector<ipb::Histogram> histograms;
    histograms.reserve(histogram_files.size());
    for (const auto& file : histogram_files) {
        histograms.push_back(ipb::Histogram::ReadFromCSV(file));
    }

    // Compute the TF-IDF histograms for all histograms
    std::vector<ipb::TFIDFHistogram> tfidf_histograms;
    tfidf_histograms.reserve(histograms.size());
    for (const auto& histogram : histograms) {
        tfidf_histograms.emplace_back(histogram, histograms);
    }

    // Save all TF-IDF histograms
    const std::filesystem::path& output_directory = input_directory / "../tfidf-histograms_1000";
    for (std::size_t i = 0; i < tfidf_histograms.size(); ++i) {
        tfidf_histograms[i].WriteToCSV(output_directory /
                                       ("tfidf_" + histogram_files[i].filename().string()));
    }

    std::cout << "TF-IDF histograms saved in " << output_directory << std::endl;
    return 0;
}
