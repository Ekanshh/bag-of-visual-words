# 
add_executable(main main.cpp)
set_target_properties(main PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
target_link_libraries(main bovw)

# 
add_executable(query query.cpp)
set_target_properties(query PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
target_link_libraries(query bovw)

# 
add_executable(compute_metrics compute_metrics.cpp)
set_target_properties(compute_metrics PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
target_link_libraries(compute_metrics bovw)