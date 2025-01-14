set(MODULE_NAME "saiga_vision")


add_subdirectory(arap)
add_subdirectory(ba)
add_subdirectory(camera)
add_subdirectory(cameraModel)
add_subdirectory(icp)
add_subdirectory(kernels)
add_subdirectory(pgo)
add_subdirectory(reconstruction)
add_subdirectory(scene)
add_subdirectory(slam)
add_subdirectory(util)

if (OPENCV_FOUND)
    add_subdirectory(orb)
endif()

if(MKL_FOUND)
    add_subdirectory(mkl)
endif()

if(G2O_FOUND)
    add_subdirectory(g2o)
endif()

if(CERES_FOUND)
    add_subdirectory(ceres)
endif()

if(EIGENRECURSIVE_FOUND)
    add_subdirectory(recursive)
endif()

FILE(GLOB VULKAN_SRC  "*.cpp" "*.cu" "*.cc")
FILE(GLOB VULKAN_HEADER  "*.h" "*.hpp" "*.inl")

SET(SAIGA_ALL_FILES
    ${SAIGA_ALL_FILES}
    ${VULKAN_SRC}
    ${VULKAN_HEADER}
    )



if(SAIGA_BUILD_SHARED)
    add_definitions(-DSAIGA_DLL_EXPORTS)
    add_library(${MODULE_NAME} SHARED ${SAIGA_ALL_FILES})
else ()
    add_library(${MODULE_NAME} STATIC ${SAIGA_ALL_FILES})
endif()

target_link_libraries(${MODULE_NAME} ${VISION_TARGETS}  ${VISION_LIBS})
target_include_directories(
    ${MODULE_NAME}
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<INSTALL_INTERFACE:include/saiga/opengl/glbinding/include>
    $<INSTALL_INTERFACE:include/saiga/opengl/glbinding-aux/include>
    $<INSTALL_INTERFACE:share/saiga/shader/include>
    "$<INSTALL_INTERFACE:${VISION_INCLUDES}>"
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/saiga/opengl/glbinding/include>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/saiga/opengl/glbinding-aux/include>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/shader/include>
    "$<BUILD_INTERFACE:${VISION_INCLUDES}>"
    #PRIVATE
    # for the internal files
    #${PROJECT_SOURCE_DIR}/src/
    )

install(
    TARGETS ${MODULE_NAME}
    EXPORT ${LIB_EXPORT}
    RUNTIME DESTINATION ${INSTALL_BIN}
    LIBRARY DESTINATION ${INSTALL_SHARED}
    ARCHIVE DESTINATION ${INSTALL_LIB}
    )

#install(
#    EXPORT ${LIB_EXPORT}
#    FILE SaigaTargets.cmake
#    DESTINATION ${INSTALL_SHARE}
#    )

##### Precompiled Headers #####
if(SAIGA_PCH)
    set_target_properties(${MODULE_NAME} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "${PROJECT_SOURCE_DIR}/src/internal/pch_vision.h")
    set_target_properties(${MODULE_NAME} PROPERTIES COTIRE_ADD_UNITY_BUILD false)
    cotire(${MODULE_NAME})
endif()
