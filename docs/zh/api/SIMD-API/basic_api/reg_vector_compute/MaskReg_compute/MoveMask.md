# MoveMask<a name="ZH-CN_TOPIC_0000001985457933"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/MaskReg_compute/MoveMask_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_maskreg_intf.h"`。

从[SetVectorMask](../../memory_vector_compute/mask_operations/SetVectorMask.md)设置的掩码寄存器{MASK1, MASK0}中读取Mask值，并按模板参数T对应的数据格式转换后写入返回值MaskReg。具体实现方式如下：

b16类型读取完整128bit的{MASK1, MASK0}，将每个bit复制为2bit；b32类型读取64bit的MASK0，并将每个bit复制为4bit。

![A2/A3与950 Mask机制对比](../../../../figures/reg_move_mask.png)

Mask与MaskReg的机制对比，具体请参考[A2/A3与950 Mask机制对比](../register_data_types/MaskReg.md)。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T> 
__simd_callee__ inline MaskReg MoveMask()
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 数据类型，决定mask值写入格式：<br>&bull; 对于b16类型，会读取完整的128bit {MASK1, MASK0}数据，并将每bit复制为2bit，写入函数返回值MaskReg。<br>&bull; 对于b32类型，会读取64bit MASK0数据，并将每bit复制为4bit，写入函数返回值MaskReg。 |

## 数据类型

支持的数据类型：b16、b32。

## 返回值说明<a name="section640mcpsimp"></a>

特殊寄存器中读取的MaskReg。

## 约束说明<a name="section633mcpsimp"></a>

- 本接口为兼容性接口，建议优先采用[CreateMask接口](../register_data_types/MaskReg.md)和[UpdateMask接口](../register_data_types/MaskReg.md)进行MaskReg计算。
- 使用前需要先调用[SetMaskCount](../../memory_vector_compute/mask_operations/SetMaskCount.md)/[SetMaskNorm](../../memory_vector_compute/mask_operations/SetMaskNorm.md)手动设置mask模式，并调用[SetVectorMask](../../memory_vector_compute/mask_operations/SetVectorMask.md)设置掩码寄存器SPR{MASK1, MASK0}。

## 调用示例<a name="section932512912207"></a>

```cpp
template <typename T>
__simd_vf__ inline void MoveMaskVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::MaskReg addMask = AscendC::Reg::MoveMask<T>();
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Adds(srcReg, srcReg, 0, addMask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, srcReg, mask);
    }
}
```
