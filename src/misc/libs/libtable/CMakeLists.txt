add_library(libtable
    libtable.c)

target_include_directories(libtable
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(libtable
    PROPERTIES VERSION ${libtable_VERSION}
    SOVERSION ${libtable_SOVERSION})


include(GNUInstallDirs)
install(
    TARGETS
      libtable
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES libtable.h libtable.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

