/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <gtest/gtest.h>
#include <type_traits>
#include "kernel_operator.h"

using namespace std;
using namespace AscendC;
using AscendC::Reg::MaskReg;
using AscendC::Reg::RegTensor;
using AscendC::Reg::UpdateMask;

#define DEFINE_GM_ACCESS_CASE(READ_API, WRITE_API, TAG)                                   \
    template <typename T, uint8_t Mode>                                                   \
    class KernelGmAccess##TAG {                                                           \
    public:                                                                               \
        __aicore__ inline KernelGmAccess##TAG() {}                                        \
        __aicore__ inline void Process(GM_ADDR dst0Gm, GM_ADDR src0Gm, uint32_t calcount) \
        {                                                                                 \
            __gm__ T* dstAddr = reinterpret_cast<__gm__ T*>(dst0Gm);                      \
            T value;                                                                      \
            if (Mode == 0) {                                                              \
                value = READ_API<T>(reinterpret_cast<__gm__ T*>(src0Gm));                 \
            } else {                                                                      \
                value = READ_API<T>(reinterpret_cast<__gm__ T*>(src0Gm + sizeof(T)));     \
            }                                                                             \
            WRITE_API<T>(dstAddr, value);                                                 \
        }                                                                                 \
                                                                                          \
    private:                                                                              \
        GlobalTensor<T> src0Global;                                                       \
        GlobalTensor<T> dst0Global;                                                       \
        TPipe pipe;                                                                       \
        TQue<QuePosition::VECIN, 1> inQueueX;                                             \
        uint32_t dataSize = 0;                                                            \
    };                                                                                    \
    template <typename T, uint8_t Mode>                                                   \
    void RunCase##TAG()                                                                   \
    {                                                                                     \
        int srcByteSize = sizeof(T);                                                      \
        int dstByteSize = sizeof(T);                                                      \
        int dataSize = 256;                                                               \
        uint8_t dstGm0[dataSize * dstByteSize] = {0};                                     \
        uint8_t srcGm0[dataSize * srcByteSize] = {0};                                     \
                                                                                          \
        KernelGmAccess##TAG<T, Mode> op;                                                  \
        op.Process(dstGm0, srcGm0, dataSize);                                             \
    }

DEFINE_GM_ACCESS_CASE(ReadGmBypassDCache, WriteGmBypassDCache, New)
DEFINE_GM_ACCESS_CASE(ReadGmByPassDCache, WriteGmByPassDCache, Old)

struct MicroGmAccessParams {
    void (*CallFunc)();
};

class MicroGmAccessTestSuite : public testing::Test, public testing::WithParamInterface<MicroGmAccessParams> {
protected:
    void SetUp() {}
    void TearDown() {}
};

INSTANTIATE_TEST_CASE_P(
    MicroGmAccessTestCases, MicroGmAccessTestSuite,
    ::testing::Values(
        MicroGmAccessParams{RunCaseNew<uint64_t, 0>}, MicroGmAccessParams{RunCaseNew<int64_t, 0>},
        MicroGmAccessParams{RunCaseNew<uint32_t, 0>}, MicroGmAccessParams{RunCaseNew<int32_t, 0>},
        MicroGmAccessParams{RunCaseNew<uint16_t, 0>}, MicroGmAccessParams{RunCaseNew<int16_t, 0>},
        MicroGmAccessParams{RunCaseNew<uint8_t, 0>}, MicroGmAccessParams{RunCaseNew<int8_t, 0>},
        MicroGmAccessParams{RunCaseNew<uint64_t, 1>}, MicroGmAccessParams{RunCaseNew<int64_t, 1>},
        MicroGmAccessParams{RunCaseNew<uint32_t, 1>}, MicroGmAccessParams{RunCaseNew<int32_t, 1>},
        MicroGmAccessParams{RunCaseNew<uint16_t, 1>}, MicroGmAccessParams{RunCaseNew<int16_t, 1>},
        MicroGmAccessParams{RunCaseNew<uint8_t, 1>}, MicroGmAccessParams{RunCaseNew<int8_t, 1>},
        MicroGmAccessParams{RunCaseOld<uint64_t, 0>}, MicroGmAccessParams{RunCaseOld<int64_t, 0>},
        MicroGmAccessParams{RunCaseOld<uint32_t, 0>}, MicroGmAccessParams{RunCaseOld<int32_t, 0>},
        MicroGmAccessParams{RunCaseOld<uint16_t, 0>}, MicroGmAccessParams{RunCaseOld<int16_t, 0>},
        MicroGmAccessParams{RunCaseOld<uint8_t, 0>}, MicroGmAccessParams{RunCaseOld<int8_t, 0>},
        MicroGmAccessParams{RunCaseOld<uint64_t, 1>}, MicroGmAccessParams{RunCaseOld<int64_t, 1>},
        MicroGmAccessParams{RunCaseOld<uint32_t, 1>}, MicroGmAccessParams{RunCaseOld<int32_t, 1>},
        MicroGmAccessParams{RunCaseOld<uint16_t, 1>}, MicroGmAccessParams{RunCaseOld<int16_t, 1>},
        MicroGmAccessParams{RunCaseOld<uint8_t, 1>}, MicroGmAccessParams{RunCaseOld<int8_t, 1>}));

TEST_P(MicroGmAccessTestSuite, MicroGmAccessTestCase)
{
    auto param = GetParam();
    param.CallFunc();
}
