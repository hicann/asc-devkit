# MatmulPolicy

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:40:37.916Z pushedAt=2026-10-27T11:28:17.451Z nl=2 -->

## Applicable Products

### MatmulPolicy

<!-- npu="950" id5 -->
- Ascend 950PR&950DT products: Supported
<!-- end id5 -->
<!-- npu="A3" id6 -->
- Atlas A3 products: Supported
<!-- end id6 -->
<!-- npu="910b" id7 -->
- Atlas A2 products: Supported
<!-- end id7 -->
<!-- npu="310b" id8 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id8 -->
<!-- npu="310p" id9 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id9 -->
<!-- npu="910" id10 -->
- Atlas training products: Not supported
<!-- end id10 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulPolicy_res.md#id2 -->

### TrianUpperMatmulPolicy/TrianLowerMatmulPolicy

<!-- npu="950" id11 -->
- Ascend 950PR&950DT products: Supported
<!-- end id11 -->
<!-- npu="A3" id12 -->
- Atlas A3 products: Supported
<!-- end id12 -->
<!-- npu="910b" id13 -->
- Atlas A2 products: Supported
<!-- end id13 -->
<!-- npu="310b" id14 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas training products: Not supported
<!-- end id16 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulPolicy_res.md#id3 -->

### NBuffer33MatmulPolicy

<!-- npu="950" id17 -->
- Ascend 950PR&950DT products: Supported
<!-- end id17 -->
<!-- npu="A3" id18 -->
- Atlas A3 products: Supported
<!-- end id18 -->
<!-- npu="910b" id19 -->
- Atlas A2 products: Supported
<!-- end id19 -->
<!-- npu="310b" id20 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id20 -->
<!-- npu="310p" id21 -->
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id21 -->
<!-- npu="910" id22 -->
- Atlas training products: Not supported
<!-- end id22 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulPolicy_res.md#id4 -->

### MatmulWithScalePolicy/SplitMMatmulPolicy/SplitNMatmulPolicy

<!-- npu="950" id23 -->
- Ascend 950PR&950DT products: Supported
<!-- end id23 -->
<!-- npu="A3" id24 -->
- Atlas A3 products: Not supported
<!-- end id24 -->
<!-- npu="910b" id25 -->
- Atlas A2 products: Not supported
<!-- end id25 -->
<!-- npu="310b" id26 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id26 -->
<!-- npu="310p" id27 -->
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id27 -->
<!-- npu="910" id28 -->
- Atlas training products: Not supported
<!-- end id28 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulPolicy_res.md#id1 -->

## Description

The **MatmulPolicy** template parameter defines the Matmul extensible module strategy. Currently, the following built-in Matmul template strategies are supported.

-   **MatmulPolicy** (default template strategy)

    Enables the default implementation strategy of the Matmul API.

