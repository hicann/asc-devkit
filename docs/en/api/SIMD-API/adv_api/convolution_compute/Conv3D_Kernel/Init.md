# Init

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:12:36.509Z pushedAt=2026-10-29T02:14:34.150Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/convolution_compute/Conv3D_Kernel/Init_res.md#id1 -->

## Description

Init is mainly used to initialize the Tiling data in the Conv3D object, divide resources based on the Tiling parameters, and obtain the Pipe object declared by the user to complete memory allocation. For details about the Tiling parameters, see [Conv3D Tiling](../Conv3D_Tiling/Conv3d_Tiling.md).

## Prototype

```
__aicore__ inline void Init(const void* __restrict cubeTiling)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| cubeTiling | Input | Tiling parameters of the Conv3D object. For the Tiling struct definition, see [TConv3DApiTiling Structure](../Conv3D_Tiling/TConv3DApiTiling_struct.md).<br><br>The Tiling parameters can be obtained through the Host-side [GetTiling](../Conv3D_Tiling/GetTiling.md) API and passed to the kernel function (Kernel) side for use. |

## Return Value

None

## Constraints

-   Before calling the Init API, you must initialize TPipe.
-   The Init API must be called before the IterateAll and End APIs, and it can be called only once. The calling sequence is as follows.

    ```
    Init(...);
    ...
    IterateAll(...);
    End();
    ```

## Examples

```
TPipe pipe;
conv3dApi.Init(&tiling);
```
