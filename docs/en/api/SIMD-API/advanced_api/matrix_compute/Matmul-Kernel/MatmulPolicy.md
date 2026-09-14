# MatmulPolicy

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:49:47.970Z pushedAt=2026-09-12T09:55:18.121Z -->

## Applicable Products

### MatmulPolicy

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference product: Supported
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

### TrianUpperMatmulPolicy/TrianLowerMatmulPolicy

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

### NBuffer33MatmulPolicy

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

### MatmulWithScalePolicy/SplitMMatmulPolicy/SplitNMatmulPolicy

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

The **MatmulPolicy** template parameter defines the Matmul extensible module strategy. The following built-in Matmul template strategies are currently supported.

-   **MatmulPolicy** (default template strategy)

    Enables the default implementation strategy of the Matmul API.

-   **TrianUpperMatmulPolicy** (upper triangular template strategy)

    The result computed by a single matrix multiplication instruction is a matrix block of size [baseM \* baseN](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct), which is called a base block. If a base block in the Matmul result matrix C is located in the lower triangular position, Matmul does not process this base block during internal data computation and data moving out, and the resulting matrix C is an upper triangular matrix. The upper triangular template strategy is shown in the following figure, where the matrix shape-related sizes are M=N=512, K=256, and baseM=baseN=baseK=32.

    **Figure 1**  TrianUpperMatmulPolicy  
    ![](../../../../figures/trianuppermatmulpolicy.png)

-   **TrianLowerMatmulPolicy** (lower triangular template strategy)

    The result computed by a single matrix multiplication instruction is a matrix block of size [baseM \* baseN](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct), which is called a base block. If a base block in the Matmul result matrix C is located in the upper triangular position, Matmul does not process this base block during internal data computation and data moving out, and the resulting matrix C is a lower triangular matrix. The lower triangular template strategy is shown in the following figure, where the matrix shape-related sizes are M=N=512, K=256, and baseM=baseN=baseK=32.

    **Figure 2**  TrianLowerMatmulPolicy  
    ![](../../../../figures/trianlowermatmulpolicy.png)

-   **NBuffer33MatmulPolicy**
<a name="li194081238103913"></a>

    The result of a single matrix multiplication instruction is a matrix block of size [baseM \* baseN](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct), which is called a base block. The A matrix computed by a single core is divided into 3x3 base blocks. All these 3x3 base blocks of the A matrix are loaded and kept in the L1 Buffer, and each time they are multiplied with 3x1 base blocks of the B matrix. Meanwhile, DoubleBuffer loads the next 3x1 base blocks of the B matrix required for the next computation in parallel, until the matrix multiplication in the singleCoreN direction is completed. The NBuffer33 template strategy is shown in the following figure, where [singleCoreM, singleCoreN, singleCoreK](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct) represent the shape sizes of the A and B matrices within a single core. The A matrix computed by a single core is divided into 3x3 base blocks, all of which are loaded onto the L1 Buffer, and these base blocks are multiplied with 3x1 base blocks of the B matrix each time.

    **Figure 3** NBuffer33MatmulPolicy  
    ![](../../../../figures/nbuffer33matmulpolicy.png)

-   **MatmulWithScalePolicy**

    Implements matrix multiplication with quantization coefficients, that is, both the left matrix and the right matrix have corresponding quantization coefficient matrices: the left quantization coefficient matrix scaleA and the right quantization coefficient matrix scaleB. In the MxMatmul scenario, the left quantization coefficient matrix is multiplied with the left matrix, the right quantization coefficient matrix is multiplied with the right matrix, and matrix multiplication is performed on the results of the two products.

    **Figure 4** MatmulWithScalePolicy

    ![](../../../../figures/matmulmx.png)

<a id="splitm-template-strategy"></a>
-   **SplitMMatmulPolicy**

    When the computation result of one Matmul [Iterate](Iterate.md) is moved from the L0C Buffer to the Unified Buffer, the dual-output mode is used. That is, in the separated mode, when the core count ratio of AIC cores to AIV cores is 1:2, after the [GetTensorC](GetTensorC.md) API is called, the computation result of one Matmul Iterate is split into two parts along the M direction of the matrix, and the two split result data blocks are moved to the Unified Buffers of the two AIV cores respectively. The schematic diagram of the template strategy is as follows.

    **Figure 5**  SplitMMatmulPolicy  
    ![](../../../../figures/splitmmatmulpolicy.png)

