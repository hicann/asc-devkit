# SetBias

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-02T06:44:41.269Z pushedAt=2026-11-18T04:53:32.857Z nl=2 -->

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
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- npu="x90" id1 -->
- Kirin X90 processor products: Supported
<!-- end id1 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetBias_res.md#id1 -->

## Description

Sets the Bias for cube multiplication.

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
| biasGlobal | Input | Bias cube. The type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). For the data types supported by different models, see [Supported Data Types](#li12616155731720).<br><br>For the supported data type combinations of cube A, cube B, and Bias, see [Matmul Input/Output Data Type Combination Description](Matmul_usage.md#table1996113269499). In the MxMatmul scenario, for the supported data type combinations of cube A, cube B, and Bias, see [MatmulTypeWithScale Parameter Description](../../../../../guide/operator_practice/simd_operator_impl/matrix_advanced_api/feature_scenarios/mx_matmul_scenario.md#zh-cn_topic_0000002270097206_table14759942142014). |
| inputBias | Input | Bias cube. The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is TSCM/VECOUT. For the data types supported by different models, see [Supported Data Types](#li12616155731720).<br><br>For the supported data type combinations of cube A, cube B, and Bias, see [Matmul Input/Output Data Type Combination Description](Matmul_usage.md#table1996113269499). In the MxMatmul scenario, for the supported data type combinations of cube A, cube B, and Bias, see [MatmulTypeWithScale Parameter Description](../../../../../guide/operator_practice/simd_operator_impl/matrix_advanced_api/feature_scenarios/mx_matmul_scenario.md#zh-cn_topic_0000002270097206_table14759942142014). |

## Return Value

None

## Constraints

-   In Matmul Tiling computation, you must set the [isBias](../Matmul_Tiling/TCubeTiling_struct.md#p2051215216314) parameter in the TCubeTiling structure to 1, that is, enable Bias, before you can call this API to set the Bias cube.
-   The size of the Bias address space passed in must be no less than singleN.
<!-- npu="950" id13 -->
-   For Ascend 950PR&950DT products, when the memory logical position of the Bias cube is TSCM and the data type is float or int32\_t, the LocalTensor space of the Bias cube must be 64-byte aligned.
<!-- end id13 -->
<!-- npu="950,A3,910b,310p,310b,x90" id14 -->
-   Supported data types<a id="li12616155731720"></a>

    <!-- npu="950" id8 -->
    For Ascend 950PR&950DT products, the supported data types are half, bfloat16_t, float, and int32_t. Only when the data types of A and B are int8_t can the data type of Bias be set to int32_t.
    <!-- end id8 -->

    <!-- npu="A3" id9 -->
    For Atlas A3 products, the supported data types are half, float, and int32_t. Only when the data types of A and B are int8_t can the data type of Bias be set to int32_t.
    <!-- end id9 -->

    <!-- npu="910b" id10 -->
    Atlas A2 products support the following data types: half, float, and int32_t. Only when the data types of A and B are int8_t can the data type of Bias be set to int32_t.
    <!-- end id10 -->

    <!-- npu="310p" id11 -->
    The AI Core of Atlas inference products supports the following data types: half, float, and int32_t. For the prototype that includes the biasGlobal parameter, the data type of Bias can be set to int32_t only when the data types of A and B are int8_t.
    <!-- end id11 -->

    <!-- npu="310b" id12 -->
    Atlas 200I/500 A2 inference products support the following data types: half, float, and int32_t. Only when the data types of A and B are int8_t can the data type of Bias be set to int32_t.
    <!-- end id12 -->

    <!-- npu="x90" id15 -->
    Kirin X90 processor products support the following data types: half and int32_t. Only when the data types of A and B are int8_t can the data type of Bias be set to int32_t.
    <!-- end id15 -->

<!-- end id14 -->

<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetBias_res.md#id2 -->

## Examples

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(gmBias);  // Set Bias.
}
mm.IterateAll(gm_c);
mm.End();
```
