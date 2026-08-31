# ClearSpr<a name="ZH-CN_TOPIC_0000002186694092"></a>

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
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/system_variable_access/ClearSpr_res.md#id1 -->


## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_gather_mask_intf.h"`。

该接口用于对指定的特殊目的寄存器（SpecialPurposeReg）进行清零操作。通过模板参数spr指定目标特殊寄存器，调用后该寄存器的值被置为0，当前支持的特殊寄存器见[表 SpecialPurposeReg模板参数说明](#table2)。
## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <SpecialPurposeReg spr>
__simd_callee__ inline void ClearSpr()
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名称 | 描述 |
| ------ | ------ |
| spr | 特殊寄存器，类型为SpecialPurposeReg枚举类。具体的取值请参考[表 SpecialPurposeReg模板参数说明](#table2)。 |

**表2**  SpecialPurposeReg模板参数说明<a id="table2"></a>

| 取值 | 含义 |
| ------ | ------ |
| AR | 一个特殊的地址寄存器，通常配合[Squeeze](../compare_and_select/Squeeze.md)Reg矢量计算API一起使用，[Squeeze](../compare_and_select/Squeeze.md)Reg矢量计算API会存储有效元素的总字节数到AR寄存器。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

本接口只能在VF函数内调用，命名空间为AscendC::Reg，函数标记符为__simd_callee__。如果需要在VF外调用，命名空间为AscendC，函数标记符为__aicore__，具体请参考[ClearSpr](../../special_register_access/ClearSpr.md)。

## 调用示例<a name="section642mcpsimp"></a>

如下示例中Gather Reg矢量计算API会存储有效元素的总字节数到AR寄存器中，在宏函数内for循环开始前通过ClearSpr对AR寄存器进行清零。

```cpp
template<typename T, typename U>
__simd_vf__ inline void VFDemo(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ U* src1Addr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0, dstReg;
    AscendC::Reg::RegTensor<U> srcReg1;
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::LoadAlign(srcReg1, src1Addr);
    AscendC::Reg::ClearSpr<AscendC::SpecialPurposeReg::AR>();
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::Gather(dstReg, srcReg0, srcReg1);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
