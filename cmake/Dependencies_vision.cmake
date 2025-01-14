# Defines the following output variables:
#
# VISION_INCLUDES:    The list of required include directories
# VISION_LIBS:        The list of required libraries for link_target
# VISION_TARGETS:     The list of required targets
# MODULE_VISION:      True if all required dependencies are found.
#

unset(PACKAGE_INCLUDES)
unset(LIB_TARGETS)
unset(LIBS)
unset(MODULE_VISION)


if(NOT MODULE_CORE)
    return()
endif()


#opencv
find_package(OpenCV QUIET)
if(OpenCV_FOUND)
    SET(SAIGA_USE_OPENCV 1)
endif()
PackageHelper(OpenCV "${OpenCV_FOUND}" "${OpenCV_INCLUDE_DIRS}" "${OpenCV_LIBRARIES}")

#Sophus
find_package(Sophus QUIET)
PackageHelperTarget(Sophus::Sophus SOPHUS_FOUND)


#Recursive
find_package(EigenRecursive QUIET)
PackageHelperTarget(Eigen::EigenRecursive EIGENRECURSIVE_FOUND)
if(EIGENRECURSIVE_FOUND)
    SET(SAIGA_USE_EIGENRECURSIVE 1)
endif()

#g2o
find_package(g2o QUIET)
PackageHelperTarget(g2o::core G2O_FOUND)
if(G2O_FOUND)
    SET(SAIGA_USE_G2O 1)
endif()

#ceres
find_package(Ceres QUIET)
if(CERES_FOUND)
    SET(SAIGA_USE_CERES 1)
endif()
PackageHelperTarget(ceres CERES_FOUND)

#cholmod
find_package(Cholmod REQUIRED)
if(CHOLMOD_FOUND)
    SET(SAIGA_USE_CHOLMOD 1)
endif()
PackageHelper(Cholmod ${CHOLMOD_FOUND} "${CHOLMOD_INCLUDES}" "${CHOLMOD_LIBRARIES}")



# Currently only mkl is supported
#set(BLA_VENDOR Intel10_64lp_seq)
#find_package(BLAS QUIET)
#PackageHelper(BLAS "${BLAS_FOUND}" "" "${BLAS_LIBRARIES}")
#message(STATUS "BLAS Library: ${BLAS_LIBRARIES}")

#find_package(LAPACK QUIET)
#PackageHelper(LAPACK "${LAPACK_FOUND}" "" "${LAPACK_LIBRARIES}")
#message(STATUS "LAPACK Library: ${LAPACK_LIBRARIES}")

#mkl
if(SAIGA_WITH_MKL)
    find_package(MKL QUIET)
    if(MKL_FOUND )
        SET(SAIGA_USE_MKL 1)
    endif()
    PackageHelper(MKL "${MKL_FOUND}" "${MKL_INCLUDE_DIR}" "${MKL_LIBRARIES}")
endif()


#openni2
find_package(OpenNI2 QUIET)
if(OPENNI2_FOUND)
    SET(SAIGA_USE_OPENNI2 1)
endif()
PackageHelper(OpenNI2 "${OPENNI2_FOUND}" "${OPENNI2_INCLUDE_DIRS}" "${OPENNI2_LIBRARIES}")







set(VISION_INCLUDES ${PACKAGE_INCLUDES})
set(VISION_LIBS ${LIBS})
set(VISION_TARGETS saiga_core ${LIB_TARGETS})

message(STATUS ${VISION_TARGETS})

if(EIGEN3_FOUND AND SOPHUS_FOUND)
    message(STATUS "Saiga vision enabled.")
    SET(MODULE_VISION 1)
    SET(SAIGA_VISION 1)
endif()


