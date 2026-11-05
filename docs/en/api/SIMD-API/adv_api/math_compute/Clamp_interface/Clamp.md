# Clamp

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T02:29:38.790Z pushedAt=2026-10-29T02:15:02.594Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Clamp_interface/Clamp_res.md#id1 -->

## Description

Replaces values in the input that are greater than max (except NaN values) with max, and values that are less than min with min. Values that are less than or equal to max and greater than or equal to min remain unchanged and are output. When min is greater than max, all values except NaN values are replaced with max. min and max can be scalars or LocalTensor.

![](../../../../figures/zh-cn_formulaimage_0000002386121366.png)

![](../../../../figures/zh-cn_formulaimage_0000002419939453.png)

## Prototype

```
template <const ClampConfig& config = DEFAULT_CLAMP_CONFIG, typename T, typename U, typename S>
__aicore__ inline void Clamp(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& min, const S& max, const uint32_t count)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| config | Configuration related to the Clamp algorithm. This parameter is optional and is of the ClampConfig type. Its definition is shown in the following code, where the parameter meanings are as follows:<br><br>isReuseSource: This parameter is reserved. Pass the default value false. |
| T | Data type of the operands. Supported data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t. |
| U | LocalTensor type or scalar type. The type is automatically deduced from the input parameter min. You do not need to configure this parameter; just ensure that min meets the data type constraints.<br><br>Supported data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t. |
| S | LocalTensor type or scalar type. The type is automatically deduced from the input parameter max. You do not need to configure this parameter; just ensure that max meets the data type constraints.<br><br>Supported data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t. |

```
struct ClampConfig {
    bool isReuseSource;
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| src | Input | Source operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be the same as that of the destination operand. |
| min | Input | Lower bound of the data. Type: scalar or LocalTensor. When the type is LocalTensor, the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type must be the same as that of the destination operand. |
| max | Input | Upper bound of the data. Type: scalar or LocalTensor. When the type is LocalTensor, the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type must be the same as that of the destination operand. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Overlapping between the source operand and the destination operand is not supported.**
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

For more examples, see [Clamp Operator Examples](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/10_math/clamp).

```
AscendC::LocalTensor<half> dst, src;
uint32_t count = 512;
half min = 30;
half max = 60;
AscendC::Clamp(dst, src, min, max, count);
```

The result is as follows:

```
Input data (src):
[13, 78, 35, 95, 83,  2,  2, 95, 51, 73, 98,  3, 55, 32, 61,  2, 40, 26, 95, ... 63]
Input data (min):
[30]
Input data (max):
[60]
Output data (dst):
[30, 60, 35, 60, 60, 30, 30, 60, 51, 60, 60, 30, 55, 32, 60, 30, 40, 30, 60, ... 60]
```
