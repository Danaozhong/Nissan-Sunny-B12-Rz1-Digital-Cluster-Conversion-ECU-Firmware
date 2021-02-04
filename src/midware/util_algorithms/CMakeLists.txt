
SET(MODULE_SOURCES
    src/util_algorithms.cpp
)


add_library(util_algorithms STATIC ${MODULE_SOURCES})

target_link_libraries(util_algorithms freertos)

target_include_directories(util_algorithms PUBLIC
    inc
)