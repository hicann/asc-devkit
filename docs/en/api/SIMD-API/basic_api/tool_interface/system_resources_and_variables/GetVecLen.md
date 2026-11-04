# GetVecLen

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T19:28:15.048Z pushedAt=2026-10-26T07:41:44.882Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetVecLen_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

Obtains the bit width VL (Vector Length) of [RegTensor](../../reg_vector_compute/register_data_types/RegTensor.md).

## Function Prototype

```cpp
__aicore__ inline constexpr uint32_t GetVecLen()
```

## Parameters

None

## Return Value

Returns the size of **Vector Length**, which is the constant 256, in bytes.

## Constraints

None

## Example

The following sample obtains the number of loop iterations through **GetVecLen**:

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
