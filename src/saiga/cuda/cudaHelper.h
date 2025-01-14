/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#pragma once

#define SAIGA_ARRAY_VIEW_THRUST

#include "saiga/config.h"
#include "saiga/core/math/imath.h"
#include "saiga/core/util/DataStructures/ArrayView.h"
#include "saiga/core/util/assert.h"
#include "saiga/cuda/cuda.h"
#include "saiga/cuda/cudaTimer.h"
#include "saiga/cuda/thrust_helper.h"



#define CHECK_CUDA_ERROR(cudaFunction)                                                                              \
    {                                                                                                               \
        cudaError_t cudaErrorCode = cudaFunction;                                                                   \
        ((cudaErrorCode == cudaSuccess)                                                                             \
             ? static_cast<void>(0)                                                                                 \
             : Saiga::saiga_assert_fail(#cudaFunction " == cudaSuccess", __FILE__, __LINE__, SAIGA_ASSERT_FUNCTION, \
                                        cudaGetErrorString(cudaErrorCode)));                                        \
    }

#if defined(CUDA_DEBUG)
#    define CUDA_SYNC_CHECK_ERROR()                    \
        {                                              \
            CHECK_CUDA_ERROR(cudaDeviceSynchronize()); \
        }
#else
#    define CUDA_SYNC_CHECK_ERROR() (static_cast<void>(0))
#endif



namespace Saiga
{
namespace CUDA
{
template <typename T1, typename T2>
HD SAIGA_CONSTEXPR T1 getBlockCount(T1 problemSize, T2 threadCount)
{
    return (problemSize + (threadCount - T2(1))) / (threadCount);
}


SAIGA_CUDA_API extern void initCUDA();
SAIGA_CUDA_API extern void destroyCUDA();

}  // namespace CUDA
}  // namespace Saiga


#define THREAD_BLOCK(_problemSize, _threadCount) Saiga::CUDA::getBlockCount(_problemSize, _threadCount), _threadCount
