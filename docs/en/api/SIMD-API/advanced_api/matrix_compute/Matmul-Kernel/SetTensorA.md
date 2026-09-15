# SetTensorA

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T07:04:37.142Z pushedAt=2026-09-12T09:55:18.149Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference product: Supported
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

Sets the left matrix A for matrix multiplication.

## Prototype

```
__aicore__ inline void SetTensorA(const GlobalTensor<SrcAT>& gm, bool isTransposeA = false)
```

```
__aicore__ inline void SetTensorA(const LocalTensor<SrcAT>& leftMatrix, bool isTransposeA = false)
```

```
__aicore__ inline void SetTensorA(SrcAT aScalar)
```

AI Core of Atlas inference products does not support the SetTensorA\(SrcAT aScalar\) API prototype.

Atlas 200I/500 A2 inference product does not support the SetTensorA\(SrcAT aScalar\) API prototype.

## Parameters

**Table 1** Parameter descriptions

| Parameter | Input/Output | Description |
| --- | --- | --- |
| gm | Input | Matrix A. The type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). The SrcAT parameter specifies the data type of matrix A. For the data types supported by different models, see [Data types supported by gm and leftMatrix](#li12616155731720). |
| leftMatrix | Input | Matrix A. The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are TSCM/VECOUT. The SrcAT parameter specifies the data type of matrix A. For the data types supported by different models, see [Data types supported by gm and leftMatrix](#li12616155731720)<br><br>If the TSCM base address is set, the matrix can be fully loaded by default and is already in TSCM, so the Iterate interface does not need to perform the GM->A1/B1 transfer. |
| aScalar | Input | The value set in matrix A. Scalar data is supported. The scalar data is expanded into a tensor of shape [1, K] that participates in the matrix multiplication, and all elements of the tensor are equal to the scalar value. For example, by setting aScalar to 1, developers can implement a reduce sum operation on matrix B along the K direction. The SrcAT parameter specifies the data type of matrix A. For the data types supported by different models, see [Data types supported by aScalar](#li12616155731723). |
| isTransposeA | Input | Whether matrix A needs to be transposed.<br><br>Note:<br>If the ISTRANS parameter of the MatmulType of matrix A is set to true, this parameter can be set to either true or false; that is, transposed and non-transposed modes can be used alternately at runtime.<br>If the ISTRANS parameter of the MatmulType of matrix A is set to false, this parameter can only be set to false. Setting it to true forcibly will cause precision anomalies.<br>For scenarios with non-half and non-bfloat16_t input types, to keep the L1 Buffer space calculation size consistent between the Tiling side and the Kernel side and ensure correct result precision, this parameter must be consistent with the [ISTRANS](matmul_usage.md#p84551411817) parameter of the MatmulType defined for matrix A on the Kernel side and the [isTrans](../Matmul-Tiling/SetAType.md) parameter of the SetAType() interface on the Tiling side; that is, the above three parameters must be set to true simultaneously or false simultaneously.<br><br>On the Atlas inference products AI Core, when matrix A is of the int8_t data type, transposition is not supported; that is, setting this parameter to true is not supported.<br><br>On Atlas A2 training products / Atlas A2 inference products, when matrix A is of the int4b_t data type, transposition is not supported; that is, setting this parameter to true is not supported.<br><br>On Atlas A3 training products / Atlas A3 inference products, when matrix A is of the int4b_t data type, transposition is not supported; that is, setting this parameter to true is not supported. |

## Return Value

None

## Constraints

-   The address space size of the passed **TensorA** must be no smaller than **singleM** \* **singleK**.
-   Data types supported by **gm** and **leftMatrix**<a id="li12616155731720"></a>

    Ascend 950PR/Ascend 950DT: Supported data types: half, bfloat16_t, int8_t, float, fp8_e4m3fn_t, fp8_e5m2_t, and hifloat8_t.

    Atlas A3 training products/Atlas A3 inference products: Supported data types: int4b_t, half, bfloat16_t, int8_t, and float.

    Atlas A2 training products/Atlas A2 inference products: Supported data types: int4b_t, half, bfloat16_t, int8_t, and float.

    AI Core of Atlas inference products: Supported data types: half, int8_t, and float.

    Atlas 200I/500 A2 inference products: Supported data types: half, bfloat16_t, int8_t, and float.

-   Data types supported by **aScalar**<a id="li12616155731723"></a>

    Ascend 950PR/Ascend 950DT: Supported data types: half and float.

    Atlas A3 training products/Atlas A3 inference products, supported data types: half, float.

    Atlas A2 training products/Atlas A2 inference products, supported data types: half, float.

    AI Core of Atlas inference products, this parameter is not supported.

    Atlas 200I/500 A2 inference products, this parameter is not supported.

## Examples

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
// Example 1: The left matrix is in global memory.
mm.SetTensorA(gm_a, isTransposeA);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(gmBias);
}
mm.IterateAll(gm_c);
mm.End();
// Example 2: The left matrix is in local memory.
mm.SetTensorA(local_a, isTransposeA);
// Example 3: Set scalar data.
mm.SetTensorA(scalar_a, isTransposeA);
```
