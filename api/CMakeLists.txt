add_library(html_api STATIC
    html_writer.cpp
    image_browser.cpp
)

target_link_libraries(html_api PUBLIC fmt)