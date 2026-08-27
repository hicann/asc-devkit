# MatmulPolicy

## 产品支持情况

### MatmulPolicy

<!-- npu="950" id5 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id5 -->
<!-- npu="A3" id6 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id6 -->
<!-- npu="910b" id7 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id7 -->
<!-- npu="310b" id8 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id8 -->
<!-- npu="310p" id9 -->
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id9 -->
<!-- npu="910" id10 -->
- Atlas 训练系列产品：不支持
<!-- end id10 -->
<!-- npu="x90" id1 -->
- Kirin X90：支持
<!-- end id1 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulPolicy_res.md#id2 -->

### TrianUpperMatmulPolicy/TrianLowerMatmulPolicy

<!-- npu="950" id11 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id11 -->
<!-- npu="A3" id12 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id12 -->
<!-- npu="910b" id13 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id13 -->
<!-- npu="310b" id14 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas 训练系列产品：不支持
<!-- end id16 -->
<!-- npu="x90" id2 -->
- Kirin X90：支持
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulPolicy_res.md#id3 -->

### NBuffer33MatmulPolicy

<!-- npu="950" id17 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id17 -->
<!-- npu="A3" id18 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id18 -->
<!-- npu="910b" id19 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id19 -->
<!-- npu="310b" id20 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id20 -->
<!-- npu="310p" id21 -->
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id21 -->
<!-- npu="910" id22 -->
- Atlas 训练系列产品：不支持
<!-- end id22 -->
<!-- npu="x90" id3 -->
- Kirin X90：支持
<!-- end id3 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulPolicy_res.md#id4 -->

### MatmulWithScalePolicy/SplitMMatmulPolicy/SplitNMatmulPolicy

<!-- npu="950" id23 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id23 -->
<!-- npu="A3" id24 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id24 -->
<!-- npu="910b" id25 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id25 -->
<!-- npu="310b" id26 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id26 -->
<!-- npu="310p" id27 -->
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id27 -->
<!-- npu="910" id28 -->
- Atlas 训练系列产品：不支持
<!-- end id28 -->
<!-- npu="x90" id4 -->
- Kirin X90：不支持
<!-- end id4 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulPolicy_res.md#id1 -->

## 功能说明

模板参数MatmulPolicy用于定义Matmul可拓展模块策略。目前支持设置以下几种Matmul内置模板策略。

-   MatmulPolicy（默认模板策略）

    启用Matmul API的默认实现策略。

