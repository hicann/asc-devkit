# Matmul Usage

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:48:31.344Z pushedAt=2026-09-12T09:55:18.123Z -->

Ascend C provides a set of high-level Matmul APIs to help you quickly implement Matmul matrix multiplication operations.

The formula for Matmul is: C = A \* B + Bias, as shown in the following figure.

-   A and B are source operands. A is the left matrix with a shape of \[M, K\]; B is the right matrix with a shape of \[K, N\].

-   C is the destination operand, which is the matrix that stores the matrix multiplication result, with a shape of \[M, N\].

-   Bias is the matrix multiplication bias, with a shape of \[1, N\]. This Bias is applied to each row of the A\*B result matrix.

**Figure 1**  Matmul matrix multiplication
![](../../../../figures/matmul_matrix_multiplication.png)

> [!NOTE]
> In the following text, the M-axis direction refers to the vertical direction of matrix A; the K-axis direction refers to the horizontal direction of matrix A or the vertical direction of matrix B; the N-axis direction refers to the horizontal direction of matrix B; and the last axis refers to the last dimension of a matrix.

The steps to implement the Matmul matrix multiplication operation on the Kernel side are summarized as follows:

1.  Create a Matmul object.

2.  Initialize the operation.

3.  Set the left matrix A, right matrix B, and Bias.

4.  Complete the matrix multiplication operation.

4.  End the matrix multiplication operation.

The specific steps for implementing matrix multiplication using the Matmul API are as follows:

