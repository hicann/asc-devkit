/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_kernel_alg_base.h"
#include "ccu_primitives_stub.h"

#include <gtest/gtest.h>

#include <vector>

namespace HcclSim {
namespace CcuSt {
namespace {

// 与 CcuEventGroup::EVENT_BIT_WIDTH 保持一致：逻辑信号按 16bit 分组到物理 event
constexpr uint32_t BIT_WIDTH = 16;

struct ExpectedEventOp {
    OpCode code;
    ResourceHandle handle;
    uint16_t mask;
};

// 在独立 CompilerContext 下执行 fn（stub 依赖 Current() 捕获 IR），返回捕获的程序
template <typename Fn>
Program CaptureProgram(Fn fn)
{
    Program program;
    CompilerContext compiler(program);
    CompilerContext::SetCurrent(&compiler);
    fn();
    CompilerContext::SetCurrent(nullptr);
    return program;
}

void ExpectEventOps(const Program& program, const std::vector<ExpectedEventOp>& expected)
{
    ASSERT_EQ(program.operations.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        const Operation& op = program.operations[i];
        EXPECT_EQ(op.code, expected[i].code) << "op[" << i << "] code";
        // stub：EVENT_RECORD 的 handle 记录在 dst，EVENT_WAIT 记录在 src0
        const ResourceHandle handle = op.code == OpCode::EVENT_RECORD ? op.dst : op.src0;
        EXPECT_EQ(handle, expected[i].handle) << "op[" << i << "] handle";
        EXPECT_EQ(op.mask, expected[i].mask) << "op[" << i << "] mask";
    }
}

// 低 count 位置 1 的掩码
uint16_t LowMask(uint32_t count) { return static_cast<uint16_t>((1u << count) - 1); }

uint16_t Bit(uint32_t idx) { return static_cast<uint16_t>(1u << idx); }

// ---- Init/Record：按 16bit 路由到物理 event，Record 发射单 bit 掩码 ----
TEST(CcuEventGroupTest, RecordRoutesSignalToPhysicalEvent)
{
    mc2_ops_hccl::CcuEventGroup group;
    std::vector<CcuResult> rets;
    const Program program = CaptureProgram([&]() {
        group.Init(34);                   // 34 = 2*16 + 2：覆盖整组、跨组边界与尾组
        rets.push_back(group.Record(0));  // 首组首位
        rets.push_back(group.Record(15)); // 首组末位
        rets.push_back(group.Record(16)); // 次组首位（跨组边界）
        rets.push_back(group.Record(33)); // 尾组末位
    });
    ASSERT_EQ(rets.size(), 4U);
    for (CcuResult ret : rets) {
        EXPECT_EQ(ret, CCU_SUCCESS);
    }
    // 34 个信号应分配 ceil(34/16)=3 个物理 event；GetEvent 接收 signalIdx，
    // 用各组首信号 0/16/32 取对应物理 event
    ASSERT_EQ(program.events.size(), 3U);
    const ResourceHandle e0 = group.GetEvent(0).handle;
    const ResourceHandle e1 = group.GetEvent(16).handle;
    const ResourceHandle e2 = group.GetEvent(32).handle;
    EXPECT_NE(e0, e1);
    EXPECT_NE(e1, e2);
    ExpectEventOps(
        program, {{OpCode::EVENT_RECORD, e0, Bit(0)},
                  {OpCode::EVENT_RECORD, e0, Bit(15)},
                  {OpCode::EVENT_RECORD, e1, Bit(0)},
                  {OpCode::EVENT_RECORD, e2, Bit(1)}});
}

// ---- WaitAll：整组全 1 掩码，尾组仅覆盖有效位 ----
TEST(CcuEventGroupTest, WaitAllSingleEventFullMask)
{
    mc2_ops_hccl::CcuEventGroup group;
    const Program program = CaptureProgram([&]() {
        group.Init(8);
        group.WaitAll();
    });
    ASSERT_EQ(program.events.size(), 1U);
    ExpectEventOps(program, {{OpCode::EVENT_WAIT, group.GetEvent(0).handle, LowMask(8)}});
}

TEST(CcuEventGroupTest, WaitAllUsesPartialMaskOnTailEvent)
{
    mc2_ops_hccl::CcuEventGroup group;
    const Program program = CaptureProgram([&]() {
        group.Init(34);
        group.WaitAll();
    });
    ExpectEventOps(
        program, {{OpCode::EVENT_WAIT, group.GetEvent(0).handle, LowMask(BIT_WIDTH)},
                  {OpCode::EVENT_WAIT, group.GetEvent(16).handle, LowMask(BIT_WIDTH)},
                  {OpCode::EVENT_WAIT, group.GetEvent(32).handle, LowMask(2)}});
}

// ---- WaitAllExcept：仅剔除 skipIdx 所在 event 的对应 bit ----
TEST(CcuEventGroupTest, WaitAllExceptClearsSkipBitOnlyInOwningEvent)
{
    mc2_ops_hccl::CcuEventGroup group;
    const Program program = CaptureProgram([&]() {
        group.Init(34);
        group.WaitAllExcept(17); // 17 落在第 1 个 event 的 bit1
    });
    ExpectEventOps(
        program, {{OpCode::EVENT_WAIT, group.GetEvent(0).handle, LowMask(BIT_WIDTH)},
                  {OpCode::EVENT_WAIT, group.GetEvent(16).handle, static_cast<uint16_t>(LowMask(BIT_WIDTH) & ~Bit(1))},
                  {OpCode::EVENT_WAIT, group.GetEvent(32).handle, LowMask(2)}});
}

// ---- WaitAllExcept：skip 后掩码为 0 的 event 不发射 IR ----
TEST(CcuEventGroupTest, WaitAllExceptOmitsFullyClearedEvent)
{
    mc2_ops_hccl::CcuEventGroup group;
    const Program program = CaptureProgram([&]() {
        group.Init(17); // 尾 event 只有 1 个信号
        group.WaitAllExcept(16);
    });
    // 尾 event 掩码 0x0001 & ~0x0001 = 0，应被整体省略
    ExpectEventOps(program, {{OpCode::EVENT_WAIT, group.GetEvent(0).handle, LowMask(BIT_WIDTH)}});
}

// ---- WaitRange：区间与各 event 求交，跨组边界聚合 ----
TEST(CcuEventGroupTest, WaitRangeAcrossEventBoundary)
{
    mc2_ops_hccl::CcuEventGroup group;
    const Program program = CaptureProgram([&]() {
        group.Init(34);
        group.WaitRange(14, 19); // [14,16) 在 e0，[16,19) 在 e1
    });
    ExpectEventOps(
        program, {{OpCode::EVENT_WAIT, group.GetEvent(0).handle, static_cast<uint16_t>(LowMask(2) << 14)},
                  {OpCode::EVENT_WAIT, group.GetEvent(16).handle, LowMask(3)}});
}

TEST(CcuEventGroupTest, WaitRangeInsideSingleEvent)
{
    mc2_ops_hccl::CcuEventGroup group;
    const Program program = CaptureProgram([&]() {
        group.Init(34);
        group.WaitRange(20, 23); // 完整落在第 1 个 event 内
    });
    ExpectEventOps(program, {{OpCode::EVENT_WAIT, group.GetEvent(16).handle, static_cast<uint16_t>(LowMask(3) << 4)}});
}

TEST(CcuEventGroupTest, WaitRangeFullSpanMatchesWaitAll)
{
    mc2_ops_hccl::CcuEventGroup group;
    const Program program = CaptureProgram([&]() {
        group.Init(34);
        group.WaitRange(0, 34); // 默认 skipIdx=UINT32_MAX 不生效
    });
    ExpectEventOps(
        program, {{OpCode::EVENT_WAIT, group.GetEvent(0).handle, LowMask(BIT_WIDTH)},
                  {OpCode::EVENT_WAIT, group.GetEvent(16).handle, LowMask(BIT_WIDTH)},
                  {OpCode::EVENT_WAIT, group.GetEvent(32).handle, LowMask(2)}});
}

// ---- WaitRangeExcept：区间内剔除 skipIdx；skip 不在相交 event 内时不影响掩码 ----
TEST(CcuEventGroupTest, WaitRangeExceptClearsSkipInsideRange)
{
    mc2_ops_hccl::CcuEventGroup group;
    const Program program = CaptureProgram([&]() {
        group.Init(34);
        group.WaitRangeExcept(10, 30, 18); // [10,16) 在 e0；[16,30) 在 e1 且剔除 bit(18-16)
    });
    ExpectEventOps(
        program, {{OpCode::EVENT_WAIT, group.GetEvent(0).handle, static_cast<uint16_t>(LowMask(6) << 10)},
                  {OpCode::EVENT_WAIT, group.GetEvent(16).handle, static_cast<uint16_t>(LowMask(14) & ~Bit(2))}});
}

TEST(CcuEventGroupTest, WaitRangeExceptKeepsMaskWhenSkipOutsideRange)
{
    mc2_ops_hccl::CcuEventGroup group;
    const Program program = CaptureProgram([&]() {
        group.Init(34);
        group.WaitRangeExcept(20, 23, 5); // skip=5 所在 event 与区间无交集
    });
    ExpectEventOps(program, {{OpCode::EVENT_WAIT, group.GetEvent(16).handle, static_cast<uint16_t>(LowMask(3) << 4)}});
}

} // namespace
} // namespace CcuSt
} // namespace HcclSim
