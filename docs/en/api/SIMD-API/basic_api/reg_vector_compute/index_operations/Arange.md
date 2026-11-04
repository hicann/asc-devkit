# Arange<a name="ZH-CN_TOPIC_0000001960926493"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:17:06.004Z pushedAt=2026-10-26T07:41:44.521Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/index_operations/Arange_res.md#id1 -->


## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_vec_arange_intf.h"`.

In Increasing Mode, this function uses the value of the passed **scalarValue** as the start value to generate increasing indices. In Decreasing Mode, it uses the value of the passed **scalarValue** as the end value to generate decreasing indices, and stores the indices in **dstReg**.

**Figure 1** Arange increasing mode diagram

![Arange diagram](../../../../figures/reg_arange_in.png)

**Figure 2** Arange decreasing mode diagram

![Arange diagram](../../../../figures/reg_arange_de.png)

## Prototype<a name="section520771712327"></a>

```cpp
template <typename T = DefaultType, IndexOrder order = IndexOrder::INCREASE_ORDER, typename U, typename S>
__simd_callee__ inline void Arange(S& dstReg, U scalarValue);
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ------ | ------ |
| T | Data type of the target operand. |
| IndexOrder | Increasing/Decreasing Mode. The following values are supported:<br>• INCREASE_ORDER: Increasing Mode<br>• DECREASE_ORDER: Decreasing Mode |
| U | Data type of the start index value. |
| S | RegTensor type of the target operand, for example, `RegTensor<half>`, which is automatically deduced by the compiler and does not need to be explicitly specified by the user. |

**Table 2** Function parameter description

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| dstReg | Output | Target operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| scalarValue | Input | Source operand.<br>The type is scalar. |

## Data Types

Supported data types: int8_t/int16_t/int32_t/half/float/int64_t.

## Constraints<a name="section177921451558"></a>

For **int8_t**/**int16_t**/**int32_t**/**int64_t**, when the index exceeds the boundary value of the data type, it wraps around to a negative number. For example, for **int8_t**, the value after 127 is -128, then -127, and so on.

## Example<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void ArangeVF(__ubuf__ T* dstAddr, T scalarValue, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    mask = AscendC::Reg::CreateMask<T>();
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::Arange(dstReg, scalarValue);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
