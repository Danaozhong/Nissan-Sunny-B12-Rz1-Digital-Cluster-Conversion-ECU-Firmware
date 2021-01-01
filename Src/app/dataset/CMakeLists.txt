
SET(MODULE_SOURCES
    src/dataset.cpp
)


add_library(dataset STATIC ${MODULE_SOURCES})

target_link_libraries(dataset nonvolatile_data_handler)
target_link_libraries(dataset lookup_table)

target_include_directories(dataset PUBLIC
    inc
)