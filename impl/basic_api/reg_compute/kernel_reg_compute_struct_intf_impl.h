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
 * \file kernel_reg_compute_struct_intf_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/kernel_reg_compute_struct_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_struct_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_STRUCT_INTF_IMPL__
#endif

#ifndef ASCENDC_KERNEL_REG_COMPUTE_STRUCT_INTERFACE_IMPL_H
#define ASCENDC_KERNEL_REG_COMPUTE_STRUCT_INTERFACE_IMPL_H

namespace AscendC {
namespace Reg {

template <typename T, const RegTrait& regTrait>
__aicore__ void RegTensor<T, regTrait>::Print() const
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    std::ostringstream os;
    os.str("");
    uint32_t printLen;
    uint32_t blockNum;
    uint32_t rowNum;
    constexpr uint16_t b4RepElems = 512;
    constexpr uint16_t b4BlkElems = 64;
    if constexpr (Std::is_same_v<T, fp4x2_e2m1_t> || Std::is_same_v<T, fp4x2_e1m2_t>) {
        printLen = b4RepElems;
        blockNum = b4BlkElems;
        rowNum = printLen / blockNum;
    } else {
        printLen = VECTOR_REG_WIDTH / sizeof(T);
        blockNum = ONE_BLK_SIZE / sizeof(T);
        rowNum = printLen / blockNum;
    }
    const int32_t width = 4;
    for (int regIdx = 0; regIdx < regTrait.REG_NUM; regIdx++) {
        if constexpr (regTrait.REG_NUM == 2) {
            os << "reg[" << regIdx << "]:" << std::endl;
        }
        for (uint32_t i = 0; i < rowNum; i++) {
            os << std::setw(width) << std::setfill('0') << i * blockNum << " : ";
            for (uint32_t j = 0; j < blockNum; j++) {
                if constexpr ((sizeof(T) == sizeof(int8_t)) || (sizeof(T) == sizeof(bool))) {
                    os << static_cast<int32_t>(reg[regIdx][i * blockNum + j]) << " ";
                } else if constexpr (
                    Std::is_same_v<T, half> || Std::is_same_v<T, bfloat16_t> || Std::is_same_v<T, fp8_e4m3fn_t> ||
                    Std::is_same_v<T, fp8_e5m2_t> || Std::is_same_v<T, hifloat8_t>) {
                    os << reg[regIdx][i * blockNum + j].ToFloat() << " ";
                } else if constexpr (Std::is_same_v<T, fp4x2_e2m1_t> || Std::is_same_v<T, fp4x2_e1m2_t>) {
                    os << ((bfloat16_t)reg[regIdx][i * blockNum + j]).ToFloat() << " ";
#if (__NPU_ARCH__ == 3510)
                } else if constexpr (Std::is_same_v<T, complex32>) {
                    const float real = (*(reinterpret_cast<const half*>(&reg[regIdx][i * blockNum + j]))).ToFloat();
                    const float image =
                        (*(reinterpret_cast<const half*>(&reg[regIdx][i * blockNum + j]) + 1)).ToFloat();
                    os << real << " " << image << " ";
                } else if (Std::is_same_v<T, complex64>) {
                    const float real = *(reinterpret_cast<const float*>(&reg[regIdx][i * blockNum + j]));
                    const float image = *(reinterpret_cast<const float*>(&reg[regIdx][i * blockNum + j]) + 1);
                    os << real << " " << image << " ";
#endif
                } else if constexpr (Std::is_same_v<T, int4x2_t>) {
                    os << static_cast<int16_t>(reg[regIdx][i * blockNum + j].data & 0xf) << " "
                       << static_cast<int16_t>((reg[regIdx][i * blockNum + j].data >> 4) & 0xf) << " ";
                } else {
                    os << reg[regIdx][i * blockNum + j] << " ";
                }
            }
            os << std::endl;
        }
    }
    os << std::endl;
    std::cout << os.str();
#endif
}

} // namespace Reg
} // namespace AscendC

#endif // ASCENDC_KERNEL_REG_COMPUTE_STRUCT_INTERFACE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_STRUCT_INTF_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_STRUCT_INTF_IMPL__
#endif
