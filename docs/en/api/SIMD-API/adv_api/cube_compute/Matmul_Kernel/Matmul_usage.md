# Matmul Usage Guide

<!-- md-trans-meta sourceCommit=20bc334a44f39ef4bd75326531a4c71eeb5f9d26 translatedAt=2026-11-02T06:34:03.991Z pushedAt=2026-11-18T04:53:12.841Z nl=2 -->

Ascend C provides a set of high-level Matmul APIs to help users quickly implement Matmul cube multiplication operations.

The computation formula of Matmul is: C = A \* B + Bias, and its diagram is as follows.

-   A and B are source operands. A is the left cube, whose shape is \[M, K\]; B is the right cube, whose shape is \[K, N\].
-   C is the destination operand, which is the cube storing the cube multiplication result, whose shape is \[M, N\].
-   Bias is the cube multiplication bias, whose shape is \[1, N\]. This Bias is applied to each row of the A\*B result cube.

**Figure 1**  Matmul cube multiplication diagram  
![](../../../../figures/matmul_matrix_diagram.png "Matmul cube multiplication diagram")

> [!NOTE] Note
>The M-axis direction mentioned below is the vertical direction of cube A; the K-axis direction is the horizontal direction of cube A or the vertical direction of cube B; the N-axis direction is the horizontal direction of cube B; the last axis is the last dimension of the cube.

The steps to implement the Matmul cube multiplication operation on the kernel function (Kernel) side are summarized as follows:

1.  Create a Matmul object.
2.  Initialize the operation.
3.  Set the left cube A, right cube B, and Bias.
4.  Complete the cube multiplication operation.
5.  End the cube multiplication operation.

The specific steps to implement cube multiplication using the Matmul API are as follows:

