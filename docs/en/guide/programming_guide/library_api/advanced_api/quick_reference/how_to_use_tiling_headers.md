# How to Use the Header File Required by Tiling<a name="ZH-CN_TOPIC_0000002522860379"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T15:22:58.457Z pushedAt=2026-11-03T07:05:13.474Z nl=2 -->

Because the Scalar computing unit of the AI processor has limited execution capability, to reduce Scalar computation on the operator kernel function side, some computation is performed on the Host side, which requires writing [host-side tiling code](../../../advanced_programming/aclnn_operator_development/design_and_implementation/host_tiling_implementation.md). Note that when calling the Tiling APIs of the advanced APIs or using the Tiling structure parameters of the advanced APIs in a program, you need to include the dependent header files. Specifically, under different Tiling implementation methods:

-   [Defining the tiling structure using standard C++ syntax](../../../advanced_programming/aclnn_operator_development/design_and_implementation/host_tiling_implementation.md#define-tilingdata-structure)

    In this method, the dependent header file to be included is as follows. All tiling structures of the advanced API are defined in the **AscendC::tiling** namespace, so you need to access the tiling structure of a specific API through **AscendC::tiling**.

    ```
    #include "kernel_tiling/kernel_tiling.h"
    
    // ...
    AscendC::tiling::TCubeTiling cubeTilingData;
    ```

-   [Defining the tiling structure using the TILING\_DATA\_DEF macro](../../../advanced_programming/aclnn_operator_development/design_and_implementation/tiling_with_advanced_api.md)

    In this method, the dependent header file to be included is as follows. All tiling structures and tiling functions of the advanced API are defined in the **optiling** namespace.

    ```
    #include "tiling/tiling_api.h"
    
    namespace optiling {
    // ...
    }
    ```

