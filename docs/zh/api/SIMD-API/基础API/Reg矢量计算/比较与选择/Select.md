# Select<a name="ZH-CN_TOPIC_0000001929668292"></a>

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

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_vec_cmpsel_intf.h"`。

给定两个源操作数srcReg0和srcReg1，根据mask的比特位值选取元素，得到目的操作数dstReg。当mask的比特位是1时，从srcReg0中选取对应位置的数，比特位是0时从srcReg1选取对应位置的数。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U>
__simd_callee__ inline void Select(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :----- | :--- |
| T | 操作数数据类型。 |
| U | 源操作数和目的操作数的`RegTensor`类型，例如`RegTensor<half>`，由编译器自动推导，用户不需要填写。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :----- | :-------- | :--- |
| dstReg | 输出 | 目的操作数。<br>类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| srcReg0、srcReg1 | 输入 | 源操作数。<br>类型为[RegTensor](../寄存器数据类型/RegTensor.md)。<br>两个源操作数的数据类型需要与目的操作数保持一致。 |
| mask | 输入 | 指定选择源操作数的规则：mask的比特位是1时，选取srcReg0，比特位是0时，选取srcReg1。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 数据类型<a name="zh-cn_topic_0000002598723617_section1962315623118"></a>

支持的数据类型为：bool、int8\_t、uint8\_t、int16\_t、uint16\_t、half、bfloat16\_t、int32\_t、uint32\_t、float、int64\_t、uint64\_t。

**表3**  数据类型组合

| srcReg0和srcReg1数据类型 | dst数据类型 |
| :----------------------- | :---------- |
| bool | bool |
| uint8_t | uint8_t |
| int8_t | int8_t |
| uint16_t | uint16_t |
| int16_t | int16_t |
| uint32_t | uint32_t |
| int32_t | int32_t |
| half | half |
| float | float |
| bfloat16_t | bfloat16_t |
| uint64_t | uint64_t |
| int64_t | int64_t |

## 约束说明<a name="section633mcpsimp"></a>

通过mask参数控制的未选中的元素保持源操作数的值而非清零。

## 调用示例<a name="section12223121385116"></a>

```cpp
template<typename T>
__simd_vf__ inline void SelectVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::MaskReg cmpReg;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);        
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);      
        AscendC::Reg::Compare<T, AscendC::CMPMODE::EQ>(cmpReg, srcReg0, srcReg1, mask);
        AscendC::Reg::Select(dstReg, srcReg0, srcReg1, cmpReg);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
