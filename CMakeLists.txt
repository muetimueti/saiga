﻿cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(LIB_NAME saiga)
set(LIB_EXPORT saiga-export)

project (${LIB_NAME} CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )
include(helper_macros)



set (SAIGA_VERSION_MAJOR 1)
set (SAIGA_VERSION_MINOR 3)
set (SAIGA_VERSION_PATCH 0)
set(SAIGA_VERSION ${SAIGA_VERSION_MAJOR}.${SAIGA_VERSION_MINOR}.${SAIGA_VERSION_PATCH})

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

############# Build Options ###############

message(STATUS "\nBuild Options")


#option (SAIGA_BUILD_SHARED "Build this library as a shared library" ON)
OptionsHelper (SAIGA_BUILD_SHARED "Build this library as a shared library" ON)

OptionsHelper (SAIGA_DEBUG "alot of error checks and more console output" OFF)
OptionsHelper (SAIGA_ASSERTS "enable the SAIGA_ASSERT makro" ON)
OptionsHelper (SAIGA_BUILD_SAMPLES "build samples" ON)
OptionsHelper (SAIGA_BUILD_TESTS "build tests" ON)
OptionsHelper (SAIGA_STRICT_FP "strict ieee floating point" OFF)
OptionsHelper (SAIGA_FULL_OPTIMIZE "finds and enables all possible optimizations" OFF)
OptionsHelper (SAIGA_ARCHNATIVE "adds the -march=native compile flag" ON)
OptionsHelper (SAIGA_CUDA_DEBUG "enables CUDA_SYNC_CHECK_ERROR and CUDA_ASSERT" ON)
OptionsHelper (SAIGA_CUDA_BLSP "enables cublas and cusparse" OFF)
OptionsHelper (SAIGA_CUDA_RDC "cuda relocateable device code" OFF)
OptionsHelper (SAIGA_CUDA_COMP "Add CUDA compile flags that make the code compatible with older GPUs but takes longer to compile" OFF)
OptionsHelper(SAIGA_BUILD_DOC "Build documentation" ON)
OptionsHelper(SAIGA_NO_INSTALL "The build directory can be used for external projects." OFF)
OptionsHelper(SAIGA_LEGACY_GLM "Use GLM instead of eigen. This feature will be removed in the near future" OFF)
OptionsHelper(SAIGA_PCH "Generate a precompiled header" OFF)
OptionsHelper(SAIGA_OPENMP "Enable OPENMP" ON)
OptionsHelper(SAIGA_LIBSTDCPP "Use the GCC std lib for the clang compiler" OFF)
OptionsHelper(SAIGA_DEBUG_ASAN "Enable the address sanitizer. Does not work in combination with TSAN." OFF)
OptionsHelper(SAIGA_DEBUG_TSAN "Enable the thread sanitizer. Does not work in combination with ASAN." OFF)


OptionsHelper(SAIGA_MODULE_CORE   "Build the core module, if possible." ON)
OptionsHelper(SAIGA_MODULE_OPENGL "Build the opengl module, if possible." ON)
OptionsHelper(SAIGA_MODULE_VULKAN "Build the vulkan module, if possible." ON)
OptionsHelper(SAIGA_MODULE_EXTRA  "Build the extra module, if possible." ON)
OptionsHelper(SAIGA_MODULE_VISION "Build the vision module, if possible." ON)
OptionsHelper(SAIGA_MODULE_CUDA   "Build the cuda module, if possible." ON)

OptionsHelper(SAIGA_WITH_MKL "Build the MKL library, if possible." OFF)


DefaultBuildType(RelWithDebInfo)



set(SAIGA_FULL_EIGEN 1)
if(SAIGA_LEGACY_GLM)
    set(SAIGA_FULL_EIGEN 0)
endif()
############# COMPILER FLAGS ###############

include(Saiga_compiler_flags)
if(SAIGA_PCH)
    include(cotire)
