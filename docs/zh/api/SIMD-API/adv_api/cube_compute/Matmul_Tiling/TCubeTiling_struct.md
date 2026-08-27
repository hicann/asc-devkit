# TCubeTiling结构体

TCubeTiling结构体包含Matmul Tiling切分算法的相关参数，被传递给Matmul核函数（Kernel）侧，用于Matmul的切块、搬运和计算过程等。TCubeTiling结构体的参数说明见[表1](#tcubetiling-struct)。

**表1**  TCubeTiling结构说明

<a id="tcubetiling-struct"></a>
| 参数名称 | 数据类型 | 说明 |
| --- | --- | --- |
| usedCoreNum | int | 使用的AI处理器核数，请根据实际情况设置。取值范围为：[1, AI处理器最大核数]。该参数与shape相关参数的关系为：usedCoreNum = (M / singleCoreM) * (N / singleCoreN)。 |
| M, N, Ka, Kb | int | A、B、C矩阵原始输入的shape大小，以元素为单位。M, Ka为A矩阵原始输入的Shape，Kb, N为B矩阵原始输入的Shape。<br>大小约束：<br><!-- npu="950" id1 -->除Ascend 950PR/Ascend 950DT外，<!-- end id1 -->下述场景需要开启MatmulConfig中的[intrinsicsCheck](../Matmul_Kernel/MatmulConfig.md#matmulconfig-params)参数，以完成Matmul计算。<br>若A矩阵为ND格式，不进行转置，Ka大于65535时需要开启intrinsicsCheck参数，M无大小限制；进行转置，M大于65535时需要开启intrinsicsCheck参数，Ka无大小限制。<br>若B矩阵为ND格式，不进行转置，N大于65535时需要开启intrinsicsCheck参数，Kb无大小限制；进行转置，Kb大于65535时需要开启intrinsicsCheck参数，N无大小限制。<br><br>对齐约束：<br>若A矩阵以NZ格式输入，则M需要以16个元素对齐，Ka需要以C0_size对齐；若B矩阵以NZ格式输入，Kb需要以C0_size对齐，N需要以16个元素对齐。<br>若A、B矩阵为ND格式，无对齐约束。<br><br>注意：NZ格式的输入，float数据类型的C0_size为8，half/bfloat16_t数据类型的C0_size为16，int8_t/fp8_e4m3fn_t/fp8_e5m2_t/hifloat8_t数据类型的C0_size为32，int4b_t/fp4x2_e2m1_t/fp4x2_e1m2_t数据类型的C0_size为64。 |
| singleCoreM, singleCoreN, singleCoreK<a name="p11899125875617"></a> | int | A、B、C矩阵单核内shape大小，以元素为单位。该参数取值必须大于0。<br><br>singleCoreK = K，多核处理时不对K进行切分；singleCoreM <= M；singleCoreN <= N。<br><br>注意：若A矩阵以NZ格式输入，则singleCoreM需要以16个元素对齐，singleCoreK需要以C0_size对齐；若B矩阵以NZ格式输入，则singleCoreK需要以C0_size对齐，singleCoreN需要以16个元素对齐。 |
| baseM, baseN, baseK<a name="p17899165811566"></a> | int | A、B、C矩阵参与一次矩阵乘指令的shape大小，以元素为单位。<br><br>A、B、C矩阵参与一次矩阵乘的shape大小需要按分形对齐，其含义请参考[Mmad](../../../basic_api/cube_compute_ISASI/mmad_compute/Mmad.md)中的数据格式说明。<br><br>注意：该参数取值必须大于0。<!-- npu="950" id2 -->MxMatmul场景，baseK必须为64的倍数。<!-- end id2 --> |
| <a id="deptha1"></a>depthA1, depthB1 | int | depthA1、depthB1分别表示L1 Buffer（A1）、L1 Buffer（B1）中全载基本块的份数，depthA1为L1 Buffer（A1）中全载baseM * baseK的份数，depthB1为L1 Buffer（B1）中全载baseN * baseK的份数。<br><br>注意：该参数取值必须大于0。 |
| stepM， stepN，stepKa，stepKb<a name="p139009583566"></a> | int | stepM为左矩阵在L1 Buffer（A1）中缓存的buffer M方向上baseM的倍数。<br><br>stepN为右矩阵在L1 Buffer（B1）中缓存的buffer N方向上baseN的倍数。<br><br>stepKa为左矩阵在L1 Buffer（A1）中缓存的buffer Ka方向上baseK的倍数。<br><br>stepKb为右矩阵在L1 Buffer（B1）中缓存的buffer Kb方向上baseK的倍数。<br><br>注意：该参数取值必须大于0。 |
| isBias<a name="p2051215216314"></a> | int | 是否启用Bias，参数取值如下：<br>0：不启用Bias（默认值）。1：启用Bias。<br><br>注意：该参数不支持除上述外的其他取值，设置为其他值时参数行为未定义。 |
| transLength<a name="p1620315053211"></a> | int | max(A1Length, B1Length, C1Length, BiasLength)。其中，A1Length, B1Length, C1Length, BiasLength分别表示A/B/C/Bias矩阵在计算过程中需要临时占用的UB空间大小。 |
| iterateOrder | int | 一次Iterate计算出[baseM, baseN]大小的C矩阵分片，Iterate完成后，Matmul会自动偏移下一次Iterate输出的C矩阵位置，iterOrder表示自动偏移的顺序。参数取值如下：<br>0：先往M轴方向偏移再往N轴方向偏移。1：先往N轴方向偏移再往M轴方向偏移。<br><br>注意：该参数不支持除上述外的其他取值，设置为其他值时参数行为未定义。 |
| dbL0A, dbL0B,<br><br>dbL0C | int | MTE1是否开启double buffer。<br><br>dbL0A：左矩阵MTE1是否开启double buffer；dbL0B：右矩阵MTE1是否开启double buffer；dbL0C：MMAD是否开启double buffer。参数取值如下：<br>1：不开启double buffer。2：开启double buffer。<br><br>注意：该参数不支持除上述外的其他取值，设置为其他值时参数行为未定义。 |
| shareMode | int | 该参数预留，开发者无需关注。 |
| shareL1Size | int | 该参数预留，开发者无需关注。 |
| shareL0CSize | int | 该参数预留，开发者无需关注。 |
| shareUbSize | int | 该参数预留，开发者无需关注。 |
| batchM | int | 该参数预留，开发者无需关注。 |
| batchN | int | 该参数预留，开发者无需关注。 |
| singleBatchM | int | 该参数预留，开发者无需关注。 |
| singleBatchN | int | 该参数预留，开发者无需关注。 |
| mxTypePara | int | 组合参数，在MxMatmul场景使用，表示scaleA/scaleB载入L1的大小与A/B矩阵载入L1大小的倍数，具体如下：<br>0~6bit表示scaleA与A矩阵在K方向载入数据量的比例系数，scaleFactorKa，即scaleFactorKa=scaleA在K方向载入数据量/A矩阵在K方向载入数据量，数据范围为[1, 127]；<br>8~14bit表示scaleB与B矩阵在K方向载入数据量的比例系数，scaleFactorKb，即scaleFactorKb=scaleB在K方向载入数据量/B矩阵在K方向载入数据量，数据范围为[1, 127]；<br>16~22bit表示scaleA与A矩阵在M方向载入数据量的比例系数，scaleFactorM，即scaleFactorM=scaleA在M方向载入数据量/A矩阵在M方向载入数据量，数据范围为[1, 127]；<br>24~30bit表示scaleB与B矩阵在N方向载入数据量的比例系数，scaleFactorN，即scaleFactorN=scaleB在N方向载入数据量/B矩阵在N方向载入数据量，数据范围为[1, 127]；<br><br>注意：<br>对于scaleA矩阵，仅当Ka方向全载时，支持开启M方向的多倍载入。即baseK * stepKa * scaleFactorKa >= singleCoreK时，才能设置scaleFactorM为大于1的取值。<br>对于scaleB矩阵，仅当Kb方向全载时，支持开启N方向的多倍载入。即baseK * stepKb * scaleFactorKb >= singleCoreK时，才能设置scaleFactorN为大于1的取值。<br>scaleA、scaleB在M、N、K方向的载入数据量不能超过实际大小。<br>该参数仅在MDL模板生效。 |

<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/TCubeTiling_struct_res.md#id1 -->

多数情况下，用户通过调用[GetTiling](GetTiling.md)接口获取TCubeTiling结构体，具体流程请参考[使用说明](Matmul_Tiling_usage.md)。如果用户自定义TCubeTiling参数，各个参数的取值需要满足[表1](#tcubetiling-struct)和[表2](#table1275812182115)中的对应参数的约束条件。如果用户通过调用[GetTiling](GetTiling.md)接口获取TCubeTiling结构体后，需要修改调整Tiling，请参考如下TCubeTiling参数约束和性能调优推荐取值，进行参数的设置。

-   TCubeTiling参数约束

    一组合法的TCubeTiling参数需要同时满足[表2](#table1275812182115)中的所有约束条件。若Matmul对象的MatmulConfig模板为[MDL模板](../Matmul_Kernel/MatmulConfig.md#template-features)，除[表2](#table1275812182115)外，还同时需要满足[表3 MDL模板补充约束条件](#table1094616401179)。

    **表2**  TCubeTiling约束条件

    <a name="table1275812182115"></a>
    | 约束条件 | 说明 |
    | --- | --- |
    | usedCoreNum <= aiCoreCnt | 使用核数小于等于当前AI处理器的最大核数 |
    | baseM * baseK * sizeof(A_type) * dbL0A< l0a_size | A矩阵base块不超过l0a buffer大小 |
    | baseN * baseK * sizeof(B_type) * dbL0B < l0b_size | B矩阵base块不超过l0b buffer大小 |
    | baseM * baseN * sizeof(l0c_type) * dbL0C < l0c_size，其中l0c_type为int32_t或者float数据类型。 | C矩阵base块不超过l0c buffer大小 |
    | baseN * sizeof(Bias_type) < biasT_size | Bias的base块不超过BiasTable buffer大小 |
    | stepM * stepKa * db = depthA1<br>    <br>db这里表示为左矩阵MTE2是否开启double buffer，即L1是否开启double buffer，取值1（不开启double buffer）或2（开启double buffer） | depthA1的取值与stepM * stepKa  * db相同 |
    | stepN * stepKb * db = depthB1<br>    <br>db这里表示为右矩阵MTE2是否开启double buffer，即L1是否开启double buffer，取值1（不开启double buffer）或2（开启double buffer） | depthB1的取值与stepN * stepKb  * db相同 |
    | 对于A矩阵在L1上的缓存块大小AL1Size、B矩阵在L1上的缓存块大小BL1Size必须满足：<br>    无bias场景：AL1Size + BL1Size <= L1_size<br>    <br>    有bias场景：AL1Size + BL1Size + baseN * sizeof(Bias_type) <= L1_size<br>    <br>    其中，AL1Size、BL1Size的计算方式如下：<br>转置场景：<br>AL1Size = CeilDiv(baseM, C0_size) * baseK * depthA1 * sizeof(A_type)<br>   BL1Size = baseN * baseK * depthB1 * sizeof(B_type)<br><br>    非转置场景：<br>AL1Size = baseM * baseK * depthA1 * sizeof(A_type)<br> BL1Size = CeilDiv(baseN, C0_size)* baseK * depthB1 * sizeof(B_type) | A矩阵、B矩阵和Bias在L1缓存块满足L1 buffer大小限制；<br>    <br>注意：float数据类型的C0_size为8，half/bfloat16_t数据类型的C0_size为16，int8_t/fp8_e4m3fn_t/fp8_e5m2_t/hifloat8_t数据类型的C0_size为32，int4b_t/fp4x2_e2m1_t/fp4x2_e1m2_t数据类型的C0_size为64。 |
    | baseM * baseK, baseK * baseN和baseM * baseN按照NZ格式的分形对齐 | A矩阵、B矩阵、C矩阵的base块需要满足对齐约束：<br>    baseM和baseN需要以16个元素对齐；A矩阵非转置且B矩阵转置场景，baseK需要以C0_size对齐；其余场景（A矩阵转置或B矩阵非转置场景），baseK以16个元素对齐；<br>    <br>注意：float/int32_t数据类型的C0_size为8，half/bfloat16_t数据类型的C0_size为16，int8_t/fp8_e4m3fn_t/fp8_e5m2_t/hifloat8_t数据类型的C0_size为32，int4b_t/fp4x2_e2m1_t/fp4x2_e1m2_t数据类型的C0_size为64。 |
    | MxMatmul场景，如果A与B矩阵的位置同时为GM，对singleCoreK没有特殊限制，在这种情况下，若scaleA和scaleB的K方向大小（即Ceil(singleCoreK, 32)）为奇数，用户需自行在scaleA和scaleB的K方向补0至偶数；对于其它A、B矩阵逻辑位置的组合情况，即A与B矩阵的位置不同时为GM，singleCoreK以32个元素向上对齐后的数值必须是32的偶数倍；<br>    <br>输入数据类型为fp4x2_e2m1_t/fp4x2_e1m2_t时，内轴必须为偶数。 | scaleA/scaleB的数据类型是fp8_e8m0_t，K方向必须2字节连续，scaleA/scaleB的K方向是A/B矩阵K的1/32；A与B矩阵的位置不同时为GM时，singleCoreK以32个元素向上对齐后的数值，必须是32的偶数倍。<br>    <br>在MxMatmul场景，输入数据类型为fp4x2_e2m1_t/fp4x2_e1m2_t，计算时的最小单元为8字节，需要将2个4字节的元素拼成一个8字节进行计算，内轴必须为偶数。 |

    **表3**  MDL模板补充约束条件

    <a name="table1094616401179"></a>
    | 约束条件 | 说明 |
    | --- | --- |
    | Ka不全载时，即Ka / baseK > stepKa，stepM = 1 | K方向非全载时，M方向只能逐块搬运 |
    | Kb不全载时，即Kb / baseK > stepKb，stepN = 1 | K方向非全载时，N方向只能逐块搬运 |
    | kaStepIter_ % kbStepIter_ = 0或者kbStepIter_ % kaStepIter_ = 0<br>    <br>kaStepIter_ = CeilDiv(tiling_->singleCoreK_, tiling_->baseK * tiling_->stepKa)<br>    <br>kbStepIter_ = CeilDiv(tiling_->singleCoreK_, tiling_->baseK * tiling_->stepKb) | MDL模板K方向循环搬运要求Ka和Kb方向迭代次数为倍数关系<br>    <br>kaStepIter_：Ka方向循环搬运迭代次数<br>    <br>kbStepIter_：Kb方向循环搬运迭代次数 |

-   性能调优推荐取值

    根据Tiling调优经验，部分TCubeTiling参数值或取值方式推荐如下：

    -   base块推荐\(baseM, baseN, baseK\)：\(128, 256, 64\)
    -   dbL0A / dbL0B = 2
    -   depthA1 / \(stepM \* stepKa\) = 2
    -   depthB1 / \(stepN \* stepKb\) = 2
    -   优先设置参数stepKa/stepKb，使得K方向全载，再考虑M方向或N方向全载
