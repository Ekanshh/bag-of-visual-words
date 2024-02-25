#
add_executable(compute_dataset_features compute_dataset_features.cpp)
set_target_properties(compute_dataset_features PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
target_link_libraries(compute_dataset_features PUBLIC 
                      bovw)

# 
add_executable(compute_dataset_dictionary compute_dataset_dictionary.cpp)
set_target_properties(compute_dataset_dictionary PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
target_link_libraries(compute_dataset_dictionary bovw)                      

# 
add_executable(compute_image_histogram compute_image_histogram.cpp)
set_target_properties(compute_image_histogram PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
target_link_libraries(compute_image_histogram bovw)                      

#
add_executable(compute_dataset_histogram compute_dataset_histogram.cpp)
set_target_properties(compute_dataset_histogram PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
target_link_libraries(compute_dataset_histogram bovw)     

# 
add_executable(reweight_dataset_histogram reweight_dataset_histogram.cpp)
set_target_properties(reweight_dataset_histogram PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
target_link_libraries(reweight_dataset_histogram bovw)   

# 
add_executable(compute_metrics compute_metrics.cpp)
set_target_properties(compute_metrics PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
target_link_libraries(compute_metrics bovw)

# 
add_executable(main main.cpp)
set_target_properties(main PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
target_link_libraries(main bovw)