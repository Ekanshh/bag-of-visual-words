#include "html_writer.hpp"

#include <filesystem>
#include <fstream>

namespace bovw::api::html_writer {

void OpenDocument(std::ofstream& file) {
    file << "<!DOCTYPE html>";
    file << "<html>";
}

void CloseDocument(std::ofstream& file) { file << "</html>"; }

void AddCSSStyle(std::ofstream& file, const std::string& stylesheet) {
    file << "<head>";
    file << R"(<link rel="stylesheet" type="text/css" href=")" << stylesheet << "\">";
    file << R"(
        <style>
            .row {
                display: flex;
                flex-wrap: wrap;
                justify-content: space-between; /* Distribute columns evenly */
            }

            .column {
                width: 30%; /* Adjust width to accommodate three columns */
                margin-bottom: 20px; /* Add space between rows */
                box-sizing: border-box; /* Include padding and border in width calculation */
            }
        </style>
    )";
    file << "</head>";
}

void AddTitle(std::ofstream& file, const std::string& title) {
    file << "<title>" << title << "</title>";
}

void OpenBody(std::ofstream& file) { file << "<body>"; }

void CloseBody(std::ofstream& file) { file << "</body>"; }

void OpenRow(std::ofstream& file) { file << R"(<div class="row">)"; }

void CloseRow(std::ofstream& file) { file << "</div>"; }

void OpenColumn(std::ofstream& file, bool highlight) {
    if (highlight) {
        file << R"(<div class="column" style="border: 5px solid green;">)";
    } else {
        file << R"(<div class="column">)";
    }
}

void CloseColumn(std::ofstream& file) { file << "</div>"; }

void AddImage(std::ofstream& file, const std::string& img_path, float score) {
    std::filesystem::path full_path(img_path);
    std::string filename = full_path.filename().string();
    std::string extension = full_path.extension().string();

    if (extension != ".png" && extension != ".jpg") {
        file << "[ERROR] File " << filename << " is not a valid image file";
        return;
    }

    file << R"(<h2>)" << filename << "</h2>\n";
    file << R"(<img src=")" << img_path << "\" />";
    file << R"(<p>score = )" << score << "</p>";
}

}  // namespace bovw::api::html_writer
