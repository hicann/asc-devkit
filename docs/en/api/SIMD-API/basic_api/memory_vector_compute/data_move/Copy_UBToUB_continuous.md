# Copy (UBToUB Continuous Data Movement)<a name="ZH-CN_TOPIC_0000002575088175"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:27:25.308Z pushedAt=2026-10-28T03:14:39.477Z nl=2 -->

## Applicable Products<a name="zh-cn_topic_0000002567699435_section796754519912"></a>

<!-- npu="950" id11 -->
- Ascend 950PR&950DT products: supported
<!-- end id11 -->
<!-- npu="A3" id12 -->
- Atlas A3 products: not supported
<!-- end id12 -->
<!-- npu="910b" id13 -->
- Atlas A2 products: not supported
<!-- end id13 -->
<!-- npu="310b" id14 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products AI Core: not supported
<!-- end id15 -->
<!-- npu="310p" id16 -->
- Atlas inference products Vector Core: not supported
<!-- end id16 -->
<!-- npu="910" id17 -->
- Atlas training products: not supported
<!-- end id17 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_move/Copy_UBToUB_continuous_res.md#id1 -->

## Description<a name="zh-cn_topic_0000002567699435_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

Supports continuous data transfer within the Unified Buffer (UB), with the format and content remaining unchanged during the transfer.

The specific supported data paths are (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- UB -> UB
    - VECIN -> VECCALC
    - VECIN -> VECOUT
    - VECCALC -> VECIN
    - VECCALC -> VECOUT
    - VECOUT -> VECIN
    - VECOUT -> VECCALC

## Prototype<a name="zh-cn_topic_0000002567699435_section82039854412"></a>

```cpp
template <typename T, bool isSetMask = true>
__aicore__ inline void Copy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## Parameters<a name="zh-cn_topic_0000002567699435_section16128134420472"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the source operand or destination operand. For supported data types, see [Data Types](#zh-cn_topic_0000002567699435_section42191353048181). |
| isSetMask | This parameter does not take effect. Keep the default value true. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor_intro.md). Supported TPosition: VECIN/VECCALC/VECOUT. The start address must be 32-byte aligned. |
| src | Input | Source operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT. The start address must be 32-byte aligned.<br>The data type of the source operand must be the same as that of the destination operand. |
| count | Input | Number of elements to be copied. |

## Data Types<a name="zh-cn_topic_0000002567699435_section42191353048181"></a>

The supported data types of the source operand and the destination operand must be consistent.
<!-- npu="950" id1 -->
Ascend 950PR&950DT products, the supported data types are: int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.
<!-- end id1 -->

## Return Value

None

## Constraints<a name="zh-cn_topic_0000002567699435_section2045914466492"></a>

- The address in the UB must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#general-address-overlap-constraints).
- The isSetMask parameter does not take effect. Keep its default value true.

## Examples<a name="zh-cn_topic_0000002567699435_section088124295117"></a>

```cpp
AscendC::Copy(dstLocal, srcLocal, 512);
```
