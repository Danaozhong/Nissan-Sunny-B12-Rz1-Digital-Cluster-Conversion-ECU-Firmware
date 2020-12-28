
SET(MODULE_SOURCES
    ex_threads/ex_thread.cpp
)


add_library(stdcppex STATIC ${MODULE_SOURCES})
target_link_libraries(stdcppex cppstdlib)



target_include_directories(stdcppex PUBLIC
    ex_threads
)