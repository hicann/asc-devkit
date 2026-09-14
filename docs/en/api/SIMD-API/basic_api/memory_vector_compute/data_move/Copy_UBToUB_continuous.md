# UB-to-UB Continuous Data Transfer (Copy)<a name="ZH-CN_TOPIC_0000002575088175"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-09-09T13:02:46.333Z -->

## Applicable Products<a name="zh-cn_topic_0000002567699435_section796754519912"></a>

<!-- npu="950" id9 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id9 -->
<!-- npu="A3" id10 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id10 -->
<!-- npu="910b" id11 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id11 -->
<!-- npu="310b" id12 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products AI Core: Not supported
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products Vector Core: Not supported
<!-- end id14 -->
<!-- npu="910" id15 -->
- Atlas training products: Not supported
<!-- end id15 -->

## Description<a name="zh-cn_topic_0000002567699435_section106841136114319"></a>

Header file path: `"basic_api/kernel_operator_data_copy_intf.h"`.

Supports continuous data copy between Unified Buffers. The format and content remain unchanged during the copy.

The supported data paths are as follows (represented by the [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Unified Buffer -> Unified Buffer
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

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the source operand or destination operand. For supported data types, see [Data Type](#zh-cn_topic_0000002567699435_section42191353048181). |
| isSetMask | This parameter does not take effect. Keep the default value true. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/localtensor_introduction.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. The start address must be 32-byte aligned. |
| src | Input | Source operand.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT. The start address must be 32-byte aligned.<br>The data type of the source operand must be the same as that of the destination operand. |
| count | Input | Number of elements to be copied. |

## Data Type<a name="zh-cn_topic_0000002567699435_section42191353048181"></a>

The source operand and destination operand support the same data type.
<!-- npu="950" id1 -->
For Ascend 950PR/Ascend 950DT, the supported data types are: int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.
<!-- end id1 -->

## Return Value

None

## Constraints<a name="zh-cn_topic_0000002567699435_section2045914466492"></a>

- Addresses in the Unified Buffer must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#general-address-overlap-constraints).
- The isSetMask parameter does not take effect. Keep its default value true.

## Example<a name="zh-cn_topic_0000002567699435_section088124295117"></a>

```cpp
AscendC::Copy(dstLocal, srcLocal, 512);
```
