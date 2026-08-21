# IterateBatch

## 产品支持情况

<!-- npu="950" id3 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas 训练系列产品：不支持
<!-- end id8 -->
<!-- npu="x90" id1 -->
- Kirin X90：支持
<!-- end id1 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/IterateBatch_res.md#id1 -->

## 功能说明

单次Matmul计算处理的shape比较小时，由于每次计算均涉及到内部的通信，可能会影响性能，该接口提供批量处理Matmul的功能，调用一次IterateBatch，可以计算出多个singleCoreM \* singleCoreN大小的C矩阵。

在使用该接口前，需要了解一些必备的数据排布格式：

-   **通用数据格式（NORMAL）**：BMNK的数据排布格式；B：Batch，批处理的大小；M、N、K为矩阵乘\[M, K\]\*\[K, N\]的矩阵维度；其数据排布格式如下：

    ![](../../../../figures/250925154552953.png)

-   **BSH/SBH**：B：Batch，批处理的大小； S：sequence length，序列长度；H = N \* D，其中，N为head的数量，D为head的大小。Layout格式如下图所示：

    ![](../../../../figures/skip_write_mode.png)

    ![](../../../../figures/skip_write_mode_46.png)

-   **BSNGD**：为原始BSH shape做reshape后的shape，S和D为单Batch的矩阵乘的M轴（或N轴）和K轴，一个SD为一个batch的计算数据，Layout格式如下图所示：

    ![](../../../../figures/250925161748040.png)

-   **SBNGD**：为原始SBH shape做reshape后shape，S和D为矩阵乘的M轴（或N轴）和K轴，一个SD为一个Batch的计算数据，Layout格式如下图所示：

    ![](../../../../figures/SBNGD.png)

-   **BNGS1S2**：一般为前两种Layout进行矩阵乘的输出，S1S2数据连续存放，一个S1S2为一个Batch的计算数据，Layout格式如下图所示：

    ![](../../../../figures/BNGS1S2.png)

实例化Matmul时，需要通过MatmulType设置输入输出的Layout格式，当前支持4种Layout类型：BSNGD、SBNGD、BNGS1S2、NORMAL（BMNK的数据排布格式使用NORMAL表示）。

对于BSNGD、SBNGD、BNGS1S2 Layout格式，调用该接口之前需要在host Tiling实现中使用[SetALayout](../Matmul_Tiling/SetALayout.md)、[SetBLayout](../Matmul_Tiling/SetBLayout.md)、[SetCLayout](../Matmul_Tiling/SetCLayout.md)、[SetBatchNum](../Matmul_Tiling/SetBatchNum.md)设置A/B/C的Layout轴信息和最大BatchNum数；对于NORMAL  Layout格式则需使用[SetBatchInfoForNormal](../Matmul_Tiling/SetBatchInfoForNormal.md)设置A/B/C的M/N/K轴信息和A/B矩阵的BatchNum数。


<a name="row_width"></a>使用ND格式输入时，是否开启MatmulConfig中的intrinsicsCheck，取决于单个BMM内A或B矩阵的源矩阵内轴长度。该内轴可理解为源ND数据中一行的连续跨度，即一次ND2NZ搬运所使用的源行宽。不同Layout下的取值规则如下：NORMAL Layout下，按普通矩阵内轴判断；BNGS1S2 Layout下，按源ND矩阵一行的实际跨度判断；BSNGD Layout下，A和B矩阵源矩阵内轴分别为ALayoutInfoN \* ALayoutInfoG \* ALayoutInfoD、BLayoutInfoN \* BLayoutInfoG \* BLayoutInfoD；SBNGD Layout下，A和B矩阵源矩阵内轴分别为ALayoutInfoB \* ALayoutInfoN \* ALayoutInfoG \* ALayoutInfoD、BLayoutInfoB \* BLayoutInfoN \* BLayoutInfoG \* BLayoutInfoD。

单个矩阵乘迭代顺序可通过tiling参数iterateOrder调整。

## 函数原型