-   TrianUpperMatmulPolicy（上三角模板策略）

    一次矩阵乘指令计算的结果为[baseM \* baseN](../Matmul_Tiling/TCubeTiling_struct.md#p17899165811566)大小的矩阵块，称该矩阵块为基本块。若Matmul结果矩阵C中的基本块位于下三角位置，则Matmul内部做数据计算和数据搬出时，将不对该基本块进行处理，最后得到的矩阵C为一个上三角矩阵。上三角模板策略如下图所示，图示中矩阵形状的相关大小为M=N=512，K=256，baseM=baseN=baseK=32。

    **图1**  上三角模板策略示意图  
    ![](../../../../figures/upper_triangle_strategy.png "上三角模板策略示意图")

-   TrianLowerMatmulPolicy（下三角模板策略）

    一次矩阵乘指令计算的结果为[baseM \* baseN](../Matmul_Tiling/TCubeTiling_struct.md#p17899165811566)大小的矩阵块，称该矩阵块为基本块。若Matmul结果矩阵C中的基本块位于上三角位置，则Matmul内部做数据计算和数据搬出时，将不对该基本块进行处理，最后得到的矩阵C为一个下三角矩阵。下三角模板策略如下图所示，图示中矩阵形状的相关大小为M=N=512，K=256，baseM=baseN=baseK=32。

    **图2**  下三角模板策略示意图  
    ![](../../../../figures/lower_triangle_strategy.png "下三角模板策略示意图")

-   NBuffer33MatmulPolicy（NBuffer33模板策略）
<a name="li194081238103913"></a>

    一次矩阵乘指令计算的结果为[baseM \* baseN](../Matmul_Tiling/TCubeTiling_struct.md#p17899165811566)大小的矩阵块，称该矩阵块为基本块。单核计算的A矩阵切分为3x3个基本块，该3x3个A矩阵的基本块全载和保持在L1 Buffer中，每次与3x1个B矩阵的基本块计算矩阵乘，同时DoubleBuffer并行搬入下次计算所需的3x1个B矩阵基本块，直到singleCoreN方向的矩阵乘计算完成。NBuffer33模板策略如下图所示，图中[singleCoreM、singleCoreN、singleCoreK](../Matmul_Tiling/TCubeTiling_struct.md#p11899125875617)表示单核内A、B矩阵的shape大小，单核计算的A矩阵切分为3x3个基本块，3x3个基本块全载在L1 Buffer上，这些基本块每次与B矩阵的3x1个基本块计算矩阵乘。

    **图3**  NBuffer33模板策略示意图  
    ![](../../../../figures/nbuffer33_strategy.png "NBuffer33模板策略示意图")

-   MatmulWithScalePolicy（MxMatmul模板策略）

    实现带有量化系数的矩阵乘法，即左矩阵和右矩阵均有对应的量化系数矩阵，左量化系数矩阵scaleA和右量化系数矩阵scaleB。MxMatmul场景中，左量化系数矩阵与左矩阵乘积，右量化系数矩阵与右矩阵乘积，对两个乘积的结果做矩阵乘法。

    **图4** MxMatmul模板策略示意图

    ![](../../../../figures/matmulmx_formula.png)

-   SplitMMatmulPolicy（SplitM模板策略）

    Matmul一次[Iterate](Iterate.md)的计算结果从L0C Buffer搬到Unified Buffer（UB）时，采用双输出模式，即在分离模式下，AIC核与AIV核的核数比为1：2时，在调用[GetTensorC](GetTensorC.md)接口后，Matmul一次Iterate的计算结果在矩阵的M方向一分为二，将被切分后的两块结果数据分别搬运到两个AIV核的UB。模板策略示意图如下所示。

    **图5**  SplitM模板策略示意图  
    ![](../../../../figures/splitm_strategy.png "SplitM模板策略示意图")

-   SplitNMatmulPolicy（SplitN模板策略）

    Matmul一次[Iterate](Iterate.md)的计算结果从L0C Buffer搬到UB时，采用双输出模式，即在分离模式下，AIC核与AIV核的核数比为1：2时，在调用[GetTensorC](GetTensorC.md)接口后，Matmul一次Iterate的计算结果在矩阵的N方向一分为二，将被切分后的两块结果数据分别搬运到两个AIV核的UB。模板策略示意图如下所示。

    **图6**  SplitN模板策略示意图  
    ![](../../../../figures/splitn_strategy.png "SplitN模板策略示意图")

## 约束说明

-   TrianUpperMatmulPolicy当前只支持[Norm模板](MatmulConfig.md#template-features)和[MDL模板](MatmulConfig.md#template-features)。
-   TrianLowerMatmulPolicy当前只支持[Norm模板](MatmulConfig.md#template-features)和[MDL模板](MatmulConfig.md#template-features)。
-   NBuffer33MatmulPolicy：
    -   当前只支持[MDL模板](MatmulConfig.md#template-features)。
    -   A矩阵、B矩阵的内存逻辑位置只支持TPosition::GM。
    -   暂不支持MIX模式（包含矩阵计算和矢量计算），仅支持纯Cube模式（只有矩阵计算）。
    -   只支持通过[IterateAll](IterateAll.md)接口获取Matmul的计算结果C矩阵。
    -   [stepM、stepKa、stepKb](../Matmul_Tiling/TCubeTiling_struct.md#p139009583566)小于等于3，且满足：stepKa=stepKb=ceil\([singleCoreK](../Matmul_Tiling/TCubeTiling_struct.md#p11899125875617)/baseK\)。
    -   A矩阵全载的基本块大小与B矩阵载入的基本块大小之和不超过L1 Buffer大小。
    -   在使用[GetTiling](../Matmul_Tiling/GetTiling.md)接口生成Tiling参数前，必须通过[SetMatmulConfigParams](../Matmul_Tiling/SetMatmulConfigParams.md)接口将scheduleTypeIn参数设置为ScheduleType::N\_BUFFER\_33，以启用NBuffer33模板策略的Tiling生成逻辑。

-   MatmulWithScalePolicy：
    -   当前只支持[Norm模板](MatmulConfig.md)和[MDL模板](MatmulConfig.md)。

-   SplitMMatmulPolicy：
    -   只支持C矩阵输出到UB。
    -   A矩阵、B矩阵类型信息MatmulType中的参数[IBSHARE](Matmul_usage.md)必须为true。

-   SplitNMatmulPolicy：
    -   只支持C矩阵输出到UB。
    -   baseN必须满足是16的倍数。
    -   [Tiling参数](../Matmul_Tiling/TCubeTiling_struct.md#tcubetiling-struct)必须满足：singleCoreM = baseM，singleCoreN = baseN，singleCoreK = baseK。
    -   A矩阵、B矩阵类型信息MatmulType中的参数[IBSHARE](Matmul_usage.md)必须为true。

## 调用示例

默认模板策略MatmulPolicy为模板参数的默认值，下面主要介绍TrianUpperMatmulPolicy（上三角模板策略）和TrianLowerMatmulPolicy（下三角模板策略）的使用方式。

-   上三角模板策略使用示例

    ```cpp
    #include "lib/matmul_intf.h"

    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    // Matmul定义时传入TrianUpperMatmulPolicy
    AscendC::Matmul<
        aType, bType, cType, biasType, CFG_NORM, MatmulCallBackFunc<nullptr, nullptr, nullptr>,
        AscendC::Impl::Detail::TrianUpperMatmulPolicy>
        mm;

    // 常规Matmul计算，最后输出上三角形式的结果
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

-   下三角模板策略使用示例

    ```cpp
    #include "lib/matmul_intf.h"

    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    // Matmul定义时传入TrianLowerMatmulPolicy
    AscendC::Matmul<
        aType, bType, cType, biasType, CFG_NORM, MatmulCallBackFunc<nullptr, nullptr, nullptr>,
        AscendC::Impl::Detail::TrianLowerMatmulPolicy>
        mm;

    // 常规Matmul计算，最后输出下三角形式的结果
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

-   NBuffer33模板策略使用示例

    ```cpp
    #define ASCENDC_CUBE_ONLY
    #include "lib/matmul_intf.h"

    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    // Matmul定义时传入NBuffer33MatmulPolicy

    AscendC::Matmul<
        aType, bType, cType, biasType, CFG_MDL, MatmulCallBackFunc<nullptr, nullptr, nullptr>,
        AscendC::Impl::Detail::NBuffer33MatmulPolicy>
        mm;

    // 使用NBuffer33逻辑进行Matmul计算，最后输出结果
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

-   MxMatmul模板策略使用示例

    ```cpp
    #include "lib/matmul_intf.h"
    typedef MatmulTypeWithScale<AscendC::TPosition::GM, AscendC::TPosition::GM, CubeFormat::ND, AType, isTransposeA> aType;
    typedef MatmulTypeWithScale<AscendC::TPosition::GM, AscendC::TPosition::GM, CubeFormat::ND, BType, isTransposeB> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    // Matmul定义时传入MatmulWithScalePolicy
    AscendC::Matmul<
        aType, bType, cType, biasType, CFG_NORM, MatmulCallBackFunc<nullptr, nullptr, nullptr>,
        AscendC::Impl::Detail::MatmulWithScalePolicy>
        mm;

    // MxMatmul计算逻辑，最后输出结果
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

-   SplitM模板策略使用示例

    ```cpp
    #include "lib/matmul_intf.h"

    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half, LayoutMode::NONE, true> aType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half, LayoutMode::NONE, true> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::VECCALC, CubeFormat::ND, float> cType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    // Matmul定义时传入SplitMMatmulPolicy
    AscendC::Matmul<
        aType, bType, cType, biasType, CFG_NORM, MatmulCallBackFunc<nullptr, nullptr, nullptr>,
        AscendC::Impl::Detail::SplitMMatmulPolicy>
        mm;
    // Matmul计算
    TPipe pipe;
    TCubeTiling tiling;
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
    mm.SetTensorA(gmA, isTransposeA);
    mm.SetTensorB(gmB, isTransposeB);
    if (tiling.isBias) {
        mm.SetBias(gmBias);
    }

    // 调用GetTensorC接口后，将Matmul一次Iterate的计算结果一分为二，搬运到两个AIV核的UB。
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

-   SplitN模板策略使用示例

    ```cpp
    #include "lib/matmul_intf.h"

    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half, LayoutMode::NONE, true> aType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half, LayoutMode::NONE, true> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::VECCALC, CubeFormat::ND, float> cType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    // Matmul定义时传入SplitNMatmulPolicy
    AscendC::Matmul<
        aType, bType, cType, biasType, CFG_NORM, MatmulCallBackFunc<nullptr, nullptr, nullptr>,
        AscendC::Impl::Detail::SplitNMatmulPolicy>
        mm;
    // Matmul计算
    TPipe pipe;
    TCubeTiling tiling;
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
    mm.SetTensorA(gmA, isTransposeA);
    mm.SetTensorB(gmB, isTransposeB);
    if (tiling.isBias) {
        mm.SetBias(gmBias);
    }

    // 调用GetTensorC接口后，将Matmul一次Iterate的计算结果一分为二，搬运到两个AIV核的UB。
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
