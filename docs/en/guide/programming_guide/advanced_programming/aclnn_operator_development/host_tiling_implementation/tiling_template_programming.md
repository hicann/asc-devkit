# Tiling Template Programming<a name="ZH-CN_TOPIC_0000002399313281"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T12:06:46.733Z pushedAt=2026-09-02T12:04:57.519Z -->

In the TilingKey programming method described in [TilingKey Programming](./basic_process.md), TilingKeys are difficult to remember and understand because they are usually long numbers without clear meaning.

In scenarios involving multiple TilingKeys, developers rely on TilingKeys to manage kernel implementations, which leads to considerable complexity in both management and usage. To simplify this process, you can use template programming to replace the traditional TilingKey programming, thereby reducing the dependency on TilingKey numeric identifiers and making kernel management more intuitive and efficient. The usage steps are as follows. For a complete sample, see [Tiling Template Programming Sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op).

1.  <a name="li1949014102516"></a>In the op\_kernel directory of the [custom operator project](../../aclnn_operator_development/overview.md), add a header file that defines template parameters and template parameter combinations. In this example, the header file is named **tiling\_key\_add\_custom\_template.h**.

    -   The header file must include the template header file **ascendc/host\_api/tiling/template\_argument.h**.
    -   Define the template parameters **ASCENDC\_TPL\_ARGS\_DECL** and the template parameter combinations **ASCENDC\_TPL\_ARGS\_SEL** (that is, the templates that can be used). For details about the APIs, see [Template Parameter Definition](../../../../../api/Utils-API/Tiling_template_programming/template_parameter_definition.md).

    ```
    #include "ascendc/host_api/tiling/template_argument.h"

    // Template parameters
    ASCENDC_TPL_ARGS_DECL(AddCustomTemplate, // Operator OpType
    ASCENDC_TPL_DATATYPE_DECL(D_T_X, C_DT_FLOAT16, C_DT_FLOAT),  // Template parameter definition of the DataType type: data type of input parameter x, with a value range of float16/float32
    ASCENDC_TPL_DATATYPE_DECL(D_T_Y, C_DT_FLOAT16, C_DT_FLOAT),  // Template parameter definition of the DataType type: data type of input parameter y, with a value range of float16/float32
    ASCENDC_TPL_DATATYPE_DECL(D_T_Z, C_DT_FLOAT16, C_DT_FLOAT),  // Template parameter definition of the DataType type: data type of the output parameter z, with a value range of float16/float32
    ASCENDC_TPL_UINT_DECL(TILE_NUM, ASCENDC_TPL_8_BW, ASCENDC_TPL_UI_MIX, 2, 0, 2, 3, 5, 10, 12, 13, 9, 8),// Template parameter definition of the custom UINT type (unsigned integer): the template parameter is the number of blocks to split, and the encoding bit width is ASCENDC_TPL_8_BW, that is, 8 bits, indicating that the number of this template parameter does not exceed the range that 8 bits can express; ASCENDC_TPL_UI_MIX indicates that the value range is expressed in mixed mode, with two groups of data {0-2} and {3-5} and enumerated values 10, 12, 13, 9, and 8. The final result is {0, 1, 2, 3, 4, 5, 10, 12, 13, 9, 8}
    ASCENDC_TPL_BOOL_DECL(IS_SPLIT, 0, 1), // Template parameter definition of the custom bool type: the template parameter is the split flag, with a value range of 0 and 1, where 1 indicates splitting and 0 indicates no splitting
    );

    // Template parameter combination
    ASCENDC_TPL_SEL(
        ASCENDC_TPL_ARGS_SEL(
        ASCENDC_TPL_DATATYPE_SEL(D_T_X, C_DT_FLOAT16),
        ASCENDC_TPL_DATATYPE_SEL(D_T_Y, C_DT_FLOAT16),
        ASCENDC_TPL_DATATYPE_SEL(D_T_Z, C_DT_FLOAT16),
        ASCENDC_TPL_UINT_SEL(TILE_NUM, ASCENDC_TPL_UI_LIST, 1, 8),
        ASCENDC_TPL_BOOL_SEL(IS_SPLIT, 0, 1)
        ),
        ASCENDC_TPL_ARGS_SEL(
        ASCENDC_TPL_DATATYPE_SEL(D_T_X, C_DT_FLOAT),
        ASCENDC_TPL_DATATYPE_SEL(D_T_Y, C_DT_FLOAT),
        ASCENDC_TPL_DATATYPE_SEL(D_T_Z, C_DT_FLOAT),
        ASCENDC_TPL_UINT_SEL(TILE_NUM, ASCENDC_TPL_UI_LIST, 1, 8),
        ASCENDC_TPL_BOOL_SEL(IS_SPLIT, 0, 1)
        ),
    );
    ```

    When different template parameter combinations require different tiling structures, add the ASCENDC_TPL_TILING_STRUCT_SEL API to the corresponding ASCENDC_TPL_ARGS_SEL. For example:

    ```
    ASCENDC_TPL_SEL(
        ASCENDC_TPL_ARGS_SEL(
        ASCENDC_TPL_DATATYPE_SEL(D_T_X, C_DT_FLOAT16),
        ASCENDC_TPL_DATATYPE_SEL(D_T_Y, C_DT_FLOAT16),
        ASCENDC_TPL_DATATYPE_SEL(D_T_Z, C_DT_FLOAT16),
        ASCENDC_TPL_UINT_SEL(TILE_NUM, ASCENDC_TPL_UI_LIST, 1, 8),
        ASCENDC_TPL_BOOL_SEL(IS_SPLIT, 0),
        ASCENDC_TPL_TILING_STRUCT_SEL(SplitTilingData)
        ),
        ASCENDC_TPL_ARGS_SEL(
        ASCENDC_TPL_DATATYPE_SEL(D_T_X, C_DT_FLOAT16),
        ASCENDC_TPL_DATATYPE_SEL(D_T_Y, C_DT_FLOAT16),
        ASCENDC_TPL_DATATYPE_SEL(D_T_Z, C_DT_FLOAT16),
        ASCENDC_TPL_UINT_SEL(TILE_NUM, ASCENDC_TPL_UI_LIST, 1, 8),
        ASCENDC_TPL_BOOL_SEL(IS_SPLIT, 1)
        ),
    );
    ```

    The preceding API is used only to specify the tiling structure for the current template parameter combination on the host side. It is not passed to the kernel as a template parameter, and it does not affect the parameter order of ASCENDC_TPL_SEL_PARAM or GET_TPL_TILING_KEY. For a template parameter combination for which ASCENDC_TPL_TILING_STRUCT_SEL is not configured, when using this API, you must pass the default tiling structure registered by REGISTER_TILING_DEFAULT. On the kernel side, you can read the tiling data corresponding to the specified structure through GET_TILING_DATA_WITH_STRUCT(TilingStructName, tiling_data, tiling).

