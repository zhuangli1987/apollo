/******************************************************************************
 * Copyright 2017 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef CYBERTRON_CROUTINE_SYSTEM_CUDA_ASYNC_H_
#define CYBERTRON_CROUTINE_SYSTEM_CUDA_ASYNC_H_

#include <cuda_runtime.h>

namespace apollo {
namespace cybertron {
namespace croutine {

void CUDART_CB CudaAsyncCallback(cudaStream_t event, cudaError_t status,
                                 void* data);
void CudaAsync(const cudaStream_t& stream);

}  // namespace croutine
}  // namespace cybertron
}  // namespace apollo

#endif  // CYBERTRON_CROUTINE_SYSTEM_CUDA_ASYNC_H_
