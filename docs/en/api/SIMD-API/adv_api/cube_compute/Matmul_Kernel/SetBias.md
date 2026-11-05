# SetBias

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:52:17.245Z pushedAt=2026-10-27T11:29:16.471Z nl=2 -->

## Applicable Products

<!-- npu="950" id2 -->
- Ascend 950PR&950DT products: Supported
<!-- end id2 -->
<!-- npu="A3" id3 -->
- Atlas A3 products: Supported
<!-- end id3 -->
<!-- npu="910b" id4 -->
- Atlas A2 products: Supported
<!-- end id4 -->
<!-- npu="310b" id5 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetBias_res.md#id1 -->

## Description

Sets the **Bias** for cube multiplication.

## Prototype

```
__aicore__ inline void SetBias(const GlobalTensor<BiasT>& biasGlobal)
```

```
__aicore__ inline void SetBias(const LocalTensor<BiasT>& inputBias)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **biasGlobal** | Input | Bias cube. Type: [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). For the data types supported by different models, see [supported data types](#li12616155731720).<br><br>For the supported data type combinations of cube A, cube B, and Bias, see [Matmul input/output data type combination description](Matmul_usage.md#table1996113269499). In the MxMatmul scenario, for the supported data type combinations of cube A, cube B, and Bias, see [MatmulTypeWithScale parameter description](../../../../../guide/operator_practice/simd_operator_impl/matrix_advanced_api/feature_scenarios/mx_matmul_scenario.md#zh-cn_topic_0000002270097206_table14759942142014). |
| **inputBias** | Input | Bias cube. Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), with supported TPosition of TSCM/VECOUT. For the data types supported by different models, see [supported data types](#li12616155731720).<br><br>For the supported data type combinations of cube A, cube B, and Bias, see [Matmul input/output data type combination description](Matmul_usage.md#table1996113269499). In the MxMatmul scenario, for the supported data type combinations of cube A, cube B, and Bias, see [MatmulTypeWithScale parameter description](../../../../../guide/operator_practice/simd_operator_impl/matrix_advanced_api/feature_scenarios/mx_matmul_scenario.md#zh-cn_topic_0000002270097206_table14759942142014). |

## Return Value

None

## Constraints

-   In Matmul Tiling computation, the [isBias](../Matmul_Tiling/TCubeTiling_struct.md#p2051215216314) parameter in the **TCubeTiling** structure must be set to 1, that is, Bias must be enabled before this API can be called to set the Bias cube.
-   The size of the Bias address space passed in must be no less than **singleN**.
<!-- npu="950" id13 -->
-   For Ascend 950PR&950DT products, when the memory logical location of the Bias cube is **TSCM** and the data type is **float** or int32\_t, the **LocalTensor** space of the Bias cube must be 64-byte aligned.
<!-- end id13 -->
<!-- npu="950,A3,910b,310p,310b" id14 -->
-   Supported data types<a id="li12616155731720"></a>

    <!-- npu="950" id8 -->
    For Ascend 950PR&950DT products, the supported data types are **half**, **bfloat16_t**, **float**, and **int32_t**. The Bias data type can be set to **int32_t** only when the data types of A and B are **int8_t**.
    <!-- end id8 -->

    <!-- npu="A3" id9 -->
    For Atlas A3 products, the supported data types are **half**, **float**, and **int32_t**. The Bias data type can be set to **int32_t** only when the data types of A and B are **int8_t**.
    <!-- end id9 -->

    <!-- npu="910b" id10 -->
    For Atlas A2 products, the supported data types are **half**, **float**, and **int32_t**. The Bias data type can be set to **int32_t** only when the data types of A and B are **int8_t**.
    <!-- end id10 -->

    <!-- npu="310p" id11 -->
    For the Atlas inference products AI Core, the supported data types are **half**, **float**, and **int32_t**. For the prototype that includes the **biasGlobal** parameter, the Bias data type can be set to **int32_t** only when the data types of A and B are **int8_t**.
    <!-- end id11 -->

    <!-- npu="310b" id12 -->
    For Atlas 200I/500 A2 Inference Products, the supported data types are **half**, **float**, and **int32_t**. The Bias data type can be set to **int32_t** only when the data types of A and B are **int8_t**.
    <!-- end id12 -->

<!-- end id14 -->

<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetBias_res.md#id2 -->

## Examples

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(gmBias);  // Set the Bias.
}
mm.IterateAll(gm_c);
mm.End();
```
