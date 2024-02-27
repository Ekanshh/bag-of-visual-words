## Bag of Visual Words (BOVW) C++ Project

This C++ project implements the Bag of Visual Words (BOVW) algorithm for image retrieval. The project allows you to build a BOVW model using a dataset of images, and then perform image queries to retrieve similar images from the dataset.

### Project Structure

The project directory structure is organized as follows:

```
.
├── api
│   ├── html_writer.cpp
│   ├── html_writer.hpp
│   ├── image_browser.cpp
│   └── image_browser.hpp
├── bin
│   ├── main
│   └── query
├── build
├── core
│   ├── bow_dictionary.cpp
│   ├── bow_dictionary.hpp
│   ├── convert_dataset.cpp
│   ├── convert_dataset.hpp
│   ├── histogram.cpp
│   ├── histogram.hpp
│   ├── metrics.cpp
│   ├── metrics.hpp
│   ├── mykmeans.cpp
│   ├── mykmeans.hpp
│   ├── serialize.cpp
│   └── serialize.hpp
├── dataset
│   ├── images
│   └── query
├── interface
│   └── bovw.h
└── src
    ├── main.cpp
    └── query.cpp
```

### Usage

1. **Dataset Directory**: Store the image dataset in the `dataset/images` directory. Place query images in the `dataset/query` directory.

2. **Building the Project**: Build the project using the provided CMakeLists.txt file. Navigate to the build directory and execute:

   ```bash
   cmake ..
   make
   ```

3. **Executing the Query**: After building the project, navigate to the `bin` directory:

   ```bash
   cd bin
   ```

   Run the query executable:

   ```bash
   ./query
   ```

   Follow the prompts to enter the name of the query image file.

4. **Viewing Results**: The results of the image queries will be stored in the `dataset/query` directory.

### Additional Notes

- The `api` directory contains HTML writer and image browser functionalities for generating HTML files to view the query results.
- The `core` directory contains the core functionalities of the BOVW algorithm, including building the dictionary, converting datasets, computing histograms, and performing metrics calculations.
- The `interface` directory contains the interface definition file `bovw.h`.
- The `src` directory contains the main and query source files.