-   SplitNMatmulPolicy (SplitN template strategy)

    When the computation result of one Matmul [Iterate](Iterate.md) is moved from the L0C Buffer to the Unified Buffer, the dual-output mode is used. That is, in the separated mode, when the core count ratio of AIC cores to AIV cores is 1:2, after the [GetTensorC](GetTensorC.md) API is called, the computation result of one Matmul Iterate is split into two parts along the N direction of the matrix, and the two split result data blocks are moved to the Unified Buffers of the two AIV cores respectively. The schematic diagram of the template strategy is as follows.

    **Figure 6**  SplitNMatmulPolicy  
    ![](../../../../figures/splitnmatmulpolicy.png)

## Constraints

-   **TrianUpperMatmulPolicy** currently supports only the [Norm template](MatmulConfig.md#table6981133810309) and the [MDL template](MatmulConfig.md#table6981133810309).
-   **TrianLowerMatmulPolicy** currently supports only the [Norm template](MatmulConfig.md#table6981133810309) and the [MDL template](MatmulConfig.md#table6981133810309).
-   **NBuffer33MatmulPolicy**:
    -   Currently supports only the [MDL template](MatmulConfig.md#table6981133810309).
    -   The logical memory positions of matrix A and matrix B support only `TPosition::GM`.
    -   The MIX mode (including both matrix computation and vector computation) is not supported yet; only the pure Cube mode (matrix computation only) is supported.
    -   The result matrix C of Matmul can be obtained only through the [IterateAll](IterateAll.md) API.
-   [stepM, stepKa, stepKb](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct) are less than or equal to 3, and satisfy: stepKa=stepKb=ceil\([singleCoreK](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct)/baseK\).
    -   The sum of the base block size for fully loading matrix A and the base block size for loading matrix B does not exceed the L1 Buffer size.
    -   Before using the [GetTiling](../Matmul-Tiling/GetTiling.md) API to generate tiling parameters, you must set the **scheduleTypeIn** parameter to ScheduleType::N\_BUFFER\_33 through the [SetMatmulConfigParams](../Matmul-Tiling/SetMatmulConfigParams.md) API to enable the tiling generation logic of the NBuffer33 template strategy.

-   MatmulWithScalePolicy:
    -   Supported only on Ascend 950PR/Ascend 950DT.
    -   Currently supports only the [Norm template](MatmulConfig.md#table6981133810309) and the [MDL template](MatmulConfig.md#table6981133810309).

-   SplitMMatmulPolicy:
    -   Supported only on Ascend 950PR/Ascend 950DT.
    -   Only the output of the C matrix to the Unified Buffer is supported.
    -   The [IBSHARE](matmul_usage.md#table1188045714378) parameter in the **MatmulType** type information of the A matrix and B matrix must be **true**.

-   SplitNMatmulPolicy:
    -   Supported only on Ascend 950PR/Ascend 950DT.
    -   Only the C matrix can be output to the Unified Buffer.
    -   **baseN** must be a multiple of 16.
    -   The [Tiling parameters](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct) must satisfy the following: **singleCoreM** = **baseM**, **singleCoreN** = **baseN**, and **singleCoreK** = **baseK**.
    -   The [IBSHARE](matmul_usage.md#table1188045714378) parameter in the **MatmulType** type information of matrices A and B must be **true**.

## Examples

The default template strategy **MatmulPolicy** is the default value of the template parameter. This section mainly describes how to use **TrianUpperMatmulPolicy** (upper triangular template strategy) and **TrianLowerMatmulPolicy** (lower triangular template strategy).

-   Upper triangular template strategy usage example

    ```cpp
    #include "lib/matmul_intf.h"

    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    // Pass TrianUpperMatmulPolicy when defining Matmul.
    AscendC::Matmul<aType, bType, cType, biasType, CFG_NORM, MatmulCallBackFunc<nullptr, nullptr, nullptr>, AscendC::Impl::Detail::TrianUpperMatmulPolicy> mm;

    // Perform regular Matmul computation and finally output the result in upper triangular form.
    TPipe pipe;
    TCubeTiling tiling;
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
    mm.SetTensorA(gmA, isTransposeA);
    mm.SetTensorB(gmB, isTransposeB);
    if (tiling.isBias) {
        mm.SetBias(gmBias);
    }
    mm.IterateAll(gmC);
    mm.End();
    ```

-   Lower triangular template strategy usage example

    ```cpp
    #include "lib/matmul_intf.h"

    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    // Pass TrianLowerMatmulPolicy when defining Matmul.
    AscendC::Matmul<aType, bType, cType, biasType, CFG_NORM, MatmulCallBackFunc<nullptr, nullptr, nullptr>, AscendC::Impl::Detail::TrianLowerMatmulPolicy> mm;

    // Perform regular Matmul computation and finally output the result in lower triangular form.
    TPipe pipe;
    TCubeTiling tiling;
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
    mm.SetTensorA(gmA, isTransposeA);
    mm.SetTensorB(gmB, isTransposeB);
    if (tiling.isBias) {
        mm.SetBias(gmBias);
    }
    mm.IterateAll(gmC);
    mm.End();
    ```

-   NBuffer33 template strategy usage example

    ```cpp
    #define ASCENDC_CUBE_ONLY
    #include "lib/matmul_intf.h"

    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    // Pass NBuffer33MatmulPolicy when defining Matmul.

    AscendC::Matmul<aType, bType, cType, biasType, CFG_MDL, MatmulCallBackFunc<nullptr, nullptr, nullptr>, AscendC::Impl::Detail::NBuffer33MatmulPolicy> mm;

    // Use the NBuffer33 logic to perform Matmul computation and output the result.
    TPipe pipe;
    TCubeTiling tiling;
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
    mm.SetTensorA(gmA, isTransposeA);
    mm.SetTensorB(gmB, isTransposeB);
    if (tiling.isBias) {
        mm.SetBias(gmBias);
    }
    mm.IterateAll(gmC);
    mm.End();
    ```

-   MxMatmul template strategy usage example

    ```cpp
    #include "lib/matmul_intf.h"
    typedef MatmulTypeWithScale<AscendC::TPosition::GM, AscendC::TPosition::GM, CubeFormat::ND, AType, isTransposeA> aType;
    typedef MatmulTypeWithScale<AscendC::TPosition::GM, AscendC::TPosition::GM, CubeFormat::ND, BType, isTransposeB> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    // Pass MatmulWithScalePolicy when defining Matmul.
    AscendC::Matmul<aType, bType, cType, biasType, CFG_NORM, MatmulCallBackFunc<nullptr, nullptr, nullptr>, AscendC::Impl::Detail::MatmulWithScalePolicy> mm;

    // MxMatmul computation logic, and output the result.
    TPipe pipe;
    TCubeTiling tiling;
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
    mm.SetTensorA(gmA, isTransposeA);
    mm.SetTensorB(gmB, isTransposeB);
    mm.SetTensorScaleA(gm_scaleA, isTransposeScaleA);
    mm.SetTensorScaleB(gm_scaleB, isTransposeScaleB);
    if (tiling.isBias) {
        mm.SetBias(gmBias);
    }
    mm.IterateAll(gmC);
    mm.End();
    ```

-   SplitM template strategy usage example

    ```cpp
    #include "lib/matmul_intf.h"

    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half, LayoutMode::NONE, true> aType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half, LayoutMode::NONE, true> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::VECCALC, CubeFormat::ND, float> cType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    // Pass SplitMMatmulPolicy when defining Matmul.
    AscendC::Matmul<aType, bType, cType, biasType, CFG_NORM, MatmulCallBackFunc<nullptr, nullptr, nullptr>, AscendC::Impl::Detail::SplitMMatmulPolicy> mm;
    // Matmul computation.
    TPipe pipe;
    TCubeTiling tiling;
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
    mm.SetTensorA(gmA, isTransposeA);
    mm.SetTensorB(gmB, isTransposeB);
    if (tiling.isBias) {
        mm.SetBias(gmBias);
    }

    // After calling the GetTensorC API, split the computation result of one Matmul Iterate into two parts and move them to the Unified Buffer of two AIV cores.
    pipe.InitBuffer(resultCMatrix, 1, tiling.M * tiling.N * sizeof(C_T));
    mm.template Iterate<false>();
    bufferC = resultCMatrix.AllocTensor<C_T>();
    uint16_t nIter_ = Ceil(tiling.singleCoreN, tiling.baseN);
    uint16_t mIter_ = Ceil(tiling.singleCoreM, tiling.baseM);
    uint16_t mnIter_ = nIter_ * mIter_;
    for (int i = 0; i < mnIter_; i++) {
         mm.template GetTensorC<false>(bufferC, false, false);  // false // kfc vec0 iterate
         PipeBarrier<PIPE_ALL>();
    }
    mm.End();
    resultCMatrix.EnQue(bufferC);
    bufferC = resultCMatrix.DeQue<C_T>();

    uint16_t baseOffset = tiling.M / 2 * tiling.N;
    uint16_t stride = tiling.M / 2 * tiling.N * sizeof(C_T) / 32;  // 32B
    const uint16_t blockCount = tiling.M / tiling.M;
    if (GetSubBlockIdxImpl() == 0) {
        DataCopy(gmC, bufferC, {blockCount, stride, stride, stride});
    } else {
        DataCopy(gmC[baseOffset], bufferC, {blockCount, stride, stride, stride});
    }
    ```

-   SplitN template strategy usage example

    ```cpp
    #include "lib/matmul_intf.h"

    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half, LayoutMode::NONE, true> aType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half, LayoutMode::NONE, true> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::VECCALC, CubeFormat::ND, float> cType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    // Pass SplitNMatmulPolicy when defining Matmul.
    AscendC::Matmul<aType, bType, cType, biasType, CFG_NORM, MatmulCallBackFunc<nullptr, nullptr, nullptr>, AscendC::Impl::Detail::SplitNMatmulPolicy> mm;
    // Perform Matmul computation.
    TPipe pipe;
    TCubeTiling tiling;
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
    mm.SetTensorA(gmA, isTransposeA);
    mm.SetTensorB(gmB, isTransposeB);
    if (tiling.isBias) {
        mm.SetBias(gmBias);
    }

    // After the GetTensorC API is called, split the computation result of one Matmul Iterate into two parts and move them to the Unified Buffers of two AIV cores.
    pipe.InitBuffer(resultCMatrix, 1, tiling.M * tiling.N * sizeof(C_T));
    mm.template Iterate<false>();
    bufferC = resultCMatrix.AllocTensor<C_T>();
    uint16_t nIter_ = Ceil(tiling.singleCoreN, tiling.baseN);
    uint16_t mIter_ = Ceil(tiling.singleCoreM, tiling.baseM);
    uint16_t mnIter_ = nIter_ * mIter_;
    for (int i = 0; i < mnIter_; i++) {
         mm.template GetTensorC<false>(bufferC, false, false);  // false // kfc vec0 iterate
         PipeBarrier<PIPE_ALL>();
    }
    mm.End();
    resultCMatrix.EnQue(bufferC);
    bufferC = resultCMatrix.DeQue<C_T>();

    uint16_t baseOffset = tiling.N / 2;
    uint16_t blockCount = tiling.M;
    uint16_t blockLen = (tiling.N / 2 * sizeof(C_T)) / 32;
    uint16_t srcStride = 0;
    uint16_t dstStride = (tiling.N / 2 * sizeof(C_T)) / 32;
    if (GetSubBlockIdxImpl() == 0) {
        DataCopy(gmC, bufferC, {blockCount, blockLen, srcStride, dstStride});
    } else {
        DataCopy(gmC[baseOffset], bufferC, {blockCount, blockLen, srcStride, dstStride});
    }
    ```
