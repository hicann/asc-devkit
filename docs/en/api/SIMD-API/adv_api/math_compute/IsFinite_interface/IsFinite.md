# IsFinite

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T03:09:18.585Z pushedAt=2026-10-29T02:15:02.774Z nl=2 -->

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
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Vector Core of Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/IsFinite_interface/IsFinite_res.md#id1 -->

## Description

Determines, element by element, whether the input floating-point number is neither NAN nor ±INF, and outputs a floating-point number or a Boolean value. For input data that is neither NAN nor ±INF, when the output is a floating-point type, the result at the corresponding position is 1 of that floating-point type; otherwise, it is 0. When the output is of the bool type, the result at the corresponding position is true; otherwise, it is false. The computation formula is as follows:

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
| dst | Output | Target operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The data type of the destination operand is the same as that of the source operand, or the data type of the destination operand is bool. For the currently supported data type combinations, see [Table 3](#tab3). |
| src | Input | Source operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| calCount | Input | Number of elements involved in the computation. |

**Table 3**  Supported input/output data type combinations<a name="tab3"></a>

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

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   For details about the operand address offset alignment requirements, see [General Description and Constraints](../../../general_description_and_constraints.md).

## Examples

For the complete operator example, see [IsFinite Operator Examples](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/10_math/isfinite).

```
// dstLocal: Tensor that stores the IsFinite computation result.
// srcLocal: Tensor that stores the input for the IsFinite computation.
// The number of elements involved in the computation is 8.
AscendC::IsFinite(dstLocal, srcLocal, 8);
```

The result example is as follows:

```
// The input data type is float, and the output data type is bool.
// Input data (src):
[1.0,+inf,3.0,4.0,nan,6.0,-inf,8.0]
// Output data (dst):
[true,false,true,true,false,true,false,true]
```