-   TrianUpperMatmulPolicy (upper triangular template strategy)

    The result computed by one cube multiplication instruction is a cube block of size [baseM \* baseN](../Matmul_Tiling/TCubeTiling_struct.md#p17899165811566), which is called a base block. If a base block in the Matmul result cube C is located in the lower triangular position, Matmul does not process this base block during internal data computation and data moving out, and the resulting cube C is an upper triangular cube. The upper triangular template strategy is shown in the following figure, where the relevant cube shape sizes are M=N=512, K=256, and baseM=baseN=baseK=32.

    **Figure 1**  Upper triangular template strategy schematic diagram  
    ![](../../../../figures/upper_triangle_strategy.png "Upper triangular template strategy schematic diagram")

-   TrianLowerMatmulPolicy (lower triangular template strategy)

    The result computed by one cube multiplication instruction is a cube block of size [baseM \* baseN](../Matmul_Tiling/TCubeTiling_struct.md#p17899165811566), which is called a base block. If a base block in the Matmul result cube C is located in the upper triangular position, Matmul does not process this base block during internal data computation and data moving out, and the resulting cube C is a lower triangular cube. The lower triangular template strategy is shown in the following figure, where the relevant cube shape sizes are M=N=512, K=256, and baseM=baseN=baseK=32.

    **Figure 2**  Lower triangular template strategy schematic diagram  
    ![](../../../../figures/lower_triangle_strategy.png "Lower triangular template strategy schematic diagram")

-   NBuffer33MatmulPolicy (NBuffer33 template strategy)
<a name="li194081238103913"></a>

    The result computed by one cube multiplication instruction is a cube block of size [baseM \* baseN](../Matmul_Tiling/TCubeTiling_struct.md#p17899165811566), which is called a base block. The A cube computed by a single core is divided into 3x3 base blocks. These 3x3 base blocks of the A cube are fully loaded into and kept in the L1 Buffer, and each time they are multiplied with 3x1 base blocks of the B cube. Meanwhile, DoubleBuffer moves in the 3x1 base blocks of the B cube required for the next computation in parallel, until the cube multiplication in the singleCoreN direction is completed. The NBuffer33 template strategy is shown in the following figure, where [singleCoreM, singleCoreN, and singleCoreK](../Matmul_Tiling/TCubeTiling_struct.md#p11899125875617) represent the shape sizes of the A and B cubes within a single core. The A cube computed by a single core is divided into 3x3 base blocks, which are fully loaded into the L1 Buffer, and these base blocks are multiplied with 3x1 base blocks of the B cube each time.

    **Figure 3**  NBuffer33 template strategy schematic diagram  
    ![](../../../../figures/nbuffer33_strategy.png "NBuffer33 template strategy schematic diagram")

-   MatmulWithScalePolicy (MxMatmul template strategy)

    Implements cube multiplication with quantization coefficients, that is, both the left cube and the right cube have corresponding quantization coefficient cubes: the left quantization coefficient cube **scaleA** and the right quantization coefficient cube **scaleB**. In the MxMatmul scenario, the left quantization coefficient cube is multiplied by the left cube, the right quantization coefficient cube is multiplied by the right cube, and cube multiplication is performed on the results of the two products.

    **Figure 4** MxMatmul template strategy schematic diagram

    ![](../../../../figures/matmulmx_formula.png)

-   SplitMMatmulPolicy (SplitM template strategy)

    When the computation result of one Matmul [Iterate](Iterate.md) is moved from the L0C Buffer to the Unified Buffer (UB), the dual-output mode is used. That is, in separated mode, when the core count ratio of AIC cores to AIV cores is 1:2, after the [GetTensorC](GetTensorC.md) API is called, the computation result of one Matmul Iterate is split into two parts in the M direction of the cube, and the two split result blocks are moved to the UBs of two AIV cores respectively. The template strategy schematic diagram is as follows.

    **Figure 5**  SplitM template strategy schematic diagram  
    ![](../../../../figures/splitm_strategy.png "SplitM template strategy schematic diagram")

-   SplitNMatmulPolicy (SplitN template strategy)

    When the computation result of one Matmul [Iterate](Iterate.md) is moved from the L0C Buffer to the UB, the dual-output mode is used. That is, in separated mode, when the core count ratio of AIC cores to AIV cores is 1:2, after the [GetTensorC](GetTensorC.md) API is called, the computation result of one Matmul Iterate is split into two parts in the N direction of the cube, and the two split result blocks are moved to the UBs of two AIV cores respectively. The template strategy schematic diagram is as follows.

    **Figure 6**  SplitN template strategy schematic diagram  
    ![](../../../../figures/splitn_strategy.png "SplitN template strategy schematic diagram")

## Constraints

-   **TrianUpperMatmulPolicy** currently supports only the [Norm template](MatmulConfig.md#template-features) and [MDL template](MatmulConfig.md#template-features).
-   **TrianLowerMatmulPolicy** currently supports only the [Norm template](MatmulConfig.md#template-features) and [MDL template](MatmulConfig.md#template-features).
-   **NBuffer33MatmulPolicy**:
    -   Currently supports only the [MDL template](MatmulConfig.md#template-features).
    -   The memory logical positions of cube A and cube B support only **TPosition::GM**.
    -   The MIX mode (including both cube computation and vector computation) is not supported. Only the pure Cube mode (cube computation only) is supported.
    -   The computation result cube C of Matmul can be obtained only through the [IterateAll](IterateAll.md) API.
    -   [stepM, stepKa, stepKb](../Matmul_Tiling/TCubeTiling_struct.md#p139009583566) must be less than or equal to 3, and must satisfy: stepKa=stepKb=ceil\([singleCoreK](../Matmul_Tiling/TCubeTiling_struct.md#p11899125875617)/baseK\).
    -   The sum of the base block size for fully loading cube A and the base block size for loading cube B must not exceed the L1 Buffer size.
    -   Before using the [GetTiling](../Matmul_Tiling/GetTiling.md) API to generate Tiling parameters, you must use the [SetMatmulConfigParams](../Matmul_Tiling/SetMatmulConfigParams.md) API to set the **scheduleTypeIn** parameter to ScheduleType::N\_BUFFER\_33 to enable the Tiling generation logic of the NBuffer33 template strategy.

-   **MatmulWithScalePolicy**:
    -   Currently only the [Norm template](MatmulConfig.md) and [MDL template](MatmulConfig.md) are supported.

-   **SplitMMatmulPolicy**:
    -   Only supports outputting the C cube to the UB.
    -   The [IBSHARE](Matmul_usage.md) parameter in the A cube and B cube type information **MatmulType** must be **true**.

-   **SplitNMatmulPolicy**:
    -   Only supports outputting the C cube to the UB.
    -   **baseN** must be a multiple of 16.
    -   The [Tiling parameters](../Matmul_Tiling/TCubeTiling_struct.md#tcubetiling-struct) must satisfy: **singleCoreM** = **baseM**, **singleCoreN** = **baseN**, **singleCoreK** = **baseK**.
    -   The [IBSHARE](Matmul_usage.md) parameter in the **MatmulType** type information of cube A and cube B must be true.

## Examples

The default template strategy **MatmulPolicy** is the default value of the template parameter. The following mainly describes how to use **TrianUpperMatmulPolicy** (upper triangular template strategy) and **TrianLowerMatmulPolicy** (lower triangular template strategy).

-   Upper triangular template strategy usage example

    ```cpp
    #include "lib/matmul_intf.h"

    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    // Pass TrianUpperMatmulPolicy when defining Matmul.
    AscendC::Matmul<
        aType, bType, cType, biasType, CFG_NORM, MatmulCallBackFunc<nullptr, nullptr, nullptr>,
        AscendC::Impl::Detail::TrianUpperMatmulPolicy>
        mm;

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
    AscendC::Matmul<
        aType, bType, cType, biasType, CFG_NORM, MatmulCallBackFunc<nullptr, nullptr, nullptr>,
        AscendC::Impl::Detail::TrianLowerMatmulPolicy>
        mm;

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

    AscendC::Matmul<
        aType, bType, cType, biasType, CFG_MDL, MatmulCallBackFunc<nullptr, nullptr, nullptr>,
        AscendC::Impl::Detail::NBuffer33MatmulPolicy>
        mm;

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
    AscendC::Matmul<
        aType, bType, cType, biasType, CFG_NORM, MatmulCallBackFunc<nullptr, nullptr, nullptr>,
        AscendC::Impl::Detail::MatmulWithScalePolicy>
        mm;

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
    AscendC::Matmul<
        aType, bType, cType, biasType, CFG_NORM, MatmulCallBackFunc<nullptr, nullptr, nullptr>,
        AscendC::Impl::Detail::SplitMMatmulPolicy>
        mm;
    // Matmul computation.
    TPipe pipe;
    TCubeTiling tiling;
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
    mm.SetTensorA(gmA, isTransposeA);
    mm.SetTensorB(gmB, isTransposeB);
    if (tiling.isBias) {
        mm.SetBias(gmBias);
    }

    // After calling the GetTensorC API, split the computation result of one Matmul Iterate into two parts and move them to the UB of two AIV cores.
    pipe.InitBuffer(resultCMatrix, 1, tiling.M* tiling.N * sizeof(C_T));
    mm.template Iterate<false>();
    bufferC = resultCMatrix.AllocTensor<C_T>();
    uint16_t nIter_ = Ceil(tiling.singleCoreN, tiling.baseN);
    uint16_t mIter_ = Ceil(tiling.singleCoreM, tiling.baseM);
    uint16_t mnIter_ = nIter_ * mIter_;
    for (int i = 0; i < mnIter_; i++) {
        mm.template GetTensorC<false>(bufferC, false, false); // false // kfc vec0 iterate
        PipeBarrier<PIPE_ALL>();
    }
    mm.End();
    resultCMatrix.EnQue(bufferC);
    bufferC = resultCMatrix.DeQue<C_T>();

    uint16_t baseOffset = tiling.M / 2 * tiling.N;
    uint16_t stride = tiling.M / 2 * tiling.N * sizeof(C_T) / 32; // 32B
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
    AscendC::Matmul<
        aType, bType, cType, biasType, CFG_NORM, MatmulCallBackFunc<nullptr, nullptr, nullptr>,
        AscendC::Impl::Detail::SplitNMatmulPolicy>
        mm;
    // Perform Matmul computation.
    TPipe pipe;
    TCubeTiling tiling;
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
    mm.SetTensorA(gmA, isTransposeA);
    mm.SetTensorB(gmB, isTransposeB);
    if (tiling.isBias) {
        mm.SetBias(gmBias);
    }

    // After calling the GetTensorC API, split the computation result of one Matmul Iterate into two parts and move them to the UB of two AIV cores.
    pipe.InitBuffer(resultCMatrix, 1, tiling.M* tiling.N * sizeof(C_T));
    mm.template Iterate<false>();
    bufferC = resultCMatrix.AllocTensor<C_T>();
    uint16_t nIter_ = Ceil(tiling.singleCoreN, tiling.baseN);
    uint16_t mIter_ = Ceil(tiling.singleCoreM, tiling.baseM);
    uint16_t mnIter_ = nIter_ * mIter_;
    for (int i = 0; i < mnIter_; i++) {
        mm.template GetTensorC<false>(bufferC, false, false); // false // kfc vec0 iterate
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
