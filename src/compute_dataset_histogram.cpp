#include <bovw.h>

#include <filesystem>
#include <iostream>

int main() {
    const std::filesystem::path& input_directory = "../dataset/test/data";
    const std::filesystem::path& output_directory = "../dataset/test/histograms_1000";
    const std::filesystem::path& dictionary_path = "../dataset/test/test_data_1000.yml";

    // Load the saved dictionary from disk
    ipb::BowDictionary::GetInstance().load(dictionary_path.string());

    // Iterate over all image files in the input directory
    for (const auto& entry : std::filesystem::directory_iterator(input_directory)) {
        if (entry.path().extension() == ".png") {
            const std::filesystem::path& image_path = entry.path();
            const std::filesystem::path& bin_path =
                    input_directory / "../bin" / (image_path.stem().string() + ".bin");

            // Deserialize the SIFT descriptors for the image
            cv::Mat descriptors = ipb::serialization::Deserialize(bin_path.string());

            // Compute the histogram of the image
            ipb::Histogram histogram(descriptors, ipb::BowDictionary::GetInstance());

            // Save the histogram to the output directory
            std::filesystem::path histogram_path =
                    output_directory / (image_path.stem().string() + ".csv");
            histogram.WriteToCSV(histogram_path.string());
        }
    }

    return 0;
}
