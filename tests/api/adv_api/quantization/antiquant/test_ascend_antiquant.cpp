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
#define private public
#define protected public
#include "kernel_operator.h"
#include "kernel_utils.h"

namespace AscendC {
template <typename InputDataType, typename OutputDataType, bool WithOffset = true>
class AntiQuantTest {
public:
    __aicore__ inline AntiQuantTest() {}
    __aicore__ inline void Init(
        GM_ADDR dstGm, GM_ADDR srcGm, GM_ADDR offsetGm, GM_ADDR scaleGm, uint32_t elementCountOfInput,
        uint32_t elementCountOfOffset, uint32_t tmpBufferSize)
    {
        m_elementCountOfInput = elementCountOfInput;
        m_elementCountOfOffset = elementCountOfOffset;

        m_dstGlobal.SetGlobalBuffer((__gm__ OutputDataType*)dstGm);
        m_srcGlobal.SetGlobalBuffer((__gm__ InputDataType*)srcGm);
        m_offsetGlobal.SetGlobalBuffer((__gm__ OutputDataType*)offsetGm);
        m_scaleGlobal.SetGlobalBuffer((__gm__ OutputDataType*)scaleGm);

        m_pipe.InitBuffer(m_queInSrc, 1, elementCountOfInput * sizeof(InputDataType));
        m_pipe.InitBuffer(m_queInOffset, 1, elementCountOfOffset * sizeof(OutputDataType));
        m_pipe.InitBuffer(m_queInScale, 1, elementCountOfOffset * sizeof(OutputDataType));
        m_pipe.InitBuffer(m_queOut, 1, elementCountOfInput * sizeof(OutputDataType));
        m_pipe.InitBuffer(m_queTmp, 1, tmpBufferSize);
    }
    __aicore__ inline void Process()
    {
        CopyIn();
        Compute();
        CopyOut();
    }

private:
    __aicore__ inline void CopyIn()
    {
        LocalTensor<InputDataType> srcLocal = m_queInSrc.AllocTensor<InputDataType>();
        DataCopy(srcLocal, m_srcGlobal, m_elementCountOfInput);
        m_queInSrc.EnQue(srcLocal);

        LocalTensor<OutputDataType> offsetLocal = m_queInOffset.AllocTensor<OutputDataType>();
        DataCopy(offsetLocal, m_offsetGlobal, m_elementCountOfOffset);
        m_queInOffset.EnQue(offsetLocal);

        LocalTensor<OutputDataType> scaleLocal = m_queInScale.AllocTensor<OutputDataType>();
        DataCopy(scaleLocal, m_scaleGlobal, m_elementCountOfOffset);
        m_queInScale.EnQue(scaleLocal);
    }
    __aicore__ inline void Compute()
    {
        LocalTensor<InputDataType> srcLocal = m_queInSrc.DeQue<InputDataType>();
        LocalTensor<OutputDataType> offsetLocal = m_queInOffset.DeQue<OutputDataType>();
        LocalTensor<OutputDataType> scaleLocal = m_queInScale.DeQue<OutputDataType>();
        LocalTensor<OutputDataType> dstLocal = m_queOut.AllocTensor<OutputDataType>();
        LocalTensor<uint8_t> sharedTmpBuffer = m_queTmp.AllocTensor<uint8_t>();

        AntiQuantShapeInfo shapeInfo{
            static_cast<uint32_t>(1), static_cast<uint32_t>(m_elementCountOfOffset), static_cast<uint32_t>(1),
            static_cast<uint32_t>(m_elementCountOfOffset)};
        if constexpr (WithOffset) {
            AscendAntiQuant<InputDataType, OutputDataType, false>(
                dstLocal, srcLocal, offsetLocal, scaleLocal, sharedTmpBuffer,
                m_elementCountOfInput / m_elementCountOfOffset, shapeInfo);
        } else {
            AscendAntiQuant<InputDataType, OutputDataType, false>(
                dstLocal, srcLocal, scaleLocal, sharedTmpBuffer, m_elementCountOfInput / m_elementCountOfOffset,
                shapeInfo);
        }

        m_queInSrc.FreeTensor(srcLocal);
        m_queInOffset.FreeTensor(offsetLocal);
        m_queInScale.FreeTensor(scaleLocal);
        m_queTmp.FreeTensor(sharedTmpBuffer);
        m_queOut.EnQue(dstLocal);
    }
    __aicore__ inline void CopyOut()
    {
        LocalTensor<OutputDataType> dstLocal = m_queOut.DeQue<OutputDataType>();
        DataCopy(m_dstGlobal, dstLocal, m_elementCountOfInput);
        m_queOut.FreeTensor(dstLocal);
    }

private:
    TPipe m_pipe;
    TQue<TPosition::VECIN, 1> m_queInSrc;
    TQue<TPosition::VECIN, 1> m_queInOffset;
    TQue<TPosition::VECIN, 1> m_queInScale;
    TQue<TPosition::VECOUT, 1> m_queTmp;
    TQue<TPosition::VECOUT, 1> m_queOut;

