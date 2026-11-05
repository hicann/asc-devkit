# Axpy

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T02:13:48.453Z pushedAt=2026-10-29T02:15:02.517Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Axpy_interface/Axpy_res.md#id1 -->

## Description

Each element in the source operand \(srcTensor\) is multiplied by the scalar and then added to the corresponding element in the destination operand \(dstTensor\). The computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002188267514.png)

![](../../../../figures/zh-cn_formulaimage_0000002188272234.png)

This API provides the same functionality as the basic API Axpy, except that it computes the instruction through a combination of Muls and Add, thereby delivering better precision.

## Prototype

```
template <typename T, typename U, bool isReuseSource = false>
__aicore__ inline void Axpy(const LocalTensor<T>& dstTensor, const LocalTensor<U>& srcTensor, const U scalarValue, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the destination operand. Supported data types: half and float. |
| U | Data type of the source operand. Supported data types: half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition values: VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition values: VECIN/VECCALC/VECOUT. |
| scalarValue | Input | Scalar value. Supported data types: half and float. The type of the scalar operand must be consistent with that of srcTensor. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition values: VECIN/VECCALC/VECOUT.<br><br>Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. The temporary space must be passed in by you through the sharedTmpBuffer parameter. For details about how to obtain the temporary space size BufferSize, see [GetAxpyMaxMinTmpSize](GetAxpyMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **The source operand and destination operand addresses must not overlap.**
-   sharedTmpBuffer must not overlap with the source operand or destination operand addresses.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   This API supports the following precision combinations:
    -   half precision combination: srcLocal data type = half; scalar data type = half; dstLocal data type = half; PAR = 128
    -   float precision combination: srcLocal data type = float; scalar data type = float; dstLocal data type = float; PAR = 64
    -   mix precision combination: srcLocal data type = half; scalar data type = half; dstLocal data type = float; PAR = 64

## Examples

```
// dstLocal: Tensor that stores the Axpy computation result.
// srcLocal: Tensor that stores the Axpy computation input.
// sharedTmpBuffer: Tensor that stores the temporary space during the Axpy computation.

// The operator input data type is half, and the number of elements to be computed is 512.
AscendC::Axpy(dstLocal, srcLocal, static_cast<half>(3.0), sharedTmpBuffer, 512);
```

The result example is as follows:

```
Input data (srcLocal):
[1. 2. 3. 4. 5. 6. ... 512.]
Input data (scalarValue): 3.0
Output data (dstLocal) initial value:
[0. 0. 0. 0. 0. 0. ... 0.]
After the Axpy computation, output data (dstLocal):
[3. 6. 9. 12. 15. 18. ... 1536.]
```
