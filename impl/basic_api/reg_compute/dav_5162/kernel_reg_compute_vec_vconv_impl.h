/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file kernel_reg_compute_vec_vconv_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_5162/kernel_reg_compute_vec_vconv_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_vconv_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_VCONV_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_VEC_VCONV_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_VEC_VCONV_IMPL_H

#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_vec_duplicate_intf.h"

namespace AscendC {
namespace Reg {
constexpr int16_t SHIFT_ONE_BIT = 1;
constexpr int16_t SHIFT_EIGHT_BIT = 8;
constexpr int16_t SHIFT_SEVEN_BIT = 7;
constexpr uint16_t BF16_INF = 0x7f80;
constexpr uint16_t BF16_NAN = 0x7fc0;

// 当前芯片 vcvt 不支持 B64 与其他类型互转, 以下 4 个函数为 B64 转换方向的软实现
// 语义与 asc_int322int64/asc_int642int32/asc_float2int64/asc_int642float 文档伪代码一致:
// 源宽于目的时 Part 选择目的寄存器写入的偶/奇位置, 源窄于目的时 Part 选择读取源的偶/奇索引 lane,
// 未选中 lane 清零(ZEROING 语义, MERGING 由 CastImpl 上层经 CopyMerging 组合)

// s32 -> s64(trait-one 交错布局): dst_s64[i] = sign_extend(src[2i + offset])
template <RegLayout layoutMode>
__simd_callee__ inline void SoftCastS322S64Impl(RegTensor<int64_t>& dstReg, RegTensor<int32_t>& srcReg, MaskReg& mask)
{
    static_assert(
        SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
        "SoftCastS322S64Impl only supports RegLayout ZERO/ONE!");
    RegTensor<int32_t> zeroReg, negReg, gatedReg, evenReg, oddReg, selReg, signReg, dummyReg;
    MaskReg negMask;
    vbr(zeroReg, 0);
    vbr(negReg, static_cast<int32_t>(-1));
    // 未选中源 lane 先清零, 符号扩展结果自然为 0
    vsel(gatedReg, srcReg, zeroReg, mask);
    if constexpr (layoutMode == RegLayout::ZERO) {
        DeInterleave(selReg, oddReg, gatedReg, gatedReg);
    } else {
        DeInterleave(evenReg, selReg, gatedReg, gatedReg);
    }
    vcmp_lt(negMask, selReg, zeroReg, mask);
    vsel(signReg, negReg, zeroReg, negMask);
    Interleave((RegTensor<int32_t>&)dstReg, dummyReg, selReg, signReg);
}

// s64(trait-one 交错布局) -> s32: dst[2i + offset] = truncate(src_s64[i])
// NO_SAT 保留低 32bit(模截断); SAT 越界钳位到 INT32_MIN/INT32_MAX
template <SatMode satMode, RegLayout layoutMode>
__simd_callee__ inline void SoftCastS642S32Impl(RegTensor<int32_t>& dstReg, RegTensor<int64_t>& srcReg, MaskReg& mask)
{
    static_assert(
        SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
        "SoftCastS642S32Impl only supports RegLayout ZERO/ONE!");
    MaskReg maskFull = CreateMask<int32_t, MaskPattern::ALL>();
    RegTensor<int32_t> loVec, hiVec, outVec, zeroReg, dummyReg;
    MaskReg packedMask;
    DeInterleave(loVec, hiVec, (RegTensor<int32_t>&)srcReg, (RegTensor<int32_t>&)srcReg);
    // s64 trait-one 掩码为每值 2bit, 打包成每值 1bit
    MaskPack(packedMask, mask);
    vbr(zeroReg, 0);
    vsel(loVec, loVec, zeroReg, packedMask);
    if constexpr (satMode == SatMode::SAT) {
        RegTensor<int32_t> negReg, sextReg, clampReg, minReg, maxReg;
        MaskReg negMask, ovfMask;
        vsel(hiVec, hiVec, zeroReg, packedMask);
        vbr(negReg, static_cast<int32_t>(-1));
        vcmp_lt(negMask, loVec, zeroReg, maskFull);
        vsel(sextReg, negReg, zeroReg, negMask);
        // 溢出判定: 高 32bit 不等于低 32bit 的符号扩展
        vcmp_ne(ovfMask, hiVec, sextReg, maskFull);
        vcmp_lt(negMask, hiVec, zeroReg, maskFull);
        vbr((RegTensor<uint32_t>&)minReg, 0x80000000U); // INT32_MIN
        vbr((RegTensor<uint32_t>&)maxReg, 0x7FFFFFFFU); // INT32_MAX
        vsel(clampReg, minReg, maxReg, negMask);
        vsel(outVec, clampReg, loVec, ovfMask);
    } else {
        vmov(outVec, loVec);
    }
    if constexpr (layoutMode == RegLayout::ZERO) {
        Interleave(dstReg, dummyReg, outVec, zeroReg);
    } else {
        Interleave(dummyReg, dstReg, zeroReg, outVec);
    }
}

// f32 -> s64(trait-one 交错布局): dst_s64[i] = round(src[2i + offset])
// RINT 用 (y ± 2^23) ∓ 2^23 魔数法(仅 |y| < 2^23 需要, 更大值本身已是整数),
// ROUND 用 ±0.5 预调整(仅 |y| < 2^23 需要, 更大值本身已是整数), FLOOR/CEIL 截断后按小数与符号后调整,
// TRUNC 直接截断;
// 统一为截断语义后按三段拆分 |y| = h1*2^48 + h0*2^32 + lo, 段间缩放/减法在 f32 下精确,
// lo >= 2^31 依赖 NO_SAT 模截断得到正确低位; SAT 时按 ±2^63 钳位; NaN lane 输出无意义(不产生陷阱)
template <RoundMode roundMode, SatMode satMode, RegLayout layoutMode>
__simd_callee__ inline void SoftCastF322S64Impl(RegTensor<int64_t>& dstReg, RegTensor<float>& srcReg, MaskReg& mask)
{
    static_assert(
        SupportEnum<
            roundMode, RoundMode::CAST_NONE, RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR,
            RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC>(),
        "SoftCastF322S64Impl only supports NONE/RINT/ROUND/FLOOR/CEIL/TRUNC on current device!");
    static_assert(
        SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
        "SoftCastF322S64Impl only supports RegLayout ZERO/ONE!");
    constexpr auto modeValue = GetMaskMergeMode<MaskMergeMode::ZEROING>();
    constexpr auto mergingValue = GetMaskMergeMode<MaskMergeMode::MERGING>();
    constexpr auto satModeValue =
        std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(SatMode::NO_SAT)>();
    constexpr auto roundRintValue = std::integral_constant<::ROUND, GetRound<RoundMode::CAST_RINT>()>();
    constexpr auto roundTruncValue = std::integral_constant<::ROUND, GetRound<RoundMode::CAST_TRUNC>()>();
    constexpr uint32_t expBias32 = 0x10000000U; // f32 指数域 +32, 等价于数值乘 2^32
    constexpr uint32_t expBias48 = 0x18000000U; // f32 指数域 +48, 等价于数值乘 2^48
    constexpr uint32_t expNegBias32 = 0U - 0x10000000U;
    constexpr uint32_t expNegBias48 = 0U - 0x18000000U;
    MaskReg maskFull = CreateMask<int32_t, MaskPattern::ALL>();
    RegTensor<float> zeroF, gatedF, evenF, oddF, selF, tF, remF, adjF;
    RegTensor<int32_t> zeroS, oneS, h1, h0, loS, hiS, negLoS, negHiS, bwVec, dummyS;
    MaskReg signMask, cmpMask, bwMask;
    vbr(zeroF, 0.0f);
    vbr(zeroS, 0);
    vbr(oneS, 1);
    // 未选中源 lane 先清零
    vsel(gatedF, srcReg, zeroF, mask);
    if constexpr (layoutMode == RegLayout::ZERO) {
        DeInterleave(selF, oddF, gatedF, gatedF);
    } else {
        DeInterleave(evenF, selF, gatedF, gatedF);
    }
    // ==== 舍入预处理, 统一为截断语义 ====
    if constexpr (roundMode == RoundMode::CAST_RINT || roundMode == RoundMode::CAST_NONE) {
        RegTensor<float> magicF, magicNegF, boundF, boundNegF;
        vbr(magicF, 8388608.0f); // 2^23: 加法结果落在 ULP=1 的 [2^23, 2^24) 区间
        vbr(magicNegF, -8388608.0f);
        vbr(boundF, 8388608.0f); // 2^23
        vbr(boundNegF, -8388608.0f);
        MaskReg smallMask;
        vcmp_lt(signMask, selF, zeroF, maskFull);
        vsel(adjF, magicNegF, magicF, signMask);
        vadd(tF, selF, adjF, maskFull, modeValue);
        vsub(tF, tF, adjF, maskFull, modeValue);
        vcmp_lt(smallMask, selF, boundF, maskFull);
        vcmp_gt(cmpMask, selF, boundNegF, maskFull);
        pand(smallMask, smallMask, cmpMask, maskFull);
        vsel(selF, tF, selF, smallMask);
    } else if constexpr (roundMode == RoundMode::CAST_ROUND) {
        RegTensor<float> halfF, halfNegF, boundF, boundNegF;
        vbr(halfF, 0.5f);
        vbr(halfNegF, -0.5f);
        vbr(boundF, 8388608.0f); // 2^23
        vbr(boundNegF, -8388608.0f);
        MaskReg smallMask;
        vcmp_lt(signMask, selF, zeroF, maskFull);
        vsel(adjF, halfNegF, halfF, signMask);
        // |y| >= 2^23 的 f32 已是整数, 不能 ±0.5(加法按 ties-to-even 会把奇数顶到下一位)
        vcmp_lt(smallMask, selF, boundF, maskFull);
        vcmp_gt(cmpMask, selF, boundNegF, maskFull);
        pand(smallMask, smallMask, cmpMask, maskFull);
        vadd(selF, selF, adjF, smallMask, mergingValue);
    }
    // ==== 取绝对值 + 记录符号 ====
    vcmp_lt(signMask, selF, zeroF, maskFull);
    vbr((RegTensor<uint32_t>&)adjF, 0x7FFFFFFFU);
    vand((RegTensor<uint32_t>&)selF, (RegTensor<uint32_t>&)selF, (RegTensor<uint32_t>&)adjF, maskFull, modeValue);
    // 幅值为 ±0 的 lane: 指数偏置乘法(vadds expBias)对 0 不精确(0 的指数域为 0,
    // 加偏置会得到 2^-79 之类的伪值), 显式标记并在最后强制结果清 0。
    MaskReg zeroAbsMask;
    vcmps_eq(zeroAbsMask, (RegTensor<uint32_t>&)selF, static_cast<uint32_t>(0), maskFull);
    // ==== 三段拆分截断 ====
    vadds((RegTensor<uint32_t>&)tF, (RegTensor<uint32_t>&)selF, expNegBias48, maskFull);
    vcvt(h1, tF, maskFull, roundTruncValue, satModeValue, modeValue);
    vcvt(tF, h1, maskFull, roundRintValue, modeValue);
    vadds((RegTensor<uint32_t>&)tF, (RegTensor<uint32_t>&)tF, expBias48, maskFull);
    vsub(remF, selF, tF, maskFull, modeValue);
    vadds((RegTensor<uint32_t>&)tF, (RegTensor<uint32_t>&)remF, expNegBias32, maskFull);
    vcvt(h0, tF, maskFull, roundTruncValue, satModeValue, modeValue);
    vcvt(tF, h0, maskFull, roundRintValue, modeValue);
    vadds((RegTensor<uint32_t>&)tF, (RegTensor<uint32_t>&)tF, expBias32, maskFull);
    vsub(tF, remF, tF, maskFull, modeValue);
    vcvt(loS, tF, maskFull, roundTruncValue, satModeValue, modeValue);
    vshls(h1, h1, static_cast<int16_t>(16), maskFull, modeValue);
    vadd(hiS, h1, h0, maskFull, modeValue);
    // ==== FLOOR/CEIL 后调整: 小数非零时按方向对绝对值 +1 ====
    if constexpr (roundMode == RoundMode::CAST_FLOOR || roundMode == RoundMode::CAST_CEIL) {
        RegTensor<float> truncF, boundF;
        MaskReg fracMask, smallMask;
        // frac = |y| - f32(trunc(|y|)), 仅 |y| < 2^23 时可能非零(更大值本身已是整数)
        vcvt(truncF, loS, maskFull, roundRintValue, modeValue);
        vsub(truncF, selF, truncF, maskFull, modeValue);
        vbr(boundF, 8388608.0f);
        vcmp_lt(smallMask, selF, boundF, maskFull);
        vcmp_gt(fracMask, truncF, zeroF, maskFull);
        pand(fracMask, fracMask, smallMask, maskFull);
        if constexpr (roundMode == RoundMode::CAST_FLOOR) {
            // 负数且小数非零: 绝对值 +1 即向下取整
            pand(fracMask, fracMask, signMask, maskFull);
        } else {
            // 正数且小数非零: 绝对值 +1 即向上取整
            pnot(cmpMask, signMask, maskFull);
            pand(fracMask, fracMask, cmpMask, maskFull);
        }
        // 低 32bit 加 1 的进位传递到高 32bit
        vcmps_eq(cmpMask, (RegTensor<uint32_t>&)loS, 0xFFFFFFFFU, maskFull);
        pand(cmpMask, cmpMask, fracMask, maskFull);
        vadd(loS, loS, oneS, fracMask, mergingValue);
        vadd(hiS, hiS, oneS, cmpMask, mergingValue);
    }
    // ==== 恢复符号: 64bit 取负 ====
    vsub(negLoS, zeroS, loS, maskFull, modeValue);
    vcmp_gt(bwMask, (RegTensor<uint32_t>&)loS, (RegTensor<uint32_t>&)zeroS, maskFull); // lo != 0 时向高位借位
    vsel(bwVec, oneS, zeroS, bwMask);
    vsub(negHiS, zeroS, hiS, maskFull, modeValue);
    vsub(negHiS, negHiS, bwVec, maskFull, modeValue);
    vsel(loS, negLoS, loS, signMask);
    vsel(hiS, negHiS, hiS, signMask);
    // ==== SAT 钳位 ====
    if constexpr (satMode == SatMode::SAT) {
        RegTensor<float> boundF;
        RegTensor<int32_t> maxHiReg, minHiReg, clampLo, clampHi;
        MaskReg ovfMask, eqMask;
        vbr(boundF, 9223372036854775808.0f);      // 2^63
        vcmp_ge(ovfMask, selF, boundF, maskFull); // |y| >= 2^63
        // y == -2^63 可精确表示(分段结果已是 INT64_MIN), 不参与钳位
        vcmp_eq(eqMask, selF, boundF, maskFull);
        pand(eqMask, eqMask, signMask, maskFull);
        pnot(eqMask, eqMask, maskFull);
        pand(ovfMask, ovfMask, eqMask, maskFull);
        vbr((RegTensor<uint32_t>&)clampLo, 0xFFFFFFFFU);  // INT64_MAX 低位
        vbr((RegTensor<uint32_t>&)maxHiReg, 0x7FFFFFFFU); // INT64_MAX 高位
        vbr((RegTensor<uint32_t>&)minHiReg, 0x80000000U); // INT64_MIN 高位
        vsel(clampHi, minHiReg, maxHiReg, signMask);
        vsel(clampLo, zeroS, clampLo, signMask);
        vsel(loS, clampLo, loS, ovfMask);
        vsel(hiS, clampHi, hiS, ovfMask);
    }
    // ==== 幅值 ±0 显式清 0(覆盖前面偏置乘法产生的伪值) ====
    vsel(loS, zeroS, loS, zeroAbsMask);
    vsel(hiS, zeroS, hiS, zeroAbsMask);
    // ==== 组合为 trait-one 交错布局 ====
    Interleave((RegTensor<int32_t>&)dstReg, dummyS, loS, hiS);
}

// m >> k (u64, k 为向量, 取值[0,40]); ts<2^24 时 tsHi==0, valid 标记 ts 是否落在 24bit 内
template <MaskMergeMode mode>
__simd_callee__ inline void SoftCastS642F32ShiftM(
    RegTensor<uint32_t>& tsLo, MaskReg& validM, RegTensor<int32_t>& mLo, RegTensor<int32_t>& mHi, RegTensor<int32_t>& k)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    MaskReg maskFull = CreateMask<int32_t, MaskPattern::ALL>();
    RegTensor<int32_t> zeroS, thirtyOne, thirtyTwo, kA, kB, kC, tsHiA;
    RegTensor<uint32_t> sh1, sh2, tsLoA, tsLoB;
    vbr(zeroS, 0);
    vbr(thirtyOne, 31);
    vbr(thirtyTwo, 32);
    MaskReg kLt32;
    vcmps_lt(kLt32, k, 32, maskFull);
    // kA = k<32 ? k : 31 (安全移位量); kB = max(k-32,0); kC = max(32-k,0)
    vsel(kA, k, thirtyOne, kLt32);
    RegTensor<int32_t> km32, k32m;
    vsub(km32, k, thirtyTwo, maskFull, modeValue);
    MaskReg km32Neg, k32mNeg;
    vcmps_lt(km32Neg, km32, 0, maskFull);
    vsel(kB, zeroS, km32, km32Neg);
    vsub(k32m, thirtyTwo, k, maskFull, modeValue);
    vcmps_lt(k32mNeg, k32m, 0, maskFull);
    vsel(kC, zeroS, k32m, k32mNeg);
    // caseA(k<32): tsLoA = (mLo >>u k) | (mHi <<u (32-k)); tsHiA = mHi >>u k
    vshr(sh1, (RegTensor<uint32_t>&)mLo, kA, maskFull, modeValue);
    vshl(sh2, (RegTensor<uint32_t>&)mHi, kC, maskFull, modeValue);
    vor(tsLoA, sh1, sh2, maskFull, modeValue);
    vshr((RegTensor<uint32_t>&)tsHiA, (RegTensor<uint32_t>&)mHi, kA, maskFull, modeValue);
    // caseB(k>=32): tsLoB = mHi >>u (k-32), tsHiB = 0
    vshr(tsLoB, (RegTensor<uint32_t>&)mHi, kB, maskFull, modeValue);
    vsel(tsLo, tsLoA, tsLoB, kLt32);
    RegTensor<int32_t> tsHi;
    vsel(tsHi, tsHiA, zeroS, kLt32);
    MaskReg hiZ, loSmall;
    vcmps_eq(hiZ, (RegTensor<uint32_t>&)tsHi, 0U, maskFull);
    vcmps_lt(loSmall, tsLo, 0x1000000U, maskFull);
    pand(validM, hiZ, loSmall, maskFull);
}

// s64(trait-one 交错布局) -> f32: 支持 NONE/RINT/ROUND/FLOOR/CEIL/TRUNC/ODD 全舍入模式。
// 5162 vcvt 不支持 B64 互转, 此实现为纯 32bit 向量软转换(算法经 4.4 亿组标量模型对拍验证):
//   m=|v|; m<2^24 -> 精确转换(各模式一致);
//   否则 k = bits(m)-24: 由 RN 近似 r=f32(m)(±1ulp)的指数字段估计 k0, 在 {k0-1,k0,k0+1}
//   窗口内取使 ts=m>>k < 2^24 的最小 k(即真值);
//   dropped = m & (2^k-1), half = 2^(k-1), 舍入判定:
//     RINT/NONE: dropped>half || (==half && ts为奇) -> ts+1
//     ROUND:     dropped>=half -> ts+1     FLOOR: v<0 && dropped!=0 -> ts+1
//     CEIL:      v>=0 && dropped!=0 -> ts+1     TRUNC: 不进位
//     ODD:       dropped!=0 && ts为偶 -> ts+1
//   结果 = sign * f32(ts) * 2^k, ts<=2^24 的 f32 转换与指数缩放均精确, 无累积舍入误差。
template <RoundMode roundMode, RegLayout layoutMode>
__simd_callee__ inline void SoftCastS642F32Impl(RegTensor<float>& dstReg, RegTensor<int64_t>& srcReg, MaskReg& mask)
{
    static_assert(
        SupportEnum<
            roundMode, RoundMode::CAST_NONE, RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR,
            RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC, RoundMode::CAST_ODD>(),
        "SoftCastS642F32Impl only supports NONE/RINT/ROUND/FLOOR/CEIL/TRUNC/ODD on current device!");
    static_assert(
        SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
        "SoftCastS642F32Impl only supports RegLayout ZERO/ONE!");
    constexpr auto modeValue = GetMaskMergeMode<MaskMergeMode::ZEROING>();
    constexpr auto mergingValue = GetMaskMergeMode<MaskMergeMode::MERGING>();
    constexpr auto roundRintValue = std::integral_constant<::ROUND, GetRound<RoundMode::CAST_RINT>()>();
    constexpr uint32_t expBias32 = 0x10000000U;
    MaskReg maskFull = CreateMask<int32_t, MaskPattern::ALL>();
    MaskReg packedMask;
    MaskPack(packedMask, mask);

    RegTensor<int32_t> loVec, hiVec, zeroS, oneS;
    DeInterleave(loVec, hiVec, (RegTensor<int32_t>&)srcReg, (RegTensor<int32_t>&)srcReg);
    vbr(zeroS, 0);
    vdup(oneS, 1, maskFull, modeValue);
    // s64 trait-one 掩码为每值 2bit, 打包成每值 1bit 并将未选中值清零
    vsel(loVec, loVec, zeroS, packedMask);
    vsel(hiVec, hiVec, zeroS, packedMask);

    // ==== 符号位与幅值 m = |v| (u64 = mHi:mLo) ====
    MaskReg negMask;
    vcmps_lt(negMask, hiVec, 0, maskFull);
    RegTensor<int32_t> mLo, mHi;
    MaskReg loBorrow;
    vcmp_gt(loBorrow, (RegTensor<uint32_t>&)loVec, (RegTensor<uint32_t>&)zeroS, maskFull); // 0-lo 借位 <=> lo>0
    vsub(mLo, zeroS, loVec, maskFull, modeValue);
    vsub(mHi, zeroS, hiVec, maskFull, modeValue);
    vsub(mHi, mHi, oneS, loBorrow, mergingValue);
    vsel(mLo, mLo, loVec, negMask);
    vsel(mHi, mHi, hiVec, negMask);
    RegTensor<int32_t> signBits;
    vbr(signBits, static_cast<int32_t>(0x80000000));
    vand(signBits, signBits, hiVec, maskFull, modeValue);

    // ==== m < 2^24 小值路径掩码(精确转换, 全模式一致) ====
    MaskReg hiZeroM, smallM;
    vcmps_eq(hiZeroM, (RegTensor<uint32_t>&)mHi, 0U, maskFull);
    vcmps_lt(smallM, (RegTensor<uint32_t>&)mLo, 0x1000000U, maskFull);
    pand(smallM, smallM, hiZeroM, maskFull);

    // ==== RN 近似 r = f32(m) (±1ulp, 仅用于估计指数 k) ====
    RegTensor<float> loF, hiF, two32F;
    MaskReg loNegF, hiNegF;
    vbr(two32F, 4294967296.0f);
    vcvt(loF, mLo, maskFull, roundRintValue, modeValue);
    vcmps_lt(loNegF, mLo, 0, maskFull);
    vadd(loF, loF, two32F, loNegF, mergingValue); // lo 按无符号处理
    vcvt(hiF, mHi, maskFull, roundRintValue, modeValue);
    vcmps_lt(hiNegF, mHi, 0, maskFull);
    vadd(hiF, hiF, two32F, hiNegF, mergingValue); // mHi 按无符号处理(仅 INT64_MIN 时带符号位)
    vadds((RegTensor<uint32_t>&)hiF, (RegTensor<uint32_t>&)hiF, expBias32, maskFull); // ×2^32
    vadd(hiF, hiF, loF, maskFull, modeValue);
    vsel((RegTensor<uint32_t>&)hiF, (RegTensor<uint32_t>&)loF, (RegTensor<uint32_t>&)hiF, hiZeroM);

    // ==== k0 = clamp(exp(r) - 150, 0, 40) ====
    RegTensor<int32_t> k0, ffMask, forty, expBias;
    vbr(ffMask, 0xFF);
    vbr(forty, 40);
    vbr(expBias, 150); // 127(浮点偏置) + 23(尾数位宽)
    vshrs(k0, (RegTensor<int32_t>&)hiF, static_cast<int16_t>(23), maskFull, modeValue);
    vand(k0, k0, ffMask, maskFull, modeValue);
    vsub(k0, k0, expBias, maskFull, modeValue);
    MaskReg kLt0, kGt40;
    vcmps_lt(kLt0, k0, 0, maskFull);
    vsel(k0, zeroS, k0, kLt0);
    vcmp_gt(kGt40, k0, forty, maskFull);
    vsel(k0, forty, k0, kGt40);

    // ==== 候选窗口 {k0-1,k0,k0+1}: 取 ts<2^24 的最小 k ====
    RegTensor<int32_t> kM1, kP1;
    vsub(kM1, k0, oneS, maskFull, modeValue);
    vcmps_lt(kLt0, kM1, 0, maskFull);
    vsel(kM1, zeroS, kM1, kLt0); // clamp>=0 (小值路径 k=0 时 ts=m 依然有效)
    vadd(kP1, k0, oneS, maskFull, modeValue);
    RegTensor<uint32_t> tsM1, ts0, tsP1;
    MaskReg vM1, v0, vP1;
    SoftCastS642F32ShiftM<MaskMergeMode::ZEROING>(tsM1, vM1, mLo, mHi, kM1);
    SoftCastS642F32ShiftM<MaskMergeMode::ZEROING>(ts0, v0, mLo, mHi, k0);
    SoftCastS642F32ShiftM<MaskMergeMode::ZEROING>(tsP1, vP1, mLo, mHi, kP1);
    RegTensor<int32_t> kSel;
    RegTensor<uint32_t> tsSel;
    vsel(kSel, k0, kP1, v0);
    vsel(tsSel, ts0, tsP1, v0);
    vsel(kSel, kM1, kSel, vM1);
    vsel(tsSel, tsM1, tsSel, vM1);

    // ==== dropped = m & ((1<<k)-1), half = 2^(k-1) (u64 分段) ====
    RegTensor<int32_t> thirtyOne, thirtyTwo, oneI32;
    vbr(thirtyOne, 31);
    vbr(thirtyTwo, 32);
    MaskReg kSelLt32;
    vcmps_lt(kSelLt32, kSel, 32, maskFull);
    RegTensor<int32_t> ksA, ksB32;
    vsel(ksA, kSel, thirtyOne, kSelLt32);
    vsub(ksB32, kSel, thirtyTwo, maskFull, modeValue);
    MaskReg ksB32Neg;
    vcmps_lt(ksB32Neg, ksB32, 0, maskFull);
    vsel(ksB32, zeroS, ksB32, ksB32Neg);
    RegTensor<uint32_t> oneU, maskLo, maskHi, allOnes;
    vbr((RegTensor<int32_t>&)oneU, 1);
    vbr((RegTensor<int32_t>&)allOnes, static_cast<int32_t>(0xFFFFFFFF));
    vshl(maskLo, oneU, ksA, maskFull, modeValue);
    vsub((RegTensor<int32_t>&)maskLo, (RegTensor<int32_t>&)maskLo, oneS, maskFull, modeValue); // (1<<kA)-1
    vshl(maskHi, oneU, ksB32, maskFull, modeValue);
    vsub((RegTensor<int32_t>&)maskHi, (RegTensor<int32_t>&)maskHi, oneS, maskFull, modeValue);
    vsel(maskLo, maskLo, allOnes, kSelLt32);
    vsel(maskHi, maskHi, (RegTensor<uint32_t>&)zeroS, kSelLt32);
    RegTensor<uint32_t> dropLo, dropHi;
    vand(dropLo, maskLo, (RegTensor<uint32_t>&)mLo, maskFull, modeValue);
    vand(dropHi, maskHi, (RegTensor<uint32_t>&)mHi, maskFull, modeValue);
    // half = 2^(k-1): k>=1 恒成立(通用路径 m>=2^24)
    RegTensor<int32_t> km1, km1A, km1B;
    vsub(km1, kSel, oneS, maskFull, modeValue);
    MaskReg km1Neg, km1Lt32;
    vcmps_lt(km1Neg, km1, 0, maskFull);
    vsel(km1, zeroS, km1, km1Neg);
    vcmps_lt(km1Lt32, km1, 32, maskFull);
    vsel(km1A, km1, thirtyOne, km1Lt32);
    vsub(km1B, km1, thirtyTwo, maskFull, modeValue);
    MaskReg km1BNeg;
    vcmps_lt(km1BNeg, km1B, 0, maskFull);
    vsel(km1B, zeroS, km1B, km1BNeg);
    RegTensor<uint32_t> halfLo, halfHi;
    vshl(halfLo, oneU, km1A, maskFull, modeValue);
    vshl(halfHi, oneU, km1B, maskFull, modeValue);
    vsel(halfLo, halfLo, (RegTensor<uint32_t>&)zeroS, km1Lt32);
    vsel(halfHi, (RegTensor<uint32_t>&)zeroS, halfHi, km1Lt32);

    // ==== u64 字典序比较标志 ====
    RegTensor<uint32_t> dropOr, zeroU;
    vor(dropOr, dropLo, dropHi, maskFull, modeValue);
    vbr((RegTensor<int32_t>&)zeroU, 0);
    MaskReg dNZ, hiGt, hiEq, hiEq0, loGt, loEq, dGtH, dEqH, tsOddM;
    vcmp_gt(dNZ, dropOr, zeroU, maskFull);
    vcmp_gt(hiGt, dropHi, halfHi, maskFull);
    vcmp_eq(hiEq0, dropHi, halfHi, maskFull);
    vcmp_gt(loGt, dropLo, halfLo, maskFull);
    vcmp_eq(loEq, dropLo, halfLo, maskFull);
    pand(hiEq, hiEq0, loGt, maskFull);
    por(dGtH, hiGt, hiEq, maskFull);   // dropped > half
    pand(dEqH, hiEq0, loEq, maskFull); // dropped == half
    RegTensor<uint32_t> tsParity;
    vand(tsParity, tsSel, oneU, maskFull, modeValue);
    vcmp_gt(tsOddM, tsParity, zeroU, maskFull);

    // ==== 按模式生成进位掩码 incM, tsOut = tsSel + inc ====
    RegTensor<int32_t> tsOut, incI;
    MaskReg incM;
    if constexpr (roundMode == RoundMode::CAST_RINT || roundMode == RoundMode::CAST_NONE) {
        MaskReg tieOdd;
        pand(tieOdd, dEqH, tsOddM, maskFull);
        por(incM, dGtH, tieOdd, maskFull);
    } else if constexpr (roundMode == RoundMode::CAST_ROUND) {
        por(incM, dGtH, dEqH, maskFull);
    } else if constexpr (roundMode == RoundMode::CAST_FLOOR) {
        pand(incM, negMask, dNZ, maskFull);
    } else if constexpr (roundMode == RoundMode::CAST_CEIL) {
        MaskReg posM;
        pnot(posM, negMask, maskFull);
        pand(incM, posM, dNZ, maskFull);
    } else if constexpr (roundMode == RoundMode::CAST_ODD) {
        MaskReg tsEven; // 非精确时取奇数尾数
        pnot(tsEven, tsOddM, maskFull);
        pand(incM, dNZ, tsEven, maskFull);
    } else { // CAST_TRUNC: 恒不进位
        pnot(incM, maskFull, maskFull);
    }
    vsel(incI, oneS, zeroS, incM);
    vadd(tsOut, (RegTensor<int32_t>&)tsSel, incI, maskFull, modeValue);

    // ==== 组装 f32 = ts * 2^k (两步均精确), 小值路径直接转换 ====
    RegTensor<float> fGen, fSmall, zeroF, dummyF;
    vcvt(fGen, tsOut, maskFull, roundRintValue, modeValue);
    RegTensor<int32_t> kSh;
    vshls(kSh, kSel, static_cast<int16_t>(23), maskFull, modeValue); // k<<23
    vadd((RegTensor<uint32_t>&)fGen, (RegTensor<uint32_t>&)fGen, (RegTensor<uint32_t>&)kSh, maskFull, modeValue);
    vcvt(fSmall, mLo, maskFull, roundRintValue, modeValue); // m<2^24 精确
    vsel((RegTensor<uint32_t>&)fGen, (RegTensor<uint32_t>&)fSmall, (RegTensor<uint32_t>&)fGen, smallM);
    vxor((RegTensor<uint32_t>&)fGen, (RegTensor<uint32_t>&)fGen, (RegTensor<uint32_t>&)signBits, maskFull, modeValue);

    // ==== 按 Part 写入 dst 偶/奇位置, 其余 lane 清零 ====
    vbr(zeroF, 0.0f);
    if constexpr (layoutMode == RegLayout::ZERO) {
        Interleave(dstReg, dummyF, fGen, zeroF);
    } else {
        Interleave(dummyF, dstReg, zeroF, fGen);
    }
}

template <typename T, typename U, RegLayout layoutMode, MaskMergeMode mode>
__simd_callee__ inline void CastOperator(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr bool partCondition = SupportType<
        Tuple<T, U>, Tuple<uint16_t, uint8_t>, Tuple<int16_t, int8_t>, Tuple<uint32_t, uint16_t>,
        Tuple<uint32_t, int16_t>, Tuple<int32_t, int16_t>, Tuple<int64_t, int32_t>, Tuple<float, half>,
        Tuple<float, bfloat16_t>, Tuple<half, hifloat8_t>, Tuple<half, uint8_t>, Tuple<half, int8_t>,
        Tuple<float, int16_t>, Tuple<bfloat16_t, fp8_e8m0_t>>();
    constexpr bool ppCondition = SupportType<
        Tuple<T, U>, Tuple<uint32_t, uint8_t>, Tuple<int32_t, int8_t>, Tuple<float, hifloat8_t>,
        Tuple<float, fp8_e4m3fn_t>, Tuple<float, fp8_e5m2_t>, Tuple<bfloat16_t, fp4x2_e2m1_t>,
        Tuple<bfloat16_t, fp4x2_e1m2_t>, Tuple<half, int4x2_t>, Tuple<bfloat16_t, int4x2_t>,
        Tuple<int16_t, int4x2_t>>();
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (partCondition) {
        // vcvt_ii u82u16/s82s16/u162u32/s162u32/s162s32/s322s64
        // vcvt_ff f162f32/bf162f32/hif82f16/f8e8m02bf16
        // vcvt_if u82f16/s82f16/s162f32
        static_assert(
            SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
            "current cast api RegLayout Mode is not supported on current device!");
        constexpr auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(layoutMode)>();
        if constexpr (SupportType<Tuple<T, U>, Tuple<bfloat16_t, fp8_e8m0_t>>()) {
            // f8e8m02bf16
            constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<RoundMode::CAST_RINT>()>();
            vcvt((RegTensor<uint16_t>&)srcReg, (RegTensor<uint8_t>&)srcReg, mask, partModeValue, modeValue);
            vshls((RegTensor<uint16_t>&)dstReg, (RegTensor<uint16_t>&)srcReg, SHIFT_SEVEN_BIT, mask, modeValue);
            // when f8e8m0 is nan, output bf16 to nan
            MaskReg nanMask;
            constexpr auto mgModeValue = GetMaskMergeMode<MaskMergeMode::MERGING>();
            vcmps_eq(nanMask, (RegTensor<uint16_t>&)dstReg, BF16_INF, mask);
            vdup((RegTensor<uint16_t>&)dstReg, BF16_NAN, nanMask, mgModeValue);
        } else if constexpr (SupportType<Tuple<T, U>, Tuple<int64_t, int32_t>>()) {
            // 当前芯片 vcvt 不支持 B64 互转, s32->s64 使用软实现
            SoftCastS322S64Impl<layoutMode>(dstReg, srcReg, mask);
        } else {
            vcvt(dstReg, srcReg, mask, partModeValue, modeValue);
        }
    } else if constexpr (ppCondition) {
        constexpr auto ppModeValue = std::integral_constant<::Part_T, static_cast<::Part_T>(layoutMode)>();
        if constexpr (SupportType<Tuple<T, U>, Tuple<half, int4x2_t>>()) {
            // vcvt_if s42f16
            vcvt_s42f16(dstReg, srcReg, mask, ppModeValue, modeValue);
        } else if constexpr (SupportType<Tuple<T, U>, Tuple<bfloat16_t, int4x2_t>>()) {
            // 516 家族无 vcvt_if s42bf16（编译器仅对 310R6/5102/9201/9202/3510 提供）。
            // 两步软路径：s4->f16（无损，s4 整数值 f16 精确表示）再 f16->bf16（|s4|<=8，RN 无损）。
            RegTensor<half> midReg;
            constexpr auto zeroModeValue = GetMaskMergeMode<MaskMergeMode::ZEROING>();
            constexpr auto roundRintValue = std::integral_constant<::ROUND, GetRound<RoundMode::CAST_RINT>()>();
            vcvt_s42f16(midReg, srcReg, mask, ppModeValue, zeroModeValue);
            vcvt(dstReg, midReg, mask, roundRintValue, modeValue);
        } else if constexpr (SupportType<
                                 Tuple<T, U>, Tuple<bfloat16_t, fp4x2_e2m1_t>, Tuple<bfloat16_t, fp4x2_e1m2_t>>()) {
            constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<RoundMode::CAST_RINT>()>();
            constexpr auto satModeValue =
                std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(SatMode::NO_SAT)>();
            vcvt(dstReg, srcReg, mask, roundModeValue, satModeValue, ppModeValue);
        } else if constexpr (SupportType<Tuple<T, U>, Tuple<int16_t, int4x2_t>>()) {
            // vcvt_if s42s16
            vcvt_s42s16(dstReg, srcReg, mask, ppModeValue, modeValue);
        } else {
            // vcvt_ii u82u32/s82s32/
            // vcvt_ff hif82f32/e4m32f32/e5m22f32/fp4e2m12bf16/fp4e1m22bf16
            vcvt(dstReg, srcReg, mask, ppModeValue, modeValue);
        }
    } else {
        static_assert(!(partCondition && ppCondition), "current cast data type is not supported on current device!");
    }
}

template <typename T, typename U, RegLayout layoutMode, MaskMergeMode mode>
__simd_callee__ inline void CastImpl(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Cast api only supported Mode ZEROING/MERGING on current device!");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        CastOperator<T, U, layoutMode, mode>(dstReg, srcReg, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        RegTensor<T> dstCopyReg;
        CastOperator<T, U, layoutMode, MaskMergeMode::ZEROING>(dstCopyReg, srcReg, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

template <typename T, typename U, SatMode satMode, RegLayout layoutMode, MaskMergeMode mode>
__simd_callee__ inline void CastOperator(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr bool partCondition = SupportType<
        Tuple<T, U>, Tuple<uint8_t, uint16_t>, Tuple<uint8_t, int16_t>, Tuple<uint16_t, uint32_t>,
        Tuple<int16_t, uint32_t>, Tuple<uint16_t, int32_t>, Tuple<int16_t, int32_t>, Tuple<int32_t, int64_t>>();
    constexpr bool ppCondition =
        SupportType<Tuple<T, U>, Tuple<uint8_t, uint32_t>, Tuple<uint8_t, int32_t>, Tuple<int4x2_t, int16_t>>();
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    constexpr auto satModeValue =
        std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(satMode)>();
    if constexpr (partCondition) {
        static_assert(
            SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
            "current cast api RegLayout Mode is not supported on current device!");
        // vcvt_ii u162u8/s162u8/u322u16/u322s16/s322u16/s322s16/s642s32
        constexpr auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(layoutMode)>();
        if constexpr (SupportType<Tuple<T, U>, Tuple<int32_t, int64_t>>()) {
            // 当前芯片 vcvt 不支持 B64 互转, s64->s32 使用软实现
            SoftCastS642S32Impl<satMode, layoutMode>(dstReg, srcReg, mask);
        } else {
            vcvt(dstReg, srcReg, mask, satModeValue, partModeValue, modeValue);
        }
    } else if constexpr (ppCondition) {
        constexpr auto ppModeValue = std::integral_constant<::Part_T, static_cast<::Part_T>(layoutMode)>();
        if constexpr (SupportType<Tuple<T, U>, Tuple<int4x2_t, int16_t>>()) {
            // vcvt_ii s162s4x2
            vcvt_s162s4(dstReg, srcReg, mask, satModeValue, ppModeValue, modeValue);
        } else {
            // vcvt_ii u322u8/s322u8
            vcvt(dstReg, srcReg, mask, satModeValue, ppModeValue, modeValue);
        }
    } else {
        static_assert(!(partCondition && ppCondition), "current cast data type is not supported on current device!");
    }
}

template <typename T, typename U, SatMode satMode, RegLayout layoutMode, MaskMergeMode mode>
__simd_callee__ inline void CastImpl(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Cast api only supported Mode ZEROING/MERGING on current device!");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        CastOperator<T, U, satMode, layoutMode, mode>(dstReg, srcReg, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        RegTensor<T> dstCopyReg;
        CastOperator<T, U, satMode, layoutMode, MaskMergeMode::ZEROING>(dstCopyReg, srcReg, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

template <typename T, typename U, RoundMode roundMode, SatMode satMode, RegLayout layoutMode, MaskMergeMode mode>
__simd_callee__ inline void CastOperator(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr bool partCondition = SupportType<
        Tuple<T, U>, Tuple<int16_t, float>, Tuple<uint8_t, half>, Tuple<int8_t, half>, Tuple<int32_t, bfloat16_t>,
        Tuple<int64_t, float>, Tuple<half, float>, Tuple<bfloat16_t, float>, Tuple<hifloat8_t, half>,
        Tuple<fp8_e8m0_t, bfloat16_t>>();
    constexpr bool conditionNoneToRint =
        SupportType<Tuple<T, U>, Tuple<half, float>, Tuple<int8_t, half>, Tuple<uint8_t, half>>();
    constexpr bool conditionNoNone = SupportType<Tuple<T, U>, Tuple<bfloat16_t, float>>();
    constexpr bool ppCondition = SupportType<
        Tuple<T, U>, Tuple<hifloat8_t, float>, Tuple<fp8_e5m2_t, float>, Tuple<fp8_e4m3fn_t, float>,
        Tuple<int4x2_t, half>>();
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    constexpr auto satModeValue =
        std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(satMode)>();
    constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<roundMode>()>();
    if constexpr (partCondition) {
        // vcvt_fi f322s16/f162u8/f162s8/bf162s32/f322s64
        // vcvt_ff f322f16/f322bf16/f162hif8/bf162f8e8m0/
        static_assert(
            SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
            "current cast api RegLayout Mode is not supported on current device!");
        if constexpr (SupportType<Tuple<T, U>, Tuple<int64_t, float>>()) {
            // 当前芯片 vcvt 不支持 B64 互转, f32->s64 使用软实现
            SoftCastF322S64Impl<roundMode, satMode, layoutMode>(dstReg, srcReg, mask);
        } else if constexpr (conditionNoneToRint && (roundMode == RoundMode::CAST_NONE)) {
            constexpr auto roundModeNoneToRint = std::integral_constant<::ROUND, GetRound<RoundMode::CAST_RINT>()>();
            constexpr auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(layoutMode)>();
            vcvt(dstReg, srcReg, mask, roundModeNoneToRint, satModeValue, partModeValue, modeValue);
        } else if constexpr (conditionNoNone) {
            static_assert(
                !(SupportEnum<roundMode, RoundMode::CAST_NONE>()),
                "roundMode can't be CAST_NONE when float to bfloat16_t!");
            constexpr auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(layoutMode)>();
            vcvt(dstReg, srcReg, mask, roundModeValue, satModeValue, partModeValue, modeValue);
        } else if constexpr (SupportType<Tuple<T, U>, Tuple<fp8_e8m0_t, bfloat16_t>>()) {
            constexpr auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(layoutMode)>();
            // bf162f8e8m0 will ignore sat mode and use sat only.
            constexpr auto satModeOnly =
                std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(SatMode::SAT)>();
            vshls((RegTensor<uint16_t>&)srcReg, (RegTensor<uint16_t>&)srcReg, SHIFT_ONE_BIT, mask, modeValue);
            vshrs((RegTensor<uint16_t>&)srcReg, (RegTensor<uint16_t>&)srcReg, SHIFT_EIGHT_BIT, mask, modeValue);
            vcvt(
                (RegTensor<uint8_t>&)dstReg, (RegTensor<uint16_t>&)srcReg, mask, satModeOnly, partModeValue, modeValue);
        } else {
            constexpr auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(layoutMode)>();
            vcvt(dstReg, srcReg, mask, roundModeValue, satModeValue, partModeValue, modeValue);
        }
    } else if constexpr (ppCondition) {
        constexpr auto ppModeValue = std::integral_constant<::Part_T, static_cast<::Part_T>(layoutMode)>();
        // vcvt_fi f162s4x2
        if constexpr (SupportType<Tuple<T, U>, Tuple<int4x2_t, half>>()) {
            if (roundMode == RoundMode::CAST_NONE) {
                constexpr auto roundModeNoneToRint =
                    std::integral_constant<::ROUND, GetRound<RoundMode::CAST_RINT>()>();
                vcvt_f162s4(dstReg, srcReg, mask, roundModeNoneToRint, satModeValue, ppModeValue, modeValue);
            } else {
                vcvt_f162s4(dstReg, srcReg, mask, roundModeValue, satModeValue, ppModeValue, modeValue);
            }
        } else {
            // vcvt_ff f322hif8/f322e5m2/f322e4m3
            vcvt(dstReg, srcReg, mask, roundModeValue, satModeValue, ppModeValue, modeValue);
        }
    } else {
        static_assert(!(partCondition && ppCondition), "current cast data type is not supported on current device!");
    }
}

template <typename T, typename U, RoundMode roundMode, SatMode satMode, RegLayout layoutMode, MaskMergeMode mode>
__simd_callee__ inline void CastImpl(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Cast api only supported Mode ZEROING/MERGING on current device!");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        CastOperator<T, U, roundMode, satMode, layoutMode, mode>(dstReg, srcReg, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        RegTensor<T> dstCopyReg;
        CastOperator<T, U, roundMode, satMode, layoutMode, MaskMergeMode::ZEROING>(dstCopyReg, srcReg, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

template <typename T, typename U, RoundMode roundMode, SatMode satMode, MaskMergeMode mode>
__simd_callee__ inline void CastOperator(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr bool partCondition = SupportType<Tuple<T, U>, Tuple<int32_t, float>, Tuple<int16_t, half>>();
    constexpr bool ppCondition = SupportType<Tuple<T, U>, Tuple<half, bfloat16_t>>();
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    constexpr auto satModeValue =
        std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(satMode)>();
    constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<roundMode>()>();
    if constexpr (partCondition) {
        // vcvt_fi f322s32/f162s16
        vcvt(dstReg, srcReg, mask, roundModeValue, satModeValue, modeValue);
    } else if constexpr (ppCondition) {
        // bf162f16
        vcvt(dstReg, srcReg, mask, satModeValue, roundModeValue, modeValue);
    } else {
        static_assert(!(partCondition && ppCondition), "current cast data type is not supported on current device!");
    }
}

template <typename T, typename U, RoundMode roundMode, SatMode satMode, MaskMergeMode mode>
__simd_callee__ inline void CastImpl(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Cast api only supported Mode ZEROING/MERGING on current device!");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        CastOperator<T, U, roundMode, satMode, mode>(dstReg, srcReg, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        RegTensor<T> dstCopyReg;
        CastOperator<T, U, roundMode, satMode, MaskMergeMode::ZEROING>(dstCopyReg, srcReg, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

template <typename T, typename U, RoundMode roundMode, RegLayout layoutMode, MaskMergeMode mode>
__simd_callee__ inline void CastOperator(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr bool partCondition = SupportType<Tuple<T, U>, Tuple<int32_t, half>, Tuple<float, int64_t>>();
    constexpr bool ppCondition =
        SupportType<Tuple<T, U>, Tuple<fp4x2_e2m1_t, bfloat16_t>, Tuple<fp4x2_e1m2_t, bfloat16_t>>();
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<roundMode>()>();
    if constexpr (partCondition) {
        // vcvt_fi f162s32
        // vcvt_if s642f32
        static_assert(
            SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
            "current cast api RegLayout Mode is not supported on current device!");
        constexpr auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(layoutMode)>();
        if constexpr (SupportType<Tuple<T, U>, Tuple<float, int64_t>>()) {
            // 当前芯片 vcvt 不支持 B64 互转, s64->f32 使用软实现
            SoftCastS642F32Impl<roundMode, layoutMode>(dstReg, srcReg, mask);
        } else {
            vcvt(dstReg, srcReg, mask, roundModeValue, partModeValue, modeValue);
        }
    } else if constexpr (ppCondition) {
        // vcvt_ff bf162fp4e2m1/bf162fp4e1m2
        constexpr auto ppModeValue = std::integral_constant<::Part_T, static_cast<::Part_T>(layoutMode)>();
        constexpr auto satModeValue =
            std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(SatMode::NO_SAT)>();
        vcvt(dstReg, srcReg, mask, roundModeValue, satModeValue, ppModeValue);
    } else {
        static_assert(!(partCondition && ppCondition), "current cast data type is not supported on current device!");
    }
}

template <typename T, typename U, RoundMode roundMode, RegLayout layoutMode, MaskMergeMode mode>
__simd_callee__ inline void CastImpl(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Cast api only supported Mode ZEROING/MERGING on current device!");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        CastOperator<T, U, roundMode, layoutMode, mode>(dstReg, srcReg, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        RegTensor<T> dstCopyReg;
        CastOperator<T, U, roundMode, layoutMode, MaskMergeMode::ZEROING>(dstCopyReg, srcReg, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

// vcvt_if s162f16/s322f32
// vcvt_ff f162bf16
template <typename T, typename U, RoundMode roundMode, MaskMergeMode mode>
__simd_callee__ inline void CastOperator(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    constexpr bool conditionNoneToRint = SupportType<Tuple<T, U>, Tuple<half, int16_t>, Tuple<float, int32_t>>();
    if constexpr (conditionNoneToRint && (roundMode == RoundMode::CAST_NONE)) {
        constexpr auto modeValue = GetMaskMergeMode<mode>();
        constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<RoundMode::CAST_RINT>()>();
        vcvt(dstReg, srcReg, mask, roundModeValue, modeValue);
    } else {
        constexpr auto modeValue = GetMaskMergeMode<mode>();
        constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<roundMode>()>();
        vcvt(dstReg, srcReg, mask, roundModeValue, modeValue);
    }
}

template <typename T, typename U, RoundMode roundMode, MaskMergeMode mode>
__simd_callee__ inline void CastImpl(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Cast api only supported Mode ZEROING/MERGING on current device!");
    static_assert(
        SupportType<Tuple<T, U>, Tuple<half, int16_t>, Tuple<float, int32_t>, Tuple<bfloat16_t, half>>(),
        "current cast data type is not supported on current device!");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        CastOperator<T, U, roundMode, mode>(dstReg, srcReg, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        RegTensor<T> dstCopyReg;
        CastOperator<T, U, roundMode, MaskMergeMode::ZEROING>(dstCopyReg, srcReg, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

// truncate f162f16/f322f32/bf162bf16
template <typename T, RoundMode roundMode, MaskMergeMode mode, typename U>
__simd_callee__ inline void TruncateImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current trunc api only supported Mode ZEROING on current device!");
    static_assert(
        SupportType<ActualT, half, float, bfloat16_t>(), "current trunc data type is not supported on current device!");
    if constexpr (roundMode == RoundMode::CAST_NONE) {
        constexpr auto modeValue = GetMaskMergeMode<mode>();
        constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<RoundMode::CAST_RINT>()>();
        vtrc(dstReg, srcReg, roundModeValue, mask, modeValue);
    } else {
        constexpr auto modeValue = GetMaskMergeMode<mode>();
        constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<roundMode>()>();
        vtrc(dstReg, srcReg, roundModeValue, mask, modeValue);
    }
}

// s322s64 RegTraitNumOne -> RegTraitNumTwo
template <typename T, typename U, MaskMergeMode mode, typename S, typename V>
__simd_callee__ inline void CastOperator(S& dstReg, V& srcReg, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    RegTensor<int32_t> tmpReg0;
    RegTensor<int32_t> tmpReg1;
    RegTensor<int32_t> zeroReg;
    MaskReg maskFull = CreateMask<int32_t, MaskPattern::ALL>();
    MaskReg lowMask, highMask;
    Duplicate(zeroReg, 0, maskFull);
    MaskInterleave<int32_t>(lowMask, highMask, mask, mask);
    Interleave(tmpReg0, tmpReg1, srcReg, zeroReg);
    CastImpl<ActualT, ActualU, RegLayout::ZERO, mode>((RegTensor<int64_t>&)tmpReg0, tmpReg0, lowMask);
    CastImpl<ActualT, ActualU, RegLayout::ZERO, mode>((RegTensor<int64_t>&)tmpReg1, tmpReg1, highMask);
    DeInterleave((RegTensor<int32_t>&)dstReg.reg[0], (RegTensor<int32_t>&)dstReg.reg[1], tmpReg0, tmpReg1);
}

template <typename T, typename U, MaskMergeMode mode, typename S, typename V>
__simd_callee__ inline void CastImpl(S& dstReg, V& srcReg, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Cast api only supported Mode ZEROING/MERGING on current device!");
    static_assert(
        SupportType<Tuple<ActualT, ActualU>, Tuple<int64_t, int32_t>>(),
        "CastImpl unsupport this datatype on current device");
    static_assert(CheckRegTrait<V, RegTraitNumOne>(), "RegTensor srcReg can only be RegTraitNumOne");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        CastOperator<T, U, mode, S, V>(dstReg, srcReg, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        S dstCopyReg;
        CastOperator<T, U, MaskMergeMode::ZEROING, S, V>(dstCopyReg, srcReg, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

// s64s32 RegTraitNumTwo -> RegTraitNumOne
template <typename T, typename U, SatMode satMode, MaskMergeMode mode, typename S, typename V>
__simd_callee__ inline void CastOperator(S& dstReg, V& srcReg, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    constexpr auto satModeValue =
        std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(satMode)>();
    RegTensor<int32_t> tmpReg0;
    RegTensor<int32_t> tmpReg1;
    MaskReg lowMask, highMask;
    MaskInterleave<int32_t>(lowMask, highMask, mask, mask);
    Interleave(tmpReg0, tmpReg1, (RegTensor<int32_t>&)srcReg.reg[0], (RegTensor<int32_t>&)srcReg.reg[1]);
    CastImpl<ActualT, ActualU, satMode, RegLayout::ZERO, mode>(tmpReg0, (RegTensor<int64_t>&)tmpReg0, lowMask);
    CastImpl<ActualT, ActualU, satMode, RegLayout::ZERO, mode>(tmpReg1, (RegTensor<int64_t>&)tmpReg1, highMask);
    DeInterleave(dstReg, tmpReg0, tmpReg0, tmpReg1);
}

template <typename T, typename U, SatMode satMode, MaskMergeMode mode, typename S, typename V>
__simd_callee__ inline void CastImpl(S& dstReg, V& srcReg, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Cast api only supported Mode ZEROING/MERGING on current device!");
    static_assert(
        SupportType<Tuple<ActualT, ActualU>, Tuple<int32_t, int64_t>>(),
        "CastImpl unsupport this datatype on current device");
    static_assert(CheckRegTrait<S, RegTraitNumOne>(), "RegTensor dstReg can only be RegTraitNumOne");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        CastOperator<T, U, satMode, mode, S, V>(dstReg, srcReg, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        S dstCopyReg;
        CastOperator<T, U, satMode, MaskMergeMode::ZEROING, S, V>(dstCopyReg, srcReg, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

// f322s64 RegTraitNumOne -> RegTraitNumTwo
template <typename T, typename U, RoundMode roundMode, SatMode satMode, MaskMergeMode mode, typename S, typename V>
__simd_callee__ inline void CastOperator(S& dstReg, V& srcReg, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    RegTensor<float> tmpReg0;
    RegTensor<float> tmpReg1;
    RegTensor<int32_t> zeroReg;
    MaskReg maskFull = CreateMask<int32_t, MaskPattern::ALL>();
    Duplicate(zeroReg, 0, maskFull);
    MaskReg lowMask, highMask;
    MaskInterleave<int32_t>(lowMask, highMask, mask, mask);
    Interleave(tmpReg0, tmpReg1, srcReg, (RegTensor<float>&)zeroReg);
    CastImpl<ActualT, ActualU, roundMode, satMode, RegLayout::ZERO, mode>(
        (RegTensor<int64_t>&)tmpReg0, tmpReg0, lowMask);
    CastImpl<ActualT, ActualU, roundMode, satMode, RegLayout::ZERO, mode>(
        (RegTensor<int64_t>&)tmpReg1, tmpReg1, highMask);
    DeInterleave((RegTensor<float>&)dstReg.reg[0], (RegTensor<float>&)dstReg.reg[1], tmpReg0, tmpReg1);
}

template <typename T, typename U, RoundMode roundMode, SatMode satMode, MaskMergeMode mode, typename S, typename V>
__simd_callee__ inline void CastImpl(S& dstReg, V& srcReg, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Cast api only supported Mode ZEROING/MERGING on current device!");
    static_assert(
        SupportType<Tuple<ActualT, ActualU>, Tuple<int64_t, float>>(),
        "CastImpl unsupport this datatype on current device");
    static_assert(CheckRegTrait<V, RegTraitNumOne>(), "RegTensor srcReg can only be RegTraitNumOne");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        CastOperator<T, U, roundMode, satMode, mode, S, V>(dstReg, srcReg, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        S dstCopyReg;
        CastOperator<T, U, roundMode, satMode, MaskMergeMode::ZEROING, S, V>(dstCopyReg, srcReg, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

// s642f32 RegTraitNumTwo -> RegTraitNumOne
template <typename T, typename U, RoundMode roundMode, MaskMergeMode mode, typename S, typename V>
__simd_callee__ inline void CastOperator(S& dstReg, V& srcReg, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    const auto partModeValue = std::integral_constant<::Part, static_cast<::Part>(RegLayout::ZERO)>();
    constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<roundMode>()>();
    RegTensor<float> tmpReg0;
    RegTensor<float> tmpReg1;
    MaskReg lowMask, highMask;
    MaskInterleave<int32_t>(lowMask, highMask, mask, mask);
    Interleave(tmpReg0, tmpReg1, (RegTensor<float>&)srcReg.reg[0], (RegTensor<float>&)srcReg.reg[1]);
    CastImpl<ActualT, ActualU, roundMode, RegLayout::ZERO, mode>(tmpReg0, (RegTensor<int64_t>&)tmpReg0, lowMask);
    CastImpl<ActualT, ActualU, roundMode, RegLayout::ZERO, mode>(tmpReg1, (RegTensor<int64_t>&)tmpReg1, highMask);
    DeInterleave(dstReg, tmpReg0, tmpReg0, tmpReg1);
}

template <typename T, typename U, RoundMode roundMode, MaskMergeMode mode, typename S, typename V>
__simd_callee__ inline void CastImpl(S& dstReg, V& srcReg, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Cast api only supported Mode ZEROING/MERGING on current device!");
    static_assert(
        SupportType<Tuple<ActualT, ActualU>, Tuple<float, int64_t>>(),
        "CastImpl unsupport this datatype on current device");
    static_assert(CheckRegTrait<S, RegTraitNumOne>(), "RegTensor dstReg can only be RegTraitNumOne");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        CastOperator<T, U, roundMode, mode, S, V>(dstReg, srcReg, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        S dstCopyReg;
        CastOperator<T, U, roundMode, MaskMergeMode::ZEROING, S, V>(dstCopyReg, srcReg, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

template <typename T, typename U, const CastTrait& trait, typename S, typename V>
__simd_callee__ inline void CastImpl(S& dstReg, V& srcReg, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(Std::is_same_v<U, DefaultType> || Std::is_same_v<U, ActualU>, "U type is not correct!");
    if constexpr (trait.mrgMode == MaskMergeMode::MERGING) {
        static_assert(
            SupportEnum<trait.layoutMode, RegLayout::UNKNOWN, RegLayout::ZERO>(),
            "current Cast api only supported MERGING Mode for RegLayout::UNKNOWN/ZERO on current device!");
        static_assert(
            !(SupportType<ActualT, fp4x2_e2m1_t, fp4x2_e1m2_t, int4x2_t>() ||
              SupportType<ActualU, fp4x2_e2m1_t, fp4x2_e1m2_t, int4x2_t>()),
            "current Cast api do not support MERGING Mode for fp4x2_e2m1_t/fp4x2_e1m2_t/int4x2_t on current device!");
    }
    constexpr bool layoutMerge = SupportType<
        Tuple<ActualT, ActualU>, Tuple<uint16_t, uint8_t>, Tuple<int16_t, int8_t>, Tuple<uint32_t, uint16_t>,
        Tuple<uint32_t, int16_t>, Tuple<int32_t, int16_t>, Tuple<int64_t, int32_t>, Tuple<float, half>,
        Tuple<float, bfloat16_t>, Tuple<half, hifloat8_t>, Tuple<half, uint8_t>, Tuple<half, int8_t>,
        Tuple<float, int16_t>, Tuple<bfloat16_t, fp8_e8m0_t>, Tuple<uint32_t, uint8_t>, Tuple<int32_t, int8_t>,
        Tuple<float, hifloat8_t>, Tuple<float, fp8_e4m3fn_t>, Tuple<float, fp8_e5m2_t>, Tuple<bfloat16_t, fp4x2_e2m1_t>,
        Tuple<bfloat16_t, fp4x2_e1m2_t>, Tuple<half, int4x2_t>, Tuple<bfloat16_t, int4x2_t>,
        Tuple<int16_t, int4x2_t>>();
    constexpr bool satLayMergeCast = SupportType<
        Tuple<ActualT, ActualU>, Tuple<uint8_t, uint16_t>, Tuple<uint8_t, int16_t>, Tuple<uint16_t, uint32_t>,
        Tuple<int16_t, uint32_t>, Tuple<uint16_t, int32_t>, Tuple<int16_t, int32_t>, Tuple<uint8_t, uint32_t>,
        Tuple<uint8_t, int32_t>, Tuple<int32_t, int64_t>, Tuple<int4x2_t, int16_t>>();
    constexpr bool rndSatLayoutMergeCast = SupportType<
        Tuple<ActualT, ActualU>, Tuple<int16_t, float>, Tuple<uint8_t, half>, Tuple<int8_t, half>,
        Tuple<int32_t, bfloat16_t>, Tuple<int64_t, float>, Tuple<half, float>, Tuple<bfloat16_t, float>,
        Tuple<hifloat8_t, half>, Tuple<hifloat8_t, float>, Tuple<fp8_e5m2_t, float>, Tuple<fp8_e4m3fn_t, float>,
        Tuple<int4x2_t, half>, Tuple<fp8_e8m0_t, bfloat16_t>>();
    constexpr bool rndSatMergeCast =
        SupportType<Tuple<ActualT, ActualU>, Tuple<int32_t, float>, Tuple<int16_t, half>, Tuple<half, bfloat16_t>>();
    constexpr bool rndLayoutMergeCast = SupportType<
        Tuple<ActualT, ActualU>, Tuple<int32_t, half>, Tuple<float, int64_t>, Tuple<fp4x2_e2m1_t, bfloat16_t>,
        Tuple<fp4x2_e1m2_t, bfloat16_t>>();
    constexpr bool rndMergeCast =
        SupportType<Tuple<ActualT, ActualU>, Tuple<half, int16_t>, Tuple<float, int32_t>, Tuple<bfloat16_t, half>>();
    constexpr bool allNotSupport =
        !(layoutMerge && satLayMergeCast && rndSatLayoutMergeCast && rndSatMergeCast && rndLayoutMergeCast &&
          rndMergeCast);
    if constexpr (layoutMerge) {
        if constexpr (CheckRegTrait<S, RegTraitNumTwo>()) {
            CastImpl<T, U, trait.mrgMode, S, V>(dstReg, srcReg, mask);
        } else {
            CastImpl<ActualT, ActualU, trait.layoutMode, trait.mrgMode>(dstReg, srcReg, mask);
        }
    } else if constexpr (satLayMergeCast) {
        if constexpr (CheckRegTrait<V, RegTraitNumTwo>()) {
            CastImpl<T, U, trait.satMode, trait.mrgMode, S, V>(dstReg, srcReg, mask);
        } else {
            CastImpl<ActualT, ActualU, trait.satMode, trait.layoutMode, trait.mrgMode>(dstReg, srcReg, mask);
        }
    } else if constexpr (rndSatLayoutMergeCast) {
        if constexpr (CheckRegTrait<S, RegTraitNumTwo>()) {
            CastImpl<T, U, trait.roundMode, trait.satMode, trait.mrgMode, S, V>(dstReg, srcReg, mask);
        } else {
            CastImpl<ActualT, ActualU, trait.roundMode, trait.satMode, trait.layoutMode, trait.mrgMode>(
                dstReg, srcReg, mask);
        }
    } else if constexpr (rndSatMergeCast) {
        CastImpl<ActualT, ActualU, trait.roundMode, trait.satMode, trait.mrgMode>(dstReg, srcReg, mask);
    } else if constexpr (rndLayoutMergeCast) {
        if constexpr (CheckRegTrait<V, RegTraitNumTwo>()) {
            CastImpl<T, U, trait.roundMode, trait.mrgMode, S, V>(dstReg, srcReg, mask);
        } else {
            CastImpl<ActualT, ActualU, trait.roundMode, trait.layoutMode, trait.mrgMode>(dstReg, srcReg, mask);
        }
    } else if constexpr (rndMergeCast) {
        CastImpl<ActualT, ActualU, trait.roundMode, trait.mrgMode>(dstReg, srcReg, mask);
    } else {
        static_assert(allNotSupport, "current cast data type is not supported on current device!");
    }
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_VEC_VCONV_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_VCONV_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_VCONV_IMPL__
#endif
