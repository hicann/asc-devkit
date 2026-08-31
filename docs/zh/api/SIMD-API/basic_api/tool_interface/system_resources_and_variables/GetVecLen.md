# GetVecLen

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetVecLen_res.md#id1 -->

## 功能说明

头文件路径为：`"basic_api/kernel_operator_sys_var_intf.h"`。

获取[RegTensor](../../reg_vector_compute/register_data_types/RegTensor.md)位宽VL（Vector Length）的大小。

## 函数原型

```cpp
__aicore__ inline constexpr uint32_t GetVecLen()
```

## 参数说明

无

## 返回值说明

Vector Length的大小，返回常量256，单位为Byte。

## 约束说明

无

## 调用示例

如下样例通过GetVecLen获取循环迭代次数：

```cpp
template <typename T>
__aicore__ inline void AddCustomImpl(__local_mem__ T *dst, __local_mem__ T *src0, __local_mem__ T *src1,
    uint32_t calCount)
{
    AscendC::Reg::RegTensor<T> reg0;
    AscendC::Reg::RegTensor<T> reg1;
    AscendC::Reg::RegTensor<T> reg2;
    AscendC::Reg::MaskReg mask;
    constexpr uint32_t repeatElm = AscendC::GetVecLen() / sizeof(T);
    uint16_t repeatTime = AscendC::CeilDivision(calCount, repeatElm);
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = AscendC::Reg::UpdateMask<T>(calCount);
        AscendC::Reg::LoadAlign(reg0, src0 + i * repeatElm);
        AscendC::Reg::LoadAlign(reg1, src1 + i * repeatElm);
        AscendC::Reg::Add(reg2, reg0, reg1, mask);
        AscendC::Reg::StoreAlign(dst + i * repeatElm, reg2, mask);
    }
}
```
