#pragma once

#include <filesystem>
#include <opencv2/opencv.hpp>
#include <vector>

#include "bow_dictionary.hpp"

namespace ipb {
class Histogram {
public:
    Histogram() = default;
    explicit Histogram(const std::vector<int>& data);
    Histogram(const cv::Mat& descriptors, const BowDictionary& dictionary);
    Histogram(const Histogram&) = default;
    Histogram(Histogram&&) = default;
    Histogram& operator=(const Histogram&) = default;
    Histogram& operator=(Histogram&&) = default;
    ~Histogram() = default;

    friend std::ostream& operator<<(std::ostream& os, const Histogram& histogram);

    void WriteToCSV(const std::string& filename) const;
    static Histogram ReadFromCSV(const std::string& filename);

    // Imitate stl_vector functionality
    const int& operator[](std::size_t index) const;
    int& operator[](std::size_t index);
    std::size_t size() const;
    bool empty() const;
    std::vector<int>::iterator begin();
    std::vector<int>::iterator end();
    std::vector<int>::const_iterator begin() const;
    std::vector<int>::const_iterator end() const;
    std::vector<int>::const_iterator cbegin() const;
    std::vector<int>::const_iterator cend() const;

    std::vector<int> data() const;
    void set_data(const std::vector<int>& data);

private:
    std::vector<int> data_ = {};
};

class TFIDFHistogram {
public:
    TFIDFHistogram() = default;
    explicit TFIDFHistogram(const std::vector<double>& data);
    TFIDFHistogram(const TFIDFHistogram&) = default;
    TFIDFHistogram(TFIDFHistogram&&) = default;
    TFIDFHistogram& operator=(const TFIDFHistogram&) = default;
    TFIDFHistogram& operator=(TFIDFHistogram&&) = default;
    ~TFIDFHistogram() = default;

    std::vector<double> data() const;
    void set_data(const std::vector<double>& data);

    void WriteToCSV(const std::string& filename) const;
    static TFIDFHistogram ReadFromCSV(const std::string& filename);

private:
    std::vector<double> data_ = {};
};

// Compute Batch TF-IDF for a set of histograms
std::vector<TFIDFHistogram> BatchTFIDF(const std::vector<Histogram>& histograms);

// Compute TF-IDF for a query histogram
TFIDFHistogram QueryTFIDF(const Histogram& histogram, const std::vector<Histogram>& histograms);

}  // namespace ipb