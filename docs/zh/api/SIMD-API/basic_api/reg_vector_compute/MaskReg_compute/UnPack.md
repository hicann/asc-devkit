# UnPack<a name="ZH-CN_TOPIC_0000001985577793"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/MaskReg_compute/UnPack_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_maskreg_intf.h"`。

根据所选的低位模式或高位模式，将源操作数src的低半部分或高半部分，展开到目的操作数dst。展开方式为：将每bit展开为2bit，高位置零。

![UnPack示意图](../../../../figures/reg_unpack.png)

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <HighLowPart part = HighLowPart::LOWEST> 
__simd_callee__ inline void UnPack(MaskReg& dst, MaskReg& src)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| part | 枚举类型，低位模式或高位模式。<br>&bull; LOWEST低位模式；<br>&bull; HIGHEST高位模式。 |

**表2** 参数说明

| 参数名 | 描述 |
| --- | --- |
| dst | 目的操作数，类型为[MaskReg](../register_data_types/MaskReg.md)。 |
| src | 源操作数，类型为[MaskReg](../register_data_types/MaskReg.md)。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="section932512912207"></a>

```cpp
template <typename T>
__simd_vf__ inline void UnpackVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::MaskReg maskFull = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();
    AscendC::Reg::MaskReg mask0;
    AscendC::Reg::MaskReg mask1;
    AscendC::Reg::UnPack<AscendC::Reg::HighLowPart::LOWEST>(mask0, maskFull);
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask1 = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Adds(srcReg, srcReg, 0, mask0);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, srcReg, mask1);
    }
}
```
