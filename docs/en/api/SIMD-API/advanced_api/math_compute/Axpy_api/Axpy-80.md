# Axpy

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T13:02:32.064Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Each element in the source operand \(srcTensor\) is multiplied by a scalar and then added to the corresponding element in the destination operand \(dstTensor\). The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002188267514.png)

![](../../../../figures/zh-cn_formulaimage_0000002188272234.png)

This API has the same function as the basic API **Axpy**, except that its instructions are computed through a combination of **Muls** and **Add**, thereby providing better precision.

## Prototype

```
template <typename T, typename U, bool isReuseSource = false>
__aicore__ inline void Axpy(const LocalTensor<T>& dstTensor, const LocalTensor<U>& srcTensor, const U scalarValue, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the destination operand. Supported data types are half and float. |
| U | Data type of the source operand. Supported data types are half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value **false**. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| scalarValue | Input | Scalar value. Supported data types are half and float. The type of the scalar operand must be consistent with **srcTensor**. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. The temporary space must be passed in by you through the **sharedTmpBuffer** input parameter. For how to obtain the temporary space size **BufferSize**, see [GetAxpyMaxMinTmpSize](GetAxpyMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   The precision combinations supported by this API are as follows:
    -   half precision combination: srcLocal data type = half; scalar data type = half; dstLocal data type = half; PAR = 128
    -   float precision combination: srcLocal data type = float; scalar data type = float; dstLocal data type = float; PAR = 64
    -   mix precision combination: srcLocal data type = half; scalar data type = half; dstLocal data type = float; PAR = 64

## Examples

```
// dstLocal: Tensor that stores the Axpy computation result.
// srcLocal: Tensor that stores the Axpy computation input.
// sharedTmpBuffer: Tensor that stores the temporary buffer during the Axpy computation.

// The input data type of the operator is half, and the number of elements involved in the computation is 512.
AscendC::Axpy(dstLocal, srcLocal, static_cast<half>(3.0), sharedTmpBuffer, 512);
```

The results are as follows:

```
Input data (srcLocal):
[1. 2. 3. 4. 5. 6. ... 512.]
Input data (scalarValue): 3.0
Initial value of output data (dstLocal):
[0. 0. 0. 0. 0. 0. ... 0.]
After the Axpy computation, output data (dstLocal):
[3. 6. 9. 12. 15. 18. ... 1536.]
```