-   mix模式
    -   输出至GM

        ```
        template <bool sync = true, bool waitIterateBatch = false>
        __aicore__ inline void IterateBatch(const GlobalTensor<DstT>& gm, uint32_t batchA, uint32_t batchB, bool enSequentialWrite, const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0, const bool enPartialSum = false, const uint8_t enAtomic = 0)
        ```

    -   输出至Unified Buffer（UB，VECIN）

        ```
        template <bool sync = true>
        __aicore__ inline void IterateBatch(const LocalTensor<DstT>& ubCmatrix, uint32_t batchA, uint32_t batchB, bool enSequentialWrite, const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0, const bool enPartialSum = false, const uint8_t enAtomic = 0)
        ```

-   纯cube模式

    使用前需先调用[SetBatchNum](SetBatchNum.md)接口设置batchA和batchB的大小。

    -   输出至GM

        ```
        __aicore__ inline void IterateBatch(const GlobalTensor<DstT>& gm, bool enPartialSum, uint8_t enAtomic, bool enSequentialWrite, const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0)
        ```

    -   输出至UB（VECIN）

        ```
        __aicore__ inline void IterateBatch(const LocalTensor<DstT>& ubCmatrix, bool enPartialSum, uint8_t enAtomic, bool enSequentialWrite, const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0)
        ```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| sync | 获取C矩阵过程分为同步和异步两种模式：<br>同步：需要同步等待IterateBatch执行结束。<br>异步：不需要同步等待IterateBatch执行结束。<br><br>通过该参数设置同步或者异步模式：同步模式设置为true；异步模式设置为false。默认为同步模式。异步场景需要配合[WaitIterateBatch](WaitIterateBatch.md)接口使用。 |
