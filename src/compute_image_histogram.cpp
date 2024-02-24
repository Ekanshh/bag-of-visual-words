#include <bovw.h>

#include <filesystem>

int main() {
    // Get a random image from the dataset and its SIFTs feature descriptors
    std::filesystem::path image_path = "../dataset/images/imageCompressedCam0_0000000.png";
    std::filesystem::path bin_path = "../dataset/bin/imageCompressedCam0_0000000.bin";
    cv::Mat descriptors = ipb::serialization::Deserialize(bin_path.string());
    std::cout << "Descriptors size: " << descriptors.size() << std::endl;

    // Load the saved dictionary from disk
    std::filesystem::path dictionary_path = "../dataset/dictionary.yml";
    ipb::BowDictionary::GetInstance().load(dictionary_path.string());

    // Compute the histogram of the image
    ipb::Histogram histogram(descriptors, ipb::BowDictionary::GetInstance());

    // Save the histogram to disk
    std::filesystem::path histogram_path = "../dataset/histogram_imageCompressedCam0_0000000.csv";
    histogram.WriteToCSV(histogram_path.string());
}