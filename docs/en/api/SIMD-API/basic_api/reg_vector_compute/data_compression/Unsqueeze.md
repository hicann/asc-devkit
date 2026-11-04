# Unsqueeze<a name="ZH-CN_TOPIC_0000002009161778"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:11:40.990Z pushedAt=2026-10-26T07:41:44.511Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/data_compression/Unsqueeze_res.md#id1 -->


## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_gather_mask_intf.h"`.

Decompresses the data in **dstReg** based on **mask**. Decompression method: the 0th element in **dstReg** is set to 0, and the i-th element in **dstReg** equals the number of 1s in **mask** from the 0th to the (i-1)-th element. The most significant bit of **mask** is ignored and not counted.

The specific algorithm is shown in Figure 1. The first element of **dstReg** is 0. For subsequent elements, when the valid **mask** bit corresponding to **dstReg**[i-1] is 1, the value of **dstReg**[i] is **dstReg**[i-1] + 1; when the corresponding valid **mask** bit is 0, the value of **dstReg**[i] is **dstReg**[i-1].

**Figure 1**  Unsqueeze diagram

![Unsqueeze](../../../../figures/reg_unsqueeze.png)

## Definition Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U>
__simd_callee__ inline void Unsqueeze(U& dstReg, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ------ | ------ |
| T | Data type of the destination operand. |
| U | RegTensor type of the destination operand, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Function parameter description

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| dstReg | Input/Output | Source operand and destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| mask | Input | Provides the unsqueeze information for **dstReg**.<br>The type is [MaskReg](../register_data_types/MaskReg.md). |

## Return Value

None

## Data Types

Supported data types: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t.

## Constraints<a name="section177921451558"></a>

None

## Example<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void UnsqueezeVF(__ubuf__ T* dstAddr, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    mask = AscendC::Reg::CreateMask<T>();
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::Unsqueeze(dstReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
