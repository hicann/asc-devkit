# MulsCast<a name="ZH-CN_TOPIC_0000002008487800"></a>

## 产品支持情况<a name="section1550532418810"></a>

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
- Atlas 推理系列产品 AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品 Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_vec_fused_intf.h"`。

该接口用于将源操作数src与标量scalar相乘，再按照CAST\_ROUND模式将结果转换为half类型，根据mask将计算结果写入目的操作数dst。计算公式如下：

$$
dst_i = cast\_round\_to\_f16(src_i \times scalarValue)
$$

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T0 = DefaultType, typename T1 = DefaultType, typename T2, RegLayout layout = RegLayout::ZERO, typename T3, typename T4>
__simd_callee__ inline void MulsCast(T3& dstReg, T4& srcReg, T2 scalarValue, MaskReg& mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :----- | :--- |
| T0 | 目的操作数数据类型。 |
| T1 | 源操作数的数据类型。 |
| T2 | 标量源操作数的数据类型。 |
| layout | [RegLayout](../辅助数据类型/RegLayout.md)枚举类型。<br>支持RegLayout::ZERO、RegLayout::ONE。 |
| T3 | 目的操作数的`RegTensor`类型，例如`RegTensor<float>`，由编译器自动推导，用户不需要填写。 |
| T4 | 源操作数的`RegTensor`类型，例如`RegTensor<float>`，由编译器自动推导，用户不需要填写。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :----- | :-------- | :--- |
| dstReg | 输出 | 目的操作数。<br>类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| srcReg | 输入 | 源操作数。<br>类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| scalarValue | 输入 | 源操作数。<br>类型为标量。 |
| mask | 输入 | 源操作数元素级操作的有效性指示，详细说明请参考[MaskReg](../寄存器数据类型/MaskReg.md)。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 数据类型<a name="zh-cn_topic_0000002580343947_section1785515470716"></a>

**表3**  数据类型组合

| srcReg | scalarValue | dstReg |
| :----- | :---------- | :----- |
| float  | float       | half   |

## 约束说明<a name="section633mcpsimp"></a>

- 本接口不支持源操作数寄存器和目的操作数寄存器重叠，支持源操作数寄存器之间重叠。
- 本接口操作数为寄存器，不涉及地址对齐。
- 本接口不修改全局寄存器的值。
- [Cast](../类型转换/Cast.md)计算按照CAST\_ROUND模式舍入。

## 调用示例<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void MulsCastVF(__ubuf__ half* dstAddr, __ubuf__ float* srcAddr, float scalarValue, uint32_t count, uint32_t srcRepeatSize, uint32_t dstRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<float> srcReg0;
    AscendC::Reg::RegTensor<half> dstReg0;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<float>(count);
        AscendC::Reg::LoadAlign(srcReg0, srcAddr + i * srcRepeatSize);
        AscendC::Reg::MulsCast<half, float, float, AscendC::Reg::RegLayout::ZERO>(dstReg0, srcReg0, scalarValue, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * dstRepeatSize, dstReg0, mask);
    }
}
```
