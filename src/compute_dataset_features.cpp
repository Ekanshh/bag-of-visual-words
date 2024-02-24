#include <bovw.h>

#include <filesystem>
#include <string>

int main() {
    std::filesystem::path dataset_path = "../dataset/test/data";
    ipb::serialization::sifts::ConvertDataset(dataset_path);
}