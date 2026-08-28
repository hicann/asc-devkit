# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------

target_sources(hccl-model-proxy PRIVATE
    ccu_kernel_proxy.cc
    ccu_primitives_stub.cc
    aclrt_stub.cc
    hccl_algorithm_stub.cc
    hccl_stub.cc
    hcclinner_stub.cc
    log_stub.cc
    mmpa_stub.cc
    error_manager_stub.cc
    hcomm_ccu_stub.cc
    hccl_profiling_dfx_stub.cc
    communicator/sim_communicator.cc
    communicator/sim_thread_manager.cc
    communicator/sim_thread.cc
    communicator/sim_channel.cc
    communicator/sim_channel_manager.cc
    communicator/sim_channel_exchange_handler.cc
    communicator/sim_context_manager.cc
    topo_model/topo_model.cc
)

target_compile_options(hccl-model-proxy PRIVATE
    -std=c++14
    -fno-common
    -fno-strict-aliasing
    -pipe
    -g
    -O0
    -D_GLIBCXX_USE_CXX11_ABI=0
)
