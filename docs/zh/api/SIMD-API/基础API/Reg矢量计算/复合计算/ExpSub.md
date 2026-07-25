# ExpSub<a name="ZH-CN_TOPIC_0000002008646072"></a>

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

srcReg0与srcReg1相减，差值作为e的指数计算，根据mask将计算结果写入dstReg。公式如下：

srcReg数据类型为float时：

$$
dstReg_i = e^{(srcReg0_i - srcReg1_i)}
$$

srcReg数据类型为half时：

$$
dstReg_i = e^{(cast\_f16\_to\_f32(srcReg0_i) - cast\_f16\_to\_f32(srcReg1_i))}
$$

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U = DefaultType, RegLayout layout = RegLayout::ZERO, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S, typename V>
__simd_callee__ inline void ExpSub(S& dstReg, V& srcReg0, V& srcReg1, MaskReg& mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :----- | :--- |
| T | 目的操作数数据类型。 |
| U | 源操作数数据类型。 |
| layout | [RegLayout](../辅助数据类型/RegLayout.md)枚举类型。<br>• src类型为half类型时，支持RegLayout::ZERO、RegLayout::ONE。具体特性参考[RegLayout](../辅助数据类型/RegLayout.md)。<br>• src类型为float类型时，配置不生效。 |
| mode | [MaskMergeMode](../辅助数据类型/MaskMergeMode.md)枚举类型。选择MERGING模式或ZEROING模式。<br>• ZEROING模式下，mask未筛选的元素在dstReg中置零。<br>• MERGING模式当前不支持。 |
| S | 目的操作数的`RegTensor`类型，例如`RegTensor<float>`，由编译器自动推导，用户不需要填写。 |
| V | 源操作数的`RegTensor`类型，例如`RegTensor<half>`，由编译器自动推导，用户不需要填写。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :----- | :-------- | :--- |
| dstReg | 输出 | 目的操作数。<br>类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| srcReg0 | 输入 | 源操作数。<br>类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| srcReg1 | 输入 | 源操作数。<br>类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| mask | 输入 | 源操作数元素操作的有效指示，详细说明请参考[MaskReg](../寄存器数据类型/MaskReg.md)。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 数据类型

**表3**  数据类型组合

| srcReg0 | srcReg1 | dstReg |
| :------ | :------ | :----- |
| half    | half    | float  |
| float   | float   | float  |

## 约束说明<a name="section633mcpsimp"></a>

- 源操作数数据类型为float时，支持寄存器全部重叠；源操作数数据类型为half时，仅支持源操作数寄存器重叠。
- 本接口操作数为寄存器，不涉及地址对齐。
- 本接口不修改全局寄存器的值。
- 源操作数类型为half时，Vector计算单元一次计算只处理最多64个元素，mask的有效情况以输入数据类型为准，只有偶数位有效，有效位共128bit，参考下图：

  ![ExpSub处理half类型示意图](../../../../figures/reg_expsub_half.png)

## 关键特性说明

精度提升。当源操作数数据类型为half，目的操作数数据类型为float时，会将源操作数提升精度到float再进行计算。

## 调用示例<a name="section642mcpsimp"></a>

```cpp
template<typename T, typename U>
static __simd_vf__ inline void ExpSubVF(__ubuf__ T* dstAddr, __ubuf__ U* src0Addr, __ubuf__ U* src1Addr, uint32_t count, uint32_t srcRepeatSize, uint32_t dstRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<U> srcReg0;
    AscendC::Reg::RegTensor<U> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<U>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * srcRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * srcRepeatSize);
        AscendC::Reg::ExpSub<T, U, AscendC::Reg::RegLayout::ZERO, AscendC::Reg::MaskMergeMode::ZEROING>(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * dstRepeatSize, dstReg, mask);
    }
}
```
