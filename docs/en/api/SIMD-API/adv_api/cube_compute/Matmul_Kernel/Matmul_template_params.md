# Matmul Template Parameters

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:35:37.175Z pushedAt=2026-10-27T11:27:19.548Z nl=2 -->

## Applicable Products

<!-- npu="950" id5 -->
- Ascend 950PR & 950DT products: Supported
<!-- end id5 -->
<!-- npu="A3" id6 -->
- Atlas A3 products: Supported
<!-- end id6 -->
<!-- npu="910b" id7 -->
- Atlas A2 products: Supported
<!-- end id7 -->
<!-- npu="310b" id8 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id8 -->
<!-- npu="310p" id9 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id9 -->
<!-- npu="910" id10 -->
- Atlas training products: Not supported
<!-- end id10 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Matmul_template_params_res.md#id1 -->

## Description

When creating a Matmul object, the following must be passed in:

-   Parameter type information of **A**, **B**, **C**, and **Bias**. The type information is defined by **MatmulType**, including the logical memory location, data format, data type, whether to transpose, data layout, and whether to enable L1 reuse.
-   **MatmulConfig** information (optional), used to configure the Matmul template information and related configuration parameters. If not configured, the default **Norm** template is used.

    <!-- npu="310b" id11 -->For the Atlas 200I/500 A2 Inference Product, only the default **Norm** template is currently supported.<!-- end id11 -->

<!-- npu="950,A3,910b,310p,310b" id20 -->
<a id="matmul-callback-func"></a>

-   **MatmulCallBackFunc** callback function information (optional), used to configure custom functions for copying cube A from GM to the L1 Buffer (A1), cube B from GM to the L1 Buffer (B1), and the computation result from the L0C Buffer (CO1) to GM. The following product models are currently supported:

    <!-- npu="950" id12 -->
    Ascend 950PR & 950DT Products
    <!-- end id12 -->
    <!-- npu="A3" id13 -->
    Atlas A3 products
    <!-- end id13 -->
    <!-- npu="910b" id14 -->
    Atlas A2 products
    <!-- end id14 -->

-   **MatmulPolicy** information (optional), used to configure the Matmul extensible module policy. If not configured, the default template policy is used. The following product models are currently supported:

    <!-- npu="950" id15 -->
    Ascend 950PR & 950DT products
    <!-- end id15 -->
    <!-- npu="A3" id16 -->
    Atlas A3 products
    <!-- end id16 -->
    <!-- npu="910b" id17 -->
    Atlas A2 products
    <!-- end id17 -->
    <!-- npu="310b" id18 -->
    Atlas 200I/500 A2 inference product
    <!-- end id18 -->
    <!-- npu="310p" id19 -->
    Atlas inference products AI Core
    <!-- end id19 -->
<!-- end id20 -->

## Prototype

The Matmul template parameters are as follows:
```
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE = C_TYPE, const auto& MM_CFG = CFG_NORM, class MM_CB = MatmulCallBackFunc<nullptr, nullptr, nullptr>, MATMUL_POLICY_DEFAULT_OF(MatmulPolicy)>
```

