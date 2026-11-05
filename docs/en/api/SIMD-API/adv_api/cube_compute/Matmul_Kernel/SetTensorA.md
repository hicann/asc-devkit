# SetTensorA

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:58:05.067Z pushedAt=2026-10-27T11:32:33.065Z nl=2 -->

## Applicable Products

<!-- npu="950" id3 -->
- Ascend 950PR&950DT products: Supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 products: Supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 products: Supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTensorA_res.md#id1 -->

## Description

Sets the left cube A of the cube multiplication.

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

<!-- npu="310p" id19 -->
The Atlas inference products AI Core does not support the SetTensorA\(SrcAT aScalar\) API prototype.
<!-- end id19 -->

<!-- npu="310b" id20 -->
Atlas 200I/500 A2 inference products do not support the SetTensorA\(SrcAT aScalar\) API prototype.
<!-- end id20 -->

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| gm | Input | Cube A. The type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). The SrcAT parameter represents the data type of cube A. For the data types supported by different models, see [Data types supported by gm and leftMatrix](#li12616155731720). |
| leftMatrix | Input | Cube A. The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is TSCM/VECOUT. The SrcAT parameter represents the data type of cube A. For the data types supported by different models, see [Data types supported by gm and leftMatrix](#li12616155731720)<br><br>If the TSCM start address is set, the cube can be fully loaded by default and is already located in TSCM, so the Iterate API does not need to perform GM->L1 Buffer (A1/B1) transfer. |
| aScalar | Input | Value set in cube A. Scalar data can be passed in, and the scalar data is expanded into a tensor of shape [1, K] to participate in cube multiplication, where all values in the tensor are the scalar value. For example, developers can set aScalar to 1 to implement the reduce sum operation of cube B in the K direction. The SrcAT parameter represents the data type of cube A. For the data types supported by different models, see [Data types supported by aScalar](#li12616155731723). |
| isTransposeA | Input | Whether cube A needs to be transposed.<br><br>Note:<br>If the ISTRANS parameter of MatmulType for cube A is set to true, this parameter can be either true or false, meaning that transposed and non-transposed modes can be used alternately at runtime;<br>If the ISTRANS parameter of MatmulType for cube A is set to false, this parameter can only be set to false. If it is forcibly set to true, precision will be abnormal;<br>For scenarios with non-half and non-bfloat16_t input types, to ensure that the L1 Buffer space calculation size on the Tiling side and the kernel function (Kernel) side remains consistent and the result precision is correct, the value of this parameter must be consistent with the [ISTRANS](Matmul_usage.md) parameter of MatmulType for cube A defined on the kernel function (Kernel) side and the [isTrans](../Matmul_Tiling/SetAType.md) parameter of the SetAType() API on the Tiling side. That is, the three parameters above must be set to true or false simultaneously.<!-- npu="310p" id21 --><br><br>For the Atlas inference products AI Core, when cube A is of the int8_t data type, transpose is not supported, i.e., this parameter cannot be set to true.<!-- end id21 --><!-- npu="910b" id22 --><br><br>For Atlas A2 products, when cube A is of the int4b_t data type, transpose is not supported, i.e., this parameter cannot be set to true.<!-- end id22 --><!-- npu="A3" id23 --><br><br>For Atlas A3 products, when cube A is of the int4b_t data type, transpose is not supported, i.e., this parameter cannot be set to true.<!-- end id23 --> |

## Return Value

None

## Constraints

-   The address space size of the passed TensorA must be no smaller than singleM \* singleK.
<!-- npu="950,A3,910b,310p,310b" id24 -->
-   Data types supported by gm and leftMatrix<a id="li12616155731720"></a>

    <!-- npu="950" id9 -->
    For Ascend 950PR&950DT products, the supported data types are: half, bfloat16_t, int8_t, float, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t.
    <!-- end id9 -->

    <!-- npu="A3" id10 -->
    For Atlas A3 products, the supported data types are: int4b_t, half, bfloat16_t, int8_t, float.
    <!-- end id10 -->

    <!-- npu="910b" id11 -->
    For Atlas A2 products, the supported data types are: int4b_t, half, bfloat16_t, int8_t, float.
    <!-- end id11 -->

    <!-- npu="310p" id12 -->
    For the Atlas inference products AI Core, the supported data types are: half, int8_t, float.
    <!-- end id12 -->

    <!-- npu="310b" id13 -->
    For Atlas 200I/500 A2 inference products, the supported data types are: half, bfloat16_t, int8_t, float.
    <!-- end id13 -->

-   Data types supported by aScalar<a id="li12616155731723"></a>

    <!-- npu="950" id14 -->
    For Ascend 950PR&950DT products, the supported data types are half and float.
    <!-- end id14 -->

    <!-- npu="A3" id15 -->
    For Atlas A3 products, the supported data types are half and float.
    <!-- end id15 -->

    <!-- npu="910b" id16 -->
    For Atlas A2 products, the supported data types are half and float.
    <!-- end id16 -->

    <!-- npu="310p" id17 -->
    This parameter is not supported on the Atlas inference products AI Core.
    <!-- end id17 -->

    <!-- npu="310b" id18 -->
    This parameter is not supported on Atlas 200I/500 A2 inference products.
    <!-- end id18 -->

<!-- end id24 -->

<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTensorA_res.md#id2 -->

## Examples

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
// Example 1: The left cube is in Global Memory.
mm.SetTensorA(gm_a, isTransposeA);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(gmBias);
}
mm.IterateAll(gm_c);
mm.End();
// Example 2: The left cube is in Local Memory.
mm.SetTensorA(local_a, isTransposeA);
// Example 3: Set scalar data.
mm.SetTensorA(scalar_a, isTransposeA);
```
