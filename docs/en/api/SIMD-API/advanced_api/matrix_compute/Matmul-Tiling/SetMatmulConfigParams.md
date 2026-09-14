# SetMatmulConfigParams

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T07:28:50.825Z pushedAt=2026-09-12T09:55:18.213Z -->

## Description

When computing tiling, this API is used to customize the **MatmulConfig** parameters in [Table 1](#table9646134355611). The functions corresponding to the parameters configured in this API must be consistent between tiling and Kernel. Therefore, the parameter values in this API must be consistent with the corresponding **MatmulConfig** parameter values on the Kernel side. For details about the **MatmulConfig** parameters, see [Table 2](../Matmul-Kernel/MatmulConfig.md#matmulconfig-params).

## Prototype

```
void SetMatmulConfigParams(int32_t mmConfigTypeIn = 1, bool enableL1CacheUBIn = false, ScheduleType scheduleTypeIn = ScheduleType::INNER_PRODUCT, MatrixTraverse traverseIn = MatrixTraverse::NOSET, bool enVecND2NZIn = false)
```

```
void SetMatmulConfigParams(const MatmulConfigParams& configParams)
```

## Parameters

**Table 1** Parameters

<a name="table9646134355611"></a>
| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| mmConfigTypeIn | Input | Sets the Matmul template type, which must be consistent with the template used to create the Matmul object. Currently only 0 or 1 is supported.<br>0: Norm template.<br>1: MDL template, Default Value: 1. |
| enableL1CacheUBIn | Input | Whether to enable the L1 cache UB computation block. Recommended scenarios: scenarios with frequent MTE3 and MTE2 pipeline serialization.<br>false: The L1 cache UB computation block is not enabled. Default Value: false.<br>true: The L1 cache UB computation block is enabled.<br><br>Atlas A3 training products/Atlas A3 inference products do not support this parameter.<br><br>Atlas A2 training products/Atlas A2 inference products do not support this parameter.<br><br>Atlas inference products AI Core supports this parameter.<br><br>Atlas 200I/500 A2 inference products do not support this parameter. |
| scheduleTypeIn | Input | Configures the Matmul data movement mode. Parameter values are as follows:<br>ScheduleType::INNER_PRODUCT: Default mode, which performs cyclic MTE1 data movement in the K direction.<br>ScheduleType::OUTER_PRODUCT: Performs cyclic MTE1 data movement in the M or N direction.<br>ScheduleType::N_BUFFER_33: Data movement mode of the [NBuffer33](../Matmul-Kernel/MatmulPolicy.md#li194081238103913) template. MTE2 moves 1x3 basic blocks of matrix A each time until all 3x3 basic blocks of matrix A are loaded into the L1 buffer. |
| traverseIn | Input | Loop iteration order of Matmul matrix computation, that is, the offset order in which, after one iteration computes a C matrix tile of size [baseM, baseN], the output automatically shifts to the C matrix position of the next iteration. Parameter values are as follows:<br><br>NOSET: 0, currently invalid.<br><br>FIRSTM: Offset in the M-axis direction first and then in the N-axis direction.<br><br>FIRSTN: Offset in the N-axis direction first and then in the M-axis direction. |
| enVecND2NZIn | Input | Whether to enable ND2NZ. |
| configParams | Input | Config-related parameters of the **MatmulConfigParams** type. The structure is defined in the following code. For parameter descriptions, see [Table 2](#table15780447181917). |

```
struct MatmulConfigParams
{
    int32_t mmConfigType;
    bool enableL1CacheUB;
    ScheduleType scheduleType;
    MatrixTraverse traverse;
    bool enVecND2NZ;
    MatmulConfigParams(int32_t mmConfigTypeIn = 1, bool enableL1CacheUBIn = false,
        ScheduleType scheduleTypeIn = ScheduleType::INNER_PRODUCT, MatrixTraverse traverseIn = MatrixTraverse::NOSET,
        bool enVecND2NZIn = false) {
        mmConfigType = mmConfigTypeIn;
        enableL1CacheUB = enableL1CacheUBIn;
        scheduleType = scheduleTypeIn;
        traverse = traverseIn;
        enVecND2NZ = enVecND2NZIn;
    }
};
```

**Table 2** Parameters in the MatmulConfigParams structure

<a name="table15780447181917"></a>
| Parameter Name | Description |
| --- | --- |
| mmConfigType | Sets the Matmul template type, which must be consistent with the template used to create the Matmul object. Currently only 0 or 1 is supported.<br>0: Norm template.<br>1: MDL template, Default Value: 1. |
| enableL1CacheUB | Whether to enable the L1 cache UB computation block. Recommended scenarios: scenarios with frequent MTE3 and MTE2 pipeline serialization.<br>false: The L1 cache UB computation block is not enabled. Default Value: false.<br>true: The L1 cache UB computation block is enabled. |
| scheduleType | Configures the Matmul data movement mode. Parameter values are as follows:<br>ScheduleType::INNER_PRODUCT: Default mode, which performs cyclic MTE1 data movement in the K direction.<br>ScheduleType::OUTER_PRODUCT: Performs cyclic MTE1 data movement in the M or N direction.<br>ScheduleType::N_BUFFER_33: Data movement mode of the [NBuffer33](../Matmul-Kernel/MatmulPolicy.md#li194081238103913) template. MTE2 moves 1x3 basic blocks of matrix A each time until all 3x3 basic blocks of matrix A are loaded into the L1 buffer. |
| traverse | Loop iteration order of Matmul matrix computation, that is, the offset order in which, after one iteration computes a C matrix tile of size [baseM, baseN], the output automatically shifts to the C matrix position of the next iteration. Parameter values are as follows:<br><br>NOSET: 0, currently invalid.<br><br>FIRSTM: Offset in the M-axis direction first and then in the N-axis direction.<br><br>FIRSTN: Offset in the N-axis direction first and then in the M-axis direction. |
| enVecND2NZ | Whether to enable ND2NZ. |

## Return Value

None

## Constraints

-   This API must be called before the [GetTiling](GetTiling.md) API.
-   If the Matmul object uses the NBuffer33 template policy, that is, MatmulPolicy is [NBuffer33MatmulPolicy](../Matmul-Kernel/MatmulPolicy.md#li194081238103913), before calling the [GetTiling](GetTiling.md) API to generate Tiling parameters, you must use this API to set the scheduleTypeIn parameter to ScheduleType::N\_BUFFER\_33 to enable the Tiling generation logic of the NBuffer33 template policy.

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
tiling.SetShape(1024, 1024, 1024);
tiling.SetOrgShape(1024, 1024, 1024);
tiling.SetBias(true);
tiling.SetBufferSpace(-1, -1, -1);
tiling.SetMatmulConfigParams(0);  // Additionally set.
// matmul_tiling::MatmulConfigParams configParams = {1, false, matmul_tiling::ScheduleType::OUTER_PRODUCT, matmul_tiling::MatrixTraverse::FIRSTM};
// tiling.SetMatmulConfigParams(configParams);
optiling::TCubeTiling tilingData;
int ret = tiling.GetTiling(tilingData);
```
