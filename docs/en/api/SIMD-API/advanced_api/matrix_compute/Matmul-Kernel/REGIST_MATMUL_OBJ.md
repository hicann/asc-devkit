# REGIST\_MATMUL\_OBJ

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:49:58.927Z pushedAt=2026-09-12T09:55:18.125Z -->

## Description

Initializes a **Matmul** object.

## Prototype

```
REGIST_MATMUL_OBJ(tpipe, workspace, ...)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **tpipe** | Input | **Tpipe** object. |
| **workspace** | Input | System **workspace** pointer. |
| ... | Input | Variable parameters, which pass in the Matmul object and its corresponding Tiling structure. The data type of the Tiling structure must be [TCubeTiling structure](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct).<br><br>The Tiling parameters can be obtained through the [GetTiling](../Matmul-Tiling/GetTiling.md) API on the host side and passed to the kernel side for use. |

## Return Value

None

## Constraints

-   In decoupled mode, this API must be called before the [InitBuffer](../../../basic_api/resource_management/TPipe/InitBuffer.md) API.

-   A maximum of 4 **Matmul** objects can be defined in a program.

-   In the internal implementation of **Matmul**, [CrossCoreSetFlag](../../../basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md) is used for cross-core synchronization control. Therefore, it is not recommended that you use **CrossCoreSetFlag** and **Matmul** at the same time, to avoid the risk of cross-core synchronization **flagId** conflicts. The **flagId** range occupied internally by the **Matmul** high-level API is related to the number of defined **Matmul** objects. Assuming N **Matmul** objects are defined, the **flagId** range occupied internally by the **Matmul** high-level API is \[0, 2 \* N - 1\]. **Matmul** supports defining up to 4 objects, in which case the occupied **flagId** range is \[0, 7\].

-   When there is only one **Matmul** object in the code, this API can be called without passing the **tiling** parameter, which can be passed separately through the [Init](Init-85.md) API.

-   When there are multiple **Matmul** objects in the code, each **Matmul** object must correspond one-to-one with its **tiling** parameter and be passed in sequence. For details, see the examples.

-   In decoupled mode, after this API is called, the AI Core does not proactively execute the API. The AI Core is triggered to execute only after the AI Vector Core executes the following APIs; other APIs are not executed on the AI Core.

    -   [matmul_kernel_api](matmul_kernel_api.md).

    -   [GMToL1 continuous data movement (DataCopy)](../../../basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_continuous.md), [GMToL1 high-dimensional tiling data movement (DataCopy)](../../../basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_highdim_split.md), and [GMToL1 ND2NZ movement with channel conversion (DataCopy)](../../../basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_ND2NZ.md).

## Examples

```
Tpipe pipe;
// Recommended: Initialize a single matmul object and pass the tiling parameter.
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
// Recommended: Initialize multiple matmul objects and pass the corresponding tiling parameters.
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm1, mm1tiling, mm2, mm2tiling, mm3, mm3tiling, mm4, mm4tiling);
// Initialize a single matmul object without passing the tiling parameter. Note that in this scenario, use the Init API to pass the tiling parameter separately. This approach separates the initialization of the matmul object from the tiling configuration. For example, in scenarios where the tiling is variable, the tiling can be reconfigured multiple times in this way.
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm);
mm.Init(&tiling);
```