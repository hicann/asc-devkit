# IsFinite

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T15:41:40.390Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Determines, element by element, whether the input floating-point number is neither NAN nor ±INF, and outputs the result as a floating-point number or a Boolean value. For input data that is neither NAN nor ±INF, when the output is a floating-point type, the result at the corresponding position is 1 of that floating-point type; otherwise, it is 0. When the output is a bool type, the result at the corresponding position is true; otherwise, it is false. The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002218496437.png)

-   When the output is a floating-point type:

    ![](../../../../figures/zh-cn_formulaimage_0000002224017961.png)

-   When the output is a bool type:

    ![](../../../../figures/zh-cn_formulaimage_0000002224061861.png)

## Prototype

```
template<typename T, typename U>
__aicore__ inline void IsFinite(const LocalTensor<U>& dst, const LocalTensor<T>& src, uint32_t calCount)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the source operand. Supported data types: half, bfloat16_t, float. |
| U | Data type of the destination operand. Supported data types: bool, half, bfloat16_t, float. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the destination operand is the same as that of the source operand, or the data type of the destination operand is bool. For the currently supported data type combinations, see [Table 3](#table-3). |
| src | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| calCount | Input | Number of elements involved in the computation. |

<a id="table-3"></a>
**Table 3** Supported data type combinations for input and output

| srcDtype | dstDtype |
| --- | --- |
| half | half |
| half | bool |
| float | float |
| float | bool |
| bfloat16_t | bfloat16_t |
| bfloat16_t | bool |

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the destination operand is not supported.**
-   For operand address offset alignment requirements, see [General Description and Constraints](../../../general_description_and_constraints.md).

## Examples

For a complete operator sample, see the [isfinite operator sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/10_math/isfinite).

```
// dstLocal: Tensor that stores the IsFinite computation result.
// srcLocal: Tensor that stores the input for the IsFinite computation.
// The number of elements involved in the computation is 8.
AscendC::IsFinite(dstLocal, srcLocal, 8);
```

The results are as follows:

```
The input data type is float, and the output data type is bool.
Input data (src):
[1.0,+inf,3.0,4.0,nan,6.0,-inf,8.0]
Output data (dst):
[true,false,true,true,false,true,false,true]
```
