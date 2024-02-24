

#include "histogram.hpp"

#include <fstream>

namespace ipb {

Histogram::Histogram(const cv::Mat& descriptors, const BowDictionary& dictionary) {
    auto vocabulary = dictionary.vocabulary();
    auto matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE);

    if (!matcher) {
        std::cerr << "[Error] Unable to create descriptor matcher." << std::endl;
        return;
    }

    std::vector<std::vector<cv::DMatch>> knn_matches;
    matcher->knnMatch(descriptors, vocabulary, knn_matches, 1);

    std::vector<int> histogram(vocabulary.rows, 0);
    for (const auto& match : knn_matches) {
        if (!match.empty()) {
            histogram[match[0].trainIdx]++;
        }
    }
    data_ = histogram;
}

std::ostream& operator<<(std::ostream& os, const Histogram& histogram) {
    for (const auto& bin : histogram.data_) {
        os << bin << ", ";
    }
    return os;
}

void Histogram::WriteToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    for (auto it = data_.cbegin(); it != data_.cend(); ++it) {
        file << *it;
        if (std::next(it) != data_.cend()) {  // Check if it's not the last element
            file << ',';                      // Add comma if it's not the last element
        }
    }
    file.close();
}

Histogram Histogram::ReadFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return {};
    }

    std::string line;
    std::vector<int> data;
    while (!file.eof()) {
        std::getline(file, line);
        std::istringstream iss(line);
        std::string token;
        while (std::getline(iss, token, ',')) {
            data.push_back(std::stoi(token));
        }
    }
    return Histogram(data);
}

const int& Histogram::operator[](std::size_t index) const { return data_[index]; }
int& Histogram::operator[](std::size_t index) { return data_[index]; }

std::size_t Histogram::size() const { return data_.size(); }
bool Histogram::empty() const { return data_.empty(); }

std::vector<int>::iterator Histogram::begin() { return data_.begin(); }
std::vector<int>::iterator Histogram::end() { return data_.end(); }
std::vector<int>::const_iterator Histogram::begin() const { return data_.begin(); }
std::vector<int>::const_iterator Histogram::end() const { return data_.end(); }
std::vector<int>::const_iterator Histogram::cbegin() const { return data_.cbegin(); }
std::vector<int>::const_iterator Histogram::cend() const { return data_.cend(); }

std::vector<int> Histogram::data() const { return data_; }

// TFIDFHistogram
TFIDFHistogram::TFIDFHistogram(const Histogram& histogram, const std::vector<Histogram>& histograms)
    : Histogram(histogram), histograms_(histograms) {
    computeTFIDF();
}

void TFIDFHistogram::computeTFIDF() {
    tfidf_.resize(size(), 0.0);
    const std::size_t n_histograms = histograms_.size();

    std::vector<std::size_t> documentsWithTerm(size(), 0);

    // Calculate document frequency (DF) for each term
    for (std::size_t i = 0; i < size(); ++i) {
        for (const auto& histogram : histograms_) {
            if (histogram[i] > 0) {
                documentsWithTerm[i]++;
            }
        }
    }

    // Calculate TF-IDF values
    for (std::size_t i = 0; i < size(); ++i) {
        // Calculate term frequency (TF)
        const double termFrequency =
                static_cast<double>((*this)[i]) / static_cast<double>(totalWords());
        // Calculate inverse document frequency (IDF)
        const double inverseDocumentFrequency = std::log(
                static_cast<double>(n_histograms) / static_cast<double>(1 + documentsWithTerm[i]));
        tfidf_[i] = termFrequency * inverseDocumentFrequency;
    }
}

void TFIDFHistogram::WriteToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    for (auto it = tfidf_.cbegin(); it != tfidf_.cend(); ++it) {
        file << *it;
        if (std::next(it) != tfidf_.cend()) {  // Check if it's not the last element
            file << ',';                       // Add comma if it's not the last element
        }
    }
    file << '\n';  // Add newline at the end of each line
    file.close();
}

void TFIDFHistogram::set_histograms(const std::vector<Histogram>& histograms) {
    histograms_ = histograms;
    computeTFIDF();
}

}  // namespace ipb
