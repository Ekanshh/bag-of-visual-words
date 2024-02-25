#include "metrics.hpp"

#include <fstream>

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

cv::Mat LoadMetricsFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file " + filename);
    }

    std::vector<std::vector<float>> data;
    std::string line;
    while (std::getline(file, line)) {
        std::vector<float> row;
        std::stringstream line_stream(line);
        std::string cell;
        while (std::getline(line_stream, cell, ',')) {
            row.push_back(std::stof(cell));
        }
        data.push_back(row);
    }

    cv::Mat measure(data.size(), data[0].size(), CV_32F);
    for (int i = 0; i < measure.rows; ++i) {
        for (int j = 0; j < measure.cols; ++j) {
            measure.at<float>(i, j) = data[i][j];
        }
    }

    return measure;
}

void VisualizeMetrics(const cv::Mat& similarity_matrix, const std::string& title) {
    cv::Mat adj_sim;
    cv::normalize(similarity_matrix, adj_sim, 0, 1, cv::NORM_MINMAX);
    cv::imshow(title, adj_sim);
    cv::waitKey(0);
}

}  // namespace ipb::metrics