# Defining a Tiling Structure Using Standard C++ Syntax<a name="ZH-CN_TOPIC_0000002347983268"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T12:02:02.901Z pushedAt=2026-09-02T11:51:40.687Z -->

## Procedure<a name="section17812263817"></a>

When defining a tiling structure, you can use standard C++ syntax to define a **POD type (Plain Old Data)**, that is, a data type compatible with the C language. The specific steps are as follows. For a complete sample, see [Sample of Defining a Tiling Structure Using Standard C++ Syntax](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op).

1.  Define a tiling structure using C++ syntax.

    >[!NOTE] Note
    >The header file where this structure is defined should be placed in the **op\_kernel** directory of the operator project. Only files in this directory are packaged into the operator package for use in online compilation scenarios. If the file is placed in another directory, online compilation may fail because the related file cannot be found.

    When using the tiling structure of the high-level API, you can reference the predefined tiling structure in "kernel\_tiling/kernel\_tiling.h" through the AscendC::tiling namespace. The following code shows a simple example of defining a tiling structure.

    ```
    #ifndef ADD_CUSTOM_TILING_H
    #define ADD_CUSTOM_TILING_H
    #include <cstdint>

    struct AddCustomTilingData {
        uint32_t totalLength;
        uint32_t tileNum;
    };
    #endif // ADD_CUSTOM_TILING_H
    ```

