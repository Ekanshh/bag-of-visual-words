
# mat_serialization library to serialize and deserialize sift features
add_library(mat_serialization STATIC
    serialize.cpp
)
target_link_libraries(mat_serialization 
    PUBLIC ${OpenCV_LIBS})
target_include_directories(mat_serialization PUBLIC ${OpenCV_INCLUDE_DIRS})

# convert_dataset library to convert dataset to binary format (sift features)
add_library(convert_dataset STATIC
    convert_dataset.cpp
)
target_link_libraries(convert_dataset 
    PUBLIC mat_serialization)

# kmeans library to perform kmeans clustering
# add_library(kmeans STATIC
#     kmeans.cpp
# )
# target_link_libraries(kmeans 
#     PUBLIC ${OpenCV_LIBS})

# target_include_directories(kmeans PUBLIC ${OpenCV_INCLUDE_DIRS})

# 
add_library(bow_dictionary STATIC
    bow_dictionary.cpp
)
target_link_libraries(bow_dictionary 
    PUBLIC ${OpenCV_LIBS})

target_include_directories(bow_dictionary PUBLIC ${OpenCV_INCLUDE_DIRS})

add_library(histogram STATIC
    histogram.cpp
)
target_link_libraries(histogram 
    PUBLIC bow_dictionary)

# metrics 
add_library(metrics STATIC
    metrics.cpp
)
target_link_libraries(metrics 
    PUBLIC histogram)