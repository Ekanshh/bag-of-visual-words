#pragma once

#include <filesystem>
#include <opencv2/core.hpp>
#include <string>
#include <vector>

#include "histogram.hpp"

namespace ipb::metrics {

void WriteMetricsToCSV(const cv::Mat& measure, const std::string& filename);

template <typename T>
cv::Mat CosineSimilarity(const std::vector<T>& histograms,
                         bool save_to_file = false,
                         const std::string& filename = "cosine_similarity.csv") {
    // Sanity checks
    // Check for empty histograms
    if (histograms.empty()) {
        throw std::invalid_argument("Histograms vector is empty");
    }
    // Check histograms size consistency
    if (std ::any_of(histograms.begin(), histograms.end(),
                     [&](const auto& hist) { return hist.size() != histograms[0].size(); })) {
        throw std::invalid_argument("All histograms must have the same size");
    }

    // Get the number of histograms and the size of each histogram
    const std::size_t n = histograms.size();
    const std::size_t m = histograms[0].size();
    cv::Mat similarity_matrix(n, n, CV_32F, cv::Scalar(0));

    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = i; j < n; ++j) {
            float dot_product = 0;
            float magnitude_i = 0;
            float magnitude_j = 0;

            // Calculate dot product and magnitudes in one loop
            for (std::size_t k = 0; k < m; ++k) {
                dot_product += histograms[i][k] * histograms[j][k];
                magnitude_i += histograms[i][k] * histograms[i][k];
                magnitude_j += histograms[j][k] * histograms[j][k];
            }

            magnitude_i = std::sqrt(magnitude_i);
            magnitude_j = std::sqrt(magnitude_j);

            // Avoid division by zero
            if (magnitude_i == 0 || magnitude_j == 0) {
                similarity_matrix.at<float>(i, j) = 0;
            } else {
                similarity_matrix.at<float>(i, j) = dot_product / (magnitude_i * magnitude_j);
            }

            // Symmetrically assign similarity values
            similarity_matrix.at<float>(j, i) = similarity_matrix.at<float>(i, j);
        }
    }

    if (save_to_file) {
        WriteMetricsToCSV(similarity_matrix, filename);
    }

    return similarity_matrix;
}

template <typename T>
cv::Mat NormalizedEuclideanDistance(
        const std::vector<T>& histograms,
        bool save_to_file = false,
        const std::string& filename = "normalized_euclidean_distance.csv") {
    // Sanity checks
    // Check for empty histograms
    if (histograms.empty()) {
        throw std::invalid_argument("Histograms vector is empty");
    }
    // Check histograms size consistency
    if (std ::any_of(histograms.begin(), histograms.end(),
                     [&](const auto& hist) { return hist.size() != histograms[0].size(); })) {
        throw std::invalid_argument("All histograms must have the same size");
    }

    // Get the number of histograms and the size of each histogram
    const std::size_t n = histograms.size();
    const std::size_t m = histograms[0].size();
    cv::Mat similarity_matrix(n, n, CV_32F, cv::Scalar(0));

    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = i; j < n; ++j) {
            float distance = 0;

            for (std::size_t k = 0; k < m; ++k) {
                distance += (histograms[i][k] - histograms[j][k]) *
                            (histograms[i][k] - histograms[j][k]);
            }

            distance = std::sqrt(distance);

            similarity_matrix.at<float>(i, j) = 1 / (1 + distance);
            similarity_matrix.at<float>(j, i) = similarity_matrix.at<float>(i, j);
        }
    }

    if (save_to_file) {
        WriteMetricsToCSV(similarity_matrix, filename);
    }

    return similarity_matrix;
}

cv::Mat LoadMetricsFromCSV(const std::string& filename);
void VisualizeMetrics(const cv::Mat& similarity_matrix, const std::string& title);

}  // namespace ipb::metrics
