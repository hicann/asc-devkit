# SetTensorB

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:58:20.545Z pushedAt=2026-10-27T11:32:43.665Z nl=2 -->

## Applicable Products

<!-- npu="950" id7 -->
- Ascend 950PR&950DT products: Supported
<!-- end id7 -->
<!-- npu="A3" id8 -->
- Atlas A3 products: Supported
<!-- end id8 -->
<!-- npu="910b" id9 -->
- Atlas A2 products: Supported
<!-- end id9 -->
<!-- npu="310b" id10 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id10 -->
<!-- npu="310p" id11 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id11 -->
<!-- npu="910" id12 -->
- Atlas training products: Not supported
<!-- end id12 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTensorB_res.md#id1 -->

## Description

Sets the right cube B of the cube multiplication.

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

<!-- npu="310p" id13 -->
The Atlas inference products AI Core does not support the SetTensorB\(SrcBT bScalar\) prototype.
<!-- end id13 -->

<!-- npu="310b" id14 -->
Atlas 200I/500 A2 inference product does not support the SetTensorB\(SrcBT bScalar\) API prototype.
<!-- end id14 -->

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| SrcBT | Data type of the operand. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| gm | Input | B cube. The type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). The **SrcBT** parameter indicates the data type of the B cube.<br><br>For the data types supported by different models, see [Data types supported by gm and rightMatrix](#li12616155731720). |
| rightMatrix | Input | B cube. The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported **TPosition** values are **TSCM**/**VECOUT**. The **SrcBT** parameter indicates the data type of the B cube.<br><br>For the data types supported by different models, see [Data types supported by gm and rightMatrix](#li12616155731720).<br><br>If the TSCM start address is set, the cube can be fully loaded by default and is already in TSCM, so the **Iterate** API does not need to perform GM->L1 Buffer (A1/B1) transfer. |
| bScalar | Input | Value set in the B cube. Scalar data can be passed in, and the scalar data is expanded into a tensor with the shape [1, K] to participate in cube multiplication, where all values of the tensor are the scalar value. For example, developers can set **bScalar** to 1 to implement the reduce sum operation of cube A in the K direction. The **SrcBT** parameter indicates the data type of the B cube.<br>For the data types supported by different models, see [Data types supported by bScalar](#li12616155731723). |
| isTransposeB | Input | Whether the B cube needs to be transposed.<br><br>Note:<br>If the **ISTRANS** parameter of the B cube **MatmulType** is set to **true**, this parameter can be either **true** or **false**, that is, transposed and non-transposed modes can be used alternately at runtime.<br>If the **ISTRANS** parameter of the B cube **MatmulType** is set to **false**, this parameter can only be set to **false**. If it is forcibly set to **true**, the precision will be abnormal.<br>For scenarios with non-half and non-bfloat16_t input types, to ensure that the L1 Buffer space calculation size on the Tiling side and the kernel function (Kernel) side remains consistent and the result precision is correct, the value of this parameter must be consistent with the [ISTRANS](Matmul_usage.md#p84551411817) parameter of the B cube **MatmulType** defined on the kernel function (Kernel) side and the [isTrans](../Matmul_Tiling/SetBType.md#p47369411111) parameter of the **SetBType()** API on the Tiling side. That is, the above three parameters must be set to **true** or **false** simultaneously. |

## Return Value

None

## Constraints

-   The address space size of the passed-in TensorB must be no less than singleK \* singleN.
<!-- npu="950,A3,910b,310p,310b" id25 -->
-   Data types supported by gm and rightMatrix<a id="li12616155731720"></a>

    <!-- npu="950" id15 -->
    Ascend 950PR&950DT products support the following data types: half, bfloat16_t, int8_t, float, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t.
    <!-- end id15 -->

    <!-- npu="A3" id16 -->
    Atlas A3 products support the following data types: int4b_t, half, bfloat16_t, int8_t, float.
    <!-- end id16 -->

    <!-- npu="910b" id17 -->
    Atlas A2 products support the following data types: int4b_t, half, bfloat16_t, int8_t, float.
    <!-- end id17 -->

    <!-- npu="310p" id18 -->
    The Atlas inference products AI Core supports the following data types: half, int8_t, float.
    <!-- end id18 -->

    <!-- npu="310b" id19 -->
    For Atlas 200I/500 A2 inference products, the supported data types are: half, bfloat16_t, int8_t, float.
    <!-- end id19 -->

-   Data types supported by bScalar<a id="li12616155731723"></a>

    <!-- npu="950" id20 -->
    For Ascend 950PR&950DT products, the supported data types are: half, float.
    <!-- end id20 -->

    <!-- npu="A3" id21 -->
    For Atlas A3 products, the supported data types are: half, float.
    <!-- end id21 -->

    <!-- npu="910b" id22 -->
    For Atlas A2 products, the supported data types are: half, float.
    <!-- end id22 -->

    <!-- npu="310p" id23 -->
    For the Atlas inference products AI Core, this parameter is not supported.
    <!-- end id23 -->

    <!-- npu="310b" id24 -->
    For Atlas 200I/500 A2 inference products, this parameter is not supported.
    <!-- end id24 -->

<!-- end id25 -->

<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTensorB_res.md#id2 -->

## Examples

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetTensorA(gm_a);
// Example 1: The right cube is in Global Memory.
mm.SetTensorB(gm_b, isTransposeB);
if (tiling.isBias) {
    mm.SetBias(gmBias);
}
mm.IterateAll(gm_c);
mm.End();
// Example 2: The right cube is in Local Memory.
mm.SetTensorB(local_a, isTransposeB);
// Example 3: Set scalar data.
mm.SetTensorB(scalar_a, isTransposeB);
```
