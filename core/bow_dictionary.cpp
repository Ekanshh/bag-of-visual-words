#include "bow_dictionary.hpp"

#include <algorithm>
#include <iostream>
#include <opencv2/features2d.hpp>
#include <vector>

namespace ipb {

cv::Mat kMeans(const std::vector<cv::Mat> &descriptors, int k, int max_iter) {
    cv::Mat descriptors_mat;
    for (const auto &desc : descriptors) {
        cv::Mat desc_float;
        desc.convertTo(desc_float, CV_32F);
        descriptors_mat.push_back(desc_float);
    }

    cv::Mat centers;
    cv::Mat labels;
    cv::TermCriteria criteria(cv::TermCriteria::EPS, max_iter, 1e-4);
    cv::kmeans(descriptors_mat, k, labels, criteria, 1, cv::KMEANS_RANDOM_CENTERS, centers);

    return centers;
}

BowDictionary &BowDictionary::GetInstance() {
    static BowDictionary instance;
    return instance;
}
void BowDictionary::build(int max_iterations,
                          std::size_t size,
                          const std::vector<cv::Mat> &descriptors) {
    auto vocabulary = kMeans(descriptors, static_cast<int>(size), max_iterations);
    set_vocabulary(vocabulary);
    auto idf = ComputeIDF(descriptors);
    set_idf(idf);  // Assuming there's a method to set IDF values in the class
}

std::vector<float> BowDictionary::ComputeIDF(const std::vector<cv::Mat> &descriptors) {
    std::vector<float> idf(vocabulary_.rows, 0);
    for (const auto &desc : descriptors) {
        auto matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE);
        std::vector<std::vector<cv::DMatch>> knn_matches;
        matcher->knnMatch(desc, vocabulary_, knn_matches, 1);

        for (const auto &match : knn_matches) {
            if (!match.empty()) {
                idf[match[0].trainIdx]++;
            }
        }
    }

    for (float &val : idf) {
        val = std::log(static_cast<float>(descriptors.size()) / (val));
    }

    return idf;
}

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