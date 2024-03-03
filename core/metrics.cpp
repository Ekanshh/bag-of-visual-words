#include "metrics.hpp"

#include <fstream>
#include <opencv2/core.hpp>
#include <string>

namespace ipb::metrics {

void WriteMetricsToCSV(const cv::Mat& measure, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file " + filename);
    }

    for (int i = 0; i < measure.rows; ++i) {
        for (int j = 0; j < measure.cols; ++j) {
            file << measure.at<float>(i, j);
            if (j < measure.cols - 1) {
                file << ",";
            }
        }
        file << std::endl;
    }
}
void VisualizeMetrics(const cv::Mat& measure, const std::string& title) {
    cv::Mat measure_norm;
    cv::normalize(measure, measure_norm, 0, 1, cv::NORM_MINMAX, CV_64F);
    cv::Mat measure_norm_8u;
    measure_norm.convertTo(measure_norm_8u, CV_8U, 255.0);
    cv::Mat measure_heatmap;
    cv::applyColorMap(measure_norm_8u, measure_heatmap, cv::COLORMAP_VIRIDIS);

    int window_width = 640;
    int window_height = 640;

    cv::namedWindow(title, cv::WINDOW_NORMAL);
    cv::resizeWindow(title, window_width, window_height);

    cv::imshow(title, measure_heatmap);

    cv::waitKey(0);
}
}  // namespace ipb::metrics