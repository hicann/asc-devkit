# SetTensorB

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T07:04:55.079Z pushedAt=2026-09-12T09:55:18.148Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference product: Supported
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

Sets the right matrix B for matrix multiplication.

## Prototype

```
__aicore__ inline void SetTensorB(const GlobalTensor<SrcBT>& gm, bool isTransposeB = false)
```

```
__aicore__ inline void SetTensorB(const LocalTensor<SrcBT>& rightMatrix, bool isTransposeB = false)
```

```
__aicore__ inline void SetTensorB(SrcBT bScalar)
```

The AI Core of Atlas inference products does not support the SetTensorB\(SrcBT bScalar\) API prototype.

Atlas 200I/500 A2 inference product does not support the SetTensorB\(SrcBT bScalar\) API prototype.

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| SrcBT | Data type of the operand. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| gm | Input | Matrix B. The type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). The SrcBT parameter represents the data type of matrix B.<br><br>For the data types supported by different models, see [Data types supported by gm and rightMatrix](#li12616155731720). |
| rightMatrix | Input | Matrix B. The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is TSCM/VECOUT. The SrcBT parameter represents the data type of matrix B.<br><br>For the data types supported by different models, see [Data types supported by gm and rightMatrix](#li12616155731720).<br><br>If the TSCM start address is set, the matrix can be fully loaded by default and is already in TSCM, so the Iterate API does not need to perform GM->A1/B1 transfer. |
| bScalar | Input | Value set in matrix B. Scalar data can be passed in. The scalar data is expanded into a tensor with the shape [1, K] to participate in the matrix multiplication, and all values of the tensor are the scalar value. For example, you can set bScalar to 1 to implement the reduce sum operation of matrix A along the K direction. The SrcBT parameter represents the data type of matrix B.<br>For the data types supported by different models, see [Data types supported by bScalar](#li12616155731723). |
| isTransposeB | Input | Whether matrix B needs to be transposed.<br><br>Note:<br>If the ISTRANS parameter of the MatmulType of matrix B is set to true, this parameter can be true or false, that is, transposed and non-transposed can be used alternately at runtime;<br>If the ISTRANS parameter of the MatmulType of matrix B is set to false, this parameter can only be set to false. If it is forcibly set to true, the precision will be abnormal;<br>For scenarios where the input type is not half or bfloat16_t, to ensure that the L1 Buffer space calculation size on the Tiling side and the Kernel side remains consistent and the result precision is correct, the value of this parameter must be consistent with the [ISTRANS](matmul_usage.md#p84551411817) parameter of the MatmulType that defines matrix B on the Kernel side and the [isTrans](../Matmul-Tiling/SetBType.md#p47369411111) parameter of the SetBType() API on the Tiling side, that is, the above three parameters must be set to true or false at the same time. |

## Return Value

None

## Constraints

-   The address space size of the passed TensorB must be no smaller than **singleK** \* **singleN**.
-   Data types supported by **gm** and **rightMatrix**<a id="li12616155731720"></a>

    Ascend 950PR/Ascend 950DT: Supported data types are half, bfloat16_t, int8_t, float, fp8_e4m3fn_t, fp8_e5m2_t, and hifloat8_t.

    Atlas A3 training products/Atlas A3 inference products: Supported data types are int4b_t, half, bfloat16_t, int8_t, and float.

    Atlas A2 training products/Atlas A2 inference products: Supported data types are int4b_t, half, bfloat16_t, int8_t, and float.

    AI Core of Atlas inference products: Supported data types are half, int8_t, and float.

    Atlas 200I/500 A2 inference products: Supported data types are half, bfloat16_t, int8_t, and float.

-   Data types supported by **bScalar**<a id="li12616155731723"></a>

    Ascend 950PR/Ascend 950DT: Supported data types are half and float.

    Atlas A3 training products/Atlas A3 inference products, supported data types: half, float.

    Atlas A2 training products/Atlas A2 inference products, supported data types: half, float.

    AI Core of Atlas inference products: this parameter is not supported.

    Atlas 200I/500 A2 inference product: this parameter is not supported.

## Examples

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetTensorA(gm_a);
// Example 1: The right matrix is in Global Memory.
mm.SetTensorB(gm_b, isTransposeB);
if (tiling.isBias) {
    mm.SetBias(gmBias);
}
mm.IterateAll(gm_c);
mm.End();
// Example 2: The right matrix is in Local Memory.
mm.SetTensorB(local_a, isTransposeB);
// Example 3: Set scalar data.
mm.SetTensorB(scalar_a, isTransposeB);
```
