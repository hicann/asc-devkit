# Basic API Migration Guide<a name="ZH-CN_TOPIC_0000002470508258"></a>

<!-- md-trans-meta sourceCommit=29c4740ae8f0275cefcb220d1dc0bf18d9f0bd15 translatedAt=2026-08-26T12:47:29.378Z -->

This section describes the impact of chip changes on basic API compatibility for [NPU architecture version 3510](../../../programming_guide/language_extension/SIMD-BuiltIn_keyword.md#table65291052154114), and provides compatibility adaptation solutions for the basic APIs.

## Vector Computation<a name="section7364115741514"></a>

-   **The 3510 architecture does not support the Subnormal feature by default.**

    **Note**: A SubNormal floating-point number is a floating-point number whose exponent bits are all 0 and whose mantissa is not 0. It is used to represent a value smaller than the minimum normal number, avoiding "underflow to 0". The 3510 version does not support Subnormal by default, and Subnormal floating-point numbers are treated as 0 in computation.

    **Compatibility solution**: Configure the Subnormal computation mode by setting the config template parameter. Software simulates the processing of Subnormal data, and avoids underflow of Subnormal floating-point numbers to 0 through processing methods such as precision extension.

    **Table 1**  APIs and config parameters related to Subnormal

    <a name="table288510533427"></a>
    <table><thead align="left"><tr id="row388575318425"><th class="cellrowborder" align="center" valign="top" width="27%" id="mcps1.2.3.1.1"><p id="p12885175314420"><a name="p12885175314420"></a><a name="p12885175314420"></a>Ascend C basic API</p>
    </th>
    <th class="cellrowborder" align="center" valign="top" width="73%" id="mcps1.2.3.1.2"><p id="p1288585311428"><a name="p1288585311428"></a><a name="p1288585311428"></a>Compatibility description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row188851453184214"><td class="cellrowborder" align="center" valign="top" width="27%" headers="mcps1.2.3.1.1 "><p id="p1188545310427"><a name="p1188545310427"></a><a name="p1188545310427"></a>Exp, Ln, Reciprocal, Sqrt, Rsqrt, Div</p>
    </td>
    <td class="cellrowborder" align="left" valign="top" width="73%" headers="mcps1.2.3.1.2 "><p id="p1055013361204"><a name="p1055013361204"></a><a name="p1055013361204"></a>The Ln API is used as an example.</p>
    <p id="p151399151208"><a name="p151399151208"></a><a name="p151399151208"></a>Configure the Subnormal computation mode through the algo parameter of the LnConfig structure. The values of algo are as follows:</p>
    <a name="ul118851253124216"></a><a name="ul118851253124216"></a><ul id="ul118851253124216"><li>LnAlgo::INTRINSIC and LnAlgo::PRECISION_1ULP_FTZ_TRUE use a single instruction to compute the result, and all Subnormal values are approximated to 0.</li><li>LnAlgo::PRECISION_1ULP_FTZ_FALSE supports Subnormal data computation.</li></ul>
    <p id="p9885135319427"><a name="p9885135319427"></a><a name="p9885135319427"></a>The default value DEFAULT_LN_CONFIG of this parameter is as follows:</p>
    <a name="screen18681133513203"></a><a name="screen18681133513203"></a><pre class="screen" codetype="Cpp" id="screen18681133513203">constexpr LnConfig DEFAULT_LN_CONFIG = { LnAlgo::INTRINSIC };</pre>
    </td>
    </tr>
    </tbody>
    </table>

    Refer to the following code snippet:

    ```
    // Define the template parameters.
    constexpr AscendC::LnConfig CONFIG = {
        AscendC::LnAlgo::PRECISION_1ULP_FTZ_FALSE
    };
    constexpr uint32_t DATA_COUNT = 1024;
    constexpr uint32_t xAddr = 0;
    constexpr uint32_t yAddr = xAddr + DATA_COUNT * sizeof(half);

    // Kernel function entry: explicitly declare a Vector kernel function using __vector__, and program with the static Tensor method.
    __vector__ __global__ void ln_custom(__gm__ uint8_t* src, __gm__ uint8_t* dst)
    {
        // For a static Tensor, manually call InitSocState to initialize the global status registers.
        AscendC::InitSocState();

        AscendC::GlobalTensor<half> srcGlobal;
        AscendC::GlobalTensor<half> dstGlobal;
        srcGlobal.SetGlobalBuffer((__gm__ half*)src, DATA_COUNT);
        dstGlobal.SetGlobalBuffer((__gm__ half*)dst, DATA_COUNT);

        // Directly construct a LocalTensor at the specified address and storage location (static Tensor method).
        AscendC::LocalTensor<half> srcLocal(AscendC::TPosition::VECCALC, xAddr, DATA_COUNT);
        AscendC::LocalTensor<half> dstLocal(AscendC::TPosition::VECCALC, yAddr, DATA_COUNT);

        AscendC::DataCopy(srcLocal, srcGlobal, DATA_COUNT);
        AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

        // Call the Ln basic API and pass in the template parameters.
        AscendC::Ln<half, CONFIG>(dstLocal, srcLocal, DATA_COUNT);

        AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
        AscendC::DataCopy(dstGlobal, dstLocal, DATA_COUNT);

        AscendC::PipeBarrier<PIPE_ALL>();
    }
    ```

## Data Movement<a name="section7530159122210"></a>

-   **The DataCopy API does not support the L1 Buffer->GM path.**

    **Note:** The hardware removes the path from L1 Buffer to GM, so data cannot be directly moved from L1 Buffer to GM. The existing API does not support direct movement from L1 Buffer to GM.

    **Compatibility solution:** For pure Cube computation scenarios: allocate an additional identity matrix in GM, output the result to the L0C Buffer through Mmad matrix multiplication, and then move it from the L0C Buffer to GM through Fixpipe. For scenarios where Vector and Cube computation are fused, data can be moved from L1 Buffer to UB and then to GM. The following uses a pure Cube computation scenario as an example to describe the core operator process. For details, see [L1 to GM Movement Compatibility Sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/06_compatibility_guide/data_copy_l1togm).

    1.  Move matrix A from GM to L1 Buffer.

        ```
        __aicore__ inline void CopyGmToL1A(AscendC::LocalTensor<T> a1Local)
        {
            AscendC::Nd2NzParams intriParams1{1, M, K, 0, K, M, 1, 0};
            AscendC::DataCopy(a1Local, aGlobal, intriParams1);
        }
        ```

    2.  Move matrix B (an identity matrix) from GM to L1 Buffer.

        ```
        __aicore__ inline void CopyGmToL1B(AscendC::LocalTensor<U> b1Local)
        {
            AscendC::Nd2NzParams intriParams2{1, K, N, 0, N, K, 1, 0};
            AscendC::DataCopy(b1Local, bGlobal, intriParams2);
        }
        ```

    3.  Move matrix A from L1 Buffer to L0A Buffer.

        ```
        __aicore__ inline void Load2DL1AToL0A(AscendC::LocalTensor<T> a1Local, AscendC::LocalTensor<T> a2Local)
        {
            AscendC::LoadData2DParamsV2 loadDataParams;
            ...
            AscendC::LoadData(a2Local, a1Local, loadDataParams);
        }
        ```

    4.  Move matrix B from L1 Buffer to L0B Buffer.

        ```
        __aicore__ inline void Load2DL1BToL0B(AscendC::LocalTensor<U> b1Local, AscendC::LocalTensor<U> b2Local)
        {
            AscendC::LoadData2DParamsV2 loadDataParams;
            ...
            AscendC::LoadData(b2Local, b1Local, loadDataParams);
        }
        ```

    5.  Perform Mmad matrix computation, and output the result to L0C Buffer.

        ```
        __aicore__ inline void Compute(
            AscendC::LocalTensor<S> co1Local, AscendC::LocalTensor<T> a2Local, AscendC::LocalTensor<U> b2Local)
        {
            AscendC::MmadParams mmadParams;
            ...
            AscendC::Mmad(co1Local, a2Local, b2Local, mmadParams);
        }
        ```

    6.  Copy matrix C from L0C Buffer to GM through FixPipe.

        ```
        __aicore__ inline void CopyL0CToGm(AscendC::LocalTensor<S> co1Local)
        {
            AscendC::FixpipeParamsV220 fixpipeParams;
            ...
            AscendC::Fixpipe<S, S, AscendC::CFG_ROW_MAJOR>(cGlobal, co1Local, fixpipeParams);
        }
        ```

-   **The SetLoadDataBoundary API is not supported.**

    **Note:** The 3510 architecture hardware removes the registers related to boundary value setting of the L1 Buffer, and the SetLoadDataBoundary API is no longer supported. This API is used to set the boundary value of the L1 Buffer during Load3D. If the address of the source operand in the L1 Buffer exceeds the set boundary when an instruction processes the source operand, data is read from the start address of the L1 Buffer. Setting the value to 0 indicates no boundary, and the entire L1 Buffer can be used.

    **Compatibility solution:**

    -   When the boundaryValue parameter of the API in [NPU architecture version 2201](../../../programming_guide/language_extension/SIMD-BuiltIn_keyword.md#table65291052154114) is set to 0, it is equivalent to the 3510 architecture version.
    -   If operands need to be read cyclically from the L1 Buffer, manually split the corresponding Load3D API into multiple instructions and wrap around manually. For details, see the [SetLoadDataBoundary compatibility sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/06_compatibility_guide/set_loaddata_boundary).

    ![](../../../figures/1_zn.png)

    As shown in the preceding figure, take the data movement from the L1 Buffer to the L0A Buffer as an example. Matrix A is of the half data type and has a size of 32 \* 32. Assume that the boundary is 512B. Data can be repeatedly moved to the L0A Buffer, and the address offset of the destination operand is set for each movement.

    1.  Move matrix A from the GM to the L1 Buffer.

        ```
        __aicore__ inline void CopyGmToA1Nd2Nz(AscendC::LocalTensor<T>& leftMatrix)
        {
            AscendC::Nd2NzParams nd2nzParams;
            ...
            AscendC::DataCopy(leftMatrix, aGlobal, nd2nzParams);
        }
        ```

    2.  Move matrix B from the GM to the L1 Buffer.

        ```
        __aicore__ inline void CopyGmToB1Nd2Nz(AscendC::LocalTensor<U>& rightMatrix)
        {
            AscendC::Nd2NzParams nd2nzParams;
            ...
            AscendC::DataCopy(rightMatrix, bGlobal, nd2nzParams);
        }
        ```

    3.  Move matrix A from the L1 Buffer to the L0A Buffer.

        ```
            __aicore__ inline void Load3DA1ToL0A(AscendC::LocalTensor<T>& leftMatrix, AscendC::LocalTensor<T>& a2)
            {
                AscendC::LoadData3DParamsV2Pro loadData3dParamsPro;
                ...
        #if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
                ...
                AscendC::SetLoadDataRepeat({0, 1, 0});
                AscendC::SetLoadDataBoundary(1024);
                AscendC::LoadData(a2, leftMatrix, loadData3dParamsPro);
        #elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
                uint16_t dstStride = AscendC::DivCeil(M / 2, 16);
                ...
                AscendC::SetLoadDataRepeatWithStride({0, 1, 0, dstStride});
                // Call LoadData multiple times to manually wrap around.
                AscendC::LoadData(a2, leftMatrix, loadData3dParamsPro);
                AscendC::LocalTensor<T> a3 = a2[256];
                AscendC::LoadData(a3, leftMatrix, loadData3dParamsPro);
                AscendC::LocalTensor<T> a4 = a2[512];
                AscendC::LoadData(a4, leftMatrix, loadData3dParamsPro);
                AscendC::LocalTensor<T> a5 = a2[768];
                AscendC::LoadData(a5, leftMatrix, loadData3dParamsPro);
        #endif
            }
        ```

    4.  Copy matrix B from L1 Buffer to L0B Buffer.

        ```
            __aicore__ inline void Load3DB1ToL0B(AscendC::LocalTensor<U>& rightMatrix, AscendC::LocalTensor<U>& b2)
            {
                AscendC::LoadData3DParamsV2<U> loadData3dParams;
                ...
                uint8_t padList[AscendC::PAD_SIZE] = {0, 0, 0, 0};
                static constexpr AscendC::IsResetLoad3dConfig LOAD3D_CONFIG = {false, false};
                AscendC::SetFmatrix(N, 1, padList, AscendC::FmatrixMode::FMATRIX_LEFT);
        #if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
                AscendC::SetLoadDataRepeat({0, 1, 0});
                AscendC::SetLoadDataBoundary(0);
                AscendC::SetLoadDataPaddingValue(0);
                AscendC::LoadData<U, LOAD3D_CONFIG>(b2, rightMatrix, loadData3dParams);
        #elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
                uint16_t dstStride = AscendC::DivCeil(N, 16);
                AscendC::SetLoadDataRepeatWithStride({0, 1, 0, dstStride});
                AscendC::SetLoadDataPaddingValue(0);
                AscendC::LoadDataWithStride<U, LOAD3D_CONFIG>(b2, rightMatrix, loadData3dParams);
        #endif
            }
        ```

    5.  Perform cube computation.

        ```
        __aicore__ inline void Compute(AscendC::LocalTensor<T>& a2, AscendC::LocalTensor<U>& b2,
                                    AscendC::LocalTensor<V>& co1Local)
        {
            AscendC::MmadParams mmadParams;
            ...
            AscendC::Mmad(co1Local, a2, b2, mmadParams);
        }
        ```

    6.  Copy matrix C from L0C Buffer to GM.

        ```
        __aicore__ inline void CopyL0CToGm(AscendC::LocalTensor<V>& co1Local, const AscendC::GlobalTensor<S>& gm)
        {
            AscendC::FixpipeParamsV220 fixpipeParams(N, static_cast<uint16_t>(M),
                                            AscendC::DivCeil(M, AscendC::BLOCK_CUBE) * AscendC::BLOCK_CUBE, static_cast<uint16_t>(N), 0);
            ...
            AscendC::Fixpipe<S, V, AscendC::CFG_ROW_MAJOR>(gm, co1Local, fixpipeParams);
        }
        ```

## Cube Computation<a name="section17560113713122"></a>

-   **The cube compute unit removes the int4b\_t data type.**

    **Note:** Compared with the 2201 architecture version, the Cube compute unit in the 3510 architecture version does not support int4b\_t. The related basic APIs include LoadData, Mmad, and LoadDataWithTranspose, which no longer support int4b\_t.

    **Compatibility solution:** On the operator side, write a CV fusion operator to perform the Cast conversion from int4b\_t to int8\_t on the Vector Core, and then move the data to L1 through UB before performing the Mmad computation. For the specific code, see [Matmul compatibility sample for the int4 data type](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/06_compatibility_guide/matmul_s4). At the graph layer, a Cast node can be added before this operator to convert int4b\_t to int8\_t.

    1.  Perform the Cast conversion from int4b\_t to int8\_t on the Vector Core, and save the converted data to a new GM space.

        ```
        __aicore__ inline void Unzip(AscendC::GlobalTensor<int8_t>& dstGlobalTensor,
            AscendC::GlobalTensor<int8_t>& srcGlobalTensor, uint32_t count)
        {
            constexpr uint32_t oneBlockBytes = 32;
            uint32_t srcAddr = 0;
            uint32_t tmpAddr = srcAddr + AscendC::AlignUp(count * sizeof(int8_t), oneBlockBytes);
            uint32_t dstAddr = tmpAddr + AscendC::AlignUp(count * 2 * sizeof(half), oneBlockBytes);

            AscendC::LocalTensor<int8_t> srcLocalTensor(AscendC::TPosition::VECCALC, srcAddr, count);
            AscendC::LocalTensor<half> tmpTensor(AscendC::TPosition::VECCALC, tmpAddr, count * 2);
            AscendC::LocalTensor<int8_t> dstLocalTensor(AscendC::TPosition::VECCALC, dstAddr, count * 2);

            AscendC::DataCopy(srcLocalTensor, srcGlobalTensor, count);
            AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
            AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

            AscendC::LocalTensor<AscendC::int4b_t> int4SrcLocalTensor =
                srcLocalTensor.ReinterpretCast<AscendC::int4b_t>();
            AscendC::Cast<half, AscendC::int4b_t>(tmpTensor, int4SrcLocalTensor, AscendC::RoundMode::CAST_NONE, count * 2);
            AscendC::PipeBarrier<PIPE_V>();
            AscendC::Cast<int8_t, half>(dstLocalTensor, tmpTensor, AscendC::RoundMode::CAST_CEIL, count * 2);

            AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
            AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
            AscendC::DataCopy(dstGlobalTensor, dstLocalTensor, count * 2);
        }
        ```

    2.  Perform the matrix computation of the int8\_t data type.

        ```
        __aicore__ inline void RunMatmul()
        {
            ...
            int offsetA = 0;
            int offsetB = 0;
            int offsetC = 0;
            ...
            auto gmA = aGlobal[offsetA];
            auto gmB = bGlobal[offsetB];
            auto gmC = cGlobal[offsetC];

            AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_MDL> mm;
            mm.SetSubBlockIdx(0);
            mm.Init(&tiling, pipe);
            mm.SetTensorA(gmA, false);
            mm.SetTensorB(gmB, false);
            mm.IterateAll(gmC);
        }
        ```

-   **The L0A Buffer fractal changes from the ZZ format to the ZN format.**

    **Note:** The involved APIs are LoadData, Mmad, and LoadDataWithTranspose.

    -   In the 2201 architecture version, when participating in matrix multiplication (A \* B = C), the data layout formats of the A, B, and C matrices are Zz, Zn, and Nz, respectively. The A, B, and C matrices are located in the L0A Buffer, L0B Buffer, and L0C Buffer, respectively.

        Matrix A: Each fractal matrix interior is in row-major order, and the fractal matrices are in row-major order. The fractal Shape is 16 x \(32B/sizeof\(AType\)\), with a size of 512Byte.

        Matrix B: Each fractal matrix is in column-major order inside the matrix, and the fractal matrices are in row-major order. The fractal shape is \(32B/sizeof\(BType\)\) x 16, with a size of 512 bytes.

        Matrix C: Each fractal matrix is in row-major order inside the matrix, and the fractal matrices are in column-major order. The fractal shape is 16 x 16, with a size of 256 elements.

        ![](../../../figures/2_zn.png)

    -   In architecture version 3510, when participating in matrix multiplication (A \* B = C), the data layout formats of matrices A, B, and C are Nz, Zn, and Nz, respectively.

        Matrix A: Each fractal matrix is in row-major order inside the matrix, and the fractal matrices are in column-major order. Its shape is 16 x \(32B/sizeof\(AType\)\), with a size of 512 bytes.

        Matrix B: Each fractal matrix is in column-major order inside the matrix, and the fractal matrices are in row-major order. Its shape is \(32B/sizeof\(BType\)\) x 16, with a size of 512 bytes.

        Matrix C: Each fractal matrix is in row-major order inside the matrix, and the fractal matrices are in column-major order. Its shape is 16 x 16, with a size of 256 elements.

        ![](../../../figures/3_zn.png)

    **Compatibility solution**: Scenarios without L0A Buffer splitting are compatible with version 2201, while scenarios with L0A Buffer splitting need to be re-adapted based on the new fractal. For specific code, see the [pattern_transformation compatibility sample](https://gitcode.com/cann/asc-devkit/tree/c6dc78cfbccfb1a8f910ed4b6845bfacde5d2d9f/examples/01_simd_cpp_api/06_compatibility_guide/pattern_transformation).

    In architecture 2201, Cube computation requires the left matrix to use the ZZ fractal (NZ in 3510) and the right matrix to use the ZN fractal. Because the data fractal of the L1 Buffer is NZ, in architecture 2201, moving the left matrix from the L1 Buffer to the L0A Buffer additionally requires converting the NZ fractal to the ZZ fractal. In architecture 3510, no fractal conversion is required.

    ![](../../../figures/4_zn.png)

    The changes brought by the fractal change are mainly reflected in the transfer process from L1 Buffer to L0A Buffer, as shown in the following code snippet:

    ```
        __aicore__ inline void DataLoadA(AscendC::LocalTensor<T> a1, AscendC::LocalTensor<T> a2)
        {
    #if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
            constexpr uint32_t mBlocks = M / CUBE_BLOCK;
            constexpr uint32_t kBlocks = K * sizeof(T) / C0_SIZE;
            int srcOffset = 0;
            int dstOffset = 0;
            for (uint32_t i = 0; i < mBlocks; ++i) {
                AscendC::LoadData2DParams loadDataParams;
                loadDataParams.repeatTimes = kBlocks;
                loadDataParams.srcStride = mBlocks;
                loadDataParams.ifTranspose = false;
                AscendC::LoadData(a2[dstOffset], a1[srcOffset], loadDataParams);
                srcOffset += CUBE_BLOCK * CUBE_BLOCK;
                dstOffset += K * CUBE_BLOCK;
            }
    #elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
            AscendC::LoadData2DParamsV2 loadDataParams;
            loadDataParams.mStartPosition = 0;
            loadDataParams.kStartPosition = 0;
            loadDataParams.mStep = AscendC::DivCeil(M, CUBE_BLOCK);
            loadDataParams.kStep = AscendC::DivCeil(K * sizeof(T), C0_SIZE);
            loadDataParams.srcStride = AscendC::DivCeil(M, CUBE_BLOCK);
            loadDataParams.dstStride = AscendC::DivCeil(M, CUBE_BLOCK);
            loadDataParams.sid = 0;
            loadDataParams.ifTranspose = false;
            AscendC::LoadData(a2, a1, loadDataParams);
    #endif
        }
    ```

-   **The 3510 architecture version removes the 4:2 structured sparsity feature from the hardware architecture.**<a name="li69092585134"></a>

    **Description**: LoadDataWithSparse is used to transfer the 512B dense weight matrix stored in L1 Buffer to L0B buffer, and simultaneously reads the 128B index matrix to sparsify the dense matrix. Since the 3510 architecture version does not support the structured sparsity feature, LoadDataWithSparse is not applicable in this version. On the other hand, MmadWithSparse is responsible for performing matrix multiply-accumulate operations, where the right matrix B is a dense matrix that needs to be loaded by calling LoadDataWithSparse. Since the 3510 architecture does not support LoadDataWithSparse, MmadWithSparse cannot be used in the 3510 architecture version either.

    **Compatibility solution**: On the operator side, you can skip calling LoadDatawithSparse for the dense-to-sparse matrix conversion, and then use Mmad for normal dense matrix computation. For sparse matrix related algorithms, refer to the introduction in MmadWithSparse.

-   **The 3510 architecture version removes the GM->L0A Buffer/L0B Buffer path.**

    **Description**: The hardware removes the GM->L0A Buffer/L0B Buffer path. When calling LoadData, these paths are no longer supported.

    **Compatibility solution**: To implement GM->L0A Buffer/L0B Buffer transfer, split it into two steps: first transfer from GM to L1 Buffer, then transfer from L1 Buffer to L0A Buffer and L0B Buffer.

     Taking the GM->L1 Buffer->L0A Buffer path as an example, refer to the following steps:

    1.  Transfer matrix A from GM to L1 Buffer.

        ```
        __aicore__ inline void CopyGmToA1(AscendC::LocalTensor<T>& leftMatrix)
        {
            AscendC::Nd2NzParams intriParams1{1, 64, 128, 0, 128, 64, 1, 0};
            AscendC::DataCopy(leftMatrix, aGlobal, intriParams1);
        }
        ```

    2.  Move matrix A from the L1 Buffer to the L0A Buffer.

        ```
        __aicore__ inline void Load2DA1ToL0A(AscendC::LocalTensor<T>& a1, AscendC::LocalTensor<T>& a2)
        {
            AscendC::LoadData2DParamsV2 loadDataParams;
            ...
            AscendC::LoadData(a2, a1, loadDataParams);
        }
        ```

-   **The 3510 architecture version removes the hardware instructions for initializing the L0A Buffer and L0B Buffer.**

    **Note:** The Fill API initializes the LocalTensor at a specific storage location to a specific value. It does not support directly initializing the L0A Buffer or L0B Buffer.

    **Compatibility solution:** First initialize the L1 Buffer by using the Fill API, and then move the data in the L1 Buffer to the L0A Buffer and L0B Buffer by using the LoadData API. For sample code, see [Fill compatibility sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/06_compatibility_guide/fill).

    Take the GM -> L1 Buffer -> L0A Buffer data path as an example:

    1.  Initialize the L1 Buffer.

        ```
        __aicore__ inline void InitConstA1(AscendC::LocalTensor<T>& a1Local)
        {
            AscendC::Fill(a1Local, {1, static_cast<uint16_t>(M * K * sizeof(T) / 32), 0, 1});
        }
        ```

    2.  Call the LoadData API to move the data in the L1 Buffer to the L0A Buffer.

        ```
        __aicore__ inline void Load2DA1ToA2(AscendC::LocalTensor<T>& a1Local, AscendC::LocalTensor<T>& a2Local)
        {
            AscendC::LoadData2DParamsV2 loadDataParams;
            ...
            AscendC::LoadData(a2Local, a1Local, loadDataParams);
        }
        ```

## System Variable Access<a name="section34615332215"></a>

-   **CheckLocalMemoryIA is not supported. The related registers are removed in architecture version 3510.**

    **Note**: CheckLocalMemoryIA monitors the UB read/write behavior within a specified range. If read/write behavior within the specified range is detected, an EXCEPTION error is reported. If no read/write behavior within the specified range is detected, no error is reported.

    **Compatibility solution**: This API is a debugging API and has no impact on functionality.

## Debugging APIs<a name="section12611957102116"></a>

-   **Tensor information printing is not supported on the L1 Buffer.**

    **Note:** Because the chip removes the L1 Buffer->GM path, the L1 Buffer->GM function is not supported.

    **Compatibility solution:** This API is a debugging API and has no impact on functionality.
