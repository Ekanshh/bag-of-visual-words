#include "image_browser.hpp"

#include <fstream>
#include <iostream>
#include <tuple>

#include "html_writer.hpp"

namespace bovw::api::image_browser {

void AddFullRow(std::ofstream& file, const ImageRow& row, bool first_row) {
    html_writer::OpenRow(file);
    for (const auto& [image_path, score] : row) {
        html_writer::OpenColumn(file, first_row);
        html_writer::AddImage(file, image_path, score);
        html_writer::CloseColumn(file);
        first_row = false;
    }
    html_writer::CloseRow(file);
}

void CreateImageBrowser(const std::string& title,
                        const std::string& stylesheet,
                        const std::vector<ImageRow>& rows,
                        const std::string& output_filename) {
    std::ofstream html_file(output_filename);
    if (!html_file.is_open()) {
        std::cerr << "Failed to open file: " << output_filename << std::endl;
        return;
    }

    html_writer::OpenDocument(html_file);
    html_writer::AddTitle(html_file, title);
    html_writer::AddCSSStyle(html_file, stylesheet);
    html_writer::OpenBody(html_file);
    for (size_t i = 0; i < rows.size(); ++i) {
        AddFullRow(html_file, rows[i], i == 0);
    }
    html_writer::CloseBody(html_file);
    html_writer::CloseDocument(html_file);

    html_file.close();
}

}  // namespace bovw::api::image_browser
