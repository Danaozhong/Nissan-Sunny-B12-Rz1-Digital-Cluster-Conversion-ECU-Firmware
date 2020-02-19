add_library(libtable
    libtable.c)

target_include_directories(libtable
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(libtable
    PROPERTIES VERSION ${libtable_VERSION}
    SOVERSION ${libtable_SOVERSION})

if(NOT FORT_HAVE_WCHAR)
    target_compile_definitions(fort
        PRIVATE
            -DFT_CONGIG_DISABLE_WCHAR
    )
endif()

if(NOT FORT_HAVE_UTF8)
    target_compile_definitions(fort
        PRIVATE
            -DFT_CONGIG_DISABLE_UTF8
    )
endif()


include(GNUInstallDirs)
install(
    TARGETS
      libtable
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES libtable.h libtable.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

