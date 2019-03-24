############# Check which compiler is used ############

set(SAIGA_CXX_WCLANG 0)
set(SAIGA_CXX_CLANG 0)
set(SAIGA_CXX_GNU 0)
set(SAIGA_CXX_INTEL 0)
set(SAIGA_CXX_MSVC 0)


if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	if(WIN32)
    set(SAIGA_CXX_WCLANG 1)
    message(STATUS "Detected Compiler:   Windows Clang")
	else()
    set(SAIGA_CXX_CLANG 1)
    message(STATUS "Detected Compiler:   Clang")
	endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(SAIGA_CXX_GNU 1)
    message(STATUS "Detected Compiler:   GNU")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set(SAIGA_CXX_INTEL 1)
    message(STATUS "Detected Compiler:   Intel")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(SAIGA_CXX_MSVC 1)
    message(STATUS "Detected Compiler:   MSVC")
else()
    message(FATAL_ERROR "Unknown CXX Compiler.")
endif()


######### warnings #########

if(SAIGA_CXX_MSVC)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W1")
    # Force to always compile with W1
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W1" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W1")
    endif()
endif()


if(SAIGA_CXX_CLANG OR SAIGA_CXX_GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-strict-aliasing")
endif()

if (SAIGA_CXX_CLANG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-braces")
endif()

######### basic #########

if(SAIGA_LIBSTDCPP AND SAIGA_CXX_CLANG)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")
    set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -stdlib=libstdc++")
    SET(LIBS ${LIBS} "-lstdc++")
endif()

if(SAIGA_CXX_CLANG OR SAIGA_CXX_GNU)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
endif()

if(SAIGA_CXX_MSVC OR SAIGA_CXX_WCLANG)
    #multiprocessor compilation for visual studio
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    add_definitions(-D_ENABLE_EXTENDED_ALIGNED_STORAGE)
endif()

######### floating point #########

if(SAIGA_STRICT_FP)
    if(SAIGA_CXX_CLANG OR SAIGA_CXX_GNU)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2 -mfpmath=sse")
    endif()
    if(SAIGA_CXX_MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:strict")
    endif()
endif()

######### debug and optimization #########

if(SAIGA_CUDA_DEBUG)
    add_definitions(-DCUDA_DEBUG)
else()
    add_definitions(-DCUDA_NDEBUG)
endif()

if(SAIGA_FULL_OPTIMIZE)
    if(SAIGA_CXX_CLANG OR SAIGA_CXX_GNU)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    endif()
    if(SAIGA_CXX_MSVC)
        #todo check if avx is present
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Oi /Ot /Oy /GL /fp:fast /Gy /arch:AVX2")
        set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} /LTCG")
        add_definitions(-D__FMA__)
    endif()
endif()

if(SAIGA_ENABLE_ASAN)
    if(SAIGA_CXX_CLANG OR SAIGA_CXX_GNU)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g")
    else()
        message(FATAL_ERROR "ASAN not supported for your compiler.")
    endif()
endif()
