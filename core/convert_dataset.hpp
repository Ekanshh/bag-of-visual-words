#pragma once

#include <filesystem>
#include <opencv2/core/mat.hpp>
#include <vector>

namespace ipb::serialization::sifts {

void ConvertDataset(const std::filesystem::path& dataset_path,
                    const std::filesystem::path& bin_path);
std::vector<cv::Mat> LoadDataset(const std::filesystem::path& bin_path);

}  // namespace ipb::serialization::sifts