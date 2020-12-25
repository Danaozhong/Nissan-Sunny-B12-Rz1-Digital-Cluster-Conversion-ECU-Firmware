
SET(MODULE_SOURCES
    condition_variable.cpp
    freertos_time.cpp
    future.cc
    gthr_key.cpp
    mutex.cc
    thread.cpp
)


add_library(stdcpp STATIC ${MODULE_SOURCES})
target_link_libraries(stdcpp freertos)

target_include_directories(stdcpp PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}
)