
set(MODULE_SOURCES
    src/replay_curve.cpp
)
add_library(replay_curve STATIC ${MODULE_SOURCES})

target_link_libraries(replay_curve lookup_table)
target_link_libraries(replay_curve trace)

target_include_directories(replay_curve PUBLIC
    inc
)