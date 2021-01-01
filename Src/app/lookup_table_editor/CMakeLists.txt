
set(MODULE_SOURCES
    src/lookup_table_editor.cpp
)


add_library(lookup_table_editor STATIC ${MODULE_SOURCES})
target_link_libraries(lookup_table_editor main_application)

target_include_directories(lookup_table_editor PUBLIC
    inc
)