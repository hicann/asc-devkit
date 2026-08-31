# Truncate<a name="ZH-CN_TOPIC_0000001929827676"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/type_conversion/Truncate_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径：`"basic_api/reg_compute/kernel_reg_compute_vec_vconv_intf.h"`。

将源操作数中的浮点数元素截断为整数值（保留原数据类型），并存入目的操作数。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, RoundMode roundMode = RoundMode::CAST_NONE, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void Truncate(S& dstReg, S& srcReg, MaskReg& mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| roundMode | 舍入模式，取值如下：<br>&bull; RoundMode::CAST_RINT<br>&bull; RoundMode::CAST_ROUND<br>&bull; RoundMode::CAST_FLOOR<br>&bull; RoundMode::CAST_CEIL<br>&bull; RoundMode::CAST_TRUNC |
| mode | [MaskMergeMode](../aux_data_types/MaskMergeMode.md)枚举类型，选择MERGING模式或ZEROING模式。<br>&bull; ZEROING模式下，mask未筛选的元素在dstReg中置零。<br>&bull; MERGING模式当前不支持。 |
| S | srcReg/dstReg类型，例如RegTensor&lt;float&gt;，由编译器自动推导，用户不需要填写。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstReg | 输出 | 目的操作数。 <br>类型为[RegTensor](../register_data_types/RegTensor.md)。 |
| srcReg | 输入 | 源操作数。 <br>类型为[RegTensor](../register_data_types/RegTensor.md) |
| mask | 输入 | 源操作数元素操作的有效指示，详细说明请参考[MaskReg](../register_data_types/MaskReg.md)。 |

## 数据类型

目的操作数与源操作数的数据类型需要保持一致。支持的数据类型为：half、bfloat16_t、float。

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- float类型只支持不饱和模式。

## 调用示例<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void TruncVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::Truncate<T, AscendC::RoundMode::CAST_FLOOR, AscendC::Reg::MaskMergeMode::ZEROING>(dstReg, srcReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