endif()

####### Dependencies #######

unset(MODULE_CORE)
unset(MODULE_CUDA)
unset(MODULE_EXTRA)
unset(MODULE_OPENGL)
unset(MODULE_VISION)
unset(MODULE_VULKAN)

if(SAIGA_MODULE_CORE)
    include (Dependencies_core)
endif()

if(MODULE_CORE)
    if(SAIGA_MODULE_CUDA)
        include (Dependencies_cuda)
    endif()
    if(SAIGA_MODULE_EXTRA)
        include (Dependencies_extra)
    endif()
    if(SAIGA_MODULE_OPENGL)
        include (Dependencies_opengl)
    endif()
    if(SAIGA_MODULE_VISION)
        include (Dependencies_vision)
    endif()
    if(SAIGA_MODULE_VULKAN)
        include (Dependencies_vulkan)
    endif()
endif()

############# INSTALL PATHS ###############

#This part is partly copied from the CMakeLists.txt of glbinding.
#If a relative path is given it is interpreted relative to the value of the CMAKE_INSTALL_PREFIX variable.
#if(UNIX)

set(INSTALL_INCLUDE   "include/${LIB_NAME}")
set(INSTALL_BIN       "bin")
set(INSTALL_SHARED    "lib")
set(INSTALL_LIB       "lib")
set(INSTALL_SHARE     "share/${LIB_NAME}")
set(INSTALL_SHADER    "share/${LIB_NAME}/shader")
set(INSTALL_DOC       "share/${LIB_NAME}/doc")
set(INSTALL_SAMPLES   "share/${LIB_NAME}/bin")

if(ajshgsdgasdgdsg)
    set(INSTALL_SHARE     "share/${LIB_NAME}")
    set(INSTALL_SHADER    "share/${LIB_NAME}/shader")
    set(INSTALL_BIN       "bin")
    set(INSTALL_SHARED    ".")
    set(INSTALL_LIB       "lib")
    set(INSTALL_INCLUDE   "include/${LIB_NAME}")
    set(INSTALL_DOC       "share/doc/${LIB_NAME}")

endif()


message(STATUS "\nCompiler Flags:")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_FLAGS_RELWITHDEBINFO: ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")

if(CUDA_FOUND)
    message(STATUS "\nCUDA Compiler Flags:")
    message(STATUS "CMAKE_CUDA_FLAGS: ${CMAKE_CUDA_FLAGS}")
    message(STATUS "CMAKE_CUDA_FLAGS_DEBUG: ${CMAKE_CUDA_FLAGS_DEBUG}")
    message(STATUS "CMAKE_CUDA_FLAGS_RELWITHDEBINFO: ${CMAKE_CUDA_FLAGS_RELWITHDEBINFO}")
    message(STATUS "CMAKE_CUDA_FLAGS_RELEASE: ${CMAKE_CUDA_FLAGS_RELEASE}")
endif()



############# Generate config file ###############



