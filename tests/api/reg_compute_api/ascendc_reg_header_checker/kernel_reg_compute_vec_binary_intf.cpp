/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef VERIFY_SINGLE_HEADER
#include "reg_compute/kernel_reg_compute_intf.h"
#else
#include "reg_compute/kernel_reg_compute_vec_binary_intf.h"
#endif

extern "C" __simd_vf__ void vec_binary_test()
{
    AscendC::Reg::RegTensor<float> dstReg, srcReg0, srcReg1;
    AscendC::Reg::RegTensor<int32_t> a, b, c;
    AscendC::Reg::RegTensor<int> shiftReg;
    AscendC::Reg::MaskReg mask, carry;

    // template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
    // __simd_callee__ inline void Add(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask);
    AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);

    // template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
    // __simd_callee__ inline void Sub(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask);
    AscendC::Reg::Sub(dstReg, srcReg0, srcReg1, mask);

    // template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
    // __simd_callee__ inline void Mul(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask);
    AscendC::Reg::Mul(dstReg, srcReg0, srcReg1, mask);

    // template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
    // __simd_callee__ inline void Div(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask);
    AscendC::Reg::Div(dstReg, srcReg0, srcReg1, mask);

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
    // Explicit algorithms must also accept the inferred register element type.
    static constexpr AscendC::Reg::DivSpecificMode divFtzFalse = {
        AscendC::Reg::MaskMergeMode::ZEROING, false, AscendC::DivAlgo::PRECISION_1ULP_FTZ_FALSE};
    static constexpr AscendC::Reg::DivSpecificMode divPreciseFtzFalse = {
        AscendC::Reg::MaskMergeMode::ZEROING, false, AscendC::DivAlgo::PRECISION_0ULP_FTZ_FALSE};
    static constexpr AscendC::Reg::DivSpecificMode divPreciseFtzTrue = {
        AscendC::Reg::MaskMergeMode::ZEROING, false, AscendC::DivAlgo::PRECISION_0ULP_FTZ_TRUE};
    AscendC::Reg::Div<AscendC::Reg::DefaultType, &divFtzFalse>(dstReg, srcReg0, srcReg1, mask);
    AscendC::Reg::Div<AscendC::Reg::DefaultType, &divPreciseFtzFalse>(dstReg, srcReg0, srcReg1, mask);
    AscendC::Reg::Div<AscendC::Reg::DefaultType, &divPreciseFtzTrue>(dstReg, srcReg0, srcReg1, mask);
    AscendC::Reg::RegTensor<half> halfDst, halfSrc0, halfSrc1;
    AscendC::Reg::Div<AscendC::Reg::DefaultType, &divFtzFalse>(halfDst, halfSrc0, halfSrc1, mask);
#endif

    // template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
    // __simd_callee__ inline void Max(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask);
    AscendC::Reg::Max(dstReg, srcReg0, srcReg1, mask);

    // template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
    // __simd_callee__ inline void Min(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask);
    AscendC::Reg::Min(dstReg, srcReg0, srcReg1, mask);

    // template <typename T = DefaultType, typename U = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING,
    // typename S, typename V>
    // __simd_callee__ inline void ShiftLeft(S& dstReg, S& srcReg0, V& srcReg1, MaskReg& mask);
    AscendC::Reg::ShiftLeft(a, b, c, mask);

    // template <typename T = DefaultType, typename U = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING,
    // typename S, typename V>
    // __simd_callee__ inline void ShiftRight(S& dstReg, S& srcReg0, V& srcReg1, MaskReg& mask);
    AscendC::Reg::ShiftRight(a, b, c, mask);

    // template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
    // __simd_callee__ inline void And(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask);
    AscendC::Reg::And(a, b, c, mask);

    // template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
    // __simd_callee__ inline void Or(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask);
    AscendC::Reg::Or(a, b, c, mask);

    // template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
    // __simd_callee__ inline void Xor(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask);
    AscendC::Reg::Xor(a, b, c, mask);

    // template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
    // __simd_callee__ inline void Prelu(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask);
    AscendC::Reg::Prelu(dstReg, srcReg0, srcReg1, mask);

    // template <typename T = DefaultType, typename U>
    // __simd_callee__ inline void Mull(U& dstReg0, U& dstReg1, U& srcReg0, U& srcReg1, MaskReg& mask);
    AscendC::Reg::Mull(a, a, b, c, mask);

    // template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
    // __simd_callee__ inline void MulAddDst(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask);
    AscendC::Reg::MulAddDst(dstReg, srcReg0, srcReg1, mask);

    // template <typename T = DefaultType, typename U>
    // __simd_callee__ inline void AddC(MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& carrySrc, MaskReg&
    // mask);
    AscendC::Reg::AddC(mask, a, b, c, mask, mask);

    // template <typename T = DefaultType, typename U>
    // __simd_callee__ inline void SubC(MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& carrySrc, MaskReg&
    // mask);
    AscendC::Reg::SubC(mask, a, b, c, mask, mask);

    // template <typename T = DefaultType, typename U>
    // __simd_callee__ inline void Add(MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask);
    AscendC::Reg::Add(mask, a, b, c, mask);

    // template <typename T = DefaultType, typename U>
    // __simd_callee__ inline void Sub(MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask);
    AscendC::Reg::Sub(mask, a, b, c, mask);
}
