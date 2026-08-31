# Move<a name="ZH-CN_TOPIC_0000001985577785"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/MaskReg_compute/Move_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_maskreg_intf.h"`。

将src中的元素复制到dst中的对应位置。如果有输入mask，则仅复制被mask选定的有效元素，无效元素填0。

如需将RegTensor复制到MaskReg，请参考接口[MaskGenWithRegTensor](../reg_data_load/MaskGenWithRegTensor_MaskReg.md)。

## 函数原型<a name="section620mcpsimp"></a>

- 有输入mask

    ```cpp
    __simd_callee__ inline void Move(MaskReg& dst, MaskReg& src, MaskReg& mask)
    ```

- 无输入mask

    ```cpp
    __simd_callee__ inline void Move(MaskReg& dst, MaskReg& src)
    ```

## 参数说明<a name="section622mcpsimp"></a>

**表1** 参数说明

| 参数名 | 描述 |
| --- | --- |
| dst | 目的操作数，类型为[MaskReg](../register_data_types/MaskReg.md)。 |
| src | 源操作数，类型为[MaskReg](../register_data_types/MaskReg.md)。 |
| mask | 指示在复制过程中哪些bit有效，类型为[MaskReg](../register_data_types/MaskReg.md)。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="section932512912207"></a>

```cpp
template <typename T>
__simd_vf__ inline void MoveVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::MaskReg src = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALLF>();
    AscendC::Reg::MaskReg dst;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Move(dst, src, mask);
        AscendC::Reg::Adds(srcReg, srcReg, 0, dst);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, srcReg, mask);
    }
}
```
