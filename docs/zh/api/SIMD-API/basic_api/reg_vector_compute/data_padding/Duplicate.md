# Duplicate<a name="ZH-CN_TOPIC_0000001937477638"></a>

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/data_padding/Duplicate_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径：`"basic_api/reg_compute/kernel_reg_compute_vec_duplicate_intf.h"`。

支持Scalar和Tensor两种模式：

-   Scalar模式：将scalarValue广播到寄存器，并保存在dstReg中（如果有mask，则保存在dstReg中被mask筛选的位置）。
-   Tensor模式：将srcReg的最低位元素广播到寄存器，并保存在dstReg中被mask筛选的位置。

## 函数原型<a name="section620mcpsimp"></a>

- 将scalarValue广播到寄存器，不支持mask

    ```cpp
    template <typename T = DefaultType, typename U, typename S>
    __simd_callee__ inline void Duplicate(S& dstReg, U scalarValue);
    ```

- 将scalarValue广播到寄存器，支持mask

    ```cpp
    template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U, typename S>
    __simd_callee__ inline void Duplicate(S& dstReg, U scalarValue, MaskReg& mask);
    ```

- 将srcReg的最低位元素广播到寄存器

    ```cpp
    template <typename T = DefaultType, HighLowPart pos = HighLowPart::LOWEST, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
    __simd_callee__ inline void Duplicate(S& dstReg, S& srcReg, MaskReg& mask)
    ```

## 参数说明<a name="section622mcpsimp"></a>

**表 1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| U | scalar的数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| mode | [MaskMergeMode](../aux_data_types/MaskMergeMode.md)枚举类型，选择MERGING模式或ZEROING模式。<br>&bull; ZEROING模式下，mask未筛选的元素在dstReg中置零。<br>&bull; MERGING模式下，mask未筛选的元素在dstReg中保留dstReg原值。 |
| pos | 默认为LOWEST，表示将srcReg中的最低位广播至dstReg（暂不支持其他取值）。 |
| S | 目的操作数的RegTensor类型， 例如RegTensor&lt;half&gt;，由编译器自动推导，用户不需要填写。 |

**表 2**  函数参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstReg | 输出 | 目的操作数。<br>类型为[RegTensor](../register_data_types/RegTensor.md)。 |
| scalarValue | 输入 | 源操作数。<br>类型为标量。 |
| srcReg | 输入 | 源操作数。<br>类型为[RegTensor](../register_data_types/RegTensor.md)。 |
| mask | 输入 | 源操作数中元素有效性的指示，详细说明请参考[MaskReg](../register_data_types/MaskReg.md)。 |

## 数据类型

目的操作数与源操作数的数据类型需要保持一致。支持的数据类型为：bool、int8_t、uint8_t、fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、complex32、int64_t、uint64_t、complex64。


## 返回值说明

无

## 约束说明<a name="section177921451558"></a>

无

## 调用示例<a name="section642mcpsimp"></a>

- 示例一

    ```cpp
    template<typename T>
    __simd_vf__ inline void DuplicateVF(__ubuf__ T* dstAddr, T scalarValue, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T>();    
        for (uint16_t i = 0; i < repeatTimes; i++) {
            AscendC::Reg::Duplicate(dstReg, scalarValue);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```

- 示例二

    ```cpp
    template<typename T>
    __simd_vf__ inline void DuplicateVF(__ubuf__ T* dstAddr, T scalarValue, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;    
        for (uint16_t i = 0; i < repeatTimes; i++) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::Duplicate(dstReg, scalarValue, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```

- 示例三

    ```cpp
    template<typename T>
    __simd_vf__ inline void DuplicateVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
            AscendC::Reg::Duplicate(dstReg, srcReg, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```
