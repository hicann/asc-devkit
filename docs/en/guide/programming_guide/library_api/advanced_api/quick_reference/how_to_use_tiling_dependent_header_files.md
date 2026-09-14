# How to Use the Dependent Header File of Tiling<a name="ZH-CN_TOPIC_0000002522860379"></a>

<!-- md-trans-meta sourceCommit=d7f8cd8648f37bc3a4868cb030f666b2f7fd6d4d translatedAt=2026-08-26T12:21:38.327Z pushedAt=2026-09-03T11:56:55.850Z -->

Because the Scalar compute unit of the AI processor has limited execution capability, to reduce the scalar computation on the kernel side of the operator, some computation is performed on the host side, which requires writing [host-side tiling code](../../../advanced_programming/aclnn_operator_development/host_tiling_implementation/basic_process.md). Note that when calling the tiling API of the high-level API or using the tiling structure parameters of the high-level API in a program, you need to include the dependent header file. Under different tiling implementation methods, the details are as follows:

-   [Defining a tiling structure using standard C++ syntax](../../../advanced_programming/aclnn_operator_development/host_tiling_implementation/define_tiling_structure_using_standard_cpp_syntax.md)

    In this method, the dependent header file to be included is as follows. All tiling structures of the high-level API are defined in the **AscendC::tiling** namespace, so you need to access the tiling structure of a specific API through **AscendC::tiling**.

    ```
    #include "kernel_tiling/kernel_tiling.h"
    
    // ...
    AscendC::tiling::TCubeTiling cubeTilingData;
    ```

-   [Defining a tiling structure using the TILING\_DATA\_DEF macro](../../../advanced_programming/aclnn_operator_development/host_tiling_implementation/tiling_implementation_for_high_level_api.md)

    In this method, the dependent header file to be included is as follows. All tiling structures and tiling functions of the high-level API are defined in the **optiling** namespace.

    ```
    #include "tiling/tiling_api.h"
    
    namespace optiling {
    // ...
    }
    ```

