/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#pragma once

#include "saiga/cuda/cudaHelper.h"


namespace Saiga
{
namespace CUDA
{
/**
 * A simple c++ wrapper for cuda streams
 *
 * Usage:
 *
 * Saiga::CUDA::CudaStream stream;
 * cudaMemcpyAsync(d_slice.data(),h_slice.data(),slizeSize,cudaMemcpyHostToDevice,stream);
 *
 */
class SAIGA_CUDA_API CudaStream
{
   public:
    cudaStream_t stream;


    CudaStream();

    ~CudaStream();

    // Let the stream wait for this event
    // this call returns immediately
    void waitForEvent(cudaEvent_t event);


    operator cudaStream_t() const;

    static cudaStream_t legacyStream();

    static cudaStream_t perThreadStream();
};



}  // namespace CUDA
}  // namespace Saiga
