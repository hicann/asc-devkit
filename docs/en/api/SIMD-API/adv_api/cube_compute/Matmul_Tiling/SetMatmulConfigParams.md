# SetMatmulConfigParams

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T13:18:41.041Z pushedAt=2026-10-27T11:39:43.448Z nl=2 -->

## Description

Used to customize the MatmulConfig parameters in [Table 1](#table9646134355611) when computing Tiling. The functions corresponding to the parameters configured in this API must remain consistent between Tiling and the kernel function (Kernel). Therefore, the parameter values in this API must be consistent with the corresponding MatmulConfig parameter values on the kernel function (Kernel) side. For details about MatmulConfig parameters, see [Table 2](../Matmul_Kernel/MatmulConfig.md#matmulconfig-params).

## Prototype

```
void SetMatmulConfigParams(int32_t mmConfigTypeIn = 1, bool enableL1CacheUBIn = false, ScheduleType scheduleTypeIn = ScheduleType::INNER_PRODUCT, MatrixTraverse traverseIn = MatrixTraverse::NOSET, bool enVecND2NZIn = false)
```

```
void SetMatmulConfigParams(const MatmulConfigParams& configParams)
```

## Parameters

**Table 1**  Parameters

<a name="table9646134355611"></a>

| Parameter | Input/Output | Description |
| --- | --- | --- |
| mmConfigTypeIn | Input | Sets the template type of Matmul, which must be consistent with the template used to create the Matmul object. Currently, only 0 or 1 is supported.<br>**0**: Represents the Norm template.<br>**1**: Represents the MDL template. The default value is 1. |
| enableL1CacheUBIn | Input | Configures whether to enable the L1 cache Unified Buffer (UB) computation block. Recommended scenario: scenarios with many serialized MTE3 and MTE2 pipelines.<br>**false**: Does not enable the L1 cache UB computation block. The default value is **false**.<br>**true**: Enables the L1 cache UB computation block.<!-- npu="A3" id1 --><br><br>Atlas A3 products do not support this parameter.<!-- end id1 --><!-- npu="910b" id2 --><br><br>Atlas A2 products do not support this parameter.<!-- end id2 --><!-- npu="310p" id3 --><br><br>The Atlas inference products AI Core supports this parameter.<!-- end id3 --><!-- npu="310b" id4 --><br><br>Atlas 200I/500 A2 inference products do not support this parameter.<!-- end id4 --> |
| scheduleTypeIn | Input | Configures the Matmul data movement mode. The parameter values are as follows:<br>**ScheduleType::INNER_PRODUCT**: Default mode, which performs cyclic MTE1 data movement in the K direction.<br>**ScheduleType::OUTER_PRODUCT**: Performs cyclic MTE1 data movement in the M or N direction.<br>**ScheduleType::N_BUFFER_33**: Data movement mode of the [NBuffer33](../Matmul_Kernel/MatmulPolicy.md#li194081238103913) template. MTE2 moves a 1x3 basic block of cube A each time until all 3x3 basic blocks of cube A are loaded into the L1 Buffer. |
| traverseIn | Input | Loop iteration order for Matmul cube operations, that is, the offset order in which the C cube position for the next iteration output is automatically offset after one iteration computes a C cube tile of size [baseM, baseN]. The parameter values are as follows:<br><br>**NOSET**: 0, currently invalid.<br><br>**FIRSTM**: Offsets in the M-axis direction first and then in the N-axis direction.<br><br>**FIRSTN**: Offsets in the N-axis direction first and then in the M-axis direction. |
| enVecND2NZIn | Input | Whether to enable ND2NZ. |
| configParams | Input | Config-related parameters, of the type **MatmulConfigParams**. The structure is defined in the code below. For parameter descriptions, see [Table 2](#table15780447181917). |

```
struct MatmulConfigParams {
    int32_t mmConfigType;
    bool enableL1CacheUB;
    ScheduleType scheduleType;
    MatrixTraverse traverse;
    bool enVecND2NZ;
    MatmulConfigParams(
        int32_t mmConfigTypeIn = 1, bool enableL1CacheUBIn = false,
        ScheduleType scheduleTypeIn = ScheduleType::INNER_PRODUCT, MatrixTraverse traverseIn = MatrixTraverse::NOSET,
        bool enVecND2NZIn = false)
    {
        mmConfigType = mmConfigTypeIn;
        enableL1CacheUB = enableL1CacheUBIn;
        scheduleType = scheduleTypeIn;
        traverse = traverseIn;
        enVecND2NZ = enVecND2NZIn;
    }
};
```

**Table 2**  Parameter description of the MatmulConfigParams structure

<a name="table15780447181917"></a>

| Parameter | Description |
| --- | --- |
| mmConfigType | Sets the template type of Matmul, which must be consistent with the template used to create the Matmul object. Currently, only 0 or 1 is supported.<br>**0**: Represents the Norm template.<br>**1**: Represents the MDL template. The default value is 1. |
| enableL1CacheUB | Configures whether to enable the L1 cache UB computation block. Recommended scenario: scenarios with many serialized MTE3 and MTE2 pipelines.<br>**false**: Does not enable the L1 cache UB computation block. The default value is **false**.<br>**true**: Enables the L1 cache UB computation block. |
| scheduleType | Configures the Matmul data movement mode. The parameter values are as follows:<br>**ScheduleType::INNER_PRODUCT**: Default mode, which performs cyclic MTE1 data movement in the K direction.<br>**ScheduleType::OUTER_PRODUCT**: Performs cyclic MTE1 data movement in the M or N direction.<br>**ScheduleType::N_BUFFER_33**: Data movement mode of the [NBuffer33](../Matmul_Kernel/MatmulPolicy.md#li194081238103913) template. MTE2 moves a 1x3 basic block of cube A each time until all 3x3 basic blocks of cube A are loaded into the L1 Buffer. |
| traverse | Loop iteration order for Matmul cube operations, that is, the offset order in which the C cube position for the next iteration output is automatically offset after one iteration computes a C cube tile of size [baseM, baseN]. The parameter values are as follows:<br><br>**NOSET**: 0, currently invalid.<br><br>**FIRSTM**: Offsets in the M-axis direction first and then in the N-axis direction.<br><br>**FIRSTN**: Offsets in the N-axis direction first and then in the M-axis direction. |
| enVecND2NZ | Whether to enable ND2NZ. |

## Return Value

None

## Constraints

-   This API must be called before the [GetTiling](GetTiling.md) API.
-   If the Matmul object uses the NBuffer33 template policy, that is, MatmulPolicy is [NBuffer33MatmulPolicy](../Matmul_Kernel/MatmulPolicy.md#li194081238103913), then before calling the [GetTiling](GetTiling.md) API to generate Tiling parameters, the **scheduleTypeIn** parameter must be set to ScheduleType::N\_BUFFER\_33 through this API to enable the Tiling generation logic of the NBuffer33 template policy.

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
tiling.SetMatmulConfigParams(0); // Additional setting.
// matmul_tiling::MatmulConfigParams configParams =
//     {1, false, matmul_tiling::ScheduleType::OUTER_PRODUCT, matmul_tiling::MatrixTraverse::FIRSTM};
// tiling.SetMatmulConfigParams(configParams);
optiling::TCubeTiling tilingData;
int ret = tiling.GetTiling(tilingData);
```
