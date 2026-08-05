/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef SIM_COMMON_H
#define SIM_COMMON_H
#include <iostream>
#include <vector>
#include "dtype_common.h"
#include "enum_factory.h"

namespace HcclSim {
MAKE_ENUM(BufferType, INPUT, OUTPUT, CCL, RESERVED)
}

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using s8 = signed char;
using s16 = signed short;
using s32 = signed int;
using s64 = signed long long;

using RankId = uint32_t;
using PodId = uint32_t;
using SerId = uint32_t;
using PhyId = uint32_t;
constexpr uint64_t SIM_MEM_BLOCK_SIZE = 0x10000000000; // 内存分配起始地址
constexpr uint64_t SIM_MEM_MASKER = 0xFFFFFF0000000000;
constexpr uint64_t SIZE_200MB = 200ULL * 1024 * 1024; // CCL 200MB
constexpr uint32_t MAX_STREAM_COUNT = 1984;
constexpr uint32_t MAX_NOTIFY_COUNT = 8192;

using ChannelHandle = uint64_t;
using ThreadHandle = uint64_t;
using PhyDeviceId = uint32_t;
using ServerMeta = std::vector<PhyDeviceId>;
using SuperPodMeta = std::vector<ServerMeta>;
using TopoMeta = std::vector<SuperPodMeta>;

namespace HcclSim {
struct NpuPos {
    PodId superpodId;
    SerId serverId;
    PhyId phyId;
};

struct MemBlock {
    BufferType bufferType;
    uint64_t startAddr;
    uint64_t size;
};
} // namespace HcclSim

using NpuPos = HcclSim::NpuPos;
using MemBlock = HcclSim::MemBlock;
using BufferType = HcclSim::BufferType;

// ReduceScatterV AllGatherV使用
struct VDataDesTag {
    std::vector<u64> displs; // 每个rank的数据在sendBuf中的偏移量（单位为dataType）
    std::vector<u64> counts; // 每个rank在sendBuf中的数据size，第i个元素表示需要向rank i发送/接受的数据量
    HcclDataType dataType; // 数据类型
};

// All2All系列使用
struct All2AllDataDesTag {
    HcclDataType sendType;       // 发送数据的数据类型
    HcclDataType recvType;       // 接收数据的数据类型
    u64 sendCount;               // 发送数据量 (All2All)
    u64 recvCount;               // 接收数据量 (All2All)
    std::vector<u64> sendCounts; // 发送数据量数组 (All2AllV)
    std::vector<u64> recvCounts; // 接收数据量数组 (All2AllV)
    std::vector<u64> sdispls;    // 发送偏移量数组 (All2AllV)，sdispls[i]=n表示本rank发给rank
                              // i的数据在sendBuf的起始位置相对sendBuf的偏移量，以sendType为基本单位
    std::vector<u64> rdispls;         // 接收偏移量数组 (All2AllV)
    std::vector<u64> sendCountMatrix; // (All2AllVC) sendCountMatrix[i * ranksize + j] 代表rank i发送到rank j的count参数
};

#endif
