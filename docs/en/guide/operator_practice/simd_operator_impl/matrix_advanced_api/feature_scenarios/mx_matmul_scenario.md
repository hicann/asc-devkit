# MxMatmul Scenario

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T12:13:18.705Z pushedAt=2026-11-03T07:05:13.309Z nl=2 -->

## Background

Floating-point numbers are widely used in scientific computing, image processing, neural networks, and other fields. Taking AI training as an example, existing floating-point formats either have an insufficient numerical range or low precision, which affects the convergence speed and performance of models. Meeting both the numerical range and precision requirements at the same time would lead to excessive memory usage, thereby increasing the cost of data storage and transmission. To address this, the industry has proposed a new floating-point format: the Microscaling (MX) format. Floating-point numbers in the MX format can support AI training and inference at lower bit widths while consuming less memory. Data formats compliant with the MX standard can achieve robust AI training and inference model precision when using 8 bits or fewer.

The MX format is a block data format. Multiple data elements can form a block (or a group), and data is organized in units of blocks. Data in the MX format consists of three parts:

-   A shared scaling factor X with a bit width of w bits;
-   Private elements P<sub>i</sub> with a bit width of d bits;
-   A block size k, which indicates how many low-bit data elements form a block;

All k elements P<sub>i</sub> have the same bit width and data type and share a single scaling factor X. Each block containing k elements can be encoded using (w+k\*d) bits. The data type of the elements and the scaling factor can be selected independently.

The following figure shows the data structure of floating-point numbers in the MX format. S, E, and M represent the sign, exponent, and mantissa fields of a floating-point number, respectively. The shared scaling factor X is a scaling factor applied to the entire data block, and it determines the dynamic range of all elements in the data block. By introducing a shared scaling factor, data in the MX format can flexibly represent data of different ranges while maintaining a low bit width. The block size k refers to the number of low-bit data elements that form a data block (or group). A private element P<sub>i</sub> refers to each low-bit data element in the data block. After being adjusted by the scaling factor X, these elements jointly represent a high-precision floating-point number or integer.

**Figure 1**  MX format composition  
![](../../../../figures/mx_format.png "MX format composition")

