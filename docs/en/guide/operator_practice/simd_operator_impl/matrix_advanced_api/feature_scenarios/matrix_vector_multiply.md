# Matrix-Vector Multiplication<a name="ZH-CN_TOPIC_0000002500548098"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:09:55.358Z pushedAt=2026-11-03T07:05:13.304Z nl=2 -->

## Feature Overview<a name="zh-cn_topic_0000002264077892_section310824820358"></a>

General matrix-vector multiplication (GEMV) refers to the scenario in Matmul computation where M=1, and the left cube A with a shape of \(1, K\) is multiplied by the right cube B with a shape of \(K, N\). Matmul supports enabling GEMV mode on both the Tiling side and the Kernel side by configuring the data format of cube A as [VECTOR](../../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md#li51557161818), thereby efficiently handling the M=1 computation scenario. If GEMV mode is not enabled when M=1, Matmul processes the M dimension as a non-aligned scenario. Compared with the non-aligned processing method, GEMV mode moves less data and delivers better performance.

The following uses a Matmul example with M=1, K=256, N=32, and the left and right cube data type of half to illustrate the internal processing of the Matmul API in GEMV mode.

-   GEMV mode

    When moving cube A from the L1 Buffer (A1) to the L0A Buffer (A2), the 1\*256 vector is processed as a 16\*16 cube, and the [LoadData](../../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/cube_compute_load.md) API is called to move a cube of 16\*16 fractal size in one operation. The movement of cube B and the cube multiplication computation are the same as those in the basic scenario, as shown in the following figure.

    **Figure 1**  Computation illustration of cube multiplication in GEMV mode with M=1<a name="zh-cn_topic_0000002264077892_fig18149171416184"></a>  
    ![](../../../../figures/gemv_m1.png "Computation illustration of cube multiplication in GEMV mode with M=1")

-   Non-GEMV mode

    When moving cube A from the L1 Buffer (A1) to the L0A Buffer (A2), the 1\*256 vector is processed as non-aligned cube data, and the M dimension is aligned to 32 bytes before the movement. The [LoadData](../../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/cube_compute_load.md) API is called to move a cube of 16\*16 fractal size each time, for a total of K/16=16 times, which increases the amount of data moved and delivers poorer performance than GEMV mode, as shown in the following figure.

    **Figure 2**  Computation illustration of cube multiplication in non-GEMV mode with M=1<a name="zh-cn_topic_0000002264077892_fig15267820101910"></a>  
    ![](../../../../figures/non_gemv_m1.png "Computation illustration of cube multiplication in non-GEMV mode with M=1")

## Scenarios<a name="zh-cn_topic_0000002264077892_section118051016163613"></a>

When the Cube A with a shape of \(1, K\) (M=1, K\>1) is used for cube multiplication, the input data of the Cube A is vector data.

## Constraints<a name="zh-cn_topic_0000002264077892_section14160134220363"></a>

-   In Matmul computation, to enable GEMV mode, the original input shape M of cube A must be equal to 1.

-   In GEMV scenarios, the left cube A does not support transposition.
-   In GEMV scenarios, the left cube data in Global Memory must be 16-byte aligned.
-   In the [MxMatmul scenario](mx_matmul_scenario.md), when computing cube-vector multiplication, the left cube A and the left quantization coefficient cube scaleA support only the memory logical position TPosition::GM.

## Example<a name="zh-cn_topic_0000002264077892_section15486294368"></a>

For the complete operator sample, see [matmul\_gemv operator sample](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_format_gemv).

-   Tiling implementation

    Call the SetAType API to set the data format of cube A to CubeFormat::VECTOR. The rest of the Tiling implementation is the same as that in the [basic scenario](../operator_impl.md).

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
    // Set the format of cube A to CubeFormat::VECTOR
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::VECTOR, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16); 
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(AscendC::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT); 
    ... // Other implementation content
    optiling::TCubeTiling tilingData;   
    int ret = tiling.GetTiling(tilingData);
    ```

-   Kernel implementation

    Compared with the [basic scenario](../operator_impl.md#zh-cn_topic_0000001622514006_li1032116474330), in the GEMV scenario, when creating a Matmul object, set the data format of the template parameter A\_TYPE to CubeFormat::VECTOR.

    ```
    #include "lib/matmul_intf.h"
    
    using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::VECTOR, half>; 
    using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half>;
    using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float>; 
    using BIAS_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float>; 
    AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE> mm; 
    ```
