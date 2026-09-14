# Operator Precision Issues Involving the Matmul High-Level API<a name="ZH-CN_TOPIC_0000002070127457"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T11:21:25.107Z pushedAt=2026-08-31T11:26:59.561Z -->

This section provides preliminary fault localization and isolation guidance for operators that use the Matmul high-level API, to troubleshoot whether an operator precision issue is caused by the way the Matmul high-level API is called in the operator. Unless otherwise specified, the following examples are based on Atlas A2 training products/Atlas A2 inference products.

The troubleshooting process consists of the following six steps:

1.  Debug in the CPU domain and observe the error information;
2.  Check whether Matmul tiling has been modified and whether the modification is reasonable;
3.  Hide the vector computation in the operator and call only the Matmul API, and check whether the operator functionality is correct;
4.  Execute on a single core and check whether the operator functionality is correct;
5.  Troubleshoot whether the Matmul API is used correctly;
6.  Check whether the golden script used for operator debugging is correct.

The details are as follows:

1.  **Debug in the CPU domain and observe the error information**.

    After completing the operator code development, debug the operator functionality first through the [CPU debugging project in kernel launch](../completing_kernel_launch_based_on_sample_project.md#section883611324486). When debugging in the CPU domain, if a compilation or execution error occurs, the log generally contains obvious error information. Based on the prompt content of the error information, you can usually quickly locate the code position corresponding to the problem. This method is especially effective for quickly locating the specific cause of basic parameter usage issues such as address out-of-bounds caused by incorrect DataCopy parameter settings, incorrect operator tiling parameter settings, and other out-of-bounds memory access.

    1.  Case:

        The following is a code snippet of the Matmul operator kernel function. This code implements calculating the address offset of the data to be used by each core based on the A and B matrices in Global Memory and the tiling information, creating a Matmul object, and computing the Matmul result.

        ```
        extern "C" __global__ __aicore__ void matmul_custom(GM_ADDR a, GM_ADDR b, GM_ADDR c, GM_ADDR workspace, GM_ADDR tilingGm)
        {
            using A_T = half;
            using B_T = half;
            using C_T = float;
         
            AscendC::TPipe pipe;
            TCubeTiling tiling;
            CopyTiling(&tiling, tilingGm);
         
            AscendC::GlobalTensor<A_T> aGlobal;
            AscendC::GlobalTensor<B_T> bGlobal;
            AscendC::GlobalTensor<C_T> cGlobal;
            aGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ A_T *>(a), tiling.M * tiling.Ka);
            bGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ B_T *>(b), tiling.Ka * tiling.N);
            cGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ C_T *>(c), tiling.M * tiling.N);
         
            int offsetA = 0;
            int offsetB = 0;
            int offsetC = 0;
            bool isTransA = false;
            bool isTransB = true;
         
            int tailM = 0;
            int tailN = 0;
            CalcGMOffset(GetBlockIdx(), tiling, offsetA, offsetB, offsetC, tailM, tailN, isTransA, isTransB);
         
            auto gmA = aGlobal[offsetA];
            auto gmB = bGlobal[offsetB];
            auto gmC = cGlobal[offsetC];
         
            AscendC::Matmul<AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, A_T>,
                   AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, B_T>,
                   AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, C_T>> mm;
            REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
        
            mm.SetTensorA(gmA, isTransA);
            mm.SetTensorB(gmB, isTransB);
            mm.SetTail(tailM, tailN);
            mm.IterateAll(gmC);
            mm.End();
        }
        ```

        The following is the execution result output by the above code during CPU domain debugging. For the paths in the following example, use the actual paths.

        ```
        [ASSERT] $HOME/Ascend/xxxxx/include/ascendc/highlevel_api/lib/matmul/matmul_client.h:268: Assertion `isTransposeB <= B_TYPE::isTrans && "It is not allowed to do B transpose when matmul B transpose is not defined."'
        [ASSERT] $HOME/Ascend/xxxxx/include/ascendc/highlevel_api/lib/matmul/matmul_client.h:268: Assertion `isTransposeB <= B_TYPE::isTrans && "It is not allowed to do B transpose when matmul B transpose is not defined."'
        ```

        The operator in this case has a precision problem, so CPU debugging is used to debug the operator functionality. After the CPU runs, according to the error information indicating that the transpose of matrix B is undefined, check the code related to the settings of matrix B. It is found that B\_TYPE::isTrans is not set when the Matmul object is defined, while the SetTensorB API sets isTransB = true, causing an execution error. Therefore, the root cause of this problem is that the isTransB value set by SetTensorB does not match B\_TYPE.

2.  **Whether Matmul tiling is modified and whether the modification is reasonable**

    Generally, in the tiling implementation of an operator that contains Matmul, the Matmul tiling is obtained by calling the [GetTiling](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/GetTiling.md) API, and its data type is the [TCubeTiling](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/tcubetiling_structure.md) structure. In this case, this set of tiling values is valid. In some cases, the user customizes a set of TCubeTiling parameter values, or modifies some of the values based on the TCubeTiling returned by the [GetTiling](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/GetTiling.md) API. Such modifications must satisfy the constraint conditions between parameters.

    To obtain all tiling parameter values, you need to print logs related to the tiling parameters. Set the log environment variable to obtain the MatmulTiling parameter values. The command for setting the environment variable is as follows:

    ```
    export ASCEND_GLOBAL_LOG_LEVEL=1
    export ASCEND_SLOG_PRINT_TO_STDOUT=1
    ```

    Search for the "MatmulTiling" keyword in the log, and refer to [TCubeTiling constraint conditions](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/tcubetiling_structure.md) to check whether the tiling values are valid. If a constraint condition is not satisfied, modify the corresponding related parameters so that this set of TCubeTiling parameter values are all valid.

    ```
    cat test_tiling.log |grep MatmulTiling // test_tiling.log is an example log file name.
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.864 [matmul_tiling_base.cpp:697][PrintTilingDataInfo] MatmulTiling: M             = 1024
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.870 [matmul_tiling_base.cpp:698][PrintTilingDataInfo] MatmulTiling: N             = 640
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.873 [matmul_tiling_base.cpp:699][PrintTilingDataInfo] MatmulTiling: Ka            = 256
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.876 [matmul_tiling_base.cpp:700][PrintTilingDataInfo] MatmulTiling: Kb            = 256
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.879 [matmul_tiling_base.cpp:701][PrintTilingDataInfo] MatmulTiling: singleCoreM   = 512
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.882 [matmul_tiling_base.cpp:702][PrintTilingDataInfo] MatmulTiling: singleCoreN   = 640
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.884 [matmul_tiling_base.cpp:703][PrintTilingDataInfo] MatmulTiling: singleCoreK   = 256
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.887 [matmul_tiling_base.cpp:704][PrintTilingDataInfo] MatmulTiling: baseM         = 256
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.890 [matmul_tiling_base.cpp:705][PrintTilingDataInfo] MatmulTiling: baseN         = 128
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.893 [matmul_tiling_base.cpp:706][PrintTilingDataInfo] MatmulTiling: baseK         = 64
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.896 [matmul_tiling_base.cpp:707][PrintTilingDataInfo] MatmulTiling: depthA1       = 10
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.899 [matmul_tiling_base.cpp:708][PrintTilingDataInfo] MatmulTiling: depthB1       = 2
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.902 [matmul_tiling_base.cpp:709][PrintTilingDataInfo] MatmulTiling: depthAL1CacheUB     = 0
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.905 [matmul_tiling_base.cpp:710][PrintTilingDataInfo] MatmulTiling: depthBL1CacheUB     = 0
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.908 [matmul_tiling_base.cpp:711][PrintTilingDataInfo] MatmulTiling: stepM         = 2
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.912 [matmul_tiling_base.cpp:712][PrintTilingDataInfo] MatmulTiling: stepN         = 1
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.915 [matmul_tiling_base.cpp:713][PrintTilingDataInfo] MatmulTiling: isBias        = 1
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.917 [matmul_tiling_base.cpp:714][PrintTilingDataInfo] MatmulTiling: transLength   = 0
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.920 [matmul_tiling_base.cpp:715][PrintTilingDataInfo] MatmulTiling: iterateOrder  = 0
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.923 [matmul_tiling_base.cpp:716][PrintTilingDataInfo] MatmulTiling: shareMode     = 0
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.926 [matmul_tiling_base.cpp:717][PrintTilingDataInfo] MatmulTiling: usedL1Size    = 295424
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.929 [matmul_tiling_base.cpp:718][PrintTilingDataInfo] MatmulTiling: usedL0CSize   = 131072
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.932 [matmul_tiling_base.cpp:719][PrintTilingDataInfo] MatmulTiling: usedUBSize    = 0
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.935 [matmul_tiling_base.cpp:720][PrintTilingDataInfo] MatmulTiling: batchM        = 1
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.938 [matmul_tiling_base.cpp:721][PrintTilingDataInfo] MatmulTiling: batchN        = 1
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.941 [matmul_tiling_base.cpp:722][PrintTilingDataInfo] MatmulTiling: singleBatchM  = 1
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.943 [matmul_tiling_base.cpp:723][PrintTilingDataInfo] MatmulTiling: singleBatchN  = 1
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.946 [matmul_tiling_base.cpp:724][PrintTilingDataInfo] MatmulTiling: stepKa        = 4
    [INFO] ASCENDCKERNEL(1202803,ascendc_kernels_bbit):2024-10-12-08:53:59.636.949 [matmul_tiling_base.cpp:725][PrintTilingDataInfo] MatmulTiling: stepKb        = 1
    ```

    For example, based on the TCubeTiling parameters printed above, check the value of each parameter against [TCubeTiling constraint conditions](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/tcubetiling_structure.md). The value of depthA1 should be equal to stepM\*stepKa or stepM\*stepKa\*2. However, the value of depthA1 is 10, which is neither equal to stepM\*stepKa=8 nor equal to stepM\*stepKa\*2=16, and therefore does not satisfy the constraint conditions. As a result, the value of depthA1 needs to be corrected.

3.  **The operator hides vector computation and only calls the Matmul API. Check whether the operator functionality is correct.**

    The code of a fused operator contains both the Matmul API and vector computation APIs. By deleting the vector computation APIs from the operator code and keeping only the Matmul API, you can perform quick fault localization to determine whether the precision issue of the fused operator is caused by incorrect use of the Matmul API. The specific troubleshooting process is as follows: modify the operator code logic, delete the vector computation code, and make corresponding modifications to the golden script. After completing the adaptation, [execute the operator on the CPU domain or NPU domain](../completing_kernel_launch_based_on_sample_project.md) and observe whether the computation result is correct. If the computation result is correct, the Matmul API is used correctly in the code, and you need to continue troubleshooting whether the vector computation is correct. Otherwise, if the computation result is incorrect, you need to continue troubleshooting whether the Matmul API is used correctly.

    -   Case:

        Take the fused operator matmul\_leakyrelu as an example. After the operator is executed, a precision issue occurs, as shown in the following figure.

        ```
        data index: 000195, expected: -0.693000019, actual: -69.300003052, rdiff: -99.000000
        data index: 000196, expected: -0.209000006, actual: -20.899999619, rdiff: -99.000000
        data index: 000197, expected: -0.517000020, actual: -51.700000763, rdiff: -99.000000
        data index: 000200, expected: -0.193000004, actual: -19.300001144, rdiff: -99.000000
        data index: 000202, expected: -0.684000015, actual: -68.400001526, rdiff: -99.000000
        data index: 000204, expected: -0.422000021, actual: -42.200000763, rdiff: -98.999992
        data index: 000209, expected: -0.109000005, actual: -10.900000572, rdiff: -99.000000
        error ratio: 0.4517, tolerance: 0.0001
        [ERROR] result error
        ```

        Modify the operator code to comment out the LeakyRelu API computation. At the same time, adapt and modify the corresponding memory allocation and related synchronization code. Then, comment out the LeakyRelu computation in the golden script. The specific modification example is as follows.

        The following is a code snippet of the operator kernel function.

        ```
        template <typename aType, typename bType, typename cType, typename biasType>
        __aicore__ inline void MatmulLeakyKernel<aType, bType, cType, biasType>::Process(AscendC::TPipe *pipe)
        {
            uint32_t computeRound = 0;
        
            matmulObj.SetTensorA(aGlobal);
            matmulObj.SetTensorB(bGlobal);
            matmulObj.SetBias(biasGlobal);
            while (matmulObj.template Iterate<true>()) {
                MatmulCompute();
                // LeakyReluCompute(); // Comment out the LeakyReluCompute vector computation.
                CopyOut(computeRound);
                computeRound++;
            }
            matmulObj.End();
        }
         
        template <typename aType, typename bType, typename cType, typename biasType>
        __aicore__ inline void MatmulLeakyKernel<aType, bType, cType, biasType>::MatmulCompute()
        {
            reluOutLocal = reluOutQueue_.AllocTensor<cType>();
            matmulObj.template GetTensorC<true>(reluOutLocal, false, true);
            reluOutQueue_.EnQue(reluOutLocal); // Move the output of reluOutLocal in the LeakyReluCompute() API to this location in advance.
        }
         
        template <typename aType, typename bType, typename cType, typename biasType>
        __aicore__ inline void MatmulLeakyKernel<aType, bType, cType, biasType>::LeakyReluCompute()
        {
            LeakyRelu(reluOutLocal, reluOutLocal, (cType)0.1, tiling.baseM * tiling.baseN);
            reluOutQueue_.EnQue(reluOutLocal);
        }
         
        template <typename aType, typename bType, typename cType, typename biasType>
        __aicore__ inline void MatmulLeakyKernel<aType, bType, cType, biasType>::CopyOut(uint32_t count)
        {
            reluOutQueue_.DeQue<cType>();
            const uint32_t roundM = tiling.singleCoreM / tiling.baseM;
            const uint32_t roundN = tiling.singleCoreN / tiling.baseN;
            uint32_t startOffset = (count % roundM * tiling.baseM * tiling.N + count / roundM * tiling.baseN);
            AscendC::DataCopyParams copyParam = {(uint16_t)tiling.baseM, (uint16_t)(tiling.baseN * sizeof(cType) / AscendC::DEFAULT_C0_SIZE), 0,
                                        (uint16_t)((tiling.N - tiling.baseN) * sizeof(cType) / AscendC::DEFAULT_C0_SIZE)};
            DataCopy(cGlobal[startOffset], reluOutLocal, copyParam);
            reluOutQueue_.FreeTensor(reluOutLocal);
        }
        ```

        The following is a code snippet of the golden generation script.

        ```
        def gen_golden_data():
            M = 1024
            N = 640
            K = 256
         
            input_a = np.random.randint(-10, 10, [M, K]).astype(np.float16)
            input_b = np.random.randint(-10, 10, [K, N]).astype(np.float16)
            input_bias = np.random.randint(-10, 10, [N]).astype(np.float32)
            alpha = 0.001
            golden = (np.matmul(input_a.astype(np.float32), input_b.astype(np.float32)) + input_bias).astype(np.float32)
            # golden = np.where(golden >= 0, golden, golden * alpha) # To stay consistent with the kernel, the golden generation also needs to comment out the corresponding LeakyRelu computation.
            os.system("mkdir -p input")
            os.system("mkdir -p output")
            input_a.tofile("./input/x1_gm.bin")
            input_b.tofile("./input/x2_gm.bin")
            input_bias.tofile("./input/bias.bin")
            golden.tofile("./output/golden.bin")
        ```

        After deleting the LeakyRelu computation, execute the case. The operator computation result is correct, as shown below.

        ```
        -- Installing: $HOME/samples/Precision_Check_Guide/samples-master/operator/MatmulLeakyReluCustomSample/KernelLaunch/MatmulLeakyReluInvocation_cube_vec/out/bin/ascendc_kernels_bbit
        8901941eee314bcd64d24ff5f8d21247  output/golden.bin
        8901941eee314bcd64d24ff5f8d21247  output/output.bin
        error ratio: 0.0000, tolerance: 0.0001
        test pass
        ```

        From this, it can be determined that the Matmul API is correctly used in the operator code and the correct Matmul API computation result is obtained. It is necessary to continue locating the problem in the use of the LeakyRelu API within the LeakyReluCompute function.

4.  **Single-core execution: Check whether the operator functionality is correct.**

    Verifying whether the operator functionality is correct in the single-core scenario helps quickly determine whether the Matmul API computation result does not meet expectations or whether the Matmul API is incorrectly called in the operator code. Because the Matmul API internally implements single-core computation logic, a case where the single-core computation result is correct but the multi-core computation result is incorrect indicates that the use and computation of the Matmul API on a single core are correct. In this case, you need to troubleshoot whether the code logic related to multi-core splitting is correct, such as whether the input and output address offsets of each core are correct and whether the tail block address settings on each core are correct. If the operator precision is incorrect in the single-core scenario, you need to troubleshoot whether the Matmul API is used correctly. For details, see [Step 5](#li1950561483219).

    Note: In the tiling implementation of an operator that contains Matmul, the multi-core tiling of Matmul requires using MultiCoreMatmulTiling to construct a multi-core tiling object, and using the SetDim API to set the number of cores used for Matmul computation. Note that the number of cores set here is the number of cores used for Matmul computation, and it is set only in the multi-core scenario to calculate the tiling parameters. The following two cases are operators in MIX mode. For the SetDim setting rules, see [Core Count Setting Rules in MIX Scenarios](../../../operator_practice/simd_operator_impl/fusion_operator_programming/cv_fusion/operator_implementation.md#zh-cn_topic_0000001644252364_li4790115115920).

    -   Case 1: Incorrect output address offset in the multi-core splitting scenario

        Take Matmul with M=512, N=1024, and K=512 as an example. In the operator code in MIX mode, the number of AIC cores is set to 4 and the number of AIV cores is set to 8. Because this case uses the separated mode as an example, SetDim is set to 8, which is the number of AIV cores. When the operator is executed in the multi-core scenario, the computation result precision is incorrect.

        The following is a code snippet of the operator tiling computation.

        ```
        uint8_t *GenerateTiling(const char *socVersion)
        {
            int M = 512;
            int N = 1024;
            int K = 512;
         
            TPosition leftPosition = TPosition::GM;
            CubeFormat leftFormat = CubeFormat::ND;
            DataType leftDtype = DataType::DT_FLOAT16;
            bool isTransA = false;
         
            TPosition rightPosition = TPosition::GM;
            CubeFormat rightFormat = CubeFormat::ND;
            DataType rightDtype = DataType::DT_FLOAT16;
            bool isTransB = false;
         
            TPosition resultPosition = TPosition::GM;
            CubeFormat resultFormat = CubeFormat::ND;
            DataType resultDtype = DataType::DT_FLOAT;
         
            bool isBias = false;
         
            int usedCoreNum = 8;
            int32_t baseM = 128;
            int32_t baseN = 256;
         
            optiling::TCubeTiling tilingData;
            auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance(socVersion);
            MultiCoreMatmulTiling tilingApi(*ascendcPlatform);
         
            tilingApi.SetDim(usedCoreNum); // Set to 8, the number of AIV cores.
            tilingApi.SetAType(leftPosition, leftFormat, leftDtype, isTransA);
            tilingApi.SetBType(rightPosition, rightFormat, rightDtype, isTransB);
            tilingApi.SetCType(resultPosition, resultFormat, resultDtype);
         
            tilingApi.SetOrgShape(M, N, K);
            tilingApi.SetShape(M, N, K);
            tilingApi.SetFixSplit(baseM, baseN, -1);
            tilingApi.SetBias(isBias);
            tilingApi.SetBufferSpace(-1, -1, -1);
         
            int64_t res = tilingApi.GetTiling(tilingData);
            if (res == -1) {
                std::cout << "gen tiling failed" << std::endl;
            }
            return GetTilingBuf(&tilingData);
        }
        ```

        The following is a code snippet of the operator kernel function.

        ```
        __aicore__ inline void CalcGMOffset(int blockIdx, const TCubeTiling &tiling, int &offsetA, int &offsetB, int &offsetC,
                                            int &tailM, int &tailN, bool isTransA, bool isTransB)
        {
            uint32_t mSingleBlocks = CeilDiv(tiling.M, tiling.singleCoreM);
            uint32_t mCoreIndx = blockIdx % mSingleBlocks;
            uint32_t nCoreIndx = blockIdx / mSingleBlocks;
         
            offsetA = mCoreIndx * tiling.Ka * tiling.singleCoreM;
            if (isTransA) {
                offsetA = mCoreIndx * tiling.singleCoreM;
            }
            offsetB = nCoreIndx * tiling.singleCoreN;
            if (isTransB) {
                offsetB = nCoreIndx * tiling.Kb * tiling.singleCoreN;
            }
            offsetC = mCoreIndx * tiling.singleCoreN * tiling.singleCoreM + nCoreIndx * tiling.singleCoreN; // The tiling.singleCoreN parameter here is incorrect. It should be tiling.N.
         
            tailM = tiling.M - mCoreIndx * tiling.singleCoreM;
            tailM = tailM < tiling.singleCoreM ? tailM : tiling.singleCoreM;
         
            tailN = tiling.N - nCoreIndx * tiling.singleCoreN;
            tailN = tailN < tiling.singleCoreN ? tailN : tiling.singleCoreN;
        }
         
        extern "C" __global__ __aicore__ void matmul_custom(GM_ADDR a, GM_ADDR b, GM_ADDR c, GM_ADDR workspace,
                                                            GM_ADDR tilingGm)
        {
            using A_T = half;
            using B_T = half;
            using C_T = float;
         
            AscendC::TPipe pipe;
            TCubeTiling tiling;
            CopyTiling(&tiling, tilingGm);
         
            AscendC::GlobalTensor<A_T> aGlobal;
            AscendC::GlobalTensor<B_T> bGlobal;
            AscendC::GlobalTensor<C_T> cGlobal;
            aGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ A_T *>(a), tiling.M * tiling.Ka);
            bGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ B_T *>(b), tiling.Ka * tiling.N);
            cGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ C_T *>(c), tiling.M * tiling.N);
         
            int offsetA = 0;
            int offsetB = 0;
            int offsetC = 0;
            bool isTransA = false;
            bool isTransB = false;
         
            int tailM = 0;
            int tailN = 0;
            CalcGMOffset(GetBlockIdx(), tiling, offsetA, offsetB, offsetC, tailM, tailN, isTransA, isTransB);
         
            auto gmA = aGlobal[offsetA];
            auto gmB = bGlobal[offsetB];
            auto gmC = cGlobal[offsetC];
         
            AscendC::Matmul<AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, A_T>,
                   AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, B_T>,
                   AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, C_T>> mm;
            REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
        
            mm.SetTensorA(gmA, isTransA);
            mm.SetTensorB(gmB, isTransB);
            mm.SetTail(tailM, tailN);
            mm.IterateAll(gmC);
            mm.End();
        }
        ```

        Execute the operator. The precision check fails:

        ```
        data index: 000609, expected: 12979.000000000, actual: 0.000000000, rdiff: 1.000000
        data index: 000610, expected: 12931.000000000, actual: 0.000000000, rdiff: 1.000000
        data index: 000611, expected: 13120.000000000, actual: 0.000000000, rdiff: 1.000000
        data index: 000612, expected: 12275.000000000, actual: 0.000000000, rdiff: 1.000000
        error ratio: 0.8750, tolerance: 0.0001
        [ERROR] result error
        ```

        Modify the test script and the operator tiling code, and verify the operator execution result on a single core to perform quick fault localization. The details are as follows:

        Modify the operator debugging code to start only a single core. In the CPU debugging code, set numBlocks in the ICPU\_RUN\_KF macro API to 1 (indicating one group of AIC and AIV). In the operator tiling implementation, set the single-core scenario, with the number of AIC cores set to 1 and the number of AIV cores set to 2, and set SetDim to 2, which is the number of AIV cores. The code is as follows.

        The following is a code snippet of the debugging script.

        ```
        uint32_t numBlocks = 1;
        ICPU_RUN_KF(matmul_custom, numBlocks, a, b, c, workspace, tiling);
        ```

        The following is a code snippet of the operator tiling computation.

        ```
        int usedCoreNum = 2;
        tilingApi.SetDim(usedCoreNum);
        ```

        After switching to the single-core scenario, execute the operator:

        ```
        -- Installing: $HOME/samples/Precision_Check_Guide/samples-master/operator/MatmulCustomSample/KernelLaunch/MatmulInvocationNeo-muticore/out/bin/ascendc_kernels_bbit
        efaf4dc1e484bc3778cac65f56244e59  output/golden.bin
        efaf4dc1e484bc3778cac65f56244e59  output/output.bin
        error ratio: 0.0000, tolerance: 0.0001
        test pass
        ```

        As shown in the preceding comparison result, the single-core verification result is correct. Therefore, the precision issue can be localized to the multi-core logic.

        First, troubleshoot the input and output address offsets after multi-core splitting. Analyze the CalcGMOffset function and locate the error in the offset address offsetC of matrix C. The correct offset should be mCoreIndx \* tiling.N \* tiling.singleCoreM + nCoreIndx \* tiling.singleCoreN. After modifying offsetC to the correct offset address, execute the operator, and the computation result comparison is correct.

        Note that in the modification verification of the single-core scenario above, the number of AIC cores is 1 and the number of AIV cores is 2. To further verify without introducing any multi-core splitting, set both the number of AIC cores and the number of AIV cores to 1. The code modification example is as follows:

        -   After the REGIST\_MATMUL\_OBJ API in the kernel function, use judgment code so that AIV cores whose BlockIdx is not 0 exit.

            The following is a code snippet of the operator kernel function.

            ```
            extern "C" __global__ __aicore__ void matmul_custom(GM_ADDR a, GM_ADDR b, GM_ADDR c, GM_ADDR workspace,
                                                                GM_ADDR tilingGm)
            {
                using A_T = half;
                using B_T = half;
                using C_T = float;
             
                AscendC::TPipe pipe;
                TCubeTiling tiling;
                CopyTiling(&tiling, tilingGm);
             
                AscendC::GlobalTensor<A_T> aGlobal;
                AscendC::GlobalTensor<B_T> bGlobal;
                AscendC::GlobalTensor<C_T> cGlobal;
                aGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ A_T *>(a), tiling.M * tiling.Ka);
                bGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ B_T *>(b), tiling.Ka * tiling.N);
                cGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ C_T *>(c), tiling.M * tiling.N);
             
                int offsetA = 0;
                int offsetB = 0;
                int offsetC = 0;
                bool isTransA = false;
                bool isTransB = false;
             
                int tailM = 0;
                int tailN = 0;
                CalcGMOffset(GetBlockIdx(), tiling, offsetA, offsetB, offsetC, tailM, tailN, isTransA, isTransB);
             
                auto gmA = aGlobal[offsetA];
                auto gmB = bGlobal[offsetB];
                auto gmC = cGlobal[offsetC];
             
                AscendC::Matmul<AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, A_T>,
                       AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, B_T>,
                       AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, C_T>> mm;
                REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
                if (GetBlockIdx() == 1) {
                    return;
                }
                 
                mm.SetTensorA(gmA, isTransA);
                mm.SetTensorB(gmB, isTransB);
                mm.SetTail(tailM, tailN);
                mm.IterateAll(gmC);
                mm.End();
            }
            ```

        -   In the ICPU\_RUN\_KF of the operator debugging script, set numBlocks and usedCoreNum in SetDim of the operator tiling to 1.

            The following is a code snippet of the operator debugging.

            ```
            uint32_t numBlocks = 1;
            ICPU_RUN_KF(matmul_custom, numBlocks, a, b, c, workspace, tiling);
            ```

            The following is a code snippet of the operator tiling computation.

            ```
            int usedCoreNum = 1;
            tilingApi.SetDim(usedCoreNum);
            ```

    -   Case 2: Incorrect tail block settings

        In the multi-core scenario, when the singleCoreM/singleCoreN/singleCoreK values of the last core differ from those of the preceding cores, you need to call the SetTail API on the last core, that is, the tail core, to adjust singleCoreM/singleCoreN/singleCoreK to the corresponding values on the actual tail core. If these parameter values are not set on the tail core, or if the set parameter values are incorrect in size, multi-core precision errors will also occur while single-core precision remains correct.

        ```
        data index: 100254, expected: 13605.000000000, actual: 13137.000000000, rdiff: 0.034399
        data index: 101277, expected: 13268.000000000, actual: 13419.000000000, rdiff: 0.011381
        data index: 102300, expected: 13509.000000000, actual: 13114.000000000, rdiff: 0.029240
        data index: 103323, expected: 13526.000000000, actual: 13400.000000000, rdiff: 0.009315
        error ratio: 0.0010, tolerance: 0.0001
        [ERROR] result error
        ```

        The following is a code snippet of the operator kernel function.

        ```
        __aicore__ inline void CalcGMOffset(int blockIdx, const TCubeTiling &tiling, int &offsetA, int &offsetB, int &offsetC,
                                            int &tailM, int &tailN, bool isTransA, bool isTransB)
        {
            uint32_t mSingleBlocks = CeilDiv(tiling.M, tiling.singleCoreM);
            uint32_t mCoreIndx = blockIdx % mSingleBlocks;
            uint32_t nCoreIndx = blockIdx / mSingleBlocks;
         
            offsetA = mCoreIndx * tiling.Ka * tiling.singleCoreM;
            if (isTransA) {
                offsetA = mCoreIndx * tiling.singleCoreM;
            }
            offsetB = nCoreIndx * tiling.singleCoreN;
            if (isTransB) {
                offsetB = nCoreIndx * tiling.Kb * tiling.singleCoreN;
            }
            offsetC = mCoreIndx * tiling.N * tiling.singleCoreM + nCoreIndx * tiling.singleCoreN;
         
            // M/N computation for the tail core. This is the correct computation method.
            tailM = tiling.M - mCoreIndx * tiling.singleCoreM;
            tailM = tailM < tiling.singleCoreM ? tailM : tiling.singleCoreM;
         
            tailN = tiling.N - nCoreIndx * tiling.singleCoreN;
            tailN = tailN < tiling.singleCoreN ? tailN : tiling.singleCoreN;
        }
         
        extern "C" __global__ __aicore__ void matmul_custom(GM_ADDR a, GM_ADDR b, GM_ADDR c, GM_ADDR workspace,
                                                            GM_ADDR tilingGm)
        {
            using A_T = half;
            using B_T = half;
            using C_T = float;
         
            AscendC::TPipe pipe;
            TCubeTiling tiling;
            CopyTiling(&tiling, tilingGm);
         
            AscendC::GlobalTensor<A_T> aGlobal;
            AscendC::GlobalTensor<B_T> bGlobal;
            AscendC::GlobalTensor<C_T> cGlobal;
            aGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ A_T *>(a), tiling.M * tiling.Ka);
            bGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ B_T *>(b), tiling.Ka * tiling.N);
            cGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ C_T *>(c), tiling.M * tiling.N);
         
            int offsetA = 0;
            int offsetB = 0;
            int offsetC = 0;
            bool isTransA = false;
            bool isTransB = false;
         
            int tailM = 0;
            int tailN = 0;
            CalcGMOffset(GetBlockIdx(), tiling, offsetA, offsetB, offsetC, tailM, tailN, isTransA, isTransB);
         
            auto gmA = aGlobal[offsetA];
            auto gmB = bGlobal[offsetB];
            auto gmC = cGlobal[offsetC];
         
            AscendC::Matmul<AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, A_T>,
                   AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, B_T>,
                   AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, C_T>> mm;
            REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
        
            mm.SetTensorA(gmA, isTransA);
            mm.SetTensorB(gmB, isTransB);
            // mm.SetTail(tailM, tailN); API for setting the tail core. If the tail block is not updated here, the precision is correct on a single core but fails on multiple cores.
            mm.IterateAll(gmC);
            mm.End();
        }
        ```

5.  <a name="li1950561483219"></a>**Check whether the Matmul API is used correctly.**

    After the preceding steps, you can determine whether the issue is caused by incorrect use of the Matmul API. If the operator precision issue is caused by incorrect use of the Matmul API, check whether each API is used correctly based on the usage instructions and constraint conditions of the Matmul APIs.

    -   Case 1: Constraint conditions of the API are not followed.

        In the Matmul MDL template, calling the [IterateBatch](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/IterateBatch.md) API causes the operator execution to fail. This is because the constraint conditions of this API are not met. The IterateBatch API supports only the Norm template.

        For such issues, carefully read the constraint conditions of each Matmul API and check whether the APIs used in the operator implementation meet the corresponding constraint conditions.

    -   Case 2: Constraint conditions of the template are not followed.

        When the [doMTE2Preload](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/MatmulConfig.md) preloading template is enabled, if the K direction is not fully loaded, the template constraint conditions are not met, causing the precision comparison to fail.

        In addition to meeting the constraint conditions of the function APIs, you must also meet the corresponding constraint conditions of the template parameters and check the use of the template parameters.

6.  **Check whether the golden script used for operator debugging is correct.**

    The golden generation script of an operator is implemented by the user based on the functional logic of the custom operator, and is used to compare whether the operator execution result is correct. Therefore, the logic of the golden script must be consistent with the implementation logic of the operator. If the golden script is implemented incorrectly, the precision comparison of the operator computation result will fail. In this case, the golden data is not trustworthy.

    Therefore, during the process of quick fault localization for operator precision, users need to check the correctness of the golden script based on the logic of the custom operator. In particular, for operators with complex computation logic, this item must be troubleshooted with priority.
