@PACKAGE_INIT@

if(rascaline_FOUND)
    return()
endif()

enable_language(CXX)

if (@BUILD_SHARED_LIBS@)
    add_library(rascaline SHARED IMPORTED GLOBAL)
else()
    add_library(rascaline STATIC IMPORTED GLOBAL)
endif()

set_target_properties(rascaline PROPERTIES
    IMPORTED_LOCATION ${PACKAGE_PREFIX_DIR}/@LIB_INSTALL_DIR@/@RASCALINE_LIB_NAME@
    INTERFACE_INCLUDE_DIRECTORIES ${PACKAGE_PREFIX_DIR}/@INCLUDE_INSTALL_DIR@/
    # we need to link with a C++ compiler to get the C++ stdlib for chemfiles
    IMPORTED_LINK_INTERFACE_LANGUAGES CXX
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT @BUILD_SHARED_LIBS@)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    # the rust standard lib uses pthread and libdl on linux
    target_link_libraries(rascaline INTERFACE Threads::Threads dl)
endif()
