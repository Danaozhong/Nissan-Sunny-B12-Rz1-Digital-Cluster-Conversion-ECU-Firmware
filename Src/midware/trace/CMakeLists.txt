

if (USE_TRACE)
    add_definitions(
        -DUSE_TRACE
    )
   
    list(APPEND PROJECT_SOURCES
        midware/trace/src/trace.cpp
    )
else()

endif()