## Bag of Visual Words (BOVW) C++ Project

This C++ project is part of the coursework for the Modern C++ for Computer Vision course. It implements the Bag of Visual Words (BOVW) algorithm for image retrieval. The project enables the construction of a BOVW model using a dataset of images and allows users to perform image queries to retrieve similar images from the dataset.

### Project Structure

The project directory structure is organized as follows:

```
.
├── api
│   ├── CMakeLists.txt
│   ├── html_writer.cpp
│   ├── html_writer.hpp
│   ├── image_browser.cpp
│   └── image_browser.hpp
├── bin
│   ├── bovw_initializer
│   └── bovw_query
├── build
│   ├── api
│   ├── CMakeCache.txt
│   ├── CMakeFiles
│   ├── cmake_install.cmake
│   ├── compile_commands.json
│   ├── core
│   ├── interface
│   ├── Makefile
│   └── src
├── cmake
│   ├── ClangFormat.cmake
│   ├── StaticAnalyzers.cmake
│   └── TestCoverage.cmake
├── CMakeLists.txt
├── core
│   ├── bow_dictionary.cpp
│   ├── bow_dictionary.hpp
│   ├── CMakeLists.txt
│   ├── convert_dataset.cpp
│   ├── convert_dataset.hpp
│   ├── histogram.cpp
│   ├── histogram.hpp
│   ├── metrics.cpp
│   ├── metrics.hpp
│   ├── serialize.cpp
│   └── serialize.hpp
├── *dataset*
│   └── images
├── interface
│   ├── bovw.h
│   └── CMakeLists.txt
├── *query*
│   ├── images
├── README.md
├── *setup*
│   ├── bin
│   ├── dictionary.yml
│   ├── histograms
│   ├── metric_histogram_cosdist.csv
│   ├── metric_tfidf_histogram_cosdist.csv
│   └── tfidf_histograms
└── src
    ├── bovw_initializer.cpp
    ├── bovw_query.cpp
    └── CMakeLists.txt
```

### Usage

1. **Dataset Directory**: Store the image dataset in the `dataset/images` directory. You need to provide this in correct directory structure as mentioned above.

2. **Building the Project**: Build the project using the provided CMakeLists.txt file

   ```bash
   cmake -B build
   make -C build
   ```

3. **Executing the Query**: After building the project, navigate to the `bin` directory:

   ```bash
   cd bin
   ```

   You can either setup the bovw pipeline again by running (This will recompute setup resouces such as dictionary, histograms, etc. and will take some time):

   ```bash
   ./bovw_initializer
   ```

   or use pre-computed bovw using the resources in `setup` directory and directly running:

   ```bash
   ./bovw_query
   ```

4. **Viewing Results**: The results of the query will be stored in the `query` directory. It will be overwritten everytime you run a query.

### Additional Notes

- The `api` directory contains HTML writer and image browser functionalities for generating HTML files to view the query results.
- The `core` directory contains the core functionalities of the BOVW algorithm, including building the dictionary, converting datasets, computing histograms, and performing metrics calculations.
- The `interface` directory contains the interface definition file `bovw.h`.
- The `src` directory contains the setup and query source files.
