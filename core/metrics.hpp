#pragma once

#include <filesystem>
#include <opencv2/core.hpp>
#include <string>
#include <vector>

#include "histogram.hpp"

namespace ipb::metrics {

template <typename T>
double PairwiseCosineDistance(const std::vector<T>& vec_a, const std::vector<T>& vec_b) {
    // Reference: https://www.simonwenkel.com/notes/ai/metrics/cosine_distance.html
    double a_dot_b = 0.0;
    double a_mag = 0;
    double b_mag = 0;

    size_t vec_size = vec_a.size();

    for (size_t i = 0; i < vec_size; ++i) {
        a_dot_b += (vec_a[i] * vec_b[i]);
        a_mag += (vec_a[i] * vec_a[i]);
        b_mag += (vec_b[i] * vec_b[i]);
    }
    double dist = 1.0 - (a_dot_b / (sqrt(a_mag) * sqrt(b_mag)));
    return dist;
}

template <typename T>
cv::Mat BatchCosineDistance(const std::vector<T>& histograms) {
    cv::Mat measure(static_cast<int>(histograms.size()), static_cast<int>(histograms.size()),
                    CV_64F);

    for (size_t i = 0; i < histograms.size(); ++i) {
        for (size_t j = 0; j < histograms.size(); ++j) {
            auto vec_a = histograms[i].data();
            auto vec_b = histograms[j].data();

            double dist = PairwiseCosineDistance(vec_a, vec_b);
            measure.at<double>(static_cast<int>(i), static_cast<int>(j)) = dist;
        }
    }
    return measure;
}

template <typename T>
cv::Mat BatchCosineDistance(const T& histogram, const std::vector<T>& histograms) {
    cv::Mat measure(1, static_cast<int>(histograms.size()), CV_64F);

    for (size_t i = 0; i < histograms.size(); ++i) {
        auto vec_a = histogram.data();
        auto vec_b = histograms[i].data();
        double dist = PairwiseCosineDistance(vec_a, vec_b);
        measure.at<double>(0, static_cast<int>(i)) = dist;
    }

    return measure;
}

template <typename T>
cv::Mat BatchEuclideanDistance(const std::vector<T>& histograms) {
    cv::Mat measure(static_cast<int>(histograms.size()), static_cast<int>(histograms.size()),
                    CV_64F);

    for (size_t i = 0; i < histograms.size(); ++i) {
        for (size_t j = 0; j < histograms.size(); ++j) {
            auto vec_a = histograms[i].data();
            auto vec_b = histograms[j].data();

            double dist = cv::norm(vec_a, vec_b, cv::NORM_L2);
            measure.at<double>(static_cast<int>(i), static_cast<int>(j)) = dist;
        }
    }

    // Normalize the measure matrix to the range [0, 1]
    cv::Mat measure_norm;
    cv::normalize(measure, measure_norm, 0, 1, cv::NORM_MINMAX, CV_64F);
    return measure_norm;
}

template <typename T>
cv::Mat BatchEuclideanDistance(const T& histogram, const std::vector<T>& histograms) {
    cv::Mat measure(1, static_cast<int>(histograms.size()), CV_64F);

    for (size_t i = 0; i < histograms.size(); ++i) {
        auto vec_a = histogram.data();
        auto vec_b = histograms[i].data();
        double dist = cv::norm(vec_a, vec_b, cv::NORM_L2);
        measure.at<double>(0, static_cast<int>(i)) = dist;
    }

    // Normalize the measure matrix to the range [0, 1]
    cv::Mat measure_norm;
    cv::normalize(measure, measure_norm, 0, 1, cv::NORM_MINMAX, CV_64F);
    return measure_norm;
}

void WriteMetricsToCSV(const cv::Mat& measure, const std::string& filename);
void VisualizeMetrics(const cv::Mat& measure, const std::string& title);

}  // namespace ipb::metrics
