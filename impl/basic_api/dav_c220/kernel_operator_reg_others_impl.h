/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file kernel_operator_reg_others_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_c220/kernel_operator_reg_others_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_REG_OTHERS_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_REG_OTHERS_IMPL_H
#define ASCENDC_MODULE_OPERATOR_REG_OTHERS_IMPL_H

#ifndef ASCENDC_CPU_DEBUG
namespace AscendC {
template <typename T>
__aicore__ inline void HsetFlag(pipe_t pipe, pipe_t tpipe, T eventID, mem_t memory, bool v)
{
    hset_flag(pipe, tpipe, eventID, memory, v);
}

template <typename T>
__aicore__ inline void HwaitFlag(pipe_t pipe, pipe_t tpipe, T eventID, mem_t memory, bool v)
{
    hwait_flag(pipe, tpipe, eventID, memory, v);
}

__aicore__ inline int64_t GetThreadDim() { return get_thread_dim(); }

__aicore__ inline int64_t GetThreadId() { return get_thread_id(); }

__aicore__ inline int64_t GetSubBlockDim() { return get_subblockdim(); }

__aicore__ inline int64_t GetSubBlockId() { return get_subblockid(); }

__aicore__ inline int64_t GetStackPhyBase() { return get_stack_phy_base(); }

__aicore__ inline int64_t GetStAtomicCfg() { return get_st_atomic_cfg(); }

__aicore__ inline int64_t GetRsvdCnt() { return get_rsvd_cnt(); }

__aicore__ inline int64_t GetMaxMinCnt() { return get_max_min_cnt(); }

__aicore__ inline int64_t GetIcachePrlSt() { return get_icache_prl_st(); }

__aicore__ inline int64_t GetFpc() { return get_fpc(); }

__aicore__ inline void GetCmpmask(__ubuf__ void* dst) { get_cmpmask(dst); }

template <typename T>
__aicore__ inline void DcPreload(__gm__ uint64_t* address, T offset)
{
    dc_preload(address, offset);
}

template <typename T>
__aicore__ inline uint64_t ld_dev(T* src, int16_t offset)
{
    return ld_dev(src, offset);
}

__aicore__ inline void SetAippSpr0(uint64_t config) { set_aipp_spr_0(config); }

__aicore__ inline void SetAippSpr1(uint64_t config) { set_aipp_spr_1(config); }

__aicore__ inline void SetAippSpr10(uint64_t config) { set_aipp_spr_10(config); }

__aicore__ inline void SetAippSpr11(uint64_t config) { set_aipp_spr_11(config); }

__aicore__ inline void SetAippSpr12(uint64_t config) { set_aipp_spr_12(config); }

__aicore__ inline void SetAippSpr13(uint64_t config) { set_aipp_spr_13(config); }

__aicore__ inline void SetAippSpr14(uint64_t config) { set_aipp_spr_14(config); }

__aicore__ inline void SetAippSpr15(uint64_t config) { set_aipp_spr_15(config); }

__aicore__ inline void SetAippSpr16(uint64_t config) { set_aipp_spr_16(config); }

__aicore__ inline void SetAippSpr17(uint64_t config) { set_aipp_spr_17(config); }

__aicore__ inline void SetAippSpr18(uint64_t config) { set_aipp_spr_18(config); }

__aicore__ inline void SetAippSpr19(uint64_t config) { set_aipp_spr_19(config); }

__aicore__ inline void SetAippSpr2(uint64_t config) { set_aipp_spr_2(config); }

__aicore__ inline void SetAippSpr20(uint64_t config) { set_aipp_spr_20(config); }

__aicore__ inline void SetAippSpr21(uint64_t config) { set_aipp_spr_21(config); }

__aicore__ inline void SetAippSpr22(uint64_t config) { set_aipp_spr_22(config); }

__aicore__ inline void SetAippSpr23(uint64_t config) { set_aipp_spr_23(config); }

__aicore__ inline void SetAippSpr24(uint64_t config) { set_aipp_spr_24(config); }

__aicore__ inline void SetAippSpr3(uint64_t config) { set_aipp_spr_3(config); }

__aicore__ inline void SetAippSpr4(uint64_t config) { set_aipp_spr_4(config); }

__aicore__ inline void SetAippSpr5(uint64_t config) { set_aipp_spr_5(config); }

__aicore__ inline void SetAippSpr6(uint64_t config) { set_aipp_spr_6(config); }

__aicore__ inline void SetAippSpr7(uint64_t config) { set_aipp_spr_7(config); }

__aicore__ inline void SetAippSpr8(uint64_t config) { set_aipp_spr_8(config); }

__aicore__ inline void SetAippSpr9(uint64_t config) { set_aipp_spr_9(config); }

__aicore__ inline void SetDataExp0(uint64_t config) { set_data_exp_0(config); }

template <typename T>
__aicore__ inline void SetDeqscale(T config)
{
    set_deqscale(config);
}

__aicore__ inline void SetFftsBaseAddr(uint64_t config) { set_ffts_base_addr(config); }

__aicore__ inline int64_t GetFftsBaseAddr() { return get_ffts_base_addr(); }

__aicore__ inline void SetMovPadVal(uint64_t config) { set_mov_pad_val(config); }

__aicore__ inline void SetPcieRdCtrl(uint64_t config) { set_pcie_rd_ctrl(config); }

template <typename T>
__aicore__ inline void StDev(T src, __gm__ T* dst, int16_t offset)
{
    st_dev(src, dst, offset);
}

__aicore__ inline void WaitFlagDev(int64_t flagID) { wait_flag_dev(flagID); }

__aicore__ inline void FftsCrossCoreSync(pipe_t pipe, uint64_t config) { ffts_cross_core_sync(pipe, config); }

__aicore__ inline int64_t Clz(uint64_t in) { return clz(in); }

__aicore__ inline uint64_t FakeOverFlowStatus1() { return fake_overflow_status_1(); }
__aicore__ inline int64_t GetArchVer()
{
    constexpr int64_t archVer = 2201L;
    return archVer;
}
__aicore__ inline int64_t GetConditionFlag() { return get_condition_flag(); }

__aicore__ inline int64_t GetCoreid() { return get_coreid(); }

__aicore__ inline int64_t GetCtrl() { return get_ctrl(); }

__aicore__ inline uint64_t GetImm(uint64_t imm0_15) { return get_imm(imm0_15); }

__aicore__ inline uint64_t GetImm(uint64_t imm0_15, uint64_t imm16_31) { return get_imm(imm0_15, imm16_31); }

__aicore__ inline uint64_t GetImm(uint64_t imm0_15, uint64_t imm16_31, uint64_t imm32_47)
{
    return get_imm(imm0_15, imm16_31, imm32_47);
}

__aicore__ inline uint64_t GetImm(uint64_t imm0_15, uint64_t imm16_31, uint64_t imm32_47, uint64_t imm48_63)
{
    return get_imm(imm0_15, imm16_31, imm32_47, imm48_63);
}

__aicore__ inline int64_t GetL2InMain() { return get_l2_in_main(); }

__aicore__ inline int64_t GetL2VAddrBase() { return get_l2_vaddr_base(); }

__aicore__ inline int64_t GetLpcnt() { return get_lpcnt(); }

__aicore__ inline uint64_t GetOverflowStatus() { return get_overflow_status(); }

__aicore__ inline int64_t GetParaBase() { return get_para_base(); }

__aicore__ inline int64_t GetStatus() { return get_status(); }

__aicore__ inline uint64_t SbitSet0(uint64_t x, int64_t idx) { return sbitset0(x, idx); }

__aicore__ inline void SetConditionFlag(uint64_t config) { set_condition_flag(config); }

__aicore__ inline void Set_ctrl(uint64_t config) { set_ctrl(config); }

template <typename T>
__aicore__ inline void SetFlag(pipe_t pipe, pipe_t tpipe, T pipeID)
{
    set_flag(pipe, tpipe, pipeID);
}

template <typename T>
__aicore__ inline void WaitFlag(pipe_t pipe, pipe_t tpipe, T pipeID)
{
    wait_flag(pipe, tpipe, pipeID);
}

__aicore__ inline void SetLpcnt(uint64_t config) { set_lpcnt(config); }

__aicore__ inline int64_t Sff0(uint64_t in) { return sff0(in); }

__aicore__ inline void VldVaReg(ub_addr8_t dst, __ubuf__ uint64_t* src, vpart_t config)
{
    vld_va_reg(dst, src, config);
}
} // namespace AscendC
#endif
#endif // ASCENDC_MODULE_OPERATOR_REG_OTHERS_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_REG_OTHERS_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_REG_OTHERS_IMPL_H__
#endif