    GlobalTensor<OutputDataType> m_dstGlobal;
    GlobalTensor<InputDataType> m_srcGlobal;
    GlobalTensor<OutputDataType> m_offsetGlobal;
    GlobalTensor<OutputDataType> m_scaleGlobal;

    uint32_t m_elementCountOfInput;
    uint32_t m_elementCountOfOffset;
}; // class AntiQuantTest
} // namespace AscendC

template <typename InputDataType, typename OutputDataType, bool WithOffset = true>
__global__ __aicore__ void testAntiQuant(
    GM_ADDR dst, GM_ADDR src, GM_ADDR offset, GM_ADDR scale, uint32_t elementCountOfInput,
    uint32_t elementCountOfOffset, uint32_t tmpBufferSize)
{
    AscendC::AntiQuantTest<InputDataType, OutputDataType, WithOffset> op;
    op.Init(dst, src, offset, scale, elementCountOfInput, elementCountOfOffset, tmpBufferSize);
    op.Process();
}

struct antiquantParams {
    uint32_t elementCountOfInput;
    uint32_t elementCountOfOffset;
    uint32_t tmpBufferSize;
    void (*cal_func)(GM_ADDR, GM_ADDR, GM_ADDR, GM_ADDR, uint32_t, uint32_t, uint32_t);
};

class AntiquantTestsuite : public testing::Test, public testing::WithParamInterface<antiquantParams> {
protected:
    void SetUp() { AscendC::SetGCoreType(2); }
    void TearDown() { AscendC::SetGCoreType(0); }
};

INSTANTIATE_TEST_CASE_P(
    TEST_OPEARATION_ANTIQUANT, AntiquantTestsuite,
    ::testing::Values(
        antiquantParams{2048, 64, 1024, testAntiQuant<int8_t, half>},
        antiquantParams{2048, 64, 81920, testAntiQuant<int8_t, half>},
        antiquantParams{5888, 256, 81920, testAntiQuant<int8_t, half>},
        antiquantParams{12288, 96, 81920, testAntiQuant<int8_t, half>}
#if !(defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)) // bf16数据类型 kirinx90 9030不支持
        ,
        antiquantParams{2048, 64, 1024, testAntiQuant<int8_t, bfloat16_t>},
        antiquantParams{2048, 64, 81920, testAntiQuant<int8_t, bfloat16_t>},
        antiquantParams{5888, 256, 81920, testAntiQuant<int8_t, bfloat16_t>},
        antiquantParams{12288, 96, 81920, testAntiQuant<int8_t, bfloat16_t>},
        antiquantParams{2048, 64, 1024, testAntiQuant<int8_t, bfloat16_t, false>}
#endif
        ));

TEST_P(AntiquantTestsuite, testAntiquant)
{
    auto param = GetParam();
    uint8_t srcGm[param.elementCountOfInput] = {0};
    uint8_t offsetGm[param.elementCountOfOffset * sizeof(half)] = {0};
    uint8_t scaleGm[param.elementCountOfOffset * sizeof(half)] = {0};
    uint8_t dstGm[param.elementCountOfInput * sizeof(half)] = {0};
    param.cal_func(
        dstGm, srcGm, offsetGm, scaleGm, param.elementCountOfInput, param.elementCountOfOffset, param.tmpBufferSize);
    for (int32_t i = 0; i < param.elementCountOfInput; i++) {
        EXPECT_EQ(dstGm[i], 0x00);
    }
}
