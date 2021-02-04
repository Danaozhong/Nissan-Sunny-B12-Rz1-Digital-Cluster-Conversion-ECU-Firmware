
set(MODULE_SOURCES
    src/lookup_table.cpp
)


add_library(lookup_table STATIC ${MODULE_SOURCES})


target_include_directories(lookup_table PUBLIC
    inc
)