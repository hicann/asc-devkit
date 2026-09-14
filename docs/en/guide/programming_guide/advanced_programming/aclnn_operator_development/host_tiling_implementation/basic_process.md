# Basic Process<a name="ZH-CN_TOPIC_0000002365793490"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T11:59:57.543Z pushedAt=2026-09-02T11:32:11.035Z -->

The [SIMD Operator Implementation](../../../../operator_practice/simd_operator_impl/overview.md) section has introduced the implementation method of the host-side tiling core. This section focuses on the programming mode and API usage when integrating with the CANN framework.

In most cases, the Local Memory storage cannot fully accommodate the input and output of an operator. It is necessary to move a portion of the input for computation each time, then move it out, and then move the next portion of the input for computation, until the complete final result is obtained. This process of data splitting and block-wise computation is called **tiling**. The computation program that determines the related parameters of the data splitting algorithm (such as the block size moved each time and the total number of loops) based on information such as the operator shape is called **tiling implementation**.

After the tiling implementation is completed, the obtained related parameters of the tiling splitting algorithm are passed to the kernel side to guide the splitting of parallel data. Since all computations completed in the tiling implementation are scalar computations, which the AI Core is not good at, they are separated and executed on the host CPU.

**Figure 1** Input and output of tiling implementation<a name="fig12226122853316"></a>  
![](../../../../figures/tiling_implementation_input_and_output.png "Input and output of tiling implementation")

As shown in the preceding figure, tiling implementation is the process of determining the related parameters of the splitting algorithm based on information such as the operator shape. Here, information such as the operator shape can be understood as the **input of tiling implementation**, and the related parameters of the splitting algorithm can be understood as the **output of tiling implementation**. Both the input and output are carried by the parameters of the tiling function (the TilingContext\* context structure). That is, developers can obtain the input, output, and attribute information of the operator from the context structure, which is the **input of tiling implementation**. After tiling computation, the TilingData data structure (related parameters of the splitting algorithm), the numBlocks variable, the TilingKey used to select different kernel implementation branches, and the size of the operator workspace are obtained, which is the **output of tiling implementation**. These outputs are then set into the context structure.

The specific explanations of the concepts TilingData, numBlocks, TilingKey, and workspace are as follows:

-   **TilingData**: related parameters of the splitting algorithm, such as the block size moved each time and the total number of loops. It is stored through a structure and designed by developers.

    The TilingData structure definition supports both the single-structure definition method and structure nesting:

    -   Single-structure definition method, defined in a flat form:

        ```
        namespace optiling {
        BEGIN_TILING_DATA_DEF(MyAddTilingData)  // Declare the tiling structure name.
          TILING_DATA_FIELD_DEF(uint32_t, field1);   // Type and name of the structure member.
          TILING_DATA_FIELD_DEF(uint32_t, field2);
          TILING_DATA_FIELD_DEF(uint32_t, field3);
        END_TILING_DATA_DEF;
         
        REGISTER_TILING_DATA_CLASS(MyAdd, MyAddTilingData)  // Register the tiling structure to the operator.
        }
        ```

        The method of assigning values to the tiling structure members in the tiling implementation function is as follows:

        ```
        MyAddTilingData myTiling;
        myTiling.set_field1(1);
        myTiling.set_field2(2);
        ```

    -   Nested structures are supported:

        ```
        namespace optiling {
        BEGIN_TILING_DATA_DEF(MyStruct1)  // Declare the name of structure 1.
          TILING_DATA_FIELD_DEF(uint32_t, field1);   // Type and name of the structure member.
          TILING_DATA_FIELD_DEF(uint32_t, field2);   // Type and name of the structure member.
        END_TILING_DATA_DEF; 
        REGISTER_TILING_DATA_CLASS(MyStruct1Op, MyStruct1) // Register the structure to <op_type>Op.
        
        BEGIN_TILING_DATA_DEF(MyStruct2)  // Declare the name of structure 2.
          TILING_DATA_FIELD_DEF(uint32_t, field3);   // Type and name of the structure member.
          TILING_DATA_FIELD_DEF(uint32_t, field4);   // Type and name of the structure member.
        END_TILING_DATA_DEF;
        REGISTER_TILING_DATA_CLASS(MyStruct2Op, MyStruct2) // Register the structure to <op_type>Op.
        
        BEGIN_TILING_DATA_DEF(MyAddTilingData)  // Declare the name of the tiling structure.
          TILING_DATA_FIELD_DEF_STRUCT(MyStruct1, st1);   // Referenced structure of the structure member.
          TILING_DATA_FIELD_DEF_STRUCT(MyStruct2, st2);   // Referenced structure of the structure member.
        END_TILING_DATA_DEF;
        REGISTER_TILING_DATA_CLASS(MyAdd, MyAddTilingData)  // Register the tiling structure to the operator.
        }
        ```

        The tiling structure members are assigned values in the tiling implementation function as follows:

        ```
        MyAddTilingData myTiling;
        myTiling.st1.set_field1(1);
        myTiling.st1.set_field2(2);
        myTiling.st2.set_field3(3);
        myTiling.st2.set_field4(4);
        ```

