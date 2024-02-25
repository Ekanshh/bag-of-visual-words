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

    std::vector<double> tf() const;
    void set_tf(const std::vector<double>& tf);
    std::vector<double> ComputeTF() const;

private:
    std::vector<int> data_ = {};
    std::vector<double> tf_ = {};
};

class TFIDFHistogram : public Histogram {
public:
    TFIDFHistogram() = default;
    explicit TFIDFHistogram(const std::vector<double>& tfidf) : tfidf_(tfidf){};
    TFIDFHistogram(const Histogram& histogram, const BowDictionary& dictionary);
    TFIDFHistogram(const TFIDFHistogram&) = default;
    TFIDFHistogram(TFIDFHistogram&&) = default;
    TFIDFHistogram& operator=(const TFIDFHistogram&) = default;
    TFIDFHistogram& operator=(TFIDFHistogram&&) = default;
    ~TFIDFHistogram() = default;

    void WriteToCSV(const std::string& filename) const;
    static TFIDFHistogram ReadFromCSV(const std::string& filename);

    [[nodiscard]] const std::vector<double>& tfidf() const;
    void set_tfidf(const std::vector<double>& tfidf);
    std::vector<double> ComputeTFIDF(const BowDictionary& dictionary) const;

private:
    std::vector<double> tfidf_ = {};
};

}  // namespace ipb