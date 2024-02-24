#include <bovw.h>

#include <filesystem>
#include <string>

int main() {
    std::filesystem::path dataset_path = "../dataset/test/data";
    const auto bin_path = dataset_path / "../bin";
    const auto descriptors = ipb::serialization::sifts::LoadDataset(bin_path);
    const auto max_iterations = 50;
    const auto num_words = 1000;
    ipb::BowDictionary::GetInstance().build(max_iterations, num_words, descriptors);

    std::filesystem::path dictionary_path = dataset_path / "../test_data_1000.yml";
    ipb::BowDictionary::GetInstance().save(dictionary_path.string());
}