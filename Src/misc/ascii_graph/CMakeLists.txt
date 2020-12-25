
SET(MODULE_SOURCES
    src/ascii_graph.cpp
)

add_library(ascii_graph STATIC ${MODULE_SOURCES})

target_link_libraries(ascii_graph stdcpp)

target_include_directories(ascii_graph PUBLIC
    inc
)