| waitIterateBatch | 是否需要通过[WaitIterateBatch](WaitIterateBatch.md)接口等待IterateBatch执行结束，仅在异步场景下使用。默认为false。<br><br>true：需要通过WaitIterateBatch接口等待IterateBatch执行结束。<br><br>false：不需要通过WaitIterateBatch接口等待IterateBatch执行结束，开发者自行处理等待IterateBatch执行结束的过程。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| gm | 输出 | C矩阵。类型为[GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md)。不同型号支持的数据类型请参考[支持的数据类型](#li12616155731722)。 |
| ubCmatrix | 输出 | C矩阵。类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)。不同型号支持的数据类型请参考[支持的数据类型](#li12616155731722)。 |
| batchA | 输入 | 左矩阵的batch数。 |
| batchB | 输入 | 右矩阵的batch数。在batchA/batchB不相同的情况下，默认做broadcast操作。<br><br>多batch计算支持在G轴上做输入broadcast和输出reduce，左矩阵、右矩阵G轴维度必须是整数倍的关系。 |
| enSequentialWrite | 输入 | 输出是否[连续存放](GetTensorC.md#fig580415103338)数据，即是否开启连续写模式（连续写，写入[baseM, baseN]；非连续写，写入[singleCoreM, singleCoreN]中对应的位置）。<br>左右矩阵和输出矩阵的存储位置为UB，则enSequentialWrite参数应配置为true；<br>输出矩阵的存储位置为GM，则enSequentialWrite参数应配置为false。 |
| matrixStrideA | 输入 | A矩阵源操作数相邻nd矩阵起始地址间的偏移，单位是元素，默认值是0。 |
| matrixStrideB | 输入 | B矩阵源操作数相邻nd矩阵起始地址间的偏移，单位是元素，默认值是0。 |
| matrixStrideC | 输入 | 该参数预留，保持默认值0即可。 |
| enPartialSum | 输入 | 是否将矩阵乘的结果累加于现有的L0C Buffer（CO1）数据，默认值为false。在L0C累加时，只支持A矩阵和B矩阵相乘的输出C矩阵规格为singleM==baseM &&singleN==baseN。 |
| enAtomic | 输入 | 是否开启Atomic操作，默认值为0。<br><br>参数取值：<br><br>0：不开启Atomic操作<br><br>1：开启AtomicAdd累加操作<br><br>2：开启AtomicMax求最大值操作<br><br>3：开启AtomicMin求最小值操作 |

## 返回值说明

无

## 约束说明

-   该接口只支持Norm模板，即BatchMatmul只支持Norm模板。
-   使用该接口时，A、B矩阵的Layout格式必须相同。
-   对于BSNGD、SBNGD、BNGS1S2 Layout格式，输入A、B矩阵按分形对齐后的多Batch数据总和应小于L1 Buffer的大小；对于NORMAL  Layout格式没有这种限制，但需通过MatmulConfig配置输入A、B矩阵多Batch数据大小与L1 Buffer的大小关系；
-   对于BSNGD、SBNGD、BNGS1S2 Layout格式，称左矩阵、右矩阵的G轴分别为ALayoutInfoG、BLayoutInfoG，则ALayoutInfoG / batchA = BLayoutInfoG / batchB；对于NORMAL  Layout格式，batchA、batchB必须满足倍数关系。
-   如果接口输出到UB上，输出C矩阵大小BaseM\*BaseN应小于分配的UB内存大小。
-   如果接口输出到UB上，且单核计算的N方向大小singleCoreN非32字节对齐，C矩阵的CubeFormat仅支持ND\_ALIGN格式，输出C矩阵片时，自动将singleCoreN方向上的数据补齐至32字节。
-   对于BSNGD、SBNGD Layout格式，输入输出只支持ND格式数据。对于BNGS1S2、NORMAL  Layout格式，输入支持ND/NZ格式数据。
-   对于BSNGD、SBNGD Layout格式，不支持连续写模式。
-   该接口不支持量化模式，即不支持SetQuantScalar、SetQuantVector接口。
-   BSNGD场景，不支持一次计算多行SD，需要算子程序中循环计算，即\(ALayoutInfoN \* ALayoutInfoG\) / batchA、\(BLayoutInfoN \* BLayoutInfoG\) / batchB均为整数。
-   **异步模式**不支持IterateBatch搬运到UB上。
-   当开启MixDualMaster（双主模式）场景时，即模板参数[enableMixDualMaster](MatmulConfig.md#matmulconfig-params)设置为true，不支持使用该接口。
<!-- npu="950" id9 -->
-   Ascend 950PR/Ascend 950DT上，不支持A、B矩阵内存逻辑位置为TPosition::VECOUT的输入；输出至VECIN的函数原型当前只支持BSNGD Layout格式。
<!-- end id9 -->
<!-- npu="310p" id10 -->
-   Atlas 推理系列产品AI Core上，只支持NORMAL  Layout格式。
-   Atlas 推理系列产品AI Core上，不支持A、B矩阵内存逻辑位置为TPosition::TSCM的输入。
<!-- end id10 -->
<!-- npu="950" id11 -->
-   Ascend 950PR/Ascend 950DT上，A、B矩阵内存逻辑位置为TPosition::TSCM时，只支持NORMAL  Layout格式。
<!-- end id11 -->
<!-- npu="310p" id12 -->
-   Atlas 推理系列产品AI Core上，Bias不支持复用，Bias的shape大小必须为Batch \* N。
<!-- end id12 -->
-   使用该接口时，A矩阵、B矩阵不支持int4b\_t类型的输入，即BatchMatmul不支持int4b\_t类型的矩阵输入。
<!-- npu="950,A3,910b,310p,x90" id17 -->
-   支持的数据类型<a id="li12616155731722"></a>

    <!-- npu="950" id13 -->
    Ascend 950PR/Ascend 950DT，支持的数据类型为：half、bfloat16_t、int32_t、float。
    <!-- end id13 -->

    <!-- npu="A3" id14 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：half、bfloat16_t、int32_t、float。
    <!-- end id14 -->

    <!-- npu="910b" id15 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：half、bfloat16_t、int32_t、float。
    <!-- end id15 -->

    <!-- npu="310p" id16 -->
    Atlas 推理系列产品AI Core，支持的数据类型为：half、bfloat16_t、int32_t、float。
    <!-- end id16 -->

    <!-- npu="x90" id2 -->
    Kirin X90，支持的数据类型为：half、int32_t。
    <!-- end id2 -->
<!-- end id17 -->

<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/IterateBatch_res.md#id2 -->

## 调用示例

-   纯cube模式的函数调用示例请参考[调用示例](SetBatchNum.md#调用示例)。
-   该示例完成aGM、bGM矩阵乘，结果保存到cGm上，其中aGM、bGM、cGM数据的layout格式均为NORMAL，左矩阵每次计算batchA个MK数据，右矩阵每次计算batchB个KN数据。

    ```
    // 定义matmul type
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half, false, LayoutMode::NORMAL> aType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half, true, LayoutMode::NORMAL> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float, false, LayoutMode::NORMAL> cType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    // 创建Matmul实例
    constexpr static MatmulConfig MM_CFG = GetNormalConfig(false, false, false, BatchMode::BATCH_LESS_THAN_L1);
    AscendC::Matmul<aType, bType, cType, biasType, MM_CFG> mm1;
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm1);
    mm1.Init(&tiling);
    mm1.SetTensorA(gm_a, isTransposeAIn);
    mm1.SetTensorB(gm_b, isTransposeBIn);
    if (tiling.isBias) {
        mm1.SetBias(gm_bias);
    }
    // 多batch Matmul计算
    mm1.IterateBatch(gm_c, batchA, batchB, false);
    ```

-   该示例完成aGM、bGM矩阵乘，结果保存到cGm上，其中aGM数据的layout格式为BSNGD，bGM数据的layout格式为BSNGD，cGM的layout格式为BNGS1S2，左矩阵每次计算batchA个SD数据，右矩阵每次计算batchB个SD数据。

    ```
    // 定义matmul type
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half, false, LayoutMode::BSNGD> aType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half, true, LayoutMode::BSNGD> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float, false, LayoutMode::BNGS1S2> cType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    // 创建Matmul实例
    AscendC::Matmul<aType, bType, cType, biasType> mm1;
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm1);
    mm1.Init(&tiling);
    int batchC = batchA > batchB ? batchA : batchB;
    int g_lay = tiling.ALayoutInfoG > tiling.BLayoutInfoG ? tiling.ALayoutInfoG : tiling.BLayoutInfoG;
    // 计算需要多Batch计算循环次数
    int for_exent = tiling.ALayoutInfoB * tiling.ALayoutInfoN * g_lay / tiling.BatchNum;
    for (int i = 0; i < for_exent; ++i) {
        // 计算每次多batch计算A/B矩阵的起始地址
        int batchOffsetA = i * tiling.ALayoutInfoD * batchA;
        int batchOffsetB = i * tiling.BLayoutInfoD * batchB;
        mm1.SetTensorA(gm_a[batchOffsetA], isTransposeAIn);
        mm1.SetTensorB(gm_b[batchOffsetB], isTransposeBIn);
        int idx_c = i * batchC;
        if (tiling.CLayoutInfoG == 1 && (tiling.BLayoutInfoG != 1 || tiling.ALayoutInfoG != 1)) {
            idx_c = idx_c / (tiling.BLayoutInfoG > tiling.ALayoutInfoG ? tiling.BLayoutInfoG : tiling.ALayoutInfoG);
        }
        if (tiling.isBias) {
            int batchOffsetBias = idx_c * tiling.CLayoutInfoS2;
            mm1.SetBias(gm_bias[batchOffsetBias]);
        }
        int batchOffsetC = idx_c * tiling.CLayoutInfoS2;
        if (C_TYPE::layout == LayoutMode::BNGS1S2) {
            batchOffsetC = idx_c * tiling.CLayoutInfoS2 * tiling.CLayoutInfoS1;
        }
        // 多batch Matmul计算
        mm1.IterateBatch(gm_c[batchOffsetC], batchA, batchB, false);
    }
    ```
