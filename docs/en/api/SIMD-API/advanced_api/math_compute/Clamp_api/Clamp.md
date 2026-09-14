# Clamp

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T13:45:54.121Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Replaces values greater than max (except nan) with max, values less than min with min, and keeps values that are less than or equal to max and greater than or equal to min unchanged as the output. When min is greater than max, all values except nan are replaced with max. min and max can be scalars or LocalTensor.

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
| config | Configuration related to the Clamp algorithm. This parameter is optional and of the ClampConfig type. Its definition is shown in the following code, where the parameter meanings are as follows:<br><br>isReuseSource: This parameter is reserved. Pass the default value false. |
| T | Data type of the operands. Supported data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t. |
| U | LocalTensor type or scalar type. The type is automatically deduced from the input parameter min. Developers do not need to configure this parameter, but must ensure that min satisfies the data type constraints.<br><br>Supported data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t. |
| S | LocalTensor type or scalar type. The type is automatically deduced from the input parameter max. Developers do not need to configure this parameter, but must ensure that max satisfies the data type constraints.<br><br>Supported data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t. |

```
struct ClampConfig {
    bool isReuseSource;
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| src | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| min | Input | Lower bound of the data. The type is scalar or LocalTensor. When the type is LocalTensor, the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type must be consistent with that of the destination operand. |
| max | Input | Upper bound of the data. The type is scalar or LocalTensor. When the type is LocalTensor, the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type must be consistent with that of the destination operand. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the destination operand is not supported.**
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

For more examples, see the [Clamp operator sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/10_math/clamp).

```
AscendC::LocalTensor<half> dst, src;
uint32_t count = 512;
half min = 30;
half max = 60;
AscendC::Clamp(dst, src, min, max, count);
```

The results are as follows:

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
