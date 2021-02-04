
SET(MODULE_SOURCES
        os_console/src/os_console.cpp
)


add_library(osservices STATIC ${MODULE_SOURCES})

target_link_libraries(osservices uart)

target_include_directories(osservices PUBLIC
    os_console/inc

)
