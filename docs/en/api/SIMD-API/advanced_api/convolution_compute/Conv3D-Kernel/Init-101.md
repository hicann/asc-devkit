# Init

<!-- md-trans-meta sourceCommit=e59e1725727133ce76073dba21b022a1b0c9fd56 translatedAt=2026-09-05T14:05:21.999Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

**Init** is mainly used to initialize the tiling data in the Conv3D object, divide resources based on the tiling parameters, and obtain the **Pipe** object declared by the user to complete memory allocation. For details about the tiling parameters, see [Conv3D Tiling APIs](../Conv3D-Tiling/conv3d_tiling_api.md).

## Prototype

```
__aicore__ inline void Init(const void* __restrict cubeTiling)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| cubeTiling | Input | Tiling parameters of the Conv3D object. For the tiling structure definition, see [TConv3DApiTiling Structure](../Conv3D-Tiling/tconv3dapitiling_structure.md).<br><br>The tiling parameters can be obtained through the Host-side [GetTiling](../Conv3D-Tiling/GetTiling-105.md) API and passed to the Kernel side for use. |

## Return Value

None

## Constraints

-   **TPipe** must be initialized before calling the **Init** API.
-   The **Init** API must be called before the **IterateAll** and **End** APIs, and it can be called only once. The calling sequence is as follows.

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

