# Matrix-Vector Multiplication<a name="ZH-CN_TOPIC_0000002500548098"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-26T13:38:44.339Z -->

## Description<a name="zh-cn_topic_0000002264077892_section310824820358"></a>

General Matrix-Vector multiplication (GEMV) refers to the scenario where M=1 in Matmul computation, that is, the left matrix A with the shape of \(1, K\) is multiplied by the right matrix B with the shape of \(K, N\). Matmul supports enabling GEMV mode on both the Tiling side and the Kernel side by configuring the data format of matrix A as [VECTOR](../../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout_format.md#li51557161818), so as to efficiently process the computation scenario where M=1. If GEMV mode is not enabled when M=1, Matmul computation processes the M dimension as a non-aligned scenario. Compared with the non-aligned processing method, GEMV mode moves less data and delivers better performance.

The following uses a Matmul with M=1, K=256, N=32, and the left and right matrix data type of half as an example to describe the internal processing of the Matmul API in GEMV mode.

-   GEMV mode

    When matrix A is moved from A1 to A2, the 1\*256 vector is processed as a 16\*16 matrix. The [LoadData](../../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/matrix_computation_load.md) API is called once to complete the matrix movement of a 16\*16 fractal size. The movement of matrix B and the matrix multiplication computation are the same as those in the basic scenario, as shown in the following figure.

    **Figure 1**  Matrix multiplication computation schematic of GEMV mode with M=1<a name="zh-cn_topic_0000002264077892_fig18149171416184"></a>  
    ![](../../../../figures/gemv_mode_m_1_matrix_multiplication_computation_schematic.png "Matrix multiplication computation schematic of GEMV mode with M=1")

-   Non-GEMV mode

    When matrix A is moved from A1 to A2, the 1\*256 vector is processed as non-aligned matrix data, and the M dimension is aligned to 32 bytes before movement. The [LoadData](../../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/matrix_computation_load.md) API is called to move a matrix of a 16\*16 fractal size each time, for a total of K/16=16 times. This increases the amount of data moved and results in poorer performance than GEMV mode, as shown in the following figure.

    **Figure 2**  Matrix multiplication computation schematic of non-GEMV mode with M=1<a name="zh-cn_topic_0000002264077892_fig15267820101910"></a>  
    ![](../../../../figures/non_gemv_mode_m_1_matrix_multiplication_computation_schematic.png "Matrix multiplication computation schematic of non-GEMV mode with M=1")

## Scenarios<a name="zh-cn_topic_0000002264077892_section118051016163613"></a>

An A matrix with the shape of \(1, K\) (M=1, K\>1) is used for matrix multiplication, that is, the data of the input A matrix is vector data.

## Constraints<a name="zh-cn_topic_0000002264077892_section14160134220363"></a>

-   In Matmul computation, to enable GEMV mode, the original input shape M of matrix A must be equal to 1.

-   In the GEMV scenario, the left matrix A does not support transposition.
-   In the GEMV scenario, the left matrix data in Global Memory must be 16-byte aligned.
-   In the [MxMatmul scenario](mxmatmul_scenario.md), when computing matrix-vector multiplication, the left matrix A and the left quantization coefficient matrix scaleA support only the memory logical position TPosition::GM.

## Calling Example<a name="zh-cn_topic_0000002264077892_section15486294368"></a>

For the complete operator sample, see [matmul_gemv operator sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_format_gemv).

-   Tiling Implementation

    Call the SetAType API to set the data format of matrix A to CubeFormat::VECTOR. The rest of the Tiling implementation is the same as that in the [basic scenario](../operator_implementation.md).

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
    // Set the format of matrix A to CubeFormat::VECTOR.
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::VECTOR, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16); 
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(AscendC::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT); 
    ... // Other implementation details
    optiling::TCubeTiling tilingData;   
    int ret = tiling.GetTiling(tilingData);
    ```

-   Kernel Implementation

    Compared with the [basic scenario](../operator_implementation.md#zh-cn_topic_0000001622514006_li1032116474330), in the GEMV scenario, when creating the Matmul object, set the data format of the template parameter A\_TYPE to CubeFormat::VECTOR.

    ```
    #include "lib/matmul_intf.h"
    
    using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::VECTOR, half>; 
    using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half>;
    using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float>; 
    using BIAS_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float>; 
    AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE> mm; 
    ```
