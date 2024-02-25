

#include "histogram.hpp"

#include <fstream>
#include <vector>

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

    set_data(histogram);
    auto tf = ComputeTF();
    set_tf(tf);
}

Histogram::Histogram(const std::vector<int>& data) {
    set_data(data);
    auto tf = ComputeTF();
    set_tf(tf);
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
void Histogram::set_data(const std::vector<int>& data) { data_ = data; }

std::vector<double> Histogram::tf() const { return tf_; }
void Histogram::set_tf(const std::vector<double>& tf) { tf_ = tf; }

std::vector<double> Histogram::ComputeTF() const {
    std::vector<double> tf_values;
    tf_values.reserve(data_.size());

    auto total_terms = static_cast<double>(data_.size());

    for (const auto& term_count : data_) {
        auto tf = term_count / total_terms;
        tf_values.push_back(tf);
    }

    return tf_values;
}

// TFIDFHistogram

TFIDFHistogram::TFIDFHistogram(const Histogram& histogram, const BowDictionary& dictionary) {
    auto data = histogram.data();
    auto tf = histogram.tf();
    auto idf = dictionary.idf();

    std::vector<double> tfidf;
    tfidf.reserve(data.size());
    for (std::size_t i = 0; i < data.size(); ++i) {
        tfidf.push_back(tf[i] * idf[i]);
    }

    set_tfidf(tfidf);
}

void TFIDFHistogram::WriteToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    for (auto it = tfidf_.cbegin(); it != tfidf_.cend(); ++it) {
        file << *it;
        if (std::next(it) != tfidf_.cend()) {  // Check if it's not the last element
            file << ',';                       // Add comma if it's not the last element
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

const std::vector<double>& TFIDFHistogram::tfidf() const { return tfidf_; }
void TFIDFHistogram::set_tfidf(const std::vector<double>& tfidf) { tfidf_ = tfidf; }

}  // namespace ipb
