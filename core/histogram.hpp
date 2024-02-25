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

    std::vector<double> ComputeTF() const;

private:
    std::vector<int> data_ = {};
};

}  // namespace ipb