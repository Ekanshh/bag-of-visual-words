add_library(bovw INTERFACE)

target_include_directories(bovw INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
)
target_link_libraries(bovw INTERFACE mat_serialization convert_dataset bow_dictionary histogram)