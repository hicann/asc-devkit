# Enabling Tiling Sinking<a name="ZH-CN_TOPIC_0000002130625528"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T14:38:33.472Z pushedAt=2026-11-03T07:05:13.434Z nl=2 -->

In static graph mode, you can optimize scheduling performance through **whole-graph sinking**. The complete computation graph is delivered to the device side at one time, and subsequent execution does not require host participation. The device completes the computation autonomously, thereby reducing host-device interaction overhead and improving execution efficiency. Some operators' tiling computation depends on specific runtime input values (**tiling value dependency**), so tiling parameters must be computed dynamically at runtime. For this scenario, the **tiling sinking** optimization can be adopted: the tiling computation is sunk to the AI CPU on the device side for execution, so that the entire computation is efficiently completed on the device side.

> [!NOTE] Note 
> - Tiling sinking operators compiled based on a new CANN package (which supports the tiling sinking feature) are incompatible with the runtime environment of an earlier CANN version (which does not support the tiling sinking feature).
> - Currently, only fused operators (fusion of vector computation and cube computation) support tiling sinking.
> - The tiling sinking feature supports only the following product models:
>    <!-- npu="A3" id1 -->
>    - Atlas A3 products
>    <!-- end id1 -->
>    <!-- npu="910b" id2 -->
>    - Atlas A2 products
>    <!-- end id2 -->
>    <!-- npu="950" id3 -->
>    - Ascend 950PR&950DT products: not supported yet
>    <!-- end id3 -->

The procedure for enabling tiling sinking for a custom operator is as follows. For the complete sample, see [Tiling Sinking Operator Sample](../../../../../../examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op).

In the tiling sinking scenario, the **op\_host** directory structure of the operator project is as follows. The tiling implementation file must be placed in a separate cpp file, which is **add\_custom\_tiling\_sink\_tiling.cpp** in the sample.

```
├── op_host
│   ├── add_custom_tiling_sink.cpp // Operator prototype definition, InferShape, and InferDataType implementation
│   ├── add_custom_tiling_sink_tiling.cpp // Tiling function implementation
│   ├── add_custom_tiling_sink_tiling.h // TilingData structure definition and Tiling function declaration
│   └── CMakeLists.txt
```

The following uses the AddCustom operator as an example to describe how to implement the key code files:

-   Declare the tiling implementation function in **add\_custom\_tiling\_sink\_tiling.h**.

    ```
    #ifndef ADD_CUSTOM_TILING_SINK_TILING_H
    #define ADD_CUSTOM_TILING_SINK_TILING_H
    #include "register/op_def_registry.h"
    namespace optiling {
        ge::graphStatus AddCustomSinkTilingFunc(gert::TilingContext* context); // Tiling function declaration
    } // namespace optiling
    #endif // ADD_CUSTOM_TILING_SINK_TILING_H
    ```

-   The operator prototype definition, InferShape, and InferDataType implementation file **add\_custom\_tiling\_sink.cpp** must include **add\_custom\_tiling\_sink\_tiling.h** to associate the tiling function with the operator prototype definition.

    Tiling sinking applies only to scenarios where the operator has a tiling value dependency (that is, InferShape does not depend on input values, and only the tiling computation requires input values) and the operator inputs are of a non-Const type. In this example, the input y is configured with a non-Const tiling value dependency through ValueDepend.

    ```
    #include "add_custom_tiling_sink_tiling.h" // Include the header file
    
    // ...
    
    namespace ops {
    class AddCustomTilingSink : public OpDef {
    public:
        explicit AddCustomTilingSink(const char *name) : OpDef(name)
        {
            this->Input("x")
                .ParamType(REQUIRED)
                .DataType({ge::DT_FLOAT})
                .Format({ge::FORMAT_ND});
            this->Input("y")
                .ParamType(REQUIRED)
                .DataType({ge::DT_FLOAT})
                .Format({ge::FORMAT_ND})
                .ValueDepend(OPTIONAL, DependScope::TILING); // Indicate that input y is a tiling value dependency
            this->Output("z")
                .ParamType(REQUIRED)
                .DataType({ge::DT_FLOAT})
                .Format({ge::FORMAT_ND});
    
            this->SetInferShape(ge::InferShape).SetInferDataType(ge::InferDataType);
    
            this->AICore().SetTiling(optiling::AddCustomSinkTilingFunc); // Associate the Tiling function with the operator prototype definition
            
            // Replace it with the actual Ascend AI processor model
            this->AICore().AddConfig("ascendxxx");
        }
    };
    OP_ADD(AddCustomTilingSink);
    } // namespace ops
    ```