-   <a name="li1153191243910"></a>**numBlocks**: specifies the number of cores on which the kernel function will be executed. For example, to compute 8 MB of data with 1 MB of data computed on each core, set numBlocks to 8. However, to fully utilize hardware resources, numBlocks is generally set to the number of cores on the hardware platform, and data is split based on the number of cores.

    >[!NOTE] Note
    >numBlocks is a logical core concept, with a value range of \[1,65535\]. To fully utilize hardware resources, it is generally set to the number of physical cores or a multiple thereof.
    >- For coupled mode and separated mode, numBlocks has some differences in its runtime meaning and setting rules, as described below:
    >    - Coupled mode: Because the Vector and Cube units are integrated together, numBlocks is used to set the number of AI Core instances to be started for execution, without distinguishing between Vector and Cube. The number of AI Cores can be obtained through [GetCoreNumAiv](../../../../../api/Utils-API/platform_info/PlatformAscendC/GetCoreNumAiv.md) or [GetCoreNumAic](../../../../../api/Utils-API/platform_info/PlatformAscendC/GetCoreNumAic.md).
    >    - Separated mode
    >        - For operators that involve only Vector computation, numBlocks is used to set the number of Vector (AIV) instances to be started. For example, if an AI processor has 40 Vector cores, it is recommended to set numBlocks to 40.
    >        - For operators that involve only Cube computation, numBlocks is used to set the number of Cube (AIC) instances to be started. For example, if an AI processor has 20 Cube cores, it is recommended to set numBlocks to 20.
    >        - For operators that involve fused Vector/Cube computation, AIV and AIC are started in combinations at startup, and numBlocks is used to set the number of combinations to be started. For example, if an AI processor has 40 Vector cores and 20 Cube cores, and one combination consists of 2 Vector cores and 1 Cube core, it is recommended to set numBlocks to 20. In this case, 20 combinations are started, that is, 40 Vector cores and 20 Cube cores. **Note: In this scenario, the number of logical cores set for numBlocks must not exceed the number of physical cores (2 Vector cores and 1 Cube core form one physical core).**
    >        - The numbers of AIC and AIV cores are obtained through the [GetCoreNumAic](../../../../../api/Utils-API/platform_info/PlatformAscendC/GetCoreNumAic.md) and [GetCoreNumAiv](../../../../../api/Utils-API/platform_info/PlatformAscendC/GetCoreNumAiv.md) APIs, respectively.
    >- If developers use the device resource limiting feature, the numBlocks set for the operator must not exceed the number of cores returned by the APIs (GetCoreNum/GetCoreNumAic/GetCoreNumAiv, etc.) provided by [PlatformAscendC](../../../../../api/Utils-API/platform_info/PlatformAscendC/PlatformAscendC.md). For example, if aclrtSetStreamResLimit is used to set the number of Vector cores at the stream level to 8, the GetCoreNumAiv API returns 8, and the numBlocks set for a vector operator must not exceed 8. Otherwise, resources of other streams will be preempted, causing the resource limit to become invalid.

-   **TilingKey (optional)**: TilingKey is a method used within an operator to distinguish different implementations by separating the kernel code. This method is similar to the C++ template mechanism. It reduces unnecessary icache misses and scalar overhead, helping optimize the performance of a single kernel call. Different kernel implementation branches can be identified by TilingKey. After TilingKey is set on the host side, the corresponding branch can be selected. For example, an operator may have different algorithm logic under different shapes. The kernel side can select different algorithm logic through TilingKey, and the host-side tiling algorithm also differs. The host and kernel sides are associated through the same TilingKey.

    Suppose there is the following kernel code:

    ```
    if (condition) {
      ProcessA();
    } else {
      ProcessB();
    }
    ```

    If the functions ProcessA and ProcessB are very large functions, the preceding code becomes even larger after compilation. However, only one branch is selected for each kernel run. When the code grows to a certain size (16-32K, varying by chip), the condition evaluation and jump cause icache misses. TilingKey can optimize this scenario by setting different TilingKeys 1 and 2 for the processing functions of the two kernels:

    ```
    if (TILING_KEY_IS(1)) {
      ProcessA();
    } else if (TILING_KEY_IS(2)) {
      ProcessB();
    }
    ```

    In this way, when the device kernel is compiled, the two TilingKeys are automatically identified and two kernel entry functions are compiled, and the condition evaluation is constant-folded. At the same time, it needs to work with the host tiling function: set TilingKey to 1 for the scenario that runs ProcessA, and set TilingKey to 2 for the scenario that runs ProcessB:

    ```
    static ge::graphStatus TilingFunc(gert::TilingContext* context)
    {
        // some code
        if (condition) {
            context->SetTilingKey(1);
        } else {
            context->SetTilingKey(2);
        }
        return ge::GRAPH_SUCCESS;
    }
    ```

    >[!NOTE] Note
    >During compilation, you can specify TilingKey by setting the [--tiling\_key](../operator_package_compilation/operator_project_compilation.md) compilation option. In this case, only the kernel code related to the specified TilingKey is compiled, which accelerates the compilation process.

