# Init

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:33:26.612Z pushedAt=2026-09-12T09:55:18.103Z -->

## Applicable Products

### API for Passing Tiling Parameters by Stack Address

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Supported
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

### Interface for Passing Tiling Parameters to the GM Address

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

**Init** is mainly used to initialize the Tiling data in the Matmul object and divide resources based on the Tiling parameters. For details about the Tiling parameters, see [matmul_tiling_class](../Matmul-Tiling/matmul_tiling_class.md).

You can first initialize a single Matmul object without passing Tiling parameters through [REGIST\_MATMUL\_OBJ](REGIST_MATMUL_OBJ.md), and then pass the Tiling parameters separately through the **Init** API to adjust the Tiling data in the Matmul object. For example, in scenarios where the Tiling parameters are variable, you can call **Init** multiple times to reset the Tiling parameters.

In scenarios where Tiling changes are not required, it is recommended to use [REGIST\_MATMUL\_OBJ](REGIST_MATMUL_OBJ.md) to pass the Tiling parameters for initialization.

## Prototype

-   **Tiling** parameter passing via stack address

    ```
    __aicore__ inline void Init(const TCubeTiling* __restrict cubeTiling, TPipe* tpipe = nullptr)
    ```

-   **Tiling** parameter passing via GM address

    ```
    __aicore__ inline void Init(const __gm__ TCubeTiling* gmCubeTiling, TPipe* tpipe = nullptr)
    ```

## Parameters

**Table 1**  Parameter description of the Tiling parameter passing via stack address API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| cubeTiling | Input | Matmul Tiling parameter. For the definition of the **TCubeTiling** structure, see [Table 1 TCubeTiling structure description](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct).<br><br>The Tiling parameter can be obtained through the host-side [GetTiling](../Matmul-Tiling/GetTiling.md) API and passed to the kernel side for use. On the kernel side, call [GET_TILING_DATA](../../../basic_api/Kernel-Tiling/GET_TILING_DATA.md) to move the Tiling parameter to the stack space in the AI Core. This API passes the stack address of the **TCubeTiling** structure in the Tiling parameter. |
| tpipe | Input | Tpipe object. |

**Table 2**  Parameter description of the Tiling parameter passing via GM address API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| gmCubeTiling | Input | Matmul Tiling parameter. This parameter points to a memory address on the GM, where the data type is the **TCubeTiling** structure. For the definition of the **TCubeTiling** structure, see [Table 1 TCubeTiling structure description](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct).<br><br>The Tiling parameter can be obtained through the host-side [GetTiling](../Matmul-Tiling/GetTiling.md) API and passed to the kernel side for use. On the kernel side, call [GET_TILING_DATA_PTR_WITH_STRUCT](../../../basic_api/Kernel-Tiling/GET_TILING_DATA_PTR_WITH_STRUCT.md) to obtain the pointer to the Tiling parameter on the GM. This API passes the GM address of the **TCubeTiling** structure in the Tiling parameter. |
| tpipe | Input | Tpipe object. |

## Return Value

None

## Constraints

-   API for passing Tiling parameters to the stack address:

    None

-   API for passing Tiling parameters to the GM address:
    -   Only the [partial constant](GetMatmulApiTiling.md#partial-constant) scenario of Matmul Tiling parameters is supported.
    -   CPU-domain debugging is not supported.

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
        // In MIX mode, only call the REGIST_MATMUL_OBJ API to pass the GM address of the Tiling parameter; the Init API does not need to be called.
        REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &(tilingDataPtr->cubeTilingData));
        ```
