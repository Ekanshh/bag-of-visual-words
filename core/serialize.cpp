#include "serialize.hpp"

#include <fstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

namespace ipb::serialization {

void Serialize(const cv::Mat& m, const std::string& filename) {
    /**
     * @Ref: https://stackoverflow.com/a/32357875
     *
     */
    std::ofstream fs(filename, std::fstream::binary);

    // Header
    int rows = m.rows;
    int cols = m.cols;
    int type = m.type();
    int channels = m.channels();
    fs.write(reinterpret_cast<char*>(&rows), sizeof(int));
    fs.write(reinterpret_cast<char*>(&cols), sizeof(int));
    fs.write(reinterpret_cast<char*>(&type), sizeof(int));
    fs.write(reinterpret_cast<char*>(&channels), sizeof(int));

    // Data
    if (m.isContinuous()) {
        fs.write(m.ptr<char>(0), (m.dataend - m.datastart));
    } else {
        int rowsz = CV_ELEM_SIZE(type) * m.cols;
        for (int r = 0; r < m.rows; ++r) {
            fs.write(m.ptr<char>(r), rowsz);
        }
    }
}

cv::Mat Deserialize(const std::string& filename) {
    /**
     * @Ref: https://stackoverflow.com/a/32357875
     *
     */
    std::ifstream fs(filename, std::fstream::binary);
    int rows = 0;
    int cols = 0;
    int type = 0;
    int channels = 0;

    fs.read(reinterpret_cast<char*>(&rows), sizeof(int));      // rows
    fs.read(reinterpret_cast<char*>(&cols), sizeof(int));      // cols
    fs.read(reinterpret_cast<char*>(&type), sizeof(int));      // type
    fs.read(reinterpret_cast<char*>(&channels), sizeof(int));  // channels

    cv::Mat m(rows, cols, type);
    size_t elem_sz = m.elemSize();
    fs.read(reinterpret_cast<char*>(m.data), elem_sz * rows * cols);
    return m;
}

}  // namespace ipb::serialization