The MX format includes multiple data types, such as MXFP8, MXFP4, MXFP16, and MXINT4. The following table lists the data types supported by the [MxMatmul scenario](#zh-cn_topic_0000002270097206_section310824820358) (Microscaling Matmul in full).

**Table 1** Data types supported by MxMatmul in MX format

<a name="zh-cn_topic_0000002270097206_table5383144710452"></a>

| Data Type | Private Element Data Type | Private Element Bit Width (d) | Block Size (k) | Shared Scaling Factor Data Type | Shared Scaling Factor Bit Width (w) |
| --- | --- | --- | --- | --- | --- |
| MXFP8 | fp8_e5m2_t | 8 | 32 | fp8_e8m0_t | 8 |
| MXFP8 | fp8_e4m3fn_t | 8 | 32 | fp8_e8m0_t | 8 |
| MXFP4 | fp4x2_e1m2_t | 4 | 32 | fp8_e8m0_t | 8 |
| MXFP4 | fp4x2_e2m1_t | 4 | 32 | fp8_e8m0_t | 8 |

## Feature Overview<a name="zh-cn_topic_0000002270097206_section310824820358"></a>

MxMatmul (Microscaling Matmul) is cube multiplication with quantization coefficients. That is, both the left cube and the right cube have corresponding quantization coefficient cubes: the left quantization coefficient cube scaleA and the right quantization coefficient cube scaleB. In the MxMatmul scenario, the left quantization coefficient cube is multiplied by the left cube, the right quantization coefficient cube is multiplied by the right cube, and cube multiplication is performed on the two products.

The formula of MxMatmul is: C = \(scaleA ⊗ A\) \* \(scaleB ⊗ B\) + Bias, where "⊗" denotes broadcast multiplication. When the left/right cube is multiplied by the left/right quantization coefficient cube, every 32 elements in the K direction share one quantization factor, as shown in [Figure 2](#zh-cn_topic_0000002270097206_fig1942919398330).

-   A, scaleA, B, and scaleB are source operands. A is the left cube with the shape \[M, K\]; scaleA is the left quantization coefficient cube with the shape \[M, K/32\]; B is the right cube with the shape \[K, N\]; scaleB is the right quantization coefficient cube with the shape \[K/32, N\].
-   C is the destination operand, which is the cube that stores the cube multiplication result, with the shape \[M, N\].
-   Bias is the cube multiplication bias, with the shape \[1, N\]. The Bias is applied to each row of the result cube of \(scaleA ⊗ A\) \* \(scaleB ⊗ B\).

**Figure 2**  MxMatmul cube multiplication diagram<a name="zh-cn_topic_0000002270097206_fig1942919398330"></a>  
![](../../../../figures/mx_matmul.png "MxMatmul cube multiplication diagram")

The layout formats of cubes A, scaleA, B, and scaleB at different positions are shown in the following figures.

**Figure 3**  Layout format of cube A at different positions  
![](../../../../figures/a_matrix_pos.png "Layout format of cube A at different positions")

**Figure 4**  Layout format of cube B at different positions  
![](../../../../figures/b_matrix_pos.png "Layout format of cube B at different positions")

**Figure 5**  scaleA cube layout format at different positions<a name="zh-cn_topic_0000002270097206_fig107863142019"></a>  
![](../../../../figures/scale_a_pos.png "scaleA cube layout format at different positions")

**Figure 6**  scaleB cube layout format at different positions<a name="zh-cn_topic_0000002270097206_fig76682054103416"></a>  
![](../../../../figures/scale_b_pos.png "scaleB cube layout format at different positions")

## Scenarios

Scenarios where cubes A and B need to be quantized before cube computation. In this scenario, the data types supported by the Matmul input and output cubes are listed in the following table.<!-- npu="950" id1 -->The quantization scenarios listed in the table are supported only on Ascend 950PR&950DT products.<!-- end id1 -->

**Table 2** Quantization scenarios supported by MxMatmul

| Cube A | Cube B | ScaleCube A/Scalecube B | Bias cube | Cube C |
| --- | --- | --- | --- | --- |
| fp4x2_e1m2_t | fp4x2_e1m2_t/fp4x2_e2m1_t | fp8_e8m0_t | float/half/bfloat16_t | float/half/bfloat16_t |
| fp4x2_e2m1_t | fp4x2_e2m1_t/fp4x2_e1m2_t | fp8_e8m0_t | float/half/bfloat16_t | float/half/bfloat16_t |
| fp8_e4m3fn_t | fp8_e4m3fn_t/fp8_e5m2_t | fp8_e8m0_t | float/half/bfloat16_t | float/half/bfloat16_t |
| fp8_e5m2_t | fp8_e4m3fn_t/fp8_e5m2_t | fp8_e8m0_t | float/half/bfloat16_t | float/half/bfloat16_t |

## Implementation Process

The key steps for automatically obtaining Tiling parameters on the host side are described as follows:

1.  **Create a Tiling object.**

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    matmul_tiling::MatmulApiTiling cubeTiling(ascendcPlatform);
    ```

    Pass in the hardware platform information to create a PlatformAscendC object, and then create a Tiling object. The hardware platform information can be obtained through GetPlatformInfo.

2.  **Set the memory logical position, format, data type, and transpose information of A, B, C, and Bias, and set the memory logical position, format, and transpose information of scaleA and scaleB.**

    Call the [SetScaleAType](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetScaleAType.md) and [SetScaleBType](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetScaleBType.md) APIs to set the memory logical position, format, and transpose information of scaleA and scaleB.

    ```
    cubeTiling.SetAType(AscendC::TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT8_E5M2, false);
    cubeTiling.SetBType(AscendC::TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT8_E5M2, true);
    cubeTiling.SetScaleAType(AscendC::TPosition::GM, CubeFormat::ND, false);
    cubeTiling.SetScaleBType(AscendC::TPosition::GM, CubeFormat::ND, true);
    cubeTiling.SetCType(AscendC::TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    cubeTiling.SetBiasType(AscendC::TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    ```

3.  **Set the MxMatmul scenario.**

    Call the [SetMadType](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetMadType.md) API to set the Tiling computation logic to the MxMatmul scenario.

    ```
    cubetiling.SetMadType(MatrixMadType::MXMODE);
    ```

4.  **Set the cube shape information.**

    ```
    cubeTiling.SetShape(M, N, K);
    cubeTiling.SetOrgShape(M, N, K); // Set the original complete shape M, N, K
    ```

5.  **Set the available space size information.**

    Set the available L1 Buffer/L0C Buffer/Unified Buffer (UB) space size for Matmul computation. -1 indicates the size of the corresponding buffer on the AI processor.

    ```
    cubeTiling.SetBufferSpace(-1, -1, -1);
    ```

6.  **Set other parameters as required, for example, enable bias to participate in the computation.**

    ```
    cubeTiling.EnableBias(true);
    ```

7.  **Obtain the Tiling parameters.**

    ```
    MatmulCustomTilingData tiling;
    if (cubeTiling.GetTiling(tiling.cubeTilingData) == -1){
        return ge::GRAPH_FAILED;
    }
    ```

8.  Perform other operations such as serializing and saving the Tiling parameters.

The key steps on the kernel side are described as follows:

1.  **Create a Matmul object.**

    ```
    // In the MxMatmul scenario, define the parameter type information of A, scaleA, B, and scaleB through MatmulTypeWithScale
    typedef AscendC::MatmulTypeWithScale<AscendC::TPosition::GM, AscendC::TPosition::GM, CubeFormat::ND, fp8_e5m2_t, isTransposeA> aType;
    typedef AscendC::MatmulTypeWithScale<AscendC::TPosition::GM, AscendC::TPosition::GM, CubeFormat::ND, fp8_e5m2_t, isTransposeB> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    // When defining the matmul object, pass MatmulWithScalePolicy to enable the MxMatmul template policy
    AscendC::Matmul<aType, bType, cType, biasType, CFG_MDL, MatmulCallBackFunc<nullptr, nullptr, nullptr>, AscendC::Impl::Detail::MatmulWithScalePolicy> mm;
    ```

    When creating the object, you need to pass the parameter type information of A, scaleA, B, scaleB, C, and Bias. The type information of A, scaleA, B, and scaleB is defined through [MatmulTypeWithScale](#zh-cn_topic_0000002270097206_table14759942142014), and the type information of C and Bias is defined through [MatmulType](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Matmul_usage.md), including the memory logical position, data format, data type, and transpose information. In addition, pass [MatmulWithScalePolicy](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulPolicy.md) through the template parameter [MatmulPolicy](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulPolicy.md) to enable the MxMatmul scenario.

    ```
    template <TPosition POSITION, TPosition SCALE_POSITION, CubeFormat FORMAT, typename TYPE, bool ISTRANS = false, TPosition SRCPOS = TPosition::GM, CubeFormat SCALE_FORMAT = FORMAT, bool SCALE_ISTRANS = ISTRANS, TPosition SCALE_SRCPOS = SRCPOS>
    struct MatmulTypeWithScale: public MatmulType<POSITION, FORMAT, TYPE, ISTRANS> {
        constexpr static TPosition scalePosition = SCALE_POSITION;
        constexpr static CubeFormat scaleFormat = SCALE_FORMAT;
        constexpr static bool isScaleTrans = SCALE_ISTRANS;
        constexpr static TPosition srcScalePos = SCALE_SRCPOS;
    };
    ```

2.  **Perform initialization.**

    ```
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling); // Initialize
    ```

3.  **Set left cube A, right cube B, left quantization coefficient cube scaleA, right quantization coefficient cube scaleB, and Bias.**

    Set the left quantization coefficient cube scaleA and right quantization coefficient cube scaleB through [SetTensorScaleA](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTensorScaleA.md) and [SetTensorScaleB](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTensorScaleB.md).

    ```
    mm.SetTensorA(gm_a, isTransposeA);    // Set left cube A
    mm.SetTensorB(gm_b, isTransposeB);    // Set right cube B
    mm.SetTensorScaleA(gm_scaleA, isTransposeScaleA);    // Set left quantization coefficient cube scaleA
    mm.SetTensorScaleB(gm_scaleB, isTransposeScaleB);    // Set right quantization coefficient cube scaleB
    mm.SetBias(gm_bias);    // Set Bias
    ```

4.  **Complete the cube multiplication operation.**
    -   Call [Iterate](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Iterate.md) to complete a single iteration computation, and use a while loop to complete the computation of all data on a single core. With the Iterate method, you can control the number of iterations to compute the required amount of data, which is more flexible.

        ```
        while (mm.Iterate()) {
            mm.GetTensorC(gm_c);
        }
        ```

    -   Call [IterateAll](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/IterateAll.md) to complete the computation of all data on a single core. The [IterateAll](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/IterateAll.md) method does not require loop iteration and is simpler to use.

        ```
        mm.IterateAll(gm_c);
        ```

5.  **End the cube multiplication operation.**

    ```
    mm.End();
    ```

For more complete operator examples, see [MxMatmul example](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_mx), [MxMatmul example with custom input source](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_mx_ub_tscm_nz), and [MxMatmul example with multiple scale caches](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_mx_scale_cache).

## Parameters

**Table 3**  MatmulTypeWithScale parameters

<a name="zh-cn_topic_0000002270097206_table14759942142014"></a>

| Parameter | Description |
| --- | --- |
| POSITION | Memory logical position of the left and right cubes. <!-- npu="950" id2 --><br><br>For Ascend 950PR&950DT products:<br><br>The A cube can be set to TPosition::GM, TPosition::VECOUT, or TPosition::TSCM.<br><br>The B cube can be set to TPosition::GM, TPosition::VECOUT, or TPosition::TSCM.<!-- end id2 --><br><br>Note: When the A and B cubes are set to TPosition::TSCM, the corresponding Format supports only CubeFormat::NZ. |
| SCALE_POSITION | Memory logical position of the quantization coefficient cube. <!-- npu="950" id3 --><br><br>For Ascend 950PR&950DT products:<br><br>The scaleA cube can be set to TPosition::GM, TPosition::VECOUT, or TPosition::TSCM.<br><br>The scaleB cube can be set to TPosition::GM, TPosition::VECOUT, or TPosition::TSCM.<!-- end id3 --><br><br>Note: When the scaleA and scaleB cubes are set to TPosition::TSCM, the corresponding SCALE_FORMAT parameter supports only CubeFormat::NZ. |
| FORMAT | Physical layout format of the data. <!-- npu="950" id4 --><br><br>For Ascend 950PR&950DT products:<br><br>The A cube can be set to CubeFormat::ND, CubeFormat::NZ, or CubeFormat::VECTOR.<br><br>The B cube can be set to CubeFormat::ND or CubeFormat::NZ.<!-- end id4 --><br><br>Note: For the NZ layout format, see [Data Format](../basics.md#zh-cn_topic_0000001622194138_section1453415011) for the layout formats of A/B. |
| TYPE | Data type. <!-- npu="950" id5 --><br><br>For Ascend 950PR&950DT products:<br><br>The A cube can be set to fp4x2_e1m2_t, fp4x2_e2m1_t, fp8_e4m3fn_t, or fp8_e5m2_t.<br><br>The B cube can be set to fp4x2_e1m2_t, fp4x2_e2m1_t, fp8_e4m3fn_t, or fp8_e5m2_t.<!-- end id5 --><br><br>Note: For the specific data type combinations, see [MxMatmul Supported Data Types](#zh-cn_topic_0000002270097206_table5383144710452). |
| ISTRANS | Whether to enable the transpose function of the A and B cubes. The default value is false. The supported values are as follows:<br><br>true: Enables the cube transpose function. After it is enabled, use the isTransposeA and isTransposeB parameters in SetTensorA and SetTensorB to set whether the A and B cubes are transposed. If the A and B cubes are set to be transposed, Matmul considers the A cube shape to be [K, M] and the B cube shape to be [N, K].<br><br>false: Disables the cube transpose function. The transpose status of the A and B cubes cannot be set through SetTensorA and SetTensorB. Matmul considers the A cube shape to be [M, K] and the B cube shape to be [K, N]. |
| SRCPOS | When the POSITION parameter of the A/B cube is set to TPosition::TSCM, set the memory logical position of the source of the cube data in TSCM. The default value is TPosition::GM. <!-- npu="950" id6 --><br><br>For Ascend 950PR&950DT products:<br><br>The A cube can be set to TPosition::GM or TPosition::VECOUT.<br><br>The B cube can be set to TPosition::GM or TPosition::VECOUT.<!-- end id6 --> |
| SCALE_FORMAT | Physical layout format of the quantization coefficient cube. For details, see [Data Format](#zh-cn_topic_0000002270097206_fig107863142019). The default value is the value of the FORMAT parameter. <!-- npu="950" id7 --><br><br>For Ascend 950PR&950DT products:<br><br>The scaleA cube can be set to CubeFormat::ND, CubeFormat::NZ, or CubeFormat::VECTOR.<br><br>The scaleB cube can be set to CubeFormat::ND or CubeFormat::NZ.<!-- end id7 --><br><br>Note:<br><br>For the NZ layout format, see [NZ](../../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md#li19960204116136). In the MxMatmul scenario, the data type of scaleA and scaleB is fp8_e8m0_t, and the fractal sizes are H0=16 and W0=2.<br><br>In the scenario where the Scale cube is in ND format, when the scaleA cube transpose is set through the SetTensorScaleA API, the scaleA memory layout format must follow (K/64, M, 2). When the scaleB cube is set to non-transposed through the SetTensorScaleB API, the scaleB memory layout format must follow (K/64, N, 2). For details, see [Data Format](#zh-cn_topic_0000002270097206_fig76682054103416). |
| SCALE_ISTRANS | Whether to enable the transpose function of the scaleA and scaleB cubes. The default value is the value of the ISTRANS parameter. The supported values are as follows:<br><br>true: Enables the cube transpose function. After it is enabled, use the isTransposeScaleA and isTransposeScaleB parameters in SetTensorScaleA and SetTensorScaleB to set whether the scaleA and scaleB cubes are transposed. In the scenario where the Scale cube is in ND format, if the scaleA and scaleB cubes are set to be transposed, Matmul considers the scaleA cube shape to be [Ceil(K/64), M, 2] and the scaleB cube shape to be [N, Ceil(K/64), 2].<br><br>false: Disables the cube transpose function. The transpose status of the scaleA and scaleB cubes cannot be set through SetTensorScaleA and SetTensorScaleB. Matmul considers the scaleA cube shape to be [M, Ceil(K/64), 2] and the scaleB cube shape to be [Ceil(K/64), N, 2]. |
| SCALE_SRCPOS | When the SCALE_POSITION parameter of the scaleA and scaleB cubes is set to TPosition::TSCM, use this parameter to set the memory logical position of the source of the cube data in TSCM. The default value is the value of the SRCPOS parameter. <!-- npu="950" id8 --><br><br>For Ascend 950PR&950DT products:<br><br>The scaleA cube can be set to TPosition::GM or TPosition::VECOUT.<br><br>The scaleB cube can be set to TPosition::GM or TPosition::VECOUT.<!-- end id8 --> |

## Constraints

-   The MxMatmul scenario supports only the [Norm template](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulConfig.md) and the [MDL template](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulConfig.md).

-   In the MxMatmul scenario, if the positions of the A and B cubes are both GM, there is no special restriction on singleKIn. In this case, if the K-direction size of scaleA and scaleB (that is, Ceil\(singleKIn, 32\)) is an odd number, you need to pad zeros in the K direction of scaleA and scaleB to make it an even number. For example, when singleKIn is 30, Ceil\(singleKIn, 32\) is 1, and you need to pad zeros in the K direction of scaleA and scaleB to make the K direction value an even number. For other combinations of the logical positions of the A and B cubes, that is, when the positions of the A and B cubes are not both GM, the value obtained by aligning singleKIn upward to 32 elements must be an even multiple of 32.
-   In the MxMatmul scenario, when the input data type is fp4x2\_e2m1\_t/fp4x2\_e1m2\_t, the inner axis must be an even number.
-   In the MxMatmul scenario, set the data format of cube A and cube scaleA to VECTOR to enable [GEMV mode](matrix_vector_multiply.md). In this mode, cube A and cube scaleA support only the GM memory logical position, and neither supports transpose.
-   When cube A and cube B are UB inputs, the inner axis of the cube must be aligned upward to 32 bytes. For example, when the shape of cube A is \(M, K\), align K to 32 bytes; when the shape of cube A is \(K, M\), align M to 32 bytes.
-   When cube scaleA and cube scaleB are UB inputs, the inner axis of the cube must be aligned upward to 32 bytes. For example, when the shape of cube scaleA is \(M, K/32\), align K/32 to 32 bytes; when the shape of cube scaleA is \(K/32, M\), align M to 32 bytes.
-   When cube scaleA and cube scaleB are input in ND format, the advanced API occupies UB temporary space when performing format conversion internally. Developers need to use the [SetLocalWorkspace](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetLocalWorkspace.md) API to configure the temporary space. The formula for calculating the temporary space size (in bytes) is as follows.

    ```
    int32_t scaleATmpBuf = 0;
    int32_t scaleBTmpBuf = 0;
    if constexpr (A_TYPE::scalePosition == TPosition::VECOUT) {
        if (A_TYPE::isScaleTrans) {
            scaleATmpBuf = CeilAlign(SingleCoreM, 32) * scaleK;
        } else {
            scaleATmpBuf = CeilAlign(scaleK, 32) * SingleCoreM;
        }
    }
    if constexpr (B_TYPE::scalePosition == TPosition::VECOUT) {
        if (B_TYPE::isScaleTrans) {
            scaleBTmpBuf = SingleCoreN * CeilAlign(scaleK, 32);
        } else {
            scaleBTmpBuf = scaleK * CeilAlign(SingleCoreN, 32);
        }
    }
    int32_t totalTmpBuf = scaleATmpBuf + scaleBTmpBuf;
    ```