1.  Create a Matmul object.

    The following is an example of creating a Matmul object:

    -   In the default MIX mode (including matrix computation and vector computation), the ASCENDC\_CUBE\_ONLY macro is usually not defined in this scenario. If the ASCENDC\_CUBE\_ONLY macro is used in the program, the ASCEND\_IS\_AIC macro and the ASCEND\_IS\_AIV macro must be used to isolate Cube computation from Vector computation.

    -   In the pure Cube mode (matrix computation only), it is recommended to define the ASCENDC\_CUBE\_ONLY macro in the code to avoid additional performance overhead.

    ```
    // In the cube only mode (matrix computation only) scenario, this code macro needs to be set, and it must be set before #include "lib/matmul_intf.h".


    // #define ASCENDC_CUBE_ONLY
    #include "lib/matmul_intf.h"

    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    AscendC::Matmul<aType, bType, cType, biasType> mm;
    ```

    When creating an object, you need to pass in the parameter type information of A, B, C, and Bias. The type information is defined by [MatmulType](#table1188045714378), including: logical memory position, data format, data type, and logical memory position of the data source.

    ```
    template <AscendC::TPosition POSITION, CubeFormat FORMAT, typename TYPE, bool ISTRANS = false, LayoutMode LAYOUT = LayoutMode::NONE, bool IBSHARE = false, TPosition SRCPOS = TPosition::GM> struct MatmulType {
        constexpr static AscendC::TPosition pos = POSITION;
        constexpr static CubeFormat format = FORMAT;
        using T = TYPE;
        constexpr static bool isTrans = ISTRANS;
        constexpr static LayoutMode layout = LAYOUT;
        constexpr static bool ibShare = IBSHARE;
        constexpr static TPosition srcPos = SRCPOS;
    };
    ```

**Table 1**  MatmulType parameters

    <a name="table1188045714378"></a>

| Parameter | Description |
| --- | --- |
| POSITION | Logical memory position.<br>    <br>For Ascend 950PR/Ascend 950DT:<br>    The A matrix can be set to TPosition::GM, TPosition::VECOUT, TPosition::TSCM.<br>The B matrix can be set to TPosition::GM, TPosition::VECOUT, TPosition::TSCM.<br>Bias can be set to TPosition::GM, TPosition::VECOUT, TPosition::TSCM.<br>The C matrix can be set to TPosition::GM, TPosition::VECIN.<br> Note: When the A matrix, B matrix, or Bias matrix is set to TPosition::VECOUT or TPosition::TSCM, all data of the corresponding matrix used for single-core computation must reside in Unified Buffer or L1 Buffer. For specific examples, refer to [matmul_vecout sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_vecout), [Matmul operator sample with TSCM input whose custom data source is VECOUT](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_tscm_src_vecout), [Matmul operator sample with TSCM input whose custom data source is GM](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_tscm).<br>    <br>For Atlas A3 Training Series Products/Atlas A3 Inference Series Products:<br>    The A matrix can be set to TPosition::GM, TPosition::VECOUT, TPosition::TSCM.<br>The B matrix can be set to TPosition::GM, TPosition::VECOUT, TPosition::TSCM.<br>Bias can be set to TPosition::GM, TPosition::VECOUT.<br>The C matrix can be set to TPosition::GM, TPosition::VECIN, TPosition::CO1.<br>   Note: When the C matrix is set to TPosition::CO1, its data layout format supports only CubeFormat::NZ, and its data type supports only float and int32_t.<br>    <br>For Atlas A2 Training Series Products/Atlas A2 Inference Series Products:<br>    The A matrix can be set to TPosition::GM, TPosition::VECOUT, TPosition::TSCM.<br>The B matrix can be set to TPosition::GM, TPosition::VECOUT, TPosition::TSCM.<br>Bias can be set to TPosition::GM, TPosition::VECOUT.<br>The C matrix can be set to TPosition::GM, TPosition::VECIN, TPosition::CO1.<br>   Note: When the C matrix is set to TPosition::CO1, its data layout format supports only CubeFormat::NZ, and its data type supports only float and int32_t.<br>    <br>For Atlas Inference Series Products AI Core:<br>    The A matrix can be set to TPosition::GM, TPosition::VECOUT.<br>The B matrix can be set to TPosition::GM, TPosition::VECOUT.<br>Bias can be set to TPosition::GM, TPosition::VECOUT.<br>The C matrix can be set to TPosition::GM, TPosition::VECIN.<br>    <br>For Atlas 200I/500 A2 Inference Products:<br>    The A matrix can be set to TPosition::GM.<br>The B matrix can be set to TPosition::GM.<br>Bias can be set to TPosition::GM.<br>The C matrix can be set to TPosition::GM |
| FORMAT | Physical layout format of the data.<br>    <br>For Ascend 950PR/Ascend 950DT:<br>    The A matrix can be set to CubeFormat::ND, CubeFormat::NZ, CubeFormat::COLUMN_MAJOR, CubeFormat::VECTOR.<br>The B matrix can be set to CubeFormat::ND, CubeFormat::NZ, CubeFormat::COLUMN_MAJOR.<br>Bias can be set to CubeFormat::ND.<br>The C matrix can be set to CubeFormat::ND, CubeFormat::NZ, CubeFormat::ND_ALIGN, CubeFormat::COLUMN_MAJOR.<br>    <br>For Ascend 950PR/Ascend 950DT, note that:<br>    Only in non-MxMatmul scenarios do the A, B, and C matrix formats support CubeFormat::COLUMN_MAJOR. When Format is CubeFormat::COLUMN_MAJOR, the corresponding matrix supports only the logical memory position TPosition::GM.<br>When the input A matrix or B matrix is set to TPosition::TSCM, the corresponding Format supports only CubeFormat::NZ.<br>When the C matrix is set to TPosition::VECIN and CubeFormat::ND, the last axis must be 32-byte aligned; for example, if the data type is half, N must be a multiple of 16.<br>    <br>For Atlas A3 Training Series Products/Atlas A3 Inference Series Products:<br>    The A matrix can be set to CubeFormat::ND, CubeFormat::NZ, CubeFormat::VECTOR.<br>The B matrix can be set to CubeFormat::ND, CubeFormat::NZ.<br>Bias can be set to CubeFormat::ND.<br>The C matrix can be set to CubeFormat::ND, CubeFormat::NZ, CubeFormat::ND_ALIGN.<br>    <br>For Atlas A2 Training Series Products/Atlas A2 Inference Series Products:<br>    The A matrix can be set to CubeFormat::ND, CubeFormat::NZ, CubeFormat::VECTOR.<br>The B matrix can be set to CubeFormat::ND, CubeFormat::NZ.<br>Bias can be set to CubeFormat::ND.<br>The C matrix can be set to CubeFormat::ND, CubeFormat::NZ, CubeFormat::ND_ALIGN.<br>    <br>For Atlas Inference Series Products AI Core:<br>    The A matrix can be set to CubeFormat::ND, CubeFormat::NZ.<br>The B matrix can be set to CubeFormat::ND, CubeFormat::NZ.<br>Bias can be set to CubeFormat::ND.<br>The C matrix can be set to CubeFormat::ND, CubeFormat::NZ, CubeFormat::ND_ALIGN.<br> Note: For Atlas Inference Series Products AI Core, when the C matrix is set to CubeFormat::ND, the last axis must be 32-byte aligned; for example, if the data type is half, N must be a multiple of 16.<br>    <br>For Atlas 200I/500 A2 Inference Products:<br>    The A matrix can be set to CubeFormat::ND, CubeFormat::NZ.<br>The B matrix can be set to CubeFormat::ND, CubeFormat::NZ.<br>Bias can be set to CubeFormat::ND.<br>The C matrix can be set to CubeFormat::ND, CubeFormat::NZ.<br>Note: For Atlas 200I/500 A2 Inference Products, when the C matrix is set to TPosition::VECIN or TPosition::TSCM with CubeFormat::ND, the last axis must be 32-byte aligned; for example, if the data type is half, N must be a multiple of 16. When the C matrix is set to TPosition::VECIN or TPosition::TSCM with CubeFormat::NZ, N must be a multiple of 16.<br>    <br>For the alignment constraints of the A, B, and C matrices in CubeFormat::NZ format, refer to [Table 3](#table98851538118). |
| TYPE | Data type.<br>    <br>For Ascend 950PR/Ascend 950DT:<br>    1. Non-MxMatmul scenarios:<br>The A matrix can be set to half, float, bfloat16_t, int8_t, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t.<br>The B matrix can be set to half, float, bfloat16_t, int8_t, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t.<br>Bias can be set to half, float, int32_t, bfloat16_t.<br>The C matrix can be set to half, float, bfloat16_t, int32_t, int8_t, fp8_e4m3fn_t, hifloat8_t.<br>    2. MxMatmul scenarios:<br>The A matrix can be set to fp8_e4m3fn_t, fp8_e5m2_t, fp4x2_e2m1_t, fp4x2_e1m2_t.<br>The B matrix can be set to fp8_e4m3fn_t, fp8_e5m2_t, fp4x2_e2m1_t, fp4x2_e1m2_t.<br>Bias can be set to half, float, bfloat16_t.<br>The C matrix can be set to half, float, bfloat16_t.<br>    <br>    For Atlas A3 Training Series Products/Atlas A3 Inference Series Products:<br>The A matrix can be set to half, float, bfloat16_t, int8_t, int4b_t.<br>The B matrix can be set to half, float, bfloat16_t, int8_t, int4b_t.<br>Bias can be set to half, float, int32_t.<br>The C matrix can be set to half, float, bfloat16_t, int32_t, int8_t.<br>    <br>    For Atlas A2 Training Series Products/Atlas A2 Inference Series Products:<br>The A matrix can be set to half, float, bfloat16_t, int8_t, int4b_t.<br>The B matrix can be set to half, float, bfloat16_t, int8_t, int4b_t.<br>Bias can be set to half, float, int32_t.<br>The C matrix can be set to half, float, bfloat16_t, int32_t, int8_t.<br><br>For Atlas Inference Series Products AI Core:<br>    The A matrix can be set to half, int8_t.<br>The B matrix can be set to half, int8_t.<br>Bias can be set to float, int32_t.<br>The C matrix can be set to half, float, int8_t, int32_t.<br>    <br>For Atlas 200I/500 A2 Inference Products:<br>    The A matrix can be set to half, float, bfloat16_t, int8_t.<br>The B matrix can be set to half, float, bfloat16_t, int8_t.<br>The Bias matrix can be set to half, float, int32_t.<br>The C matrix can be set to half, float, bfloat16_t, int32_t.<br>Note: Except for the two data types fp8_e4m3fn_t/fp8_e5m2_t and the case where the B matrix is int8_t, the data types of the A and B matrices must be the same. For specific data type combinations, refer to [Table 2](#table1996113269499). When the A and B matrices are of the int4b_t data type, the number of data elements on the inner axis of the matrix must be even. For example, when the A matrix is of the int4b_t data type and is not transposed, [singleCoreK](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct) must be even. |
| ISTRANS | Whether to enable matrix transpose.<br>    true: Enables matrix transpose. At runtime, whether the A and B matrices are transposed can be set through the isTransposeA and isTransposeB parameters in [SetTensorA](SetTensorA.md) and [SetTensorB](SetTensorB.md), respectively. If the A and B matrices are set to be transposed, Matmul considers the A matrix shape to be [K, M] and the B matrix shape to be [N, K].<br>false: Default value. Does not enable matrix transpose. The transpose settings of the A and B matrices cannot be set through [SetTensorA](SetTensorA.md) and [SetTensorB](SetTensorB.md). Matmul considers the A matrix shape to be [M, K] and the B matrix shape to be [K, N].<br>    <br>Note: Because matrix data in L1 Buffer has fractal alignment constraints, the L1 space required when the A and B matrices are transposed and when they are not transposed may differ. When matrix transpose is enabled, you must ensure that the L1 space allocated according to the[Matmul tiling parameters](../Matmul-Tiling/tcubetiling_structure.md) does not exceed the L1 Buffer specification. The judgment method is (depthA1*Ceil(baseM/c0Size)*baseK + depthB1*Ceil(baseN/c0Size)*baseK) * db * sizeof(dtype) < L1Size, where db indicates whether L1 enables double buffer, taking the value 1 (double buffer disabled) or 2 (double buffer enabled). For the meanings of the other parameters, refer to [Table 1] (../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct). |
| LAYOUT | Represents the data layout.<br>    <br>NONE: Default value, indicating that BatchMatmul is not used; other options indicate that BatchMatmul is used.<br>    <br>NORMAL: B MNK data layout format. For details, refer to the introduction to this data layout in [IterateBatch](IterateBatch.md).<br>    <br>BSNGD: Data layout after reshaping the original BSH shape. For details, refer to the introduction to this data layout in [IterateBatch](IterateBatch.md).<br>    <br>SBNGD: Data layout after reshaping the original SBH shape. For details, refer to the introduction to this data layout in [IterateBatch](IterateBatch.md).<br>    <br>BNGS1S2: Generally the output of matrix multiplication of the first two data layouts. The S1S2 data is stored contiguously, and one S1S2 is the computation data of one batch. For details, refer to the introduction to this data layout in [IterateBatch](IterateBatch.md). |
| IBSHARE<a name="p84551411817"></a> | Whether to enable IBShare (IntraBlock Share). The function of IBShare is to reuse the same A matrix or B matrix data in L1 Buffer. The reused matrix must be fully loaded in L1 Buffer. The scenario where only one of the A matrix and B matrix enables IBShare is used together with the [IBShare template](MatmulConfig.md#table6981133810309). For specific parameter settings, refer to [Table 2](MatmulConfig.md#matmulconfig-params).<br>    <br>Note: The scenario where the A matrix and B matrix enable IBShare at the same time means that the A matrix and B matrix in L1 Buffer are reused at the same time, and the following requirements must be met:<br>    The A matrix and B matrix of other Matmul objects in the same operator must also enable IBShare at the same time.<br>For Atlas A2 Training Series Products/Atlas A2 Inference Series Products, when obtaining the matrix computation result, only the [IterateAll](IterateAll.md) API can be called, and only output to GlobalTensor is supported, that is, the computation result is placed at an address in Global Memory.<br>For Atlas A3 Training Series Products/Atlas A3 Inference Series Products, when obtaining the matrix computation result, only the [IterateAll](IterateAll.md) API can be called, and only output to GlobalTensor is supported, that is, the computation result is placed at an address in Global Memory.<br>For Ascend 950PR/Ascend 950DT, when obtaining the matrix computation result, output to both GlobalTensor and LocalTensor is supported. For output to LocalTensor, that is, the scenario where the computation result is placed in Local Memory, the default implementation strategy is the same as the [SplitM template strategy](MatmulPolicy.md#splitm-template-strategy), and only the output data type float is supported, and only the [Norm template](MatmulConfig.md) is supported.<br>    <br>Ascend 950PR/Ascend 950DT supports this parameter.<br>    <br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products supports this parameter.<br>    <br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products supports this parameter.<br>    <br>Atlas Inference Series Products AI Core does not support this parameter.<br>    <br>Atlas 200I/500 A2 Inference Products does not support this parameter. |
| SRC_POSITION | This parameter is supported only on Ascend 950PR/Ascend 950DT.<br>    <br>When the POSITION parameter of the A/B matrix is configured as TPosition::TSCM, you must set the logical memory position of the source of the matrix data in TSCM, which defaults to TPosition::GM.<br>    <br>For Ascend 950PR/Ascend 950DT:<br>    The A matrix can be set to TPosition::GM, TPosition::VECOUT.<br>The B matrix can be set to TPosition::GM, TPosition::VECOUT. |

**Table 2**  Supported list of Matmul input and output data types

<a name="table1996113269499"></a>

| A matrix | B matrix | Bias | C matrix | Supported platforms |
| --- | --- | --- | --- | --- |
| float | float | float/half | float/half/bfloat16_t | Ascend 950PR/Ascend 950DT<br>    <br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>    <br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products<br>    <br>Atlas 200I/500 A2 Inference Products |
| half | half | float | float | Ascend 950PR/Ascend 950DT<br>    <br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>    <br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products<br>    <br>Atlas Inference Series Products AI Core<br>    <br>Atlas 200I/500 A2 Inference Products |
| half | half | half | float/bfloat16_t | Ascend 950PR/Ascend 950DT<br>    <br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>    <br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products<br>    <br>Atlas 200I/500 A2 Inference Products |
| int8_t | int8_t | int32_t | int32_t/half | Ascend 950PR/Ascend 950DT<br>    <br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>    <br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products<br>    <br>Atlas Inference Series Products AI Core<br>    <br>Atlas 200I/500 A2 Inference Products |
| int4b_t | int4b_t | int32_t | int32_t/half | Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>    <br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| bfloat16_t | bfloat16_t | float | float | Ascend 950PR/Ascend 950DT<br>    <br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>    <br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products<br>    <br>Atlas 200I/500 A2 Inference Products |
| bfloat16_t | bfloat16_t | half | float/half | Ascend 950PR/Ascend 950DT<br>    <br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>    <br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| half | half | float | int8_t | Ascend 950PR/Ascend 950DT<br>    <br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>    <br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| bfloat16_t | bfloat16_t | float | int8_t | Ascend 950PR/Ascend 950DT<br>    <br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>    <br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| int8_t | int8_t | int32_t | int8_t | Ascend 950PR/Ascend 950DT<br>    <br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>    <br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products<br>    <br>Atlas Inference Series Products AI Core |
| half | half | float | half/bfloat16_t | Ascend 950PR/Ascend 950DT<br>    <br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>    <br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products<br>    <br>Atlas Inference Series Products AI Core<br>    <br>Atlas 200I/500 A2 Inference Products |
| half | half | half | half/bfloat16_t | Ascend 950PR/Ascend 950DT<br>    <br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>    <br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products<br>    <br>Atlas 200I/500 A2 Inference Products |
| bfloat16_t | bfloat16_t | float | bfloat16_t/half | Ascend 950PR/Ascend 950DT<br>    <br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>    <br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products<br>    <br>Atlas 200I/500 A2 Inference Products |
| half | int8_t | float | float | Atlas Inference Series Products AI Core |
| fp8_e4m3fn_t/fp8_e5m2_t | fp8_e4m3fn_t/fp8_e5m2_t | float/half/bfloat16_t | fp8_e4m3fn_t/half/bfloat16_t/float | Ascend 950PR/Ascend 950DT |
| hifloat8_t | hifloat8_t | float/half/bfloat16_t | hifloat8_t/half/bfloat16_t/float | Ascend 950PR/Ascend 950DT |
| float | float | bfloat16_t | float/half/bfloat16_t | Ascend 950PR/Ascend 950DT |
| bfloat16_t | bfloat16_t | bfloat16_t | float/half/bfloat16_t | Ascend 950PR/Ascend 950DT |
| half | half | bfloat16_t | float/half/bfloat16_t | Ascend 950PR/Ascend 950DT |
| int8_t | int8_t | int32_t | bfloat16_t | Ascend 950PR/Ascend 950DT |

2.  Initialize the operation.

<a name="li5878185413338"></a>

    ```
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling); // 初始化matmul对象，参数含义请参考REGIST_MATMUL_OBJ章节
    ```

3.  Set the left matrix A, right matrix B, and Bias.

    ```
    mm.SetTensorA(gm_a);    // Set the left matrix A.
    mm.SetTensorB(gm_b);    // Set the right matrix B.
    mm.SetBias(gm_bias);    // Set the Bias.

    // On the AI Core of Atlas inference products, additionally call the SetLocalWorkspace API to set the UB space required for computation.
    mm.SetLocalWorkspace(usedUbBufLen);
    ```

4.  Complete the matrix multiplication operation.

You can choose one of the following three invocation methods.

    -   Call [Iterate](Iterate.md#li135771283591) to complete a single iteration of computation, and add a while loop to complete the computation of all data on a single core. With the Iterate method, you can control the number of iterations to complete the computation of the required amount of data, which is more flexible.

        ```
        // The API internally determines the loop termination condition.
        while (mm.Iterate()) {
            mm.GetTensorC(gm_c);
        }
        ```

    -   Call [IterateAll](IterateAll.md) to complete the computation of all data on a single core. The IterateAll method requires no loop iteration and is simpler to use.

  ```
    mm.IterateAll(gm_c);
    ```

<a id="user-managed-co1-matmul"></a>

    -   You apply for the CO1 memory at the logical position for storing the matrix multiplication result, call [Iterate](Iterate.md#li4843165185812) once or multiple times to complete one or more iterations of computation, and when the calculation result needs to be moved out, call the [Fixpipe](../../../basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md) API to move the calculation result out of CO1, and then release the applied CO1 memory. With this method, you can flexibly control the pace of computation and data movement. As needed, one computation corresponds to one result move-out, or multiple computation results are cached in the CO1 memory and then moved out at once.

        With this calling method, when creating the Matmul object, you must define the memory logical position of matrix C as TPosition::CO1, the data layout as CubeFormat::NZ, and the data type as float or int32_t.

        -   Ascend 950PR/Ascend 950DT does not support this method yet.

        -   The AI Core of the Atlas inference series products does not support this method yet.

        -   Atlas 200I/500 A2 inference products does not support this method yet.

        ```
        // Define the type information of matrix C.
        typedef AscendC::MatmulType<AscendC::TPosition::CO1, CubeFormat::NZ, float> cType;
        // Create the Matmul object.
        AscendC::Matmul<aType, bType, cType, biasType> mm;

        // Apply for the CO1 memory l0cTensor in advance.
        TQue<TPosition::CO1, 1> CO1_;
        // 128 * 1024 is the requested CO1 memory size.
        GetTPipePtr()->InitBuffer(CO1_, 1, 128 * 1024);
        // L0cT is the data type of matrix C.
        // When the data type of matrix A is int8_t or int4b_t, the data type of matrix C is int32_t.
        // When the data type of matrix A is half, float, or bfloat16_t, the data type of matrix C is float.
        LocalTensor<L0cT> l0cTensor = CO1_.template AllocTensor<L0cT>();

        // Pass l0cTensor to Iterate as an input parameter. The matrix multiplication result is output to the user-allocated l0cTensor.
        mm.Iterate(false, l0cTensor);

        // Call the Fixpipe API to move the calculation result on CO1 to GM.
        FixpipeParamsV220 params;
        params.nSize = nSize;
        params.mSize = mSize;
        params.srcStride = srcStride;
        params.dstStride = dstStride;
        CO1_.EnQue(l0cTensor);
        CO1_.template DeQue<L0cT>();
        Fixpipe<cType, L0cT, CFG_ROW_MAJOR>(gm[dstOffset], l0cTensor, params);

        // Free the CO1 memory.
        CO1_.FreeTensor(l0cTensor);
        ```

5.   End the matrix multiplication operation.

```
mm.End();
```

**Table 3**  Matrix alignment requirements for the CubeFormat::NZ format

<a name="table98851538118"></a>

| Source/Destination Operations | Outer Axis | Inner Axis |
| --- | --- | --- |
| A matrix/B matrix | Multiple of 16 | Multiple of C0_size |
| C matrix | multiple of 16 | Multiple of 16 |
| C matrix (with channel_split enabled) | Multiple of 16 | Multiple of C0_size |
| C matrix (with channel_split disabled) | Multiple of 16 | **float/int32_t**: multiple of 16<br><br>**half/bfloat16_t/int8_t/fp8_e4m3fn_t/fp8_e5m2_t/hifloat8_t**: multiple of C0_size |

The supplementary notes for the related parameters and concepts in the preceding table are as follows:

-   The C0_size of the float/int32_t data type is 8, the C0_size of the half/bfloat16_t data type is 16, the C0_size of the int8_t/fp8_e4m3fn_t/fp8_e5m2_t/hifloat8_t data type is 32, and the C0_size of the int4b_t/fp4x2_e2m1_t/fp4x2_e1m2_t data type is 64.

-   The channel_split feature is configured through the isEnableChannelSplit parameter in [MatmulConfig](MatmulConfig.md#matmulconfig-params). For details, see [MatmulConfig](MatmulConfig.md#matmulconfig-params).

## Header Files to Include

```
#include "lib/matmul/matmul_intf.h"
```

## Implementation Principle

Take the scenario where input matrix A is \(GM, ND, half\), matrix B is \(GM, ND, half\), and output matrix C is \(GM, ND, float\), with no bias, as an example. Here, \(GM, ND, half\) indicates that the data is stored in GM, the data format is ND, and the data type is half. The internal algorithm block diagram of a typical Matmul high-level API scenario is shown in the following figure.

**Figure 2** Matmul algorithm  
![](../../../../figures/matmul_algorithm.png)

The computation process is divided into the following steps:

1.  Data is moved from GM to A1: DataCopy moves out one stepM\*baseM\*stepKa\*baseK matrix block a1 from matrix A each time, and repeats this multiple times to complete the movement of matrix A. Data is moved from GM to B1: DataCopy moves out one stepKb\*baseK\*stepN\*baseN matrix block b1 from matrix B each time, and repeats this multiple times to complete the movement of matrix B.

2.  Data is moved from A1 to A2: LoadData moves out one baseM \* baseK matrix block a0 from matrix block a1 each time. Data is moved from B1 to B2, and the transpose is completed: LoadData moves out one baseK \* baseN matrix block from matrix block b1 each time, and transposes it into a baseN \* baseK matrix block b0.

3.  Matrix multiplication: Each time, the computation of one matrix block a0 \* b0 is completed, obtaining a baseM \* baseN matrix block co1.

4.  Data is moved from matrix block co1 to matrix block co2: DataCopy moves one baseM \* baseN matrix block co1 to a singleCoreM \* singleCoreN matrix block co2 each time.

5.  Repeat steps 2 to 4 to complete the computation of matrix block a1 \* b1.

6.  Data is moved from matrix block co2 to matrix block C: DataCopy moves one singleCoreM \* singleCoreN matrix block co2 to matrix block C each time.

7.  Repeat steps 1 to 6 to complete the calculation of matrix A \* B = C.