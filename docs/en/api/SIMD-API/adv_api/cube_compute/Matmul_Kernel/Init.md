# Init

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:27:36.077Z pushedAt=2026-10-27T11:25:23.729Z nl=2 -->

## Applicable Products

### API for Passing Tiling Parameters via Stack Address

<!-- npu="950" id5 -->
- Ascend 950PR&950DT products: Supported
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
- Vector Core of Atlas inference products: not supported
<!-- end id9 -->
<!-- npu="910" id10 -->
- Atlas training products: not supported
<!-- end id10 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Init_res.md#id2 -->

### API for Passing Tiling Parameters via GM Address

<!-- npu="950" id11 -->
- Ascend 950PR&950DT products: Supported
<!-- end id11 -->
<!-- npu="A3" id12 -->
- Atlas A3 products: Supported
<!-- end id12 -->
<!-- npu="910b" id13 -->
- Atlas A2 products: Supported
<!-- end id13 -->
<!-- npu="310b" id14 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas training products: Not supported
<!-- end id16 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Init_res.md#id1 -->

## Description

**Init** is mainly used to initialize the Tiling data in the **Matmul** object and partition resources based on the Tiling parameters. For details about the Tiling parameters, see [Matmul Tiling-side API](../Matmul_Tiling/Matmul_Tiling.md).

Developers can first initialize a single **Matmul** object without passing Tiling parameters through [REGIST\_MATMUL\_OBJ](REGIST_MATMUL_OBJ.md), and then pass Tiling parameters separately through the **Init** API to adjust the Tiling data in the **Matmul** object. For example, in scenarios where Tiling parameters are variable, **Init** can be called multiple times to reset the Tiling parameters.

In scenarios where Tiling changes are not required, it is recommended to use [REGIST\_MATMUL\_OBJ](REGIST_MATMUL_OBJ.md) to pass Tiling parameters for initialization.

## Prototype

-   Tiling parameters are passed through the stack address.

    ```
    __aicore__ inline void Init(const TCubeTiling* __restrict cubeTiling, TPipe* tpipe = nullptr)
    ```

-   Tiling parameters are passed through the GM address.

    ```
    __aicore__ inline void Init(const __gm__ TCubeTiling* gmCubeTiling, TPipe* tpipe = nullptr)
    ```

## Parameters

**Table 1** Parameter description of the API for passing Tiling parameters through the stack address

| Parameter | Input/Output | Description |
| --- | --- | --- |
| cubeTiling | Input | Matmul Tiling parameters. For the definition of the TCubeTiling structure, see [Table 1 TCubeTiling structure description](../Matmul_Tiling/TCubeTiling_struct.md#tcubetiling-struct).<br><br>The Tiling parameters can be obtained through the host-side [GetTiling](../Matmul_Tiling/GetTiling.md) API and passed to the kernel side for use. On the kernel side, call [GET_TILING_DATA](../../../basic_api/Kernel-Tiling/GET_TILING_DATA.md) to move the Tiling parameters to the stack space in the AI Core. This API passes the stack address of the TCubeTiling structure in the Tiling parameters. |
| tpipe | Input | Tpipe object. |

**Table 2** Parameter description of the API for passing Tiling parameters through the GM address

| Parameter | Input/Output | Description |
| --- | --- | --- |
| gmCubeTiling | Input | Matmul Tiling parameters. This parameter points to a memory address on the GM, and the data type at that address is the TCubeTiling structure. For the definition of the TCubeTiling structure, see [Table 1 TCubeTiling structure description](../Matmul_Tiling/TCubeTiling_struct.md#tcubetiling-struct).<br><br>The Tiling parameters can be obtained through the host-side [GetTiling](../Matmul_Tiling/GetTiling.md) API and passed to the kernel side for use. On the kernel side, call [GET_TILING_DATA_PTR_WITH_STRUCT](../../../basic_api/Kernel-Tiling/GET_TILING_DATA_PTR_WITH_STRUCT.md) to obtain the pointer to the Tiling parameters on the GM. This API passes the GM address of the TCubeTiling structure in the Tiling parameters. |
| tpipe | Input | Tpipe object. |

## Return Value

None

## Constraints

-   API for passing Tiling parameters through the stack address:

    None

-   API for passing Tiling parameters through the GM address:
    -   Only the [partial constantization](GetMatmulApiTiling.md#description) scenario of Matmul Tiling parameters is supported.
    -   CPU domain debugging is not supported.

## Examples

-   Tiling parameter passing via stack address

    ```
    GET_TILING_DATA(tilingData, tiling);
    // ...
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm);
    mm.Init(&(tiling.cubeTilingData));
    ```

-   Tiling parameter passing via GM address
    -   Pure Cube mode

        ```
        #define ASCENDC_CUBE_ONLY

        GET_TILING_DATA_PTR_WITH_STRUCT(MatmulCustomTilingData, tilingDataPtr, tiling);
        KERNEL_TASK_TYPE_DEFAULT(KERNEL_TYPE_AIC_ONLY);
        // ...
        REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm);
        mm.Init(&(tilingDataPtr->cubeTilingData));
        ```

    -   MIX mode

        ```
        GET_TILING_DATA_PTR_WITH_STRUCT(MatmulCustomTilingData, tilingDataPtr, tiling);
        KERNEL_TASK_TYPE_DEFAULT(KERNEL_TYPE_MIX_AIC_1_2);
        // ...
        // In MIX mode, only call the REGIST_MATMUL_OBJ API to pass the GM address of the Tiling parameters; there is no need to call the Init API.
        REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &(tilingDataPtr->cubeTilingData));
        ```