-   Tiling function implementation file **add\_custom\_tiling\_sink\_tiling.cpp**

    -   In the tiling function, determine whether the current phase is compile time by checking whether the data pointer of the value-dependent InputTensor, that is, input y, is a null pointer. In the tiling sinking scenario, memory must be allocated for the operator at compile time, including the workspace it requires. To ensure high efficiency at run time, set a reasonable maximum workspace size at compile time based on the operator's execution requirements to avoid memory shortage or waste. The AddCustomTilingSink sample does not require a user workspace, so no setting is involved. A fixed value is set here only as an example.
    -   Complete the registration of the sinking tiling function: include the **device\_op\_impl\_registry.h** header file and use the DEVICE\_IMPL\_OP\_OPTILING macro for registration.

    ```
    #include "../../op_kernel/add_custom_tiling_sink/add_custom_tiling_sink_tiling_struct.h"
    #include "add_custom_tiling_sink_tiling.h"
    #include "register/device_op_impl_registry.h"
    #include "tiling/platform/platform_ascendc.h"
    namespace optiling {
    static constexpr uint32_t NUM_BLOCKS = 8;
    static constexpr uint32_t TILE_NUM = 3;
    static constexpr size_t MAX_WORKSPACE_SIZE = 32; // Maximum user workspace size required by the operator. The AddCustomTilingSink operator itself does not require user workspace, so a fixed value is set here only as an example.
    static constexpr size_t DEFAULT_WORKSPACE_SIZE = 0;
    ge::graphStatus AddCustomSinkTilingFunc(gert::TilingContext *context)
    {
        TilingSinkTilingData *tiling = context->GetTilingData<TilingSinkTilingData>();
        uint32_t totalLength = context->GetInputTensor(0)->GetShapeSize();
        context->SetSimdNumBlocks(NUM_BLOCKS);
        tiling->totalLength = totalLength;
        tiling->tileNum = TILE_NUM;
        size_t *currentWorkspace = context->GetWorkspaceSizes(1);
        auto platform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
        size_t sysWorkspaceSize = platform.GetLibApiWorkSpaceSize();
        currentWorkspace[0] = sysWorkspaceSize + DEFAULT_WORKSPACE_SIZE; // Set the runtime workspace size, which is the system workspace size plus the user workspace size.
        if (context->GetInputTensor(1) != nullptr && context->GetInputTensor(1)->GetData<float>() == nullptr) {
            // Determine whether the current phase is compile time by checking whether the Data pointer of the value-dependent InputTensor is a null pointer.
            // In the tiling sinking scenario, memory must be allocated for the operator at compile time, including the workspace it requires. To ensure runtime efficiency, set a reasonable maximum workspace size at compile time based on the operator's execution requirements to avoid insufficient memory or waste.
            currentWorkspace[0] = sysWorkspaceSize + MAX_WORKSPACE_SIZE; // Set the compile-time workspace size, which is the system workspace size plus the maximum user workspace size.
        }
        return ge::GRAPH_SUCCESS;
    }
    DEVICE_IMPL_OP_OPTILING(AddCustomTilingSink).Tiling(optiling::AddCustomSinkTilingFunc); // Register the tiling sinking function.
    } // namespace optiling
    ```

-   Operator kernel implementation

    Currently, tiling sinking supports only fused operators. To simulate a fused operator scenario, use the [KERNEL\_TASK\_TYPE\_DEFAULT](../../../../api/SIMD-API/basic_api/Kernel-Tiling/set_Kernel_type.md) API to force the operator to run in a mixed AIC and AIV scenario.

    ```
    extern "C" __global__ __aicore__ void add_custom_tiling_sink(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
    {
        REGISTER_TILING_DEFAULT(TilingSinkTilingData);
        GET_TILING_DATA(tiling_data, tiling);
        KERNEL_TASK_TYPE_DEFAULT(KERNEL_TYPE_MIX_AIC_1_2); // Force the operator to run in a mixed AIC and AIV scenario to simulate a fused operator scenario.
        if ASCEND_IS_AIC {
            return;
        }
        AscendC::KernelAdd op;
        op.Init(x, y, z, tiling_data.totalLength, tiling_data.tileNum);
        op.Process();
    }
    ```

-   Modify the compilation script **CMakeLists.txt** in the **op\_host** directory to add the tiling sinking compilation command. The specific code is as follows:

    ```
    npu_op_device_tiling_library(cust_opmaster SHARED  # Task name, fixed to cust_opmaster
        add_custom_tiling_sink/add_custom_tiling_sink_tiling.cpp  # Tiling function implementation source file
    )
    ```
