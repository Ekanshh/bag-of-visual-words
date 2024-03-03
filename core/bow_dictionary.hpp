#pragma once

#include <opencv2/core.hpp>
#include <string>
#include <vector>

namespace ipb {

class BowDictionary {
protected:
    BowDictionary() = default;

public:
    static BowDictionary &GetInstance();

    BowDictionary(const BowDictionary &) = delete;
    BowDictionary &operator=(const BowDictionary &) = delete;
    BowDictionary(BowDictionary &&) = delete;
    BowDictionary &operator=(BowDictionary &&) = delete;
    ~BowDictionary() = default;

    void build(int max_iterations, std::size_t size, const std::vector<cv::Mat> &descriptors);
    void save(const std::string &filename) const;
    void load(const std::string &filename);

    [[nodiscard]] const cv::Mat &vocabulary() const;
    void set_vocabulary(const cv::Mat &vocabulary);
    [[nodiscard]] std::size_t size() const;
    [[nodiscard]] bool empty() const;

private:
    cv::Mat vocabulary_{};
};

cv::Mat kMeans(const std::vector<cv::Mat> &descriptors, int k, int max_iter);
// my implementation of kMeans
cv::Mat naive_kMeans(const std::vector<cv::Mat> &descriptors, int k, int max_iter);

}  // namespace ipb