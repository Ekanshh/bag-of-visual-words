# bovw_initializer
add_executable(bovw_initializer bovw_initializer.cpp)
set_target_properties(bovw_initializer PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
target_link_libraries(bovw_initializer bovw)

# bovw_query
add_executable(bovw_query bovw_query.cpp)
set_target_properties(bovw_query PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
target_link_libraries(bovw_query bovw)