if(SAIGA_NO_INSTALL)
    set(SAIGA_SHADER_PREFIX "${PROJECT_SOURCE_DIR}/shader")
    set(SAIGA_INCLUDE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/src")
    set(SAIGA_LIB_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/bin")
    set(SAIGA_BUILD_INCLUDE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/include")

    set(SAIGA_CONFIG_CMAKE_FILE_NO_INSTALL "${CMAKE_CURRENT_BINARY_DIR}/SAIGAConfig.cmake")
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/SAIGAConfigNoInstall.cmake.in"
        "${SAIGA_CONFIG_CMAKE_FILE_NO_INSTALL}"
        @ONLY
        )
else()
    set(SAIGA_SHADER_PREFIX "${CMAKE_INSTALL_PREFIX}/share/saiga/shader")
    set(SAIGA_INCLUDE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/include")
    set(SAIGA_BUILD_INCLUDE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/include")
    set(SAIGA_LIB_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/lib")
endif()

STRING(REGEX REPLACE "\\\\" "/" SAIGA_SHADER_PREFIX ${SAIGA_SHADER_PREFIX} )
STRING(REGEX REPLACE "\\\\" "/" SAIGA_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} )

set(SAIGA_BUILDCONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/include")
set(SAIGA_BUILDCONFIG_FILE "${SAIGA_BUILDCONFIG_DIR}/saiga/saiga_buildconfig.h")
set(SAIGA_MODULES_FILE "${SAIGA_BUILDCONFIG_DIR}/saiga/saiga_modules.h")
INCLUDE_DIRECTORIES(${SAIGA_BUILDCONFIG_DIR})
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/src/${LIB_NAME}/saiga_buildconfig.h.in"
    "${SAIGA_BUILDCONFIG_FILE}"
    )
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/src/${LIB_NAME}/saiga_modules.h.in"
    "${SAIGA_MODULES_FILE}"
    )

set(SAIGA_CONFIG_CMAKE_FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/SAIGAConfig.cmake")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/SAIGAConfig.cmake.in"
    "${SAIGA_CONFIG_CMAKE_FILE}"
    @ONLY
    )




# Generate Version File
set(SAIGA_VERSION_CMAKE_FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/SAIGAConfigVersion.cmake")
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${SAIGA_VERSION_CMAKE_FILE}
    VERSION ${SAIGA_VERSION}
    COMPATIBILITY AnyNewerVersion
    )

############# C++ Standard and Filesystem stuff ###############

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


############# SOURCE ###############

set(OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${OUTPUT_DIR}")



add_subdirectory(src)
add_subdirectory(shader)

if(SAIGA_BUILD_SAMPLES)
    message(STATUS " ")
    add_subdirectory(samples)
    message(STATUS " ")
else()
    message(STATUS "\nNo samples.")
endif()

if(SAIGA_BUILD_TESTS)
    message(STATUS " ")
    add_subdirectory(tests)
    message(STATUS " ")
else()
    message(STATUS "\nNo tests.")
endif()

#set_target_properties (saiga PROPERTIES FOLDER lib)

############# INSTALL ###############

#install config file to share/
install(
    FILES "${SAIGA_CONFIG_CMAKE_FILE}" "${SAIGA_VERSION_CMAKE_FILE}"
    DESTINATION ${INSTALL_SHARE}
    )

#install buildconfig include/
install(
    FILES "${SAIGA_BUILDCONFIG_FILE}"
    DESTINATION ${INSTALL_INCLUDE}
    )

install(
    FILES "${SAIGA_MODULES_FILE}"
    DESTINATION ${INSTALL_INCLUDE}
    )

install(
    DIRECTORY ./src/saiga/
    DESTINATION ${INSTALL_INCLUDE}
    FILES_MATCHING PATTERN "*.h"
    PATTERN "*.inl"
    PATTERN "*.hpp"
    )


############# UNINSTALL ###############

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/uninstall.cmake
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/uninstall.cmake)

############# Doxygen ###############

# first we can indicate the documentation build as an option and set it to ON by default


# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND AND SAIGA_BUILD_DOC)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    set(DOXYGEN_MAIN ${CMAKE_SOURCE_DIR}/docs/main.md)
    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message(STATUS "Doxygen found. Generate with 'make doc'.")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )

    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html)

    install(
        DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
        DESTINATION ${INSTALL_DOC}
        FILES_MATCHING PATTERN "*"

        )
endif()




# Request that saiga be built with --std=c++14
# As this is a public compile feature anything that links to saiga
# will also build with -std=c++14

# This currently does not work with cuda
#target_compile_features(${LIB_NAME} PUBLIC cxx_std_17)

#set_property(TARGET ${LIB_NAME} PROPERTY CXX_STANDARD 17)
#set_property(TARGET ${LIB_NAME} PROPERTY CUDA_STANDARD 14)