-   **WorkspaceSize**: The workspace is a block of memory in the Global Memory on the device side. The workspace size can be set in the tiling function. After it is set: in the single-operator API execution scenario, you can call the first-stage API of the single-operator API to obtain the workspace size, and then apply for Global Memory of the corresponding size. In the graph mode scenario, the framework automatically allocates Global Memory of the corresponding size based on the set size. After the workspace is allocated, this workspace memory can be used during the operator kernel implementation.

    The workspace memory consists of two parts: the workspace memory required by the Ascend C APIs and the workspace memory used by the operator implementation (on demand).

    -   Workspace memory reserved for Ascend C APIs

        The APIs require some workspace memory as a cache during computation. Therefore, the operator tiling function needs to reserve workspace memory for the APIs. The reserved memory size is obtained through the [GetLibApiWorkSpaceSize](../../../../../api/Utils-API/platform_info/PlatformAscendC/GetLibApiWorkSpaceSize.md) API.

    -   Workspace memory used by the operator implementation (on demand)

        It needs to be allocated only when the operator implementation requires additional device buffer for data exchange or caching, and it is allocated based on the space required by the operator computation.

    The overall workspace buffer is the sum of the two parts described above. It is set in the tiling function as follows:

    ```
    auto workspaceSizes = context->GetWorkspaceSizes(1); // Use only one workspace block.
    workspaceSizes[0] = sysWorkspaceSize + usrWorkspaceSize;
    ```

## Basic Tiling Implementation Process<a name="section19693181516514"></a>

The following figure shows the tiling implementation development process:

**Figure 2**  Tiling development process<a name="fig593716586394"></a>  
![](../../../../figures/tiling_development_process.png "Tiling development process")

The following uses a simple Add operator as an example to describe the tiling implementation process. In this sample, the shape of the data to be processed can be evenly distributed to each core and aligned to the size of a datablock (32B).

**First**, complete the header file for the operator TilingData structure definition. The file is named "operator name_tiling.h" and is located in the **op_host** directory of the operator project. The sample code is as follows:

```
#ifndef ADD_CUSTOM_TILING_H
#define ADD_CUSTOM_TILING_H
#include "register/tilingdata_base.h"

namespace optiling {
BEGIN_TILING_DATA_DEF(TilingData)               // Register a tiling class, using the tiling name as the input parameter.
  TILING_DATA_FIELD_DEF(uint32_t, totalLength); // Add a tiling field for the total amount of data to be computed.
  TILING_DATA_FIELD_DEF(uint32_t, tileNum);     // Add a tiling field for the number of data blocks on each core.
END_TILING_DATA_DEF;
// Register the operator tilingdata class to the corresponding AddCustom operator.
REGISTER_TILING_DATA_CLASS(AddCustom, TilingData)
}
#endif // ADD_CUSTOM_TILING_H
```

The specific steps are as follows:

1.  Write the code framework. Add the `\#ifndef...` condition to prevent duplicate inclusion of the header file, and include the `register/tilingdata\_base.h` header file, which defines multiple macros for tilingdata registration. The sample code is as follows:

    ```
    #ifndef ADD_CUSTOM_TILING_H
    #define ADD_CUSTOM_TILING_H
    #include "register/tilingdata_base.h"
    
    namespace optiling {
    // Tiling structure definition and registration code
    // ...
    }
    #endif // ADD_CUSTOM_TILING_H
    ```

