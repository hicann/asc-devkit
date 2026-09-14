# Matmul Template Parameters

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:44:07.942Z pushedAt=2026-09-12T09:55:18.116Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported

- Atlas A3 training products/Atlas A3 inference products: Supported

- Atlas A2 training products/Atlas A2 inference products: Supported

- Atlas 200I/500 A2 inference product: Supported

- AI Core of Atlas inference products: Supported

- Vector Core of Atlas inference products: Not supported

- Atlas training products: Not supported

## Description

The following information must be passed when creating a Matmul object:

-   Parameter type information of **A**, **B**, **C**, and **Bias**, which is defined by **MatmulType**, including the logical memory location, data format, data type, whether to transpose, data layout, and whether to enable L1 reuse.

-   **MatmulConfig** information (optional), used to configure the Matmul template information and related configuration parameters. If not configured, the **Norm** template is used by default.

    For Atlas 200I/500 A2 inference product, only the default **Norm** template is currently supported.

<a id="matmulcallbackfunc"></a>

-   **MatmulCallBackFunc** callback function information (optional), used to configure custom functions for copying the left and right matrices from GM to A1/B1 and copying the computation result from CO1 to GM. The following product models are currently supported:

    Ascend 950PR/Ascend 950DT

    Atlas A3 training products/Atlas A3 inference products

    Atlas A2 training products/Atlas A2 inference products

-   **MatmulPolicy** information (optional), used to configure the **Matmul** extensible module policy. If not configured, the default template policy is used. The following product models are currently supported:

    Ascend 950PR/Ascend 950DT

    Atlas A3 training products/Atlas A3 inference products

    Atlas A2 training products/Atlas A2 inference products

    Atlas 200I/500 A2 inference products

    AI Core of Atlas inference products

## Prototype

The Matmul template parameters are as follows:

```
template <class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE = C_TYPE, const auto& MM_CFG = CFG_NORM, class MM_CB = MatmulCallBackFunc<nullptr, nullptr, nullptr>, MATMUL_POLICY_DEFAULT_OF(MatmulPolicy)>
```

-   The A\_TYPE, B\_TYPE, and C\_TYPE type information is defined via [MatmulType](matmul_usage.md#table1188045714378).

-   The auto-typed parameter MM\_CFG (optional):

    -   Supports the MatmulConfig type:

        Matmul template information. For details, see [MatmulConfig](MatmulConfig.md).

    -   Supports the MatmulApiStaticTiling type:

        For the MatmulApiStaticTiling parameter description, see [Table 1](#table7939847143412).

        The MatmulApiStaticTiling structure includes a set of constantized Tiling parameters and the MatmulConfig structure. This type of parameter is defined as follows: by calling the template acquisition API described in the [MatmulConfig](MatmulConfig.md) chapter, specify the \(singleM, singleN, singleK, baseM, baseN, baseK\) parameters to obtain a custom template; then pass this template to the [GetMatmulApiTiling](GetMatmulApiTiling.md) API to obtain the constantized parameters. This constantization method produces a set of constantized parameters defined in the MatmulApiStaticTiling structure, which can optimize the Scalar computation in Matmul computation. The templates currently supported for Tiling parameters defined as MatmulApiStaticTiling constantization are Norm, IBShare, and MDL templates. In the MxMatmul scenario, the templates supported for Tiling parameters defined as MatmulApiStaticTiling constantization are Norm and MDL templates.

-   MM\_CB (optional), used to support different copy-in and copy-out requirements and implement customized copy-in and copy-out functionality. For details, see [MatmulCallBackFunc](MatmulCallBackFunc.md).

-   MATMUL\_POLICY\_DEFAULT\_OF\(MatmulPolicy\) (optional), used to configure the policy of the Matmul extensible module. Currently supported: not configuring this parameter (using the default template policy) or configuring one MatmulPolicy parameter.


    MATMUL\_POLICY\_DEFAULT\_OF is defined as follows to simplify the type declaration of MATMUL\_POLICY. For details about how to use this template parameter, see [MatmulPolicy](MatmulPolicy.md).

 ```
    #define MATMUL_POLICY_DEFAULT_OF(DEFAULT)      \
    template <const auto& = MM_CFG, typename ...>  \
            class MATMUL_POLICY = AscendC::Impl::Detail::DEFAULT
    ```

## Parameters

**Table 1**  MatmulApiStaticTiling constant Tiling parameter description

<a name="table7939847143412"></a>

| Parameter | Data Type | Description |
| --- | --- | --- |
| M, N, Ka, Kb,<br><br>singleCoreM, singleCoreN, singleCoreK,<br><br>baseM, baseN, baseK,<br><br>depthA1, depthB1,<br><br>stepM， stepN，stepKa，stepKb,<br><br>isBias,<br><br>transLength,<br><br>iterateOrder,<br><br>dbL0A, dbL0B,<br><br>dbL0C,<br><br>shareMode,<br><br>shareL1Size,<br><br>shareL0CSize,<br><br>shareUbSize,<br><br>batchM,<br><br>batchN,<br><br>singleBatchM,<br><br>singleBatchN,<br><br>mxTypePara | int32_t | The meaning is the same as that of the parameters with the same names in the [TCubeTiling](../Matmul-Tiling/tcubetiling_structure.md) structure. The parameters in this structure are constants after constant compilation. |
| cfg | [MatmulConfig](MatmulConfig.md#matmulconfig-params) | Parameter configuration of the Matmul template. |

## Return Value

None

## Constraints

None

## Examples

```
// User-defined callback function.
void DataCopyOut(const __gm__ void *gm, const LocalTensor<int8_t> &co1Local, const void *dataCopyOutParams, const uint64_t tilingPtr, const uint64_t dataPtr);
void CopyA1(const AscendC::LocalTensor<int8_t> &aMatrix, const __gm__ void *gm, int row, int col, int useM, int useK, const uint64_t tilingPtr, const uint64_t dataPtr);
void CopyB1(const AscendC::LocalTensor<int8_t> &bMatrix, const __gm__ void *gm, int row, int col, int useK, int useN, const uint64_t tilingPtr, const uint64_t dataPtr);

// Define the type information of the A, B, C, and Bias parameters to be passed when creating an object.
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;

// Use the MDL template to create a Matmul instance.
AscendC::Matmul<aType, bType, cType, biasType, CFG_MDL> mm1;

AscendC::MatmulConfig mmConfig{false/*// Do not use the Norm template.*/, true/*// Use the BasicBlock template.*/, false/*// Do not use the MDL template.*/, 128/*// Length of the base block along the M axis during Matmul computation.*/, 128/*// Length of the base block along the N axis during Matmul computation.*/, 64/*// Length of the base block along the K axis during Matmul computation.*/};
mmConfig.enUnitFlag = false; // Disable the UnitFlag feature.
// Use the custom mmConfig to create a Matmul instance.
AscendC::Matmul<aType, bType, cType, biasType, mmConfig> mm2;

// Use the NORM template, the custom mmConfig, and the custom callback function to create a Matmul instance.
AscendC::Matmul<aType, bType, cType, biasType, CFG_NORM, AscendC::MatmulCallBackFunc<DataCopyOut, CopyA1, CopyB1>> mm3;
```