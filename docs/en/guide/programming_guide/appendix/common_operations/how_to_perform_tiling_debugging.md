# How to Debug Tiling<a name="ZH-CN_TOPIC_0000002305497828"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T11:14:02.397Z pushedAt=2026-08-31T07:57:25.428Z -->

During [engineering-based operator development](../../advanced_programming/aclnn_operator_development/overview.md), developers need to implement the tiling function, whose prototype is fixed and accepts TilingContext as input. The framework constructs TilingContext and calls the tiling function. To debug tiling independently, developers can load the compiled tiling dynamic library through OpTilingRegistry, obtain the pointer to the tiling function, and call it. When calling the tiling function, the TilingContext input parameter is constructed using ContextBuilder.

The following describes the specific steps:

1.  Complete the operator implementation by referring to the development steps of engineering-based operator development, and obtain the corresponding tiling dynamic library file through **operator package compilation** or **operator dynamic library compilation**.
    -   Operator package compilation: The dynamic library corresponding to the tiling implementation is liboptiling.so in the operator package deployment directory. For the specific path, see [Operator Package Deployment](../../advanced_programming/aclnn_operator_development/operator_package_compilation/operator_package_deployment.md).
    -   Dynamic library compilation: The tiling implementation is integrated into the operator dynamic library libcust\_opapi.so. For the specific path, see [Static and Dynamic Operator Library Compilation](../../advanced_programming/aclnn_operator_development/operator_dynamic_and_static_library_compilation.md).

2.  Write the test code.

    -   Use ContextBuilder to configure the shape, data type, format, and platform information of the input and output tensors, and construct TilingContext.
    -   Load the tiling dynamic library through the [LoadTilingLibrary](../../../../api/Utils-API/Tiling_tuning/OpTilingRegistry/LoadTilingLibrary.md) API of OpTilingRegistry, and obtain the tiling function pointer through the [GetTilingFunc](../../../../api/Utils-API/Tiling_tuning/OpTilingRegistry/GetTilingFunc.md) API.
    -   Execute the tiling function and verify its correctness.

    ```
    // test.cpp
    #include <iostream>
    #include "exe_graph/runtime/storage_shape.h"
    #include "tiling/context/context_builder.h"
    
    int main()
    {
        gert::StorageShape x_shape = {{2, 32}, {2, 32}};
        gert::StorageShape y_shape = {{2, 32}, {2, 32}};
        gert::StorageShape z_shape = {{2, 32}, {2, 32}};
    
        auto param = gert::TilingData::CreateCap(4096);
        auto workspace_size_holder = gert::ContinuousVector::Create<size_t>(4096);
        auto ws_size = reinterpret_cast<gert::ContinuousVector *>(workspace_size_holder.get());
    
        auto holder = context_ascendc::ContextBuilder()
                                    .NodeIoNum(2, 1)
                                    .IrInstanceNum({1, 1})
                                    .AddInputTd(0, ge::DT_FLOAT, ge::FORMAT_ND, ge::FORMAT_ND, x_shape)
                                    .AddInputTd(1, ge::DT_FLOAT, ge::FORMAT_ND, ge::FORMAT_ND, y_shape)
                                    .AddOutputTd(0, ge::DT_FLOAT, ge::FORMAT_ND, ge::FORMAT_ND, z_shape)
                                    .TilingData(param.get())
                                    .Workspace(ws_size)
                                    .AddPlatformInfo("Ascendxxxyy")
                                    .BuildTilingContext();
        auto tilingContext = holder->GetContext<gert::TilingContext>();
        context_ascendc::OpTilingRegistry tmpIns;
        bool flag = tmpIns.LoadTilingLibrary("/your/path/to/so_path/liboptiling.so");  // Load the corresponding tiling dynamic library file.
        if (flag == false) {
            std::cout << "Failed to load tiling so" << std::endl;
            return -1;
        }
        context_ascendc::TilingFunc tilingFunc = tmpIns.GetTilingFunc("AddCustom");  // Obtain the tiling function corresponding to the AddCustom operator. The input parameter here is OpType.
        if (tilingFunc != nullptr) {
            ge::graphStatus ret = tilingFunc(tilingContext);  // Execute the tiling function.
            if (ret != ge::GRAPH_SUCCESS) {
                std::cout << "Exec tiling func failed." << std::endl;
                return -1;
            }
        } else {
            std::cout << "Get tiling func failed." << std::endl;
            return -1;
        }
        return 0;
    }
    ```

3.  Compile the test code.

    ```
    g++ test.cpp -I${INSTALL_DIR}/include  -L${INSTALL_DIR}/lib64 -Wl,-rpath,${INSTALL_DIR}/lib64 -ltiling_api -lc_sec -lgraph_base -lregister -lunified_dlog -lplatform -o test
    ```

    -   Replace $\{INSTALL\_DIR\} with the path where CANN software is stored after installation. For example, if you install CANN as the root user, the default storage path is /usr/local/Ascend/cann.
    -   Link the required dynamic libraries as needed. The dynamic libraries that must be linked are as follows:
        -   libtiling\_api.so: dynamic library related to tiling functions, including the ContextBuilder class, OpTilingRegistry class, and so on.
        -   libc\_sec.so: secure function library, on which libtiling\_api.so depends.
        -   libgraph\_base.so: basic data structure and API library, on which libtiling\_api.so depends.
        -   libregister.so: library related to service function registration (for example, tiling function registration and operator prototype registration).
        -   libunified\_dlog.so: log library on which libtiling\_api.so depends.
        -   libplatform.so: platform information library on which libtiling\_api.so depends. This library is required when hardware platform information is used in tiling functions.

4.  Execute the executable file.

    ```
    ./test
    ```