2.  Design the TilingData parameters. TilingData parameters are essentially parameters related to parallel data splitting. This sample operator uses two tiling parameters: `totalLength` and `tileNum`. `totalLength` indicates the amount of data to be computed, and `tileNum` indicates the number of data blocks into which the total computation on each core is divided. For example, after the `totalLength` parameter is passed to the kernel side, it can be divided by the number of cores participating in the computation to obtain the computation amount on each core, thereby completing the splitting of multi-core data.
3.  <a name="li5612175610125"></a>Define the TilingData structure. Use the `BEGIN_TILING_DATA_DEF` API to define a TilingData class, use the `TILING_DATA_FIELD_DEF` API to add the two fields `totalLength` and `tileNum` of TilingData, and use the `END_TILING_DATA_DEF` API to end the TilingData definition. For details about the related APIs, see the TilingData structure definition.

    ```
    BEGIN_TILING_DATA_DEF(TilingData)               // Register a tiling class, using the tiling name as the input parameter.
      TILING_DATA_FIELD_DEF(uint32_t, totalLength); // Add a tiling field for the total amount of data to be computed.
      TILING_DATA_FIELD_DEF(uint32_t, tileNum);     // Add a tiling field for the number of data blocks into which the total computation on each core is divided.
    END_TILING_DATA_DEF;
    ```

4.  Register the TilingData structure. Use the `REGISTER_TILING_DATA_CLASS` API to register the TilingData class and associate it with the custom operator. The first parameter of `REGISTER_TILING_DATA_CLASS` is `op_type` (operator type), which is `AddCustom` in this sample, and the second parameter is the class name of TilingData. For details about the `REGISTER_TILING_DATA_CLASS` API, see the TilingData structure registration.

    ```
    // Register the operator tilingdata class to the corresponding AddCustom operator.
    REGISTER_TILING_DATA_CLASS(AddCustom, TilingData)
    ```

**Then** implement the tiling function in the host implementation cpp file of the operator. The file is named "operator name.cpp" and is located in the `op_host` directory of the operator project. The prototype of the tiling function is fixed. It accepts a `TilingContext` as input, from which the shape pointers of the input and output can be obtained. The registered tiling function is called by the framework, and the `TilingContext` parameter is passed in when it is called. The sample code is as follows:

```
namespace optiling {
const uint32_t NUM_BLOCKS = 8;
const uint32_t TILE_NUM = 8;
static ge::graphStatus TilingFunc(gert::TilingContext *context)
{
    TilingData tiling;
    uint32_t totalLength = context->GetInputShape(0)->GetOriginShape().GetShapeSize();
    context->SetBlockDim(NUM_BLOCKS);
    tiling.set_totalLength(totalLength);
    tiling.set_tileNum(TILE_NUM);
    tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
    context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
    size_t *currentWorkspace = context->GetWorkspaceSizes(1);
    currentWorkspace[0] = 0;
    return ge::GRAPH_SUCCESS;
}
} // namespace optiling
```

The specific steps are as follows:

1.  Obtain the context of TilingContext, that is, the input parameter **gert::TilingContext\* context** of the tiling function.
2.  Set TilingData. After the TilingData class is defined in [Step 3](#li5612175610125), you can create an instance of the class and set the value of each field by calling the **set\_\{field\_name\}** method (where **field\_name** is the tiling field name defined in [Step 3](#li5612175610125)). After the tiling fields are set, call the **SaveToBuffer** method to serialize and save the TilingData instance.
    1.  Obtain the input/output shape information through the context. In this sample, the input shape size is obtained through the GetInputShape API of TilingContext.

        ```
        // Obtain the input shape information
        uint32_t totalLength = context->GetInputShape(0)->GetOriginShape().GetShapeSize();
        ```

    2.  Set TilingData. Set the field values of TilingData by calling the **set\_\{field\_name\}** method.

        ```
        // Define a specific instance using TilingData
        TilingData tiling;
        // Set TilingData
        tiling.set_totalLength(totalLength);
        tiling.set_tileNum(TILE_NUM);
        ```

    3.  Call the SaveToBuffer API of the TilingData class to serialize and save the data to the TilingContext context. The first parameter of SaveToBuffer is the start address of the buffer to be stored, and the second parameter is the length of the buffer. Obtain the address of the untyped TilingData by calling GetRawTilingData, and then obtain the data pointer through GetData as the start address of the buffer. Obtain the address of the untyped TilingData by calling GetRawTilingData, and then obtain the length of TilingData through GetCapacity as the length of the buffer. After the SaveToBuffer operation is complete, set the length of TilingData through SetDataSize. This length is obtained through the GetDataSize API of the TilingData class.

        ```
        // Serialize and save
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        ```

3. Call **SetBlockDim** to set **numBlocks**.

    ```
    context->SetBlockDim(NUM_BLOCKS);
    ```

4. (Optional) Call **SetTilingKey** to set **TilingKey**.

    ```
    context->SetTilingKey(1);
    ```

5. (Optional) Call **GetWorkspaceSizes** to obtain the workspace size pointer and set the size. This is only an example, where the workspace size is set to 0.

    ```
    size_t *currentWorkspace = context->GetWorkspaceSizes(1);
    currentWorkspace[0] = 0;
    ```
