# GetCoreNum

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:14:24.022Z pushedAt=2026-09-12T09:55:18.171Z -->

## Description

Obtains the **NumBlocks** parameter used for multi-core partitioning.

## Prototype

-   **MultiCoreMatmulTiling** class

    ```
    int32_t GetCoreNum(int32_t &dim, int32_t &mDim, int32_t &nDim)
    ```

-   **BatchMatmulTiling** class

    ```
    int32_t GetCoreNum(int32_t &dim, int32_t &mDim, int32_t &nDim, int32_t &batchCoreM, int32_t &batchCoreN)
    ```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dim | Output | Number of cores needed for computing, dim = mDim * nDim. |
| mDim | Output | Number of cores needed for computing in the M direction. |
| nDim | Output | Number of cores needed for computing in the N direction. |
| batchCoreM | Output | Number of cores needed for computing in the batch M direction. Supported only by the BatchMatmulTiling class. |
| batchCoreN | Output | Number of cores needed for computing in the batch N direction. Supported only by the BatchMatmulTiling class. |

## Return Value

-1 indicates failure; 0 indicates success.

## Constraints

Call this API using the created Tiling object, and call it after the Tiling computation is complete ([GetTiling](GetTiling.md)).

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MultiCoreMatmulTiling tiling(ascendcPlatform);
tiling.SetDim(1);
tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
tiling.SetShape(1024, 1024, 1024);
tiling.SetSingleShape(1024, 1024, 1024);
tiling.SetOrgShape(1024, 1024, 1024);
tiling.SetBias(true);
tiling.SetBufferSpace(-1, -1, -1);
optiling::TCubeTiling tilingData;
int ret = tiling.GetTiling(tilingData);

// Obtain the NumBlocks used after multi-core partitioning.
int32_t dim, mDim, nDim;
int ret1 = tiling.GetCoreNum(dim, mDim, nDim);
```