1.  Create a Matmul object.

    An example of creating a Matmul object is as follows:

    -   The default is MIX mode (including cube computation and vector computation). In this scenario, the ASCENDC\_CUBE\_ONLY macro is usually not defined. If the ASCENDC\_CUBE\_ONLY macro is used in the program, the ASCEND\_IS\_AIC macro and the ASCEND\_IS\_AIV macro must be used to separate Cube computation from Vector computation.
    -   In the pure Cube mode (cube computation only) scenario, it is recommended to define the ASCENDC\_CUBE\_ONLY macro in the code to avoid additional performance overhead.

    ```
    // In pure Cube mode (cube computation only), set this code macro, and it must be set before #include "lib/matmul_intf.h"
    // #define ASCENDC_CUBE_ONLY
    #include "lib/matmul_intf.h"

    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    AscendC::Matmul<aType, bType, cType, biasType> mm;
    ```

    When creating an object, the parameter type information of A, B, C, and Bias must be passed in. The type information is defined through [MatmulType](#table1188045714378), including: memory logical position, data format, data type, and the memory logical position of the data source.

    ```
    template <
        AscendC::TPosition POSITION, CubeFormat FORMAT, typename TYPE, bool ISTRANS = false,
        LayoutMode LAYOUT = LayoutMode::NONE, bool IBSHARE = false, TPosition SRCPOS = TPosition::GM>
    struct MatmulType {
        constexpr static AscendC::TPosition pos = POSITION;
        constexpr static CubeFormat format = FORMAT;
        using T = TYPE;
        constexpr static bool isTrans = ISTRANS;
        constexpr static LayoutMode layout = LAYOUT;
        constexpr static bool ibShare = IBSHARE;
        constexpr static TPosition srcPos = SRCPOS;
    };
    ```

    **Table 1**  MatmulType parameter description

    <a name="table1188045714378"></a>

    | Parameter | Description |
    | --- | --- |
    | POSITION | Memory logical position. <!-- npu="950" id15 --><br><br>For Ascend 950PR&950DT products:<br>    Cube A can be set to TPosition::GM, TPosition::VECOUT, TPosition::TSCM<br>Cube B can be set to TPosition::GM, TPosition::VECOUT, TPosition::TSCM<br>Bias can be set to TPosition::GM, TPosition::VECOUT, TPosition::TSCM<br>Cube C can be set to TPosition::GM, TPosition::VECIN<br> Note that when cube A, cube B, or the Bias cube is set to TPosition::VECOUT or TPosition::TSCM, all data of the corresponding cube used for single-core computation must be on the Unified Buffer (UB) or L1 Buffer. For specific examples, see [matmul_vecout sample](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_vecout), [Matmul operator sample with custom data source as VECOUT for TSCM input](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_tscm_src_vecout), and [Matmul operator sample with custom data source as GM for TSCM input](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_tscm).<!-- end id15 --><!-- npu="A3" id16 --><br><br>For Atlas A3 products:<br>    Cube A can be set to TPosition::GM, TPosition::VECOUT, TPosition::TSCM<br>Cube B can be set to TPosition::GM, TPosition::VECOUT, TPosition::TSCM<br>Bias can be set to TPosition::GM, TPosition::VECOUT<br>Cube C can be set to TPosition::GM, TPosition::VECIN, TPosition::CO1<br>   Note that when cube C is set to TPosition::CO1, the data format of cube C supports only CubeFormat::NZ, and the data type of cube C supports only float and int32_t.<!-- end id16 --><!-- npu="910b" id17 --><br>   <br>For Atlas A2 products:<br>    Cube A can be set to TPosition::GM, TPosition::VECOUT, TPosition::TSCM<br>Cube B can be set to TPosition::GM, TPosition::VECOUT, TPosition::TSCM<br>Bias can be set to TPosition::GM, TPosition::VECOUT<br>Cube C can be set to TPosition::GM, TPosition::VECIN, TPosition::CO1<br>   Note that when cube C is set to TPosition::CO1, the data format of cube C supports only CubeFormat::NZ, and the data type of cube C supports only float and int32_t.<!-- end id17 --><!-- npu="310p" id18 --><br><br>For the AI Core of Atlas inference products:<br>    Cube A can be set to TPosition::GM, TPosition::VECOUT<br>Cube B can be set to TPosition::GM, TPosition::VECOUT<br>Bias can be set to TPosition::GM, TPosition::VECOUT<br>Cube C can be set to TPosition::GM, TPosition::VECIN<!-- end id18 --><!-- npu="310b" id19 --><br>    <br>For Atlas 200I/500 A2 inference products:<br>    Cube A can be set to TPosition::GM<br>Cube B can be set to TPosition::GM<br>Bias can be set to TPosition::GM<br>Cube C can be set to TPosition::GM<!-- end id19 --><!-- npu="x90" id1 --><br>    <br>For Kirin X90 processor products:<br>    Cube A can be set to TPosition::GM, TPosition::VECOUT, TPosition::TSCM<br>Cube B can be set to TPosition::GM, TPosition::VECOUT, TPosition::TSCM<br>Bias can be set to TPosition::GM, TPosition::VECOUT<br>Cube C can be set to TPosition::GM, TPosition::CO1<!-- end id1 --><!-- npu="9030" id2 --><br>    <br>For Kirin 9030 processor products:<br>    Cube A can be set to TPosition::TSCM<br>Cube B can be set to TPosition::TSCM<br>Bias can be set to TPosition::GM<br>Cube C can be set to TPosition::GM<!-- end id2 --> |
    | FORMAT | Physical layout format of the data. <!-- npu="950" id20 --><br><br>For Ascend 950PR&950DT products:<br>    Cube A can be set to CubeFormat::ND, CubeFormat::NZ, CubeFormat::COLUMN_MAJOR, CubeFormat::VECTOR<br>Cube B can be set to CubeFormat::ND, CubeFormat::NZ, CubeFormat::COLUMN_MAJOR<br>Bias can be set to CubeFormat::ND<br>Cube C can be set to CubeFormat::ND, CubeFormat::NZ, CubeFormat::ND_ALIGN, CubeFormat::COLUMN_MAJOR<br>    <br>For Ascend 950PR&950DT products, note the following:<br>    Only in non-MxMatmul scenarios do the A, B, and C cube formats support CubeFormat::COLUMN_MAJOR. When the format is CubeFormat::COLUMN_MAJOR, the corresponding cube supports only the memory logical position TPosition::GM.<br>When input cube A or cube B is set to TPosition::TSCM, the corresponding format supports only CubeFormat::NZ.<br>When cube C is set to TPosition::VECIN and CubeFormat::ND, the last axis must be 32-byte aligned. For example, when the data type is half, N must be a multiple of 16.<!-- end id20 --><!-- npu="A3" id21 --><br>    <br>For Atlas A3 products:<br>    Cube A can be set to CubeFormat::ND, CubeFormat::NZ, CubeFormat::VECTOR<br>Cube B can be set to CubeFormat::ND, CubeFormat::NZ<br>Bias can be set to CubeFormat::ND<br>Cube C can be set to CubeFormat::ND, CubeFormat::NZ, CubeFormat::ND_ALIGN<!-- end id21 --><!-- npu="910b" id22 --><br>    <br>For Atlas A2 products:<br>    Cube A can be set to CubeFormat::ND, CubeFormat::NZ, CubeFormat::VECTOR<br>Cube B can be set to CubeFormat::ND, CubeFormat::NZ<br>Bias can be set to CubeFormat::ND<br>Cube C can be set to CubeFormat::ND, CubeFormat::NZ, CubeFormat::ND_ALIGN<!-- end id22 --><!-- npu="310p" id23 --><br>    <br>For the AI Core of Atlas inference products:<br>    Cube A can be set to CubeFormat::ND, CubeFormat::NZ<br>Cube B can be set to CubeFormat::ND, CubeFormat::NZ<br>Bias can be set to CubeFormat::ND<br>Cube C can be set to CubeFormat::ND, CubeFormat::NZ, CubeFormat::ND_ALIGN<br> Note: For the AI Core of Atlas inference products, when cube C is set to CubeFormat::ND, the last axis must be 32-byte aligned. For example, when the data type is half, N must be a multiple of 16.<!-- end id23 --><!-- npu="310b" id24 --><br>    <br>For Atlas 200I/500 A2 inference products:<br>    Cube A can be set to CubeFormat::ND, CubeFormat::NZ<br>Cube B can be set to CubeFormat::ND, CubeFormat::NZ<br>Bias can be set to CubeFormat::ND<br>Cube C can be set to CubeFormat::ND, CubeFormat::NZ<!-- end id24 --><!-- npu="x90" id3 --><br>    <br>For Kirin X90 processor products:<br>    Cube A can be set to CubeFormat::ND, CubeFormat::NZ, CubeFormat::VECTOR<br>Cube B can be set to CubeFormat::ND, CubeFormat::NZ<br>Bias can be set to CubeFormat::ND<br>Cube C can be set to CubeFormat::ND, CubeFormat::NZ, CubeFormat::ND_ALIGN<!-- end id3 --><!-- npu="9030" id4 --><br>    <br>For Kirin 9030 processor products:<br>Cube A can be set to CubeFormat::NZ, CubeFormat::VECTOR<br>Cube B can be set to CubeFormat::NZ<br>Bias can be set to CubeFormat::ND<br>Cube C can be set to CubeFormat::ND<!-- end id4 --><!-- npu="310b" id25 --><br><br>Note: For Atlas 200I/500 A2 inference products, when cube C is set to TPosition::VECIN or TPosition::TSCM and CubeFormat::ND, the last axis must be 32-byte aligned. For example, when the data type is half, N must be a multiple of 16. When cube C is set to TPosition::VECIN or TPosition::TSCM and CubeFormat::NZ, N must be a multiple of 16.<!-- end id25 --><br>    <br>For the alignment constraints of cubes A, B, and C in CubeFormat::NZ format, see [Cube alignment requirements for NZ format](#table98851538118). |
    | TYPE | Data type. <!-- npu="950" id26 --><br><br>For Ascend 950PR&950DT products:<br>    1. Non-MxMatmul scenarios:<br>Cube A can be set to half, float, bfloat16_t, int8_t, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t<br>Cube B can be set to half, float, bfloat16_t, int8_t, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t<br>Bias can be set to half, float, int32_t, bfloat16_t<br>Cube C can be set to half, float, bfloat16_t, int32_t, int8_t, fp8_e4m3fn_t, hifloat8_t<br>    2. MxMatmul scenarios:<br>Cube A can be set to fp8_e4m3fn_t, fp8_e5m2_t, fp4x2_e2m1_t, fp4x2_e1m2_t<br>Cube B can be set to fp8_e4m3fn_t, fp8_e5m2_t, fp4x2_e2m1_t, fp4x2_e1m2_t<br>Bias can be set to half, float, bfloat16_t<br>Cube C can be set to half, float, bfloat16_t<!-- end id26 --><!-- npu="A3" id27 --><br>    <br>For Atlas A3 products:<br>Cube A can be set to half, float, bfloat16_t, int8_t, int4b_t<br>Cube B can be set to half, float, bfloat16_t, int8_t, int4b_t<br>Bias can be set to half, float, int32_t<br>Cube C can be set to half, float, bfloat16_t, int32_t, int8_t<!-- end id27 --><!-- npu="910b" id28 --><br>    <br>For Atlas A2 products:<br>Cube A can be set to half, float, bfloat16_t, int8_t, int4b_t<br>Cube B can be set to half, float, bfloat16_t, int8_t, int4b_t<br>Bias can be set to half, float, int32_t<br>Cube C can be set to half, float, bfloat16_t, int32_t, int8_t<!-- end id28 --><!-- npu="310p" id29 --><br><br>For the AI Core of Atlas inference products:<br>    Cube A can be set to half, int8_t<br>Cube B can be set to half, int8_t<br>Bias can be set to float, int32_t<br>Cube C can be set to half, float, int8_t, int32_t<!-- end id29 --><!-- npu="310b" id30 --><br>    <br>For Atlas 200I/500 A2 inference products:<br>    Cube A can be set to half, float, bfloat16_t, int8_t<br>Cube B can be set to half, float, bfloat16_t, int8_t<br>Bias cube can be set to half, float, int32_t<br>Cube C can be set to half, float, bfloat16_t, int32_t<!-- end id30 --><!-- npu="x90" id5 --><br>    <br>For Kirin X90 processor products:<br>    Cube A can be set to half, int8_t<br>Cube B can be set to half, int8_t<br>Bias can be set to half, int32_t<br>Cube C can be set to half, int32_t, int8_t<!-- end id5 --><!-- npu="9030" id6 --><br>    <br>For Kirin 9030 processor products:<br>    Cube A can be set to half<br>Cube B can be set to half<br>Bias can be set to half<br>Cube C can be set to half<!-- end id6 --><br>Note: Except for the fp8_e4m3fn_t/fp8_e5m2_t data types and the case where cube B is of the int8_t data type, the data types of cube A and cube B must be the same. For specific data type combination relationships, see [Table 2](#table1996113269499). When cube A and cube B are of the int4b_t data type, the number of data elements on the inner axis of the cube must be even. For example, when cube A is of the int4b_t data type and is not transposed, [singleCoreK](../Matmul_Tiling/TCubeTiling_struct.md#p11899125875617) must be even. |
    | ISTRANS | Whether to enable the cube transpose feature.<br>    true: Enables the cube transpose feature. At runtime, you can set whether cubes A and B are transposed through the isTransposeA and isTransposeB parameters in [SetTensorA](SetTensorA.md) and [SetTensorB](SetTensorB.md), respectively. If cubes A and B are set to be transposed, Matmul considers the shape of cube A to be [K, M] and the shape of cube B to be [N, K].<br>false: Default value. The cube transpose feature is not enabled, and the transpose status of cubes A and B cannot be set through [SetTensorA](SetTensorA.md) and [SetTensorB](SetTensorB.md). Matmul considers the shape of cube A to be [M, K] and the shape of cube B to be [K, N].<br>    <br>Note that because cube data on the L1 Buffer has fractal alignment constraints, the L1 space required for transposed and non-transposed cubes A and B may differ. When the cube transpose feature is enabled, you must ensure that the L1 space requested according to [Matmul Tiling parameters](../Matmul_Tiling/TCubeTiling_struct.md) does not exceed the L1 Buffer specification. The judgment method is (depthA1*Ceil(baseM/c0Size)*baseK + depthB1*Ceil(baseN/c0Size)*baseK) * db * sizeof(dtype) < L1Size, where db indicates whether double buffer is enabled for L1, taking the value 1 (double buffer not enabled) or 2 (double buffer enabled). For the meanings of the other parameters, see [Table 1](../Matmul_Tiling/TCubeTiling_struct.md#tcubetiling-struct).<!-- npu="9030" id7 --><br>    <br>Kirin 9030 processor products do not support this parameter.<!-- end id7 --> |
    | LAYOUT | Represents the data format.<br>    <br>NONE: Default value, indicating that BatchMatmul is not used. Other options indicate that BatchMatmul is used.<br>    <br>NORMAL: BMNK data format. For details, see the introduction to this data format in [IterateBatch](IterateBatch.md#description).<br>    <br>BSNGD: Data format after reshaping the original BSH shape. For details, see the introduction to this data format in [IterateBatch](IterateBatch.md#description).<br>    <br>SBNGD: Data format after reshaping the original SBH shape. For details, see the introduction to this data format in [IterateBatch](IterateBatch.md#description).<br>    <br>BNGS1S2: Generally the output of cube multiplication of the first two data formats. S1S2 data is stored contiguously, and one S1S2 is the computation data of one batch. For details, see the introduction to this data format in [IterateBatch](IterateBatch.md#description).<!-- npu="9030" id8 --><br>    <br>Kirin 9030 processor products do not support this parameter.<!-- end id8 --> |
    | IBSHARE<a name="p84551411817"></a> | Whether to enable IBShare (IntraBlock Share). The function of IBShare is to reuse the same cube A or cube B data on the L1 Buffer. The reused cube must be fully loaded on the L1 Buffer. The scenario where only one of cube A and cube B enables IBShare is used together with the [IBShare template](MatmulConfig.md#template-features). For specific parameter settings, see [Table 2](MatmulConfig.md#matmulconfig-params).<br>    <br>Note that the scenario where cube A and cube B enable IBShare at the same time indicates that cube A and cube B on the L1 Buffer are reused simultaneously, which requires the following:<br>    In the same operator, cube A and cube B of other Matmul objects must also enable IBShare at the same time;<!-- npu="910b" id90 --><br><br>For Atlas A2 products, when getting the cube computation result, only the [IterateAll](IterateAll.md) API is supported, and only output to GlobalTensor is supported, that is, the computation result is placed in the Global Memory address.<!-- end id90 --><!-- npu="A3" id91 --><br><br>For Atlas A3 products, when getting the cube computation result, only the [IterateAll](IterateAll.md) API is supported, and only output to GlobalTensor is supported, that is, the computation result is placed in the Global Memory address.<!-- end id91 --><!-- npu="950" id92 --><br><br>For Ascend 950PR&950DT products, when getting the cube computation result, output to GlobalTensor and LocalTensor is supported. For output to LocalTensor, that is, the scenario where the computation result is placed in Local Memory, the default implementation strategy is the same as the [SplitM template strategy](MatmulPolicy.md), and only the float output data type is supported, and only the [Norm template](MatmulConfig.md) is supported.<!-- end id92 --><!-- npu="950" id31 --><br>    <br>Ascend 950PR&950DT products support this parameter.<!-- end id31 --><!-- npu="A3" id32 --><br>    <br>Atlas A3 products support this parameter.<!-- end id32 --><!-- npu="910b" id33 --><br>    <br>Atlas A2 products support this parameter.<!-- end id33 --><!-- npu="310p" id34 --><br>    <br>The AI Core of Atlas inference products does not support this parameter.<!-- end id34 --><!-- npu="310b" id35 --><br>    <br>Atlas 200I/500 A2 inference product does not support this parameter.<!-- end id35 --><!-- npu="x90" id9 --><br>    <br>Kirin X90 processor products do not support this parameter.<!-- end id9 --><!-- npu="9030" id10 --><br>    <br>Kirin 9030 processor products do not support this parameter.<!-- end id10 --> |
    | SRC_POSITION | <!-- npu="950" id36 -->This parameter is supported only on Ascend 950PR&950DT products.<br>    <br><!-- end id36 -->When the POSITION parameter of the A/B cube is configured as TPosition::TSCM, the memory logical position of the source of the cube data in TSCM must be set, defaulting to TPosition::GM.<!-- npu="950" id37 --><br><br>For Ascend 950PR&950DT products:<br>    The A cube can be set to TPosition::GM, TPosition::VECOUT<br>The B cube can be set to TPosition::GM, TPosition::VECOUT<!-- end id37 --> |

    <!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Matmul_usage_res.md#id5 -->

    <!-- npu="950,A3,910b,310p,310b,x90,9030" id102 -->
    **Table 2**  Supported list of Matmul input and output data types

    <a name="table1996113269499"></a>

    | A Cube | B Cube | Bias | C Cube | Supported Platform |
    | --- | --- | --- | --- | --- |
    | float | float | float/half | float/half/bfloat16_t | <!-- npu="950" id38 -->Ascend 950PR&950DT products<!-- end id38 --><!-- npu="A3" id39 --><br>    <br>Atlas A3 products<!-- end id39 --><!-- npu="910b" id40 --><br>   <br>Atlas A2 products<!-- end id40 --><!-- npu="310b" id41 --><br>    <br>Atlas 200I/500 A2 inference products<!-- end id41 --> |
    | half | half | float | float | <!-- npu="950" id42 -->Ascend 950PR&950DT products<!-- end id42 --><!-- npu="A3" id43 --><br>    <br>Atlas A3 products<!-- end id43 --><!-- npu="910b" id44 --><br>   <br>Atlas A2 products<!-- end id44 --><!-- npu="310p" id45 --><br>    <br>The AI Core of Atlas inference products<!-- end id45 --><!-- npu="310b" id46 --><br>    <br>Atlas 200I/500 A2 inference products<!-- end id46 --> |
    | half | half | half | float/bfloat16_t | <!-- npu="950" id47 -->Ascend 950PR&950DT products<!-- end id47 --><!-- npu="A3" id48 --><br>    <br>Atlas A3 products<!-- end id48 --><!-- npu="910b" id49 --><br>   <br>Atlas A2 products<!-- end id49 --><!-- npu="310b" id50 --><br>    <br>Atlas 200I/500 A2 inference products<!-- end id50 --> |
    | int8_t | int8_t | int32_t | int32_t/half | <!-- npu="950" id93 -->Ascend 950PR&950DT products<!-- end id93 --><!-- npu="A3" id94 --><br>    <br>Atlas A3 products<!-- end id94 --><!-- npu="910b" id95 --><br>   <br>Atlas A2 products<!-- end id95 --><!-- npu="310p" id96 --><br>    <br>The AI Core of Atlas inference products<!-- end id96 --><!-- npu="310b" id97 --><br>    <br>Atlas 200I/500 A2 inference products<!-- end id97 --><!-- npu="x90" id11 --><br>    <br>Kirin X90 processor products<!-- end id11 --> |
    | int4b_t | int4b_t | int32_t | int32_t/half | <!-- npu="A3" id51 -->Atlas A3 products<!-- end id51 --><!-- npu="910b" id52 --><br>   <br>Atlas A2 products<!-- end id52 --> |
    | bfloat16_t | bfloat16_t | float | float | <!-- npu="950" id53 -->Ascend 950PR&950DT products<!-- end id53 --><!-- npu="A3" id54 --><br>    <br>Atlas A3 products<!-- end id54 --><!-- npu="910b" id55 --><br>   <br>Atlas A2 products<!-- end id55 --><!-- npu="310b" id56 --><br>    <br>Atlas 200I/500 A2 inference products<!-- end id56 --> |
    | bfloat16_t | bfloat16_t | half | float/half | <!-- npu="950" id57 -->Ascend 950PR&950DT products<!-- end id57 --><!-- npu="A3" id58 --><br>    <br>Atlas A3 products<!-- end id58 --><!-- npu="910b" id59 --><br>   <br>Atlas A2 products<!-- end id59 --> |
    | half | half | float | int8_t | <!-- npu="950" id60 -->Ascend 950PR&950DT products<!-- end id60 --><!-- npu="A3" id61 --><br>    <br>Atlas A3 products<!-- end id61 --><!-- npu="910b" id62 --><br>   <br>Atlas A2 products<!-- end id62 --> |
    | bfloat16_t | bfloat16_t | float | int8_t | <!-- npu="950" id63 -->Ascend 950PR&950DT products<!-- end id63 --><!-- npu="A3" id64 --><br>    <br>Atlas A3 products<!-- end id64 --><!-- npu="910b" id65 --><br>   <br>Atlas A2 products<!-- end id65 --> |
    | int8_t | int8_t | int32_t | int8_t | <!-- npu="950" id66 -->Ascend 950PR&950DT products<!-- end id66 --><!-- npu="A3" id67 --><br>    <br>Atlas A3 products<!-- end id67 --><!-- npu="910b" id68 --><br>   <br>Atlas A2 products<!-- end id68 --><!-- npu="310p" id69 --><br>    <br>The AI Core of Atlas inference products<!-- end id69 --><!-- npu="x90" id12 --><br>    <br>Kirin X90 processor products<!-- end id12 --> |
    | half | half | float | half/bfloat16_t | <!-- npu="950" id70 -->Ascend 950PR&950DT products<!-- end id70 --><!-- npu="A3" id71 --><br>    <br>Atlas A3 products<!-- end id71 --><!-- npu="910b" id72 --><br>   <br>Atlas A2 products<!-- end id72 --><!-- npu="310p" id73 --><br>    <br>The AI Core of Atlas inference products<!-- end id73 --><!-- npu="310b" id74 --><br>    <br>Atlas 200I/500 A2 inference products<!-- end id74 --> |
    | half | half | half | half/bfloat16_t | <!-- npu="950" id98 -->Ascend 950PR&950DT products<!-- end id98 --><!-- npu="A3" id99 --><br>    <br>Atlas A3 products<!-- end id99 --><!-- npu="910b" id100 --><br>   <br>Atlas A2 products<!-- end id100 --><!-- npu="310b" id101 --><br>    <br>Atlas 200I/500 A2 inference products<!-- end id101 --><!-- npu="x90" id13 --><br>    <br>Kirin X90 processor products<!-- end id13 --><!-- npu="9030" id14 --><br>    <br>Kirin 9030 processor products<!-- end id14 --> |
    | bfloat16_t | bfloat16_t | float | bfloat16_t/half | <!-- npu="950" id75 -->Ascend 950PR&950DT products<!-- end id75 --><!-- npu="A3" id76 --><br>    <br>Atlas A3 products<!-- end id76 --><!-- npu="910b" id77 --><br>   <br>Atlas A2 products<!-- end id77 --><!-- npu="310b" id78 --><br>    <br>Atlas 200I/500 A2 inference products<!-- end id78 --> |
    | half | int8_t | float | float | <!-- npu="310p" id79 -->The AI Core of Atlas inference products<!-- end id79 --> |
    | fp8_e4m3fn_t/fp8_e5m2_t | fp8_e4m3fn_t/fp8_e5m2_t | float/half/bfloat16_t | fp8_e4m3fn_t/half/bfloat16_t/float | <!-- npu="950" id80 -->Ascend 950PR&950DT products<!-- end id80 --> |
    | hifloat8_t | hifloat8_t | float/half/bfloat16_t | hifloat8_t/half/bfloat16_t/float | <!-- npu="950" id81 -->Ascend 950PR&950DT products<!-- end id81 --> |
    | float | float | bfloat16_t | float/half/bfloat16_t | <!-- npu="950" id82 -->Ascend 950PR&950DT products<!-- end id82 --> |
    | bfloat16_t | bfloat16_t | bfloat16_t | float/half/bfloat16_t | <!-- npu="950" id83 -->Ascend 950PR&950DT products<!-- end id83 --> |
    | half | half | bfloat16_t | float/half/bfloat16_t | <!-- npu="950" id84 -->Ascend 950PR&950DT products<!-- end id84 --> |
    | int8_t | int8_t | int32_t | bfloat16_t | <!-- npu="950" id85 -->Ascend 950PR&950DT products<!-- end id85 --> |

    <!-- end id102 -->

    <!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Matmul_usage_res.md#id6 -->

2.  Initialize the operation.
<a name="li5878185413338"></a>

    ```
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling); // Initialize the matmul object. For the parameter meanings, see the REGIST_MATMUL_OBJ section.
    ```

3.  Set the left cube A, right cube B, and Bias.

    ```
    mm.SetTensorA(gm_a);    // Set left cube A.
    mm.SetTensorB(gm_b);    // Set right cube B.
    mm.SetBias(gm_bias);    // Set Bias.
    ```

    <!-- npu="310p" id86 -->
    On the AI Core of Atlas inference products, you need to additionally call the [SetLocalWorkspace](SetLocalWorkspace.md) API to set the UB space required for computation.

    ```
    mm.SetLocalWorkspace(usedUbBufLen);
    ```
    <!-- end id86 -->

4.  Complete the cube multiplication operation.

    You can choose one of the following three calling methods.

    -   Call [Iterate](Iterate.md#li135771283591) to complete a single iteration, and add a while loop to complete the computation of all data on a single core. With the Iterate method, you can control the number of iterations by yourself to complete the computation of the required amount of data, which is more flexible.

        ```
        // The API internally determines the loop termination condition.
        while (mm.Iterate()) {
            mm.GetTensorC(gm_c);
        }
        ```

    -   Call [IterateAll](IterateAll.md) to complete the computation of all data on a single core. The IterateAll method does not require loop iteration and is simpler to use.

        ```
        mm.IterateAll(gm_c);
        ```

    <a id="user-managed-co1"></a>
    
    -   The user applies for L0C Buffer (CO1) memory to store the cube multiplication result, calls [Iterate](Iterate.md#li4843165185812) once or multiple times to complete one or more iterations, and when the computation result needs to be moved out, calls the [Fixpipe](../../../basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md) API to move the computation result from the L0C Buffer (CO1), and then releases the applied L0C Buffer (CO1) memory. With this method, the user can flexibly control the pace of computation and data movement. As needed, one computation corresponds to moving out the result once, or multiple computation results can be cached in the L0C Buffer (CO1) memory and then moved out all at once.

        In this calling method, when creating the Matmul object, the memory logical position of the C cube must be defined as **TPosition::CO1**, the data format as **CubeFormat::NZ**, and the data type as **float** or int32\_t.

          <!-- npu="950" id87 -->
        - Ascend 950PR&950DT products do not support this method yet.
          <!-- end id87 -->
          <!-- npu="310p" id88 -->
        - The AI Core of Atlas inference products does not support this method yet.
          <!-- end id88 -->
          <!-- npu="310b" id89 -->
        - Atlas 200I/500 A2 inference products do not support this method yet.
          <!-- end id89 -->
        <!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Matmul_usage_res.md#id7 -->

        ```
        // Define the type information of cube C.
        typedef AscendC::MatmulType<AscendC::TPosition::CO1, CubeFormat::NZ, float> cType;
        // Create a Matmul object.
        AscendC::Matmul<aType, bType, cType, biasType> mm;

        // Allocate the memory l0cTensor of the L0C Buffer (CO1) in advance.
        TQue<TPosition::CO1, 1> CO1_;
        // 128 * 1024 is the memory size of the applied L0C Buffer (CO1).
        GetTPipePtr()->InitBuffer(CO1_, 1, 128 * 1024);
        // L0cT is the data type of cube C.
        // When the data type of cube A is int8_t or int4b_t, the data type of cube C is int32_t.
        // When the data type of cube A is half, float, or bfloat16_t, the data type of cube C is float.
        LocalTensor<L0cT> l0cTensor = CO1_.template AllocTensor<L0cT>();

        // Pass l0cTensor to Iterate as an input parameter, and the cube multiplication result is output to the l0cTensor allocated by the user.
        mm.Iterate(false, l0cTensor);

        // Call the Fixpipe API to move the computation result on the L0C Buffer (CO1) to GM.
        FixpipeParamsV220 params;
        params.nSize = nSize;
        params.mSize = mSize;
        params.srcStride = srcStride;
        params.dstStride = dstStride;
        CO1_.EnQue(l0cTensor);
        CO1_.template DeQue<L0cT>();
        Fixpipe<cType, L0cT, CFG_ROW_MAJOR>(gm[dstOffset], l0cTensor, params);

        // Release the L0C Buffer (CO1) memory.
        CO1_.FreeTensor(l0cTensor);
        ```

5.  End the cube multiplication operation.

    ```
    mm.End();
    ```

<a name="table98851538118"></a>
The cube alignment requirements for the CubeFormat::NZ format are as follows:

- Cube A/Cube  B: The outer axis is a multiple of 16, and the inner axis is a multiple of C0_size.
- C cube:
  <!-- npu="310p,310b" id103 -->
  - For the following product models:
    <!-- npu="310p" id105 -->
    - AI Core of Atlas inference products
    <!-- end id105 -->
    <!-- npu="310b" id106 -->
    - Atlas 200I/500 A2 inference products
    <!-- end id106 -->
    The outer axis is a multiple of 16, and the inner axis is a multiple of 16.
  <!-- end id103 -->
  <!-- npu="950,A3,910b" id104 -->
  - For the following product models:
    <!-- npu="950" id107 -->
    - Ascend 950PR&950DT products
    <!-- end id107 -->
    <!-- npu="A3" id108 -->
    - Atlas A3 products
    <!-- end id108 -->
    <!-- npu="910b" id109 -->
    - Atlas A2 products
    <!-- end id109 -->
    With the channel_split feature enabled: The outer axis is a multiple of 16, and the inner axis is a multiple of C0_size.

    With the channel_split feature disabled: The outer axis is a multiple of 16. For the float/int32_t type, the inner axis is a multiple of 16. For the half/bfloat16_t/int8_t/fp8_e4m3fn_t/fp8_e5m2_t/hifloat8_t type, the inner axis is a multiple of C0_size.
  <!-- end id104 -->
  <!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Matmul_usage_res.md#id8 -->

The supplementary description of the above parameters and concepts is as follows:

-   The C0\_size for float/int32\_t data types is 8, the C0\_size for half/bfloat16\_t data types is 16, the C0\_size for int8\_t/fp8\_e4m3fn\_t/fp8\_e5m2\_t/hifloat8\_t data types is 32, and the C0\_size for int4b\_t/fp4x2\_e2m1\_t/fp4x2\_e1m2\_t data types is 64.
-   channel\_split function is configured through the isEnableChannelSplit parameter in [MatmulConfig](MatmulConfig.md#matmulconfig-params). For details, see [MatmulConfig](MatmulConfig.md#matmulconfig-params).

## Required Header File

```
#include "lib/matmul/matmul_intf.h"
```

## Implementation Principle

Take the scenario of input cube A \(GM, ND, half\), cube B\(GM, ND, half\), output cube C \(GM, ND, float\), with no Bias, as an example, where \(GM, ND, half\) indicates that the data is stored in GM, the data format is ND, and the data type is half. The internal algorithm diagram of a typical Matmul high-level API scenario is described as shown in the following figure.

**Figure 2**  Matmul algorithm diagram  
![](../../../../figures/matmul_algorithm_diagram.png "Matmul algorithm diagram")

The computation process is divided into the following steps:

1.  Data is moved from GM to L1 Buffer (A1): DataCopy moves a stepM\*baseM\*stepKa\*baseK cube block a1 from cube A each time, and loops multiple times to complete moving cube A; data is moved from GM to L1 Buffer (B1): DataCopy moves a stepKb\*baseK\*stepN\*baseN cube block b1 from cube B each time, and loops multiple times to complete moving cube B;
2.  Data is moved from L1 Buffer (A1) to L0A Buffer (A2): LoadData moves a baseM \* baseK cube block a0 from cube block a1 each time; data is moved from L1 Buffer (B1) to L0B Buffer (B2), and the transpose is completed: LoadData moves a baseK \* baseN cube block from cube block b1 each time, and transposes it into a baseN \* baseK cube block b0;
3.  Cube multiplication: each time the computation of a cube block a0 \* b0 is completed, a baseM \* baseN cube block co1 is obtained;
4.  Data is moved from cube block co1 to cube block co2: DataCopy moves a baseM \* baseN cube block co1 to a singleCoreM \* singleCoreN cube block co2 each time;
5.  Repeat steps 2-4 to complete the computation of cube block a1 \* b1;
6.  Data is moved from cube block co2 to cube block C: DataCopy moves a singleCoreM \* singleCoreN cube block co2 to cube block C each time;
7.  Repeat steps 1 to 6 to complete the computation of cube A \* B = C.
