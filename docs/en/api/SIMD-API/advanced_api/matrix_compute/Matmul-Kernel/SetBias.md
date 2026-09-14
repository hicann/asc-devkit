# SetBias

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T06:59:27.278Z pushedAt=2026-09-12T09:55:18.136Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported

- Atlas A3 training products/Atlas A3 inference products: Supported

- Atlas A2 training products/Atlas A2 inference products: Supported

- Atlas 200I/500 A2 inference products: Supported

- AI Core of Atlas inference products: Supported

- Vector Core of Atlas inference products: Not supported

- Atlas training products: Not supported

## Description

Sets the bias for matrix multiplication.

## Prototype

```
__aicore__ inline void SetBias(const GlobalTensor<BiasT>& biasGlobal)
```

```
__aicore__ inline void SetBias(const LocalTensor<BiasT>& inputBias)
```

## Parameters

**Table 1** Parameter Description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| biasGlobal | Input | Bias matrix. Type: [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). For the supported data types on different models, see [supported data types](#li12616155731720).<br><br>For the supported data type combinations of matrix A, matrix B, and Bias, see [Matmul input and output data type combination](matmul_usage.md#table1996113269499). In the MxMatmul scenario, for the supported data type combinations of matrix A, matrix B, and Bias, see [MatmulTypeWithScale parameters](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/guide/operator_practice/simd_operator_impl/matrix_advanced_api/feature_scenarios/mxmatmul_scenario.md#zh-cn_topic_0000002270097206_section2756107144914). |
| inputBias | Input | Bias matrix. Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), with supported TPosition of TSCM/VECOUT. For the supported data types on different models, see [supported data types](#li12616155731720).<br><br>For the supported data type combinations of matrix A, matrix B, and Bias, see [Matmul input and output data type combination](matmul_usage.md#table1996113269499). In the MxMatmul scenario, for the supported data type combinations of matrix A, matrix B, and Bias, see [MatmulTypeWithScale parameters](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/guide/operator_practice/simd_operator_impl/matrix_advanced_api/feature_scenarios/mxmatmul_scenario.md#zh-cn_topic_0000002270097206_section2756107144914). |

## Return Value

None.

## Constraints

-   In Matmul Tiling computation, you must set the [isBias](../Matmul-Tiling/tcubetiling_structure.md#p2051215216314) parameter in the **TCubeTiling** structure to 1, that is, enable **Bias**, before you can call this API to set the **Bias** matrix.

-   The size of the **Bias** address space passed in must be no smaller than **singleN**.

-   For Ascend 950PR/Ascend 950DT, when the memory logical location of the **Bias** matrix is TSCM and the data type is float or int32\_t, the **LocalTensor** space of the **Bias** matrix must be 64-byte aligned.

-   Supported data types<a id="li12616155731720"></a>

    For Ascend 950PR/Ascend 950DT, the supported data types are half, bfloat16_t, float, and int32_t. The **Bias** data type can be set to int32_t only when the data types of **A** and **B** are int8_t.

    For Atlas A3 training products/Atlas A3 inference products, the supported data types are half, float, and int32_t. The **Bias** data type can be set to int32_t only when the data types of **A** and **B** are int8_t.

    For Atlas A2 training products/Atlas A2 inference products, the supported data types are half, float, and int32_t. The **Bias** data type can be set to int32_t only when the data types of **A** and **B** are int8_t.

    For the AI Core of Atlas inference products, the supported data types are half, float, and int32_t. For prototypes that include the **biasGlobal** parameter, the **Bias** data type can be set to int32_t only when the data types of **A** and **B** are int8_t.

    For Atlas 200I/500 A2 inference products, the supported data types are half, float, and int32_t. The **Bias** data type can be set to int32_t only when the data types of **A** and **B** are int8_t.

## Examples

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(gmBias);  // Set the bias.
}
mm.IterateAll(gm_c);
mm.End();
```