2.  Assign values to the tiling structure in the host-side tiling function.

    -   Include the header file where the tiling structure is defined.
    -   Obtain the tiling structure pointer through GetTilingData and assign values to its member variables.

    ```
    #include "../../op_kernel/add_custom/add_custom_tiling.h"  // Include the header file where the tiling structure is defined.
    #include "register/op_def_registry.h"

    namespace optiling {
    const uint32_t NUM_BLOCKS = 8;
    const uint32_t TILE_NUM = 8;
    static ge::graphStatus TilingFunc(gert::TilingContext *context)
    {
        // Obtain the tiling structure pointer.
        AddCustomTilingData *tiling = context->GetTilingData<AddCustomTilingData>();
        uint32_t totalLength = context->GetInputShape(0)->GetOriginShape().GetShapeSize();
        context->SetBlockDim(NUM_BLOCKS);
        // Assign values to the member variables of tiling.
        tiling->totalLength = totalLength;
        tiling->tileNum = TILE_NUM;
        size_t *currentWorkspace = context->GetWorkspaceSizes(1);
        currentWorkspace[0] = 0;
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

3.  Register the tiling structure on the kernel side, parse the tiling data into the TilingData structure, and use it.

    -   The header file that defines the tiling structure must be included.
    -   Register the tiling structure through [REGISTER\_TILING\_DEFAULT](../../../../../api/SIMD-API/basic_api/Kernel-Tiling/REGISTER_TILING_DEFAULT.md) or [REGISTER\_TILING\_FOR\_TILINGKEY](../../../../../api/SIMD-API/basic_api/Kernel-Tiling/REGISTER_TILING_FOR_TILINGKEY.md); parse the tiling data into the TilingData structure through [GET\_TILING\_DATA](../../../../../api/SIMD-API/basic_api/Kernel-Tiling/GET_TILING_DATA.md) and use it. REGISTER\_TILING\_DEFAULT is also used to identify that the TilingData structure is defined using standard C++ syntax.

    ```
    #include "kernel_operator.h"
    #include "add_custom_tiling.h"  // Include the header file that defines the tiling structure.
    ...
    // Implement the KernelAdd class.
    ...

    extern "C" __global__ __aicore__ void add_custom(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z, __gm__ uint8_t* workspace, __gm__ uint8_t* tiling)
    {
        REGISTER_TILING_DEFAULT(AddCustomTilingData);
        GET_TILING_DATA(tilingData, tiling);
        KernelAdd op;
        op.Init(x, y, z, tilingData.totalLength, tilingData.tileNum);
        op.Process();
    }
    ```

## Advantages of Defining Tiling Structures Using Standard C++ Syntax<a name="section92385314106"></a>

Compared with the method of using macros such as BEGIN\_TILING\_DATA\_DEF for definition, this method not only better aligns with the development habits of C++ developers, but also provides strong flexibility.

-   Supports the bool type, arrays, structure arrays, and list initialization.

    ```
    class A {
    public:
        bool xxx;
        uint32_t xxx[2][128] = {0};
    };
    
    class B {
    public:
        bool xxx = false;
        uint8_t xxx[2][2]{0};
        A[10];
    };
    ```

-   Different operators can define tiling structures with the same name but different structures, and they can be distinguished by having each operator reference its corresponding header file. This method allows each operator to use a tiling structure definition that meets its own requirements without conflicting with other operators.

    In contrast, when using macros such as BEGIN\_TILING\_DATA\_DEF to define tiling structures with the same name but different structures, because these structures are registered in the global tiling structure management variable, accessing them later by structure name may fail to accurately obtain the tiling structure actually used by the current operator, resulting in undefined behavior.

    Operator A:

    ```
    class TilingData {
    public:
        uint32_t length;
    };
    ```

    Operator B:

    ```
    class TilingData {
    public:
        uint32_t length;
        uint16_t coreNum;
    };
    ```

-   Supports custom tiling assignment. You can assign values directly by accessing the member variables of the tiling structure, or customize a tiling assignment function. (With the macro definition method, you can only assign/access values through the **set\_xx**/**get\_xx** methods generated by the framework.)

    Tiling structure definition:

    ```
    class TilingData {
    public:
        uint32_t xxx1;
        uint32_t xxx2;
        uint8_t xxx3;
        bool xxx4;
    };
    ```

    Host-side tiling function:

    ```
    #include "../op_kernel/xxx_custom_tiling.h"  // Include the header file that defines the tiling structure file that defines the tiling structure.
    ...
    
    namespace optiling {
    static void ComputeTiling(TilingData* tiling, ...)
    {
        // Calculate the tiling logic.
        ...
        tiling->xxx1 = xxx;
        tiling->xxx2 = xxx;
        tiling->xxx3 = xxx;
        tiling->bool = xxx;
    }
    static ge::graphStatus TilingFunc(gert::TilingContext *context)
    {    
        ...
        TilingData *tiling = context->GetTilingData<TilingData>();
        ...
        ComputeTiling(tiling, ...)
        ...
    
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

## Constraints<a name="section318444831016"></a>

The following constraints apply when you use standard C++ syntax to define a tiling structure:

-   Member functions cannot be defined in a tiling structure. This is because member functions differ between the device side and the host side (functions on the device side require the \_\_aicore\_\_ modifier), while the tiling structure is shared by both the device side and the host side, which causes problems during compilation or execution:

    ```
    class TilingData {
    public:
        uint32_t xxx;
    
        __aicore__ funcA() { ... }  // Error: The __aicore__ modifier is not supported during compilation on the host side, causing a compilation error.
        void func() { ... }         // Error: The __aicore__ modifier is missing on the device side, so the function cannot be executed.
    };
    ```

-   Pointer and reference types are not supported for member variables of a tiling structure. Such data types cause exceptions when data is parsed from the host side to the device side:

    ```
    class TilingData {
    public:
        uint32_t* totalLength; // The pointer scenario is not supported because the host cannot pass a pointer to the device.
        uint32_t& tileNum;       // The reference scenario is not supported because the host cannot pass a pointer to the device.
    };
    ```

-   A tiling structure supports only POD types. It does not support object-oriented features such as virtual functions and virtual inheritance, nor does it support template classes:

    ```
    class A {
    public:
        uint32_t totalLength;
        uint32_t tileNum;
    };
    class B: public A {
    public:
        uint32_t xxx;
        uint32_t xxx;
    };
    static ge::graphStatus TilingFunc(gert::TilingContext* context)
    {
        // Incorrect usage
        B *tiling = context->GetTilingData<A>(); // Not supported. This may cause unknown issues.
        // Correct usage
        B *tiling = context->GetTilingData<B>();
        ......
        return ge::GRAPH_SUCCESS;
    }
    ```

-   The tiling data obtained by GetTilingData does not contain initial values. You need to explicitly assign values or define and call a tiling assignment function in the tiling structure.

    ```
    static ge::graphStatus TilingFunc(gert::TilingContext* context)
    {
        TilingData *tiling = context->GetTilingData<TilingData>(); //Obtain the tiling structure. At this point, totalLength and tileNum are 0, and the initial values are not carried over.
        ......
        // Explicit assignment is required.
        tiling->totalLength = totalLength;  // Assign a value to the tiling structure member variable.
        tiling->tileNum = TILE_NUM;         // Assign a value to the tiling structure member variable.
        ......
        return ge::GRAPH_SUCCESS;
    }
    ```

-   The tiling structures on the host side and kernel side support passing template parameters. Due to the special nature of the comma operator in macro functions, using a template type with commas (for example, template<int32\_t sizeA, int32\_t sizeB\>) in the Kernel-side macro function (REGISTER\_TILING\_DEFAULT or REGISTER\_TILING\_FOR\_TILINGKEY) causes compilation errors. Therefore, you need to use an alias to define a template type with commas (for example, using size = template<int32\_t sizeA, int32\_t sizeB\>). The following is an example:

    ```
    // Scenario where the number of template parameters is greater than 1
    template<int32_t sizeA, int32_t sizeB>
    class A {
    public:
        uint32_t totalLength;
        uint32_t tileNum;
        uint32_t dataArray[sizeA];
    };
    // Scenario where the number of template parameters is equal to 1
    template<int32_t sizeA>
    class B {
    public:
        uint32_t totalLength;
        uint32_t tileNum;
        uint32_t dataArray[sizeA];
    };
    
    // The host side can directly pass the tiling structure and the corresponding template parameters.
    static ge::graphStatus TilingFunc(gert::TilingContext* context)
    {
        // The template parameters can be directly passed when the number of template parameters is equal to 1 or greater than or equal to 1.
        A<3, 5> *tiling = context->GetTilingData<A<3,5>>();
        B<3> *tiling = context->GetTilingData<B<3>>();
        ......
        return ge::GRAPH_SUCCESS;
    }
    
    // Kernel side code
    #include "kernel_operator.h"
    #include "add_custom_tiling.h"  // Include the header file that defines the tiling structure.
    extern "C" __global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
    {
        using aa = A<3,5>;
        REGISTER_TILING_DEFAULT(aa);                                // When the number of template parameters is greater than 1, you must use using to specify them.
        REGISTER_TILING_FOR_TILINGKEY("TILING_KEY_VAR == 2", B<3>);  // When the number of template parameters is equal to 1, you can directly specify the template parameters.
        ......
    }
    ```

## How to Modify a Macro-Defined Tiling Structure to Use Standard C++ Syntax<a name="section11732131341116"></a>

This section describes how to modify the method that uses macros such as BEGIN\_TILING\_DATA\_DEF to define a tiling structure into the method that uses standard C++ syntax.

1.  **First**, move the header file that defines the tiling structure from the op\_host directory to the op\_kernel directory. The content before and after the change is compared as follows. **Note that the included header file changes, and the header file related to macro definitions no longer needs to be included.**

    **Table 1** Comparison of the two methods

    <a name="table882614511524"></a>
    <table><thead align="left"><tr id="row78261151165217"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p19826125115523"><a name="p19826125115523"></a><a name="p19826125115523"></a>Macro Definition Method</p>
    </th>
    <th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p1882605117521"><a name="p1882605117521"></a><a name="p1882605117521"></a>Standard C++ Syntax Definition Method</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row18266511523"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><pre class="screen" id="screen98261951195215"><a name="screen98261951195215"></a><a name="screen98261951195215"></a>#include "register/tilingdata_base.h"
    namespace optiling {
    BEGIN_TILING_DATA_DEF(AddCustomTilingData)
    TILING_DATA_FIELD_DEF(uint32_t, totalLength);
    TILING_DATA_FIELD_DEF(uint32_t, tileNum);
    END_TILING_DATA_DEF;
    REGISTER_TILING_DATA_CLASS(AddCustom, AddCustomTilingData)
    } // namespace optiling</pre>
        </td>
        <td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><pre class="screen" id="screen168271451135220"><a name="screen168271451135220"></a><a name="screen168271451135220"></a>#include &lt;cstdint&gt;

    struct AddCustomTilingData {
        uint32_t totalLength;
        uint32_t tileNum;
    };</pre>
    </td>
    </tr>
    </tbody>
    </table>

2.  **Then**, modify the tiling function implementation on the host side. At this point, assigning values to the member variables of the tiling structure no longer requires the set methods generated by the macro definitions. Instead, use the C++ pointer assignment method that users are familiar with.

    **Table 2** Comparison between the two methods

    <a name="table3481171754810"></a>
    <table><thead align="left"><tr id="row5482151714815"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p1848218179487"><a name="p1848218179487"></a><a name="p1848218179487"></a>Macro Definition Method</p>
    </th>
    <th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p948201764811"><a name="p948201764811"></a><a name="p948201764811"></a>Standard C++ Syntax Definition Method</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row148281774813"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><pre class="screen" id="screen1773181331120"><a name="screen1773181331120"></a><a name="screen1773181331120"></a>namespace optiling {
    static ge::graphStatus TilingFunc(gert::TilingContext *context)
    {
        ...
        AddCustomTilingData tiling;
        tiling.set_totalLength(totalLength);  // Use the set method generated by the macro definition method.
        tiling.set_tileNum(TILE_NUM);        // Use the set method generated by the macro definition method.
        ...
        // Save the local variable to the context.
        tiling.SaveToBuffer(context-&gt;GetRawTilingData()-&gt;GetData(), context-&gt;GetRawTilingData()-&gt;GetCapacity());
        ...

        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling</pre>
    </td>
    <td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><pre class="screen" id="screen34968325556"><a name="screen34968325556"></a><a name="screen34968325556"></a>#include "../op_kernel/add_custom/add_custom_tiling.h"  // Include the header file that defines the tiling structure.
    ...

    namespace optiling {
    static ge::graphStatus TilingFunc(gert::TilingContext *context)
    {
        ...
        AddCustomTilingData *tiling = context-&gt;GetTilingData&lt;AddCustomTilingData&gt;();
        ...
        tiling-&gt;totalLength = totalLength;  // Assign the member variable using the user-friendly C++ pointer method.
        tiling-&gt;tileNum = TILE_NUM;         // Assign the member variable using the user-friendly C++ pointer method.
        ...

        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling</pre>
    </td>
    </tr>
    </tbody>
    </table>

3.  **Finally**, add a [REGISTER\_TILING\_DEFAULT](../../../../../api/SIMD-API/basic_api/Kernel-Tiling/REGISTER_TILING_DEFAULT.md) call at the entry of the kernel function to register the tiling structure. This registration operation informs the framework that the user has defined the tiling structure using standard C++ syntax and specifies its type, so that the framework can correctly identify and use the structure when parsing tiling data.

    ```
    #include "add_custom_tiling.h"
    ...

    extern "C" __global__ __aicore__ void add_custom(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z, __gm__ uint8_t* workspace, __gm__ uint8_t* tiling)
    {
        REGISTER_TILING_DEFAULT(AddCustomTilingData);  // Add a REGISTER_TILING_DEFAULT call to register the tiling structure.
        ...
    }
    ```
