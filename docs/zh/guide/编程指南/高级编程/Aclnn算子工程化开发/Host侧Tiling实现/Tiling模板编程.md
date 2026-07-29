# Tiling模板编程<a name="ZH-CN_TOPIC_0000002399313281"></a>

在[TilingKey编程](./基本流程.md)章节介绍的TilingKey编程方式中，TilingKey不易于记忆和理解，因为它们通常是较长又没有明确含义的数字。

在涉及多个TilingKey的场景中，开发者依赖TilingKey来管理kernel的实现，无论是在管理还是使用上都会遇到相当大的复杂性。为了简化这一过程，可以采用模板编程的方法来替代传统的TilingKey编程，从而减少对TilingKey数值标识的依赖，使kernel的管理更加直观和高效。使用步骤如下，完整样例请参考[Tiling模板编程样例](../../../../../../../examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op)。

1.  <a name="li1949014102516"></a>在[自定义算子工程](../../Aclnn算子工程化开发/概述.md)的op\_kernel目录下，新增定义模板参数和模板参数组合的头文件，本示例中头文件命名为tiling\_key\_add\_custom\_template.h。

    -   该头文件中需要包含模板头文件ascendc/host\_api/tiling/template\_argument.h。
    -   定义模板参数ASCENDC\_TPL\_ARGS\_DECL和模板参数组合ASCENDC\_TPL\_ARGS\_SEL（即可使用的模板）。具体API参考见[模板参数定义](../../../../../api/Utils-API/Tiling模板编程/模板参数定义.md)。

    ```
    #include "ascendc/host_api/tiling/template_argument.h"

    // 模板参数
    ASCENDC_TPL_ARGS_DECL(AddCustomTemplate, // 算子OpType
    ASCENDC_TPL_DATATYPE_DECL(D_T_X, C_DT_FLOAT16, C_DT_FLOAT),  // DataType类型的模板参数定义：输入参数x的数据类型，取值范围为float16/float32
    ASCENDC_TPL_DATATYPE_DECL(D_T_Y, C_DT_FLOAT16, C_DT_FLOAT),  // DataType类型的模板参数定义：输入参数y的数据类型，取值范围为float16/float32
    ASCENDC_TPL_DATATYPE_DECL(D_T_Z, C_DT_FLOAT16, C_DT_FLOAT),  // DataType类型的模板参数定义：输出参数z的数据类型，取值范围为float16/float32
    ASCENDC_TPL_UINT_DECL(TILE_NUM, ASCENDC_TPL_8_BW, ASCENDC_TPL_UI_MIX, 2, 0, 2, 3, 5, 10, 12, 13, 9, 8),// 自定义UINT类型（无符号整形）的模板参数定义：模板参数为切分的块数，编码位宽为ASCENDC_TPL_8_BW即8比特，表示该模板参数的个数不超过8比特能表达的范围；ASCENDC_TPL_UI_MIX表示通过混合模式表达取值范围，有2组的数据{0-2}、{3-5}和穷举值10、12、13、9、8，最后结果为{0, 1, 2, 3, 4, 5, 10, 12, 13, 9, 8}
    ASCENDC_TPL_BOOL_DECL(IS_SPLIT, 0, 1), // 自定义bool类型的模板参数定义：模板参数为是否切分标志位，取值范围为0和1，1表示切分，0表示不切分
    );

    // 模板参数组合
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

    当不同模板参数组合需要使用不同的Tiling结构体时，可以在对应的ASCENDC_TPL_ARGS_SEL中增加ASCENDC_TPL_TILING_STRUCT_SEL接口。例如：

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

    上述接口仅用于在host侧为当前模板参数组合指定Tiling结构体，不作为kernel的模板参数传入，同时也不影响ASCENDC_TPL_SEL_PARAM或GET_TPL_TILING_KEY的参数顺序。若未配置ASCENDC_TPL_TILING_STRUCT_SEL的模板参数组合，在使用该接口时，必须传入REGISTER_TILING_DEFAULT注册的默认Tiling结构体。kernel侧可通过GET_TILING_DATA_WITH_STRUCT(TilingStructName, tiling_data, tiling)读取指定结构体对应的tiling数据。

2.  host侧调用ASCENDC\_TPL\_SEL\_PARAM接口自动生成并配置TilingKey。

    -   host实现文件中包含[步骤1](#li1949014102516)中定义模板参数和模板参数组合的头文件，以及Tiling结构体定义头文件。
    -   通过GetTilingData获取Tiling结构体指针，并对其成员变量进行赋值。
    -   调用ASCENDC\_TPL\_SEL\_PARAM接口自动生成并配置TilingKey，ASCENDC\_TPL\_SEL\_PARAM输入参数为模板参数的具体值，传入时需要与模板参数声明处的顺序保持一致。

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
        context->SetSimdNumBlocks(NUM_BLOCKS);
        // 获取Tiling结构体指针并赋值
        TilingDataTemplate *tiling = context->GetTilingData<TilingDataTemplate>();
        tiling->totalLength = totalLength;
        // 模板参数tilingkey配置
        ASCENDC_TPL_SEL_PARAM(context, D_T_X, D_T_Y, D_T_Z, TILE_NUM, IS_SPLIT);
        size_t *currentWorkspace = context->GetWorkspaceSizes(1);
        currentWorkspace[0] = 0;
        return ge::GRAPH_SUCCESS;
    }
    }  // namespace optiling
    ```

3.  kernel侧实现

    -   kernel实现文件中包含[步骤1](#li1949014102516)中定义模板参数和模板参数组合的头文件，以及Tiling结构体定义头文件。
    -   核函数添加template模板，以便支持模板参数的传入，参数顺序需要与模板参数声明处的参数顺序保持一致。
    -   通过REGISTER\_TILING\_DEFAULT注册Tiling结构体，通过GET\_TILING\_DATA\_WITH\_STRUCT解析Tiling数据。
    -   通过对模板参数的分支判断，选择不同的kernel侧实现。

    ```
    #include "kernel_operator.h"
    #include "add_custom_template_tiling.h"
    #include "tiling_key_add_custom_template.h"
    ...
    // KernelAdd模板类实现
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

>[!NOTE]说明
>Tiling模板编程场景下，编译时，可以通过[--kernel-template-input](../算子包编译/算子工程编译.md)编译选项配置仅编译指定的模板参数组合相关的Kernel代码，用于加速编译过程。
