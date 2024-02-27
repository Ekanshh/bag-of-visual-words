#pragma once

#include <fstream>
#include <string>

namespace bovw::api::html_writer {

void OpenDocument(std::ofstream& file);

void CloseDocument(std::ofstream& file);

void AddCSSStyle(std::ofstream& file, const std::string& stylesheet);

void AddTitle(std::ofstream& file, const std::string& title);

void OpenBody(std::ofstream& file);

void CloseBody(std::ofstream& file);

void OpenRow(std::ofstream& file);

void CloseRow(std::ofstream& file);

void OpenColumn(std::ofstream& file, bool highlight);

void CloseColumn(std::ofstream& file);

void AddImage(std::ofstream& file, const std::string& img_path, float score);

}  // namespace bovw::api::html_writer
