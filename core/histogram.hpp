#pragma once

#include <filesystem>
#include <opencv2/opencv.hpp>
#include <vector>

#include "bow_dictionary.hpp"

namespace ipb {
class Histogram {
public:
    Histogram() = default;
    explicit Histogram(const std::vector<int>& data) : data_(data){};
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

private:
    std::vector<int> data_ = {};
};

class TFIDFHistogram : public Histogram {
public:
    using Histogram::Histogram;

    TFIDFHistogram() = default;
    TFIDFHistogram(const Histogram& histogram, const std::vector<Histogram>& histograms);
    TFIDFHistogram(const TFIDFHistogram&) = default;
    TFIDFHistogram(TFIDFHistogram&&) = default;
    TFIDFHistogram& operator=(const TFIDFHistogram&) = default;
    TFIDFHistogram& operator=(TFIDFHistogram&&) = default;
    ~TFIDFHistogram() = default;

    void computeTFIDF();
    void WriteToCSV(const std::string& filename) const;

    void set_histograms(const std::vector<Histogram>& histograms);
    const std::vector<Histogram>& histograms() const;
    std::vector<Histogram>& histograms();
    const double& tfidf(std::size_t index);

private:
    std::vector<Histogram> histograms_;
    std::vector<double> tfidf_;
    std::size_t totalWords() const {
        std::size_t total = 0;
        for (const auto& histogram : histograms_) {
            total += histogram.size();
        }
        return total;
    }
};
}  // namespace ipb