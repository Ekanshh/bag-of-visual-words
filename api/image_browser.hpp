#pragma once

#include <array>
#include <string>
#include <vector>

namespace bovw::api::image_browser {

using ScoredImage = std::tuple<std::string, float>;
using ImageRow = std::array<ScoredImage, 3>;

void AddFullRow(std::ofstream& file, const ImageRow& row, bool first_row);

void CreateImageBrowser(const std::string& title,
                        const std::string& stylesheet,
                        const std::vector<ImageRow>& rows,
                        const std::string& output_filename);

}  // namespace bovw::api::image_browser
