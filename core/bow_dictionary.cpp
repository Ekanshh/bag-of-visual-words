#include "bow_dictionary.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <unordered_set>
#include <vector>

namespace ipb {

cv::Mat kMeans(const std::vector<cv::Mat> &descriptors, int k, int max_iter) {
    auto total_descriptors =
            std::accumulate(descriptors.begin(), descriptors.end(), 0,
                            [](int sum, const cv::Mat &desc) { return sum + desc.rows; });
    std::cout << "Total descriptors: " << total_descriptors << std::endl;
    int descriptor_dim = descriptors[0].cols;
    cv::Mat concatenated_descriptors(total_descriptors, descriptor_dim, CV_32F);

    int offset = 0;
    for (const auto &desc : descriptors) {
        desc.copyTo(concatenated_descriptors.rowRange(offset, offset + desc.rows));
        offset += desc.rows;
    }

    cv::Mat centers;
    std::vector<int> labels;
    cv::TermCriteria criteria(cv::TermCriteria::EPS, max_iter, 1e-4);
    cv::kmeans(concatenated_descriptors, k, labels, criteria, 1, cv::KMEANS_RANDOM_CENTERS,
               centers);

    return centers;
}

// my implementation of kMeans
cv::Mat naive_kMeans(const std::vector<cv::Mat> &descriptors, int k, int max_iter) {
    // Accumulate descriptors into a single matrix
    int total_descriptors =
            std::accumulate(descriptors.begin(), descriptors.end(), 0,
                            [](int sum, const cv::Mat &desc) { return sum + desc.rows; });
    // std::cout << "Total descriptors: " << total_descriptors << std::endl;
    int descriptor_dim = descriptors[0].cols;
    cv::Mat concatenated_descriptors(total_descriptors, descriptor_dim, CV_32F);
    int offset = 0;
    for (const auto &desc : descriptors) {
        desc.copyTo(concatenated_descriptors.rowRange(offset, offset + desc.rows));
        offset += desc.rows;
    }
    // std::cout << "Concatenated descriptors: " << concatenated_descriptors.size << std::endl;

    // Randomly initialize means
    static std::random_device seed;
    static std::mt19937 random_number_generator(seed());
    std::uniform_int_distribution<int> indices(0, concatenated_descriptors.rows - 1);
    std::vector<int> unique_indices;
    std::unordered_set<int> seen_indices;
    while (static_cast<int>(unique_indices.size()) < k) {
        int index = indices(random_number_generator);
        if (seen_indices.find(index) == seen_indices.end()) {
            unique_indices.push_back(index);
            seen_indices.insert(index);
        }
    }
    cv::Mat means(k, descriptor_dim, CV_32F);
    for (int i = 0; i < k; ++i) {
        int index = unique_indices[i];
        concatenated_descriptors.row(index).copyTo(means.row(i));
    }
    // std::cout << "Initial means: " << means.size << std::endl;

    // Iterations
    for (int iter = 0; iter < max_iter; ++iter) {
        // Assign descriptors to clusters
        std::vector<std::vector<int>> cluster_indices(k);
        for (int i = 0; i < total_descriptors; ++i) {
            cv::Mat distances;
            cv::batchDistance(concatenated_descriptors.row(i), means, distances, CV_32F,
                              cv::noArray(), cv::NORM_L2SQR);
            cv::Point minLoc;
            cv::minMaxLoc(distances, nullptr, nullptr, &minLoc, nullptr);
            cluster_indices[minLoc.x].push_back(i);
        }

        // Check for empty clusters
        for (int i = 0; i < k; ++i) {
            if (cluster_indices[i].empty()) {
                std::cerr << "Empty cluster " << i << " detected. Reinitializing means..."
                          << std::endl;
                std::cerr << "Try decreasing the number of clusters..." << std::endl;
            }
        }

        // Compute new means
        cv::Mat new_means(k, descriptor_dim, CV_32F);
        for (int i = 0; i < k; ++i) {
            cv::Mat cluster_descriptors(static_cast<int>(cluster_indices[i].size()), descriptor_dim,
                                        CV_32F);
            for (int j = 0; j < static_cast<int>(cluster_indices[i].size()); ++j) {
                concatenated_descriptors.row(cluster_indices[i][j])
                        .copyTo(cluster_descriptors.row(j));
            }
            cv::reduce(cluster_descriptors, new_means.row(i), 0, cv::REDUCE_AVG);
        }

        // Check convergence
        if (cv::norm(new_means, means) < 1e-8) {
            // std::cout << "Converged after " << iter << " iterations" << std::endl;
            return new_means;
        }

        // Update means
        means = new_means.clone();
    }

    return means;
}

BowDictionary &BowDictionary::GetInstance() {
    static BowDictionary instance;
    return instance;
}
void BowDictionary::build(int max_iterations,
                          std::size_t size,
                          const std::vector<cv::Mat> &descriptors) {
    auto vocabulary = naive_kMeans(descriptors, static_cast<int>(size), max_iterations);
    set_vocabulary(vocabulary);
}

const cv::Mat &BowDictionary::vocabulary() const { return vocabulary_; }
void BowDictionary::set_vocabulary(const cv::Mat &vocabulary) { vocabulary_ = vocabulary.clone(); }

std::size_t BowDictionary::size() const {
    return static_cast<std::size_t>(vocabulary_.empty() ? 0 : vocabulary_.rows);
}
bool BowDictionary::empty() const { return vocabulary_.empty(); }

void BowDictionary::save(const std::string &filename) const {
    cv::FileStorage fs(filename, cv::FileStorage::WRITE);
    fs << "vocabulary" << vocabulary_;
    fs.release();
}

void BowDictionary::load(const std::string &filename) {
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    fs["vocabulary"] >> vocabulary_;
    fs.release();
}

}  // namespace ipb