2.  On the host side, call the ASCENDC\_TPL\_SEL\_PARAM API to automatically generate and configure the TilingKey.

    -   The host implementation file includes the header file that defines the template parameters and template parameter combinations in [Step 1](#li1949014102516), as well as the header file that defines the tiling structure.
    -   Obtain the tiling structure pointer through GetTilingData and assign values to its member variables.
    -   Call the ASCENDC\_TPL\_SEL\_PARAM API to automatically generate and configure the TilingKey. The input parameters of ASCENDC\_TPL\_SEL\_PARAM are the specific values of the template parameters, and they must be passed in the same order as the template parameter declarations.

    ```
    #include "../../op_kernel/add_custom_template/add_custom_template_tiling.h"
    #include "../../op_kernel/add_custom_template/tiling_key_add_custom_template.h"

    namespace optiling {
    const uint32_t NUM_BLOCKS = 8;
    const uint32_t DEFAULT_TILE_NUM = 8;
    constexpr uint32_t MIN_LENGTH_FOR_SPLIT = 2048;

    static ge::graphStatus TilingFunc(gert::TilingContext *context)
    {
        uint32_t totalLength = context->GetInputShape(0)->GetOriginShape().GetShapeSize();
        ge::DataType dtype_x = context->GetInputDesc(0)->GetDataType();
        ge::DataType dtype_y = context->GetInputDesc(1)->GetDataType();
        ge::DataType dtype_z = context->GetOutputDesc(0)->GetDataType();
        uint32_t D_T_X = static_cast<uint32_t>(dtype_x);
        uint32_t D_T_Y = static_cast<uint32_t>(dtype_y);
        uint32_t D_T_Z = static_cast<uint32_t>(dtype_z);
        uint32_t TILE_NUM = 1;
        uint32_t IS_SPLIT = 0;
        if (totalLength < MIN_LENGTH_FOR_SPLIT) {
            IS_SPLIT = 0;
            TILE_NUM = 1;
        } else {
            IS_SPLIT = 1;
            TILE_NUM = DEFAULT_TILE_NUM;
        }
        context->SetBlockDim(NUM_BLOCKS);
        // Obtain the tiling structure pointer and assign values.
        TilingDataTemplate *tiling = context->GetTilingData<TilingDataTemplate>();
        tiling->totalLength = totalLength;
        // Configure the template parameter tilingkey.
        ASCENDC_TPL_SEL_PARAM(context, D_T_X, D_T_Y, D_T_Z, TILE_NUM, IS_SPLIT);
        size_t *currentWorkspace = context->GetWorkspaceSizes(1);
        currentWorkspace[0] = 0;
        return ge::GRAPH_SUCCESS;
    }
    }  // namespace optiling
    ```

3.  Kernel-side implementation

    -   The kernel implementation file includes the header file that defines the template parameters and template parameter combinations in [Step 1](#li1949014102516), as well as the header file that defines the tiling structure.
    -   Add a template to the kernel function to support the passing of template parameters. The parameter order must be consistent with the order in which the template parameters are declared.
    -   Register the tiling structure through REGISTER\_TILING\_DEFAULT, and parse the tiling data through GET\_TILING\_DATA\_WITH\_STRUCT.
    -   Select different kernel-side implementations by branching on the template parameters.

    ```
    #include "kernel_operator.h"
    #include "add_custom_template_tiling.h"
    #include "tiling_key_add_custom_template.h"
    ...
    // Implement the KernelAdd template class.
    ...

    template <typename D_T_X, typename D_T_Y, typename D_T_Z, int TILE_NUM, int IS_SPLIT>
     __global__ __aicore__ void add_custom_template(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z, __gm__ uint8_t* workspace, __gm__ uint8_t* tiling)
    {
        REGISTER_TILING_DEFAULT(TilingDataTemplate);
        GET_TILING_DATA_WITH_STRUCT(TilingDataTemplate, tiling_data, tiling);
        KernelAdd<D_T_X, D_T_Y, D_T_Z> op;
        op.Init(x, y, z, tiling_data.totalLength, TILE_NUM);

        if constexpr (IS_SPLIT == 0) {
            op.Process1();
        } else if constexpr (IS_SPLIT == 1) {
            op.Process2();
        }
    }
    ```

>[!NOTE] Note
>In the tiling template programming scenario, during compilation, you can use the [--kernel-template-input](../operator_package_compilation/operator_project_compilation.md) compilation option to configure compilation of only the kernel code related to the specified template parameter combinations, so as to accelerate the compilation process.
