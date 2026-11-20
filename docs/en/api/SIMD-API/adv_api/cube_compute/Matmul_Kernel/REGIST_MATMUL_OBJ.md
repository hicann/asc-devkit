# REGIST\_MATMUL\_OBJ

<!-- md-trans-meta sourceCommit=b5af9c3c096d05d4b5351438b866f2318efe6aca translatedAt=2026-11-02T06:36:13.965Z pushedAt=2026-11-18T04:53:20.254Z nl=2 -->

## Description

Initializes a Matmul object.

## Prototype

```
REGIST_MATMUL_OBJ(tpipe, workspace, ...)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tpipe | Input | Tpipe object. |
| workspace | Input | System workspace pointer. |
| ... | Input | Variable arguments, which pass the Matmul object and its corresponding Tiling structure. The data type of the Tiling structure must be [TCubeTiling structure](../Matmul_Tiling/TCubeTiling_struct.md#tcubetiling-struct).<br><br>The Tiling parameters can be obtained through the [GetTiling](../Matmul_Tiling/GetTiling.md) API on the Host side and passed to the kernel side for use. |

## Return Value

None

## Constraints

-   In split mode, this API must be called before the [InitBuffer](../../../basic_api/resource_management/TPipe/InitBuffer.md) API.
-   A maximum of four Matmul objects can be defined in a program.
-   The internal implementation of Matmul uses [CrossCoreSetFlag](../../../basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md) and [CrossCoreWaitFlag](../../../basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md) for inter-core synchronization control. Therefore, when developers use CrossCoreSetFlag (or CrossCoreWaitFlag) together with Matmul, they must avoid flagId conflicts. For the complete description of the flagId occupied by each API (including differences across NPU architecture versions), see [Inter-Core Synchronization flagId Usage](../../../basic_api/sync_control/inter_core_sync/key_features.md#inter_core_sync_flagid_usage).
-   When there is only one Matmul object in the code, this API can be called without passing the tiling parameter, and the tiling parameter can be passed separately through the [Init](Init.md) API.
-   When there are multiple Matmul objects in the code, each Matmul object must correspond one-to-one with its tiling parameter and be passed in sequence. For details, see the call example.
-   In split mode, after this API is called, the AIC core does not actively execute the API. The AIC core is triggered to execute only after the AIV core executes the following APIs. Other APIs are not executed on the AIC core.
    -   [Matmul Kernel-Side APIs](Matmul_Kernel.md).
    -   [DataCopy (GMToL1 continuous data copy)](../../../basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_continuous.md), [DataCopy (GMToL1 high-dimensional split data copy)](../../../basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_highdim_split.md), [DataCopy (GMToL1 in-path conversion - ND2NZ copy)](../../../basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_ND2NZ.md).

## Examples

```
Tpipe pipe;
// Recommended: Initialize a single matmul object and pass the tiling parameters.
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
// Recommended: Initialize multiple matmul objects and pass the corresponding tiling parameters.
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm1, mm1tiling, mm2, mm2tiling, mm3, mm3tiling, mm4, mm4tiling);
// Initialize a single matmul object without passing tiling parameters. Note that in this scenario, use the Init API to pass the tiling parameters separately. This approach separates matmul object initialization from tiling configuration. For example, in scenarios where Tiling is variable, you can reconfigure Tiling multiple times this way.
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm);
mm.Init(&tiling);
```
