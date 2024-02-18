#include "convert_dataset.hpp"

#include <filesystem>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/xfeatures2d.hpp>

#include "serialize.hpp"

namespace ipb::serialization::sifts {

void ConvertDataset(const std::filesystem::path& dataset_path) {
    std::string bin_path = dataset_path.string() + "/../bin/";
    if (!std::filesystem::exists(bin_path)) {
        std::filesystem::create_directory(bin_path);
    }

    // Iterate over the images in the dataset
    for (const auto& entry : std::filesystem::directory_iterator(dataset_path)) {
        if (entry.path().extension() == ".png") {
            cv::Mat img = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);
            cv::Ptr<cv::SIFT> detector = cv::SIFT::create();
            std::vector<cv::KeyPoint> keypoints;
            cv::Mat descriptors;
            detector->detectAndCompute(img, cv::noArray(), keypoints, descriptors);

            std::string bin_img = bin_path + entry.path().stem().string() + ".bin";
            ipb::serialization::Serialize(descriptors, bin_img);
        }
    }
}

std::vector<cv::Mat> LoadDataset(const std::filesystem::path& bin_path) {
    std::vector<cv::Mat> descriptors;
    for (const auto& entry : std::filesystem::directory_iterator(bin_path)) {
        if (entry.path().extension().string() == ".bin") {
            cv::Mat descriptor = ipb::serialization::Deserialize(entry.path().string());
            descriptors.push_back(descriptor);
        }
    }
    return descriptors;
}

}  // namespace ipb::serialization::sifts
