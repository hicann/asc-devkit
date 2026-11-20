# Init

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-02T06:20:37.546Z pushedAt=2026-11-18T04:52:48.635Z nl=2 -->

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
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id9 -->
<!-- npu="910" id10 -->
- Atlas training products: Not supported
<!-- end id10 -->
<!-- npu="x90" id1 -->
- Kirin X90 processor products: Supported
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030 processor products: Supported
<!-- end id2 -->
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
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas training products: Not supported
<!-- end id16 -->
<!-- npu="x90" id3 -->
- Kirin X90 processor products: Supported
<!-- end id3 -->
<!-- npu="9030" id4 -->
- Kirin 9030 processor products: Supported
<!-- end id4 -->
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

-   Tiling parameters are passed through the stack address.

    ```
    GET_TILING_DATA(tilingData, tiling);
    // ...
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm);
    mm.Init(&(tiling.cubeTilingData));
    ```

-   Tiling parameters are passed through the GM address.
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
        // In MIX mode, call only the REGIST_MATMUL_OBJ API, passing the GM address of the Tiling parameters. There is no need to call the Init API.
        REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &(tilingDataPtr->cubeTilingData));
        ```