-   The A\_TYPE, B\_TYPE, and C\_TYPE type information is defined by [MatmulType](Matmul_usage.md#table1188045714378).
-   The **auto**-type parameter MM\_CFG (optional):
    -   Supports the **MatmulConfig** type:

        Matmul template information. For details, see [MatmulConfig](MatmulConfig.md).

    -   Supports the **MatmulApiStaticTiling** type:

        For the **MatmulApiStaticTiling** parameter description, see [Table 1](#table7939847143412).

        The **MatmulApiStaticTiling** structure contains a set of constant tiling parameters and a **MatmulConfig** structure. This type of parameter is defined as follows: call the template acquisition API described in the [MatmulConfig](MatmulConfig.md) chapter, specify the \(singleM, singleN, singleK, baseM, baseN, baseK\) parameters to obtain a custom template, and then pass the template to the [GetMatmulApiTiling](GetMatmulApiTiling.md) API to obtain the constant parameters. This constantization approach yields a set of constant parameters defined in the **MatmulApiStaticTiling** structure, which can optimize the Scalar computation in Matmul computation. The templates currently supported for defining the tiling parameters constantized as **MatmulApiStaticTiling** are: Norm, IBShare, and MDL templates. For the MxMatmul scenario, the templates supported for defining the tiling parameters constantized as **MatmulApiStaticTiling** are: Norm and MDL templates.

-   MM\_CB (optional), used to support different data movement-in and movement-out requirements and implement customized data movement-in and movement-out functions. For details, see [MatmulCallBackFunc](MatmulCallBackFunc.md).
-   MATMUL\_POLICY\_DEFAULT\_OF\(MatmulPolicy\) (optional), used to configure the policy of the Matmul extensible module. Currently, either not configuring this parameter (using the default template policy) or configuring one **MatmulPolicy** parameter is supported.

    MATMUL\_POLICY\_DEFAULT\_OF is defined as follows, used to simplify the type declaration of MATMUL\_POLICY. For details on how to use this template parameter, see [MatmulPolicy](MatmulPolicy.md).

    ```
    #define MATMUL_POLICY_DEFAULT_OF(DEFAULT)      \
    template <const auto& = MM_CFG, typename ...>  \
            class MATMUL_POLICY = AscendC::Impl::Detail::DEFAULT
    ```

## Parameters

**Table 1**  MatmulApiStaticTiling constant tiling parameters

<a name="table7939847143412"></a>

| Parameter | Data Type | Description |
| --- | --- | --- |
| M, N, Ka, Kb,<br><br>singleCoreM, singleCoreN, singleCoreK,<br><br>baseM, baseN, baseK,<br><br>depthA1, depthB1,<br><br>stepM, stepN, stepKa, stepKb,<br><br>isBias,<br><br>transLength,<br><br>iterateOrder,<br><br>dbL0A, dbL0B,<br><br>dbL0C,<br><br>shareMode,<br><br>shareL1Size,<br><br>shareL0CSize,<br><br>shareUbSize,<br><br>batchM,<br><br>batchN,<br><br>singleBatchM,<br><br>singleBatchN,<br><br>mxTypePara | int32_t | The meaning is consistent with the parameters of the same name in the [TCubeTiling](../Matmul_Tiling/TCubeTiling_struct.md) structure. The parameters in this structure are constant values after constantization. |
| cfg | [MatmulConfig](MatmulConfig.md#matmulconfig-params) | Parameter configuration of the Matmul template. |

## Return Value

None

## Constraints

None

## Examples

```
// User-defined callback function.
void DataCopyOut(
    const __gm__ void* gm, const LocalTensor<int8_t>& co1Local, const void* dataCopyOutParams, const uint64_t tilingPtr,
    const uint64_t dataPtr);
void CopyA1(
    const AscendC::LocalTensor<int8_t>& aMatrix, const __gm__ void* gm, int row, int col, int useM, int useK,
    const uint64_t tilingPtr, const uint64_t dataPtr);
void CopyB1(
    const AscendC::LocalTensor<int8_t>& bMatrix, const __gm__ void* gm, int row, int col, int useK, int useN,
    const uint64_t tilingPtr, const uint64_t dataPtr);

// Define the type information of the A, B, C, and Bias parameters to be passed in when creating the object.
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;

// Use the MDL template to create a Matmul instance.
AscendC::Matmul<aType, bType, cType, biasType, CFG_MDL> mm1;

AscendC::MatmulConfig mmConfig{false /* Do not use the Norm template. */,          true /* Use the BasicBlock template. */,
                               false /* Do not use the MDL template. */,           128 /* Length of the base block M-axis during Matmul computation. */,
                               128 /* Length of the base block N-axis during Matmul computation. */, 64 /* Length of the base block K-axis during Matmul computation. */};
mmConfig.enUnitFlag = false; // Disable the UnitFlag feature.
// Use the custom mmConfig to create a Matmul instance.
AscendC::Matmul<aType, bType, cType, biasType, mmConfig> mm2;

// Use the NORM template, the custom mmConfig, and the custom callback function to create a Matmul instance.
AscendC::Matmul<aType, bType, cType, biasType, CFG_NORM, AscendC::MatmulCallBackFunc<DataCopyOut, CopyA1, CopyB1>> mm3;
```
