

#include "histogram.hpp"

#include <algorithm>
#include <fstream>
#include <numeric>
#include <vector>

namespace ipb {

Histogram::Histogram(const cv::Mat& descriptors, const BowDictionary& dictionary) {
    const auto& vocabulary = dictionary.vocabulary();
    cv::FlannBasedMatcher matcher;

    std::vector<std::vector<cv::DMatch>> knn_matches;
    matcher.knnMatch(descriptors, vocabulary, knn_matches, 1);

    std::vector<int> histogram(vocabulary.rows, 0);
    for (const auto& match : knn_matches) {
        if (!match.empty()) {
            histogram[match[0].trainIdx]++;
        }
    }

    set_data(histogram);
}

Histogram::Histogram(const std::vector<int>& data) { set_data(data); }

std::ostream& operator<<(std::ostream& os, const Histogram& histogram) {
    for (const auto& bin : histogram.data_) {
        os << bin << ", ";
    }
    return os;
}

void Histogram::WriteToCSV(const std::string& filename) const {
    std::filesystem::path directory = std::filesystem::path(filename).parent_path();
    if (!std::filesystem::exists(directory)) {
        std::filesystem::create_directories(directory);
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }

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
void Histogram::set_data(const std::vector<int>& data) { data_ = data; }

// TFIDFHistogram
TFIDFHistogram::TFIDFHistogram(const std::vector<double>& data) { set_data(data); }
std::vector<double> TFIDFHistogram::data() const { return data_; }
void TFIDFHistogram::set_data(const std::vector<double>& data) { data_ = data; }

void TFIDFHistogram::WriteToCSV(const std::string& filename) const {
    std::filesystem::path directory = std::filesystem::path(filename).parent_path();

    if (!std::filesystem::exists(directory)) {
        std::filesystem::create_directories(directory);
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }

    for (auto it = data_.cbegin(); it != data_.cend(); ++it) {
        file << *it;
        if (std::next(it) != data_.cend()) {  // Check if it's not the last element
            file << ',';                      // Add comma if it's not the last element
        }
    }
    file.close();
}
TFIDFHistogram TFIDFHistogram::ReadFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return {};
    }

    std::string line;
    std::vector<double> data;
    while (!file.eof()) {
        std::getline(file, line);
        std::istringstream iss(line);
        std::string token;
        while (std::getline(iss, token, ',')) {
            data.push_back(std::stod(token));
        }
    }
    return TFIDFHistogram(data);
}

std::vector<TFIDFHistogram> BatchTFIDF(const std::vector<Histogram>& histograms) {
    const std::size_t N = histograms.size();

    std::vector<int> document_frequency(histograms[0].size(), 0);
    for (const auto& histogram : histograms) {
        for (std::size_t i = 0; i < histogram.size(); ++i) {
            if (histogram[i] > 0) {
                document_frequency[i]++;
            }
        }
    }

    std::vector<TFIDFHistogram> tfidf_histograms;
    for (const auto& histogram : histograms) {
        TFIDFHistogram tfidf_histogram;
        std::vector<double> tfidf(histogram.size(), 0);
        double total_terms = std::accumulate(histogram.begin(), histogram.end(), 0);
        for (std::size_t i = 0; i < histogram.size(); ++i) {
            double tf = static_cast<double>(histogram[i]) / total_terms;
            double idf = std::log(static_cast<double>(N) / document_frequency[i]);
            tfidf[i] = tf * idf;
        }
        tfidf_histogram.set_data(tfidf);
        tfidf_histograms.push_back(tfidf_histogram);
    }

    return tfidf_histograms;
}

TFIDFHistogram QueryTFIDF(const Histogram& query_histogram,
                          const std::vector<Histogram>& histograms) {
    const std::size_t N = histograms.size();

    std::vector<int> document_frequency(query_histogram.size(), 0);
    for (const auto& histogram : histograms) {
        for (std::size_t i = 0; i < histogram.size(); ++i) {
            if (histogram[i] > 0) {
                document_frequency[i]++;
            }
        }
    }

    TFIDFHistogram tfidf_histogram;
    std::vector<double> tfidf(query_histogram.size(), 0);
    double total_terms = std::accumulate(query_histogram.begin(), query_histogram.end(), 0);
    for (std::size_t i = 0; i < query_histogram.size(); ++i) {
        double tf = static_cast<double>(query_histogram[i]) / total_terms;
        double idf = std::log(static_cast<double>(N) / document_frequency[i]);
        tfidf[i] = tf * idf;
    }
    tfidf_histogram.set_data(tfidf);

    return tfidf_histogram;
}

}  // namespace ipb
