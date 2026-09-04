# GetMMConfig

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
<!-- npu="9030" id2 -->
- Kirin 9030：不支持
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetMMConfig_res.md#id1 -->

## 功能说明

灵活的自定义Matmul模板参数配置。通过设置[MatmulConfigMode](#table17837129144319)、[MatmulShapeParams](#table16317184295116)、[MatmulQuantParams](#table8313111211573)、[MatmulBatchParams](#table15129204644)、[MatmulFuncParams](#table66217141862)，获取自定义的[MatmulConfig](MatmulConfig.md#matmulconfig-params)。

MatmulConfigMode指定了获取并要修改的MatmulConfig模板，各模板介绍请参考[模板特性](MatmulConfig.md#template-features)；用户根据使用需求通过设置可变参数，即一个或多个任意顺序的MatmulShapeParams、MatmulQuantParams、MatmulBatchParams、MatmulFuncParams，修改该MatmulConfig模板的相应参数配置。相比[GetNormalConfig](GetNormalConfig.md)、[GetMDLConfig](GetMDLConfig.md)等获取模板的接口，该接口提供了更灵活的自定义Matmul模板参数的配置方式。

## 函数原型

```
template <MatmulConfigMode configMode, typename... ArgTypes>
__aicore__ inline constexpr MatmulConfig GetMMConfig(ArgTypes&&... args)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| configMode | 获取的MatmulConfig模板。 |
| ArgTypes | 可变模板参数。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| args | 输入 | 可变参数，任意顺序传入需要设置的MatmulShapeParams、MatmulQuantParams、MatmulBatchParams、MatmulFuncParams中的一个或多个。 |

**表3**  MatmulConfigMode参数说明

<a name="table17837129144319"></a>

| 参数 | 说明 |
| --- | --- |
| CONFIG_NORM | 表示设置MatmulConfig默认值为Norm模板 |
| CONFIG_MDL | 表示设置MatmulConfig默认值为MDL模板 |
| CONFIG_SPECIALMDL | 表示设置MatmulConfig默认值为SpecialMDL模板 |
| CONFIG_IBSHARE | 表示设置MatmulConfig默认值为IBShare模板 |

**表4**  MatmulShapeParams参数说明

<a name="table16317184295116"></a>

| 参数 | 数据类型 | 说明 |
| --- | --- | --- |
| singleCoreM | uint32_t | 单核内M轴shape大小，以元素为单位。 |
| singleCoreN | uint32_t | 单核内N轴shape大小，以元素为单位。 |
| singleCoreK | uint32_t | 单核内K轴shape大小，以元素为单位。 |
| basicM | uint32_t | 与[TCubeTiling结构体](../Matmul_Tiling/TCubeTiling_struct.md)中的baseM参数含义相同，Matmul计算时base块M轴长度，以元素为单位。 |
| basicN | uint32_t | 与[TCubeTiling结构体](../Matmul_Tiling/TCubeTiling_struct.md)中的baseN参数含义相同，Matmul计算时base块N轴长度，以元素为单位。 |
| basicK | uint32_t | 与[TCubeTiling结构体](../Matmul_Tiling/TCubeTiling_struct.md)中的baseK参数含义相同，Matmul计算时base块K轴长度，以元素为单位。 |

**表5**  MatmulQuantParams参数说明

<a name="table8313111211573"></a>

| 参数 | 数据类型 | 说明 |
| --- | --- | --- |
| isPerTensor | bool | A矩阵half类型输入且B矩阵int8_t类型输入场景，开启B矩阵量化时是否为per tensor。<br>true：per tensor量化。false：per channel量化。<!-- npu="950" id9 --><br><br>对于Ascend 950PR/Ascend 950DT，MxMatmul场景不支持此参数。<!-- end id9 --> |
| hasAntiQuantOffset | bool | A矩阵half类型输入且B矩阵int8_t类型输入场景，开启B矩阵量化时是否使用offset系数。<!-- npu="950" id10 --><br><br>对于Ascend 950PR/Ascend 950DT，MxMatmul场景不支持此参数。<!-- end id10 --> |

**表6**  MatmulBatchParams参数说明

<a name="table15129204644"></a>

| 参数 | 数据类型 | 说明 |
| --- | --- | --- |
| isNBatch | bool | 是否多Batch输入多Batch输出。仅对BatchMatmul有效，开启多Batch后，仅支持Norm模板，且需调用[IterateNBatch](IterateNBatch.md)实现多Batch输入多Batch输出。参数取值如下：<br>false：不开启多Batch（默认值）。<br>true：开启多Batch。 |
| batchMode | BatchMode | BatchMatmul场景中Layout类型为NORMAL时，设置BatchMatmul输入A/B矩阵的多batch数据总和与L1 Buffer的大小关系。参数取值如下：<br>BatchMode::BATCH_LESS_THAN_L1：多batch数据总和<L1 Buffer Size；<br>BatchMode::BATCH_LARGE_THAN_L1：多batch数据总和>L1 Buffer Size；<br>BatchMode::SINGLE_LARGE_THAN_L1：单batch数据总和>L1 Buffer Size。 |
| isBiasBatch | bool | 批量多Batch的Matmul场景，即BatchMatmul场景，Bias的大小是否带有Batch轴。参数取值如下：<br>true：Bias带有Batch轴，Bias大小为Batch * N（默认值）。<br>false：Bias不带Batch轴，Bias大小为N，多Batch计算Matmul时，会复用Bias。<br>注意：BatchMode::SINGLE_LARGE_THAN_L1场景仅支持设置为true。<!-- npu="950" id14 --><br><br>除MxMatmul场景外，Ascend 950PR/Ascend 950DT支持该参数。<!-- end id14 --><!-- npu="910b" id15 --><br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品支持该参数。<!-- end id15 --><!-- npu="A3" id16 --><br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品支持该参数。<!-- end id16 --><!-- npu="310p" id17 --><br><br>Atlas 推理系列产品AI Core不支持设置为false。<!-- end id17 --><!-- npu="310b" id18 --><br><br>Atlas 200I/500 A2 推理产品不支持设置为false。<!-- end id18 --><!-- npu="x90" id3 --><br><br>Kirin X90支持此参数。<!-- end id3 --> |
| bmmOutMode | BatchOutMode | 预留参数。 |

**表7**  MatmulFuncParams参数说明

<a name="table66217141862"></a>

| 参数 | 数据类型 | 说明 |
| --- | --- | --- |
| intrinsicsLimit | bool | 当左矩阵或右矩阵在单核上内轴（即尾轴）大于等于65535（元素个数）时，是否开启循环执行数据从Global Memory到L1 Buffer的搬入。例如，左矩阵A[M, K]，单核上的内轴数据singleCoreK大于65535，配置该参数为true后，API内部通过循环执行数据的搬入。参数取值如下：<br>false：当左矩阵或右矩阵在单核上内轴大于等于65535时，不开启循环执行数据的搬入（默认值）。<br>true：当左矩阵或右矩阵在单核上内轴大于等于65535时，开启循环执行数据的搬入。<!-- npu="950" id19 --><br><br>对于Ascend 950PR/Ascend 950DT，MxMatmul场景不支持此参数。<!-- end id19 --> |
| enVecND2NZ | bool | 开启通过vector指令进行ND2NZ。开启时需要设置[SetLocalWorkspace](SetLocalWorkspace.md)。参数取值如下：<br>false：不开启通过vector指令进行ND2NZ（默认值）。<br>true：开启通过vector指令进行ND2NZ。<!-- npu="310p" id20 --><br><br>针对Atlas 推理系列产品AI Core，在Unified Buffer（UB）空间足够的条件下（UB空间大于2倍TCubeTiling的[transLength](../Matmul_Tiling/TCubeTiling_struct.md#p1620315053211)参数），建议优先开启，搬运性能更好。<!-- end id20 --><!-- npu="950" id21 --><br><br>对于Ascend 950PR/Ascend 950DT，MxMatmul场景不支持此参数。<!-- end id21 --> |
| enableDoubleCache | bool | 开启IBShare模板后，在L1 Buffer上是否同时缓存两块数据。参数取值如下：<br>false：L1 Buffer上同时缓存一块数据（默认值）。<br>true：开启L1 Buffer上同时缓存两块数据。<br><br>注意：该参数取值为true时，需要控制基本块大小，防止两块数据的缓存超过L1 Buffer大小限制。<!-- npu="950" id22 --><br><br>包括MxMatmul场景，Ascend 950PR/Ascend 950DT不支持此参数。<!-- end id22 --><!-- npu="x90" id4 --><br><br>Kirin X90不支持此参数。<!-- end id4 --> |
| enableL1CacheUB | bool | 是否开启L1 Buffer缓存UB计算块。建议在MTE3和MTE2流水串行较多的场景使用。参数取值如下：<br>true：开启L1 Buffer缓存UB计算块。<br>false：不开启L1 Buffer缓存UB计算块。<br><br>若要开启L1 Buffer缓存UB计算块，必须在Tiling实现中调用[SetMatmulConfigParams](../Matmul_Tiling/SetMatmulConfigParams.md)接口将参数enableL1CacheUBIn设置为true。<!-- npu="950" id23 --><br><br>包括MxMatmul场景，Ascend 950PR/Ascend 950DT不支持该参数。<!-- end id23 --><!-- npu="A3" id24 --><br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品不支持该参数。<!-- end id24 --><!-- npu="910b" id25 --><br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品不支持该参数。<!-- end id25 --><!-- npu="310p" id26 --><br><br>Atlas 推理系列产品AI Core支持该参数。<!-- end id26 --><!-- npu="310b" id27 --><br><br>Atlas 200I/500 A2 推理产品不支持该参数。<!-- end id27 --><!-- npu="x90" id5 --><br><br>Kirin X90不支持该参数。<!-- end id5 --> |
| doMTE2Preload | uint32_t | 在MTE2流水间隙较大，且M/N数值较大时可通过该参数开启对应M/N方向的预加载功能，开启后能减小MTE2间隙，提升性能。预加载功能仅在MDL模板有效（不支持SpecialMDL模板）。参数取值如下：<br>0：不开启（默认值）。<br>1：开启M方向preload。<br>2：开启N方向preload。<br><br>注意：开启M/N方向的预加载功能时需保证K全载且M/N方向开启DoubleBuffer；其中，M方向的K全载条件为：singleCoreK/baseK <= stepKa；N方向的K全载条件为：singleCoreK/baseK <= stepKb。 |
| iterateOrder | IterateOrder | Matmul做矩阵运算的循环迭代顺序，与[表1](../Matmul_Tiling/TCubeTiling_struct.md#tcubetiling-struct)中的iterateOrder参数含义相同。当ScheduleType参数取值为ScheduleType::OUTER_PRODUCT时，本参数生效。参数取值如下：<br><br>ORDER_M：先往M轴方向偏移再往N轴方向偏移。<br><br>ORDER_N：先往N轴方向偏移再往M轴方向偏移。<br><br>UNDEF：当前无效。<br><br>注：Norm模板的Matmul场景、MDL模板使用时，若IterateOrder取值ORDER_M，[TCubeTiling结构](../Matmul_Tiling/TCubeTiling_struct.md#tcubetiling-struct)中的stepN需要大于1，IterateOrder取值ORDER_N时，TCubeTiling结构中的stepM需要大于1。MxMatmul仅支持MDL模板。<!-- npu="A3" id28 --><br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品支持该参数。<!-- end id28 --><!-- npu="910b" id29 --><br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品支持该参数。<!-- end id29 --><!-- npu="310p" id30 --><br><br>Atlas 推理系列产品AI Core不支持该参数。<!-- end id30 --><!-- npu="310b" id31 --><br><br>Atlas 200I/500 A2 推理产品不支持该参数。<!-- end id31 --><!-- npu="x90" id6 --><br><br>Kirin X90支持该参数。<!-- end id6 --> |
| scheduleType | ScheduleType | 配置Matmul数据搬运模式。参数取值如下：<br>ScheduleType::INNER_PRODUCT：默认模式，在K方向上做MTE1的循环搬运；<br>ScheduleType::OUTER_PRODUCT：在M或N方向上做MTE1的循环搬运；设置该取值后，需要与IterateOrder参数配合使用。该配置当前只在BatchMatmul场景（开启Norm模板）或Matmul场景（开启MDL模板或Norm模板）生效。<br>若IterateOrder取值ORDER_M，则N方向循环搬运（在singleCoreN大于baseN场景可能有性能提升），即B矩阵的MTE1搬运并行；<br>若IterateOrder取值ORDER_N，则M方向循环搬运（在singleCoreM大于baseM场景可能有性能提升），即A矩阵的MTE1搬运并行；<br>不能同时开启M方向和N方向循环搬运；<br><br>注：<br>Norm模板的Batch Matmul场景或者MDL模板中，singleCoreK>baseK时，不能设置为ScheduleType::OUTER_PRODUCT取值，需使用默认模式。<br>Norm模板或MDL模板的Matmul场景，仅支持在纯Cube模式（只有矩阵计算）下配置ScheduleType::OUTER_PRODUCT。<br>MDL模板仅在调用[IterateAll](IterateAll.md)计算的场景支持配置ScheduleType::OUTER_PRODUCT。<br>仅在C矩阵输出至GM时，支持配置ScheduleType::OUTER_PRODUCT。<!-- npu="950" id32 --><br><br>Ascend 950PR/Ascend 950DT支持该参数。<!-- end id32 --><!-- npu="A3" id33 --><br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品支持该参数。<!-- end id33 --><!-- npu="910b" id34 --><br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品支持该参数。<!-- end id34 --><!-- npu="310p" id35 --><br><br>Atlas 推理系列产品AI Core不支持该参数。<!-- end id35 --><!-- npu="310b" id36 --><br><br>Atlas 200I/500 A2 推理产品不支持该参数。<!-- end id36 --><!-- npu="x90" id7 --><br><br>Kirin X90支持该参数。<!-- end id7 --> |
| enableReuse | bool | [SetSelfDefineData](SetSelfDefineData.md)函数设置的回调函数中的dataPtr是否直接传递计算数据。若未调用SetSelfDefineData设置dataPtr，该参数仅支持默认值true。参数取值如下：<br>true：直接传递计算数据，仅限单个值。<br>false：传递GM上存储的数据地址信息。<!-- npu="950" id37 --><br><br>对于Ascend 950PR/Ascend 950DT，MxMatmul场景不支持该参数。<!-- end id37 --> |
| enableUBReuse | bool | 是否开启UB复用。在UB空间足够的条件下（UB空间大于4倍TCubeTiling的[transLength](../Matmul_Tiling/TCubeTiling_struct.md#p1620315053211)参数），设置开启UB复用后，UB空间分为互不重叠的两份，分别存储Matmul计算相邻前后两轮迭代的数据，后一轮迭代数据的搬入将不必等待前一轮迭代的UB空间释放，从而优化流水。参数取值如下：<br>true：开启UB复用。<br>false：不开启UB复用。<!-- npu="950" id38 --><br><br>包括MxMatmul场景，Ascend 950PR/Ascend 950DT不支持该参数。<!-- end id38 --><!-- npu="A3" id39 --><br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品不支持该参数。<!-- end id39 --><!-- npu="910b" id40 --><br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品不支持该参数。<!-- end id40 --><!-- npu="310p" id41 --><br><br>Atlas 推理系列产品AI Core支持该参数。<!-- end id41 --><!-- npu="310b" id42 --><br><br>Atlas 200I/500 A2 推理产品不支持该参数。<!-- end id42 --><!-- npu="x90" id8 --><br><br>Kirin X90不支持该参数。<!-- end id8 --> |
| isPartialOutput | bool | 是否开启PartialOutput功能，即控制Matmul顺序输出K方向的基本块计算方式：Matmul一次Iterate计算的K轴是否进行累加计算。参数取值如下：<br>true：开启PartialOutput功能，一次Iterate的K轴不进行累加计算，Matmul每次计算输出局部baseK的baseM * baseN大小的矩阵分片。<br>false：不开启PartialOutput功能，一次Iterate的K轴进行累加计算，Matmul每次计算输出SingleCoreK长度的baseM * baseN大小的矩阵分片。<!-- npu="950" id43 --><br><br>Ascend 950PR/Ascend 950DT支持该参数。<!-- end id43 --><!-- npu="A3" id44 --><br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品支持该参数。<!-- end id44 --><!-- npu="910b" id45 --><br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品支持该参数。<!-- end id45 --><!-- npu="310p" id46 --><br><br>Atlas 推理系列产品AI Core不支持该参数。<!-- end id46 --><!-- npu="310b" id47 --><br><br>Atlas 200I/500 A2 推理产品不支持该参数。<!-- end id47 --><!-- npu="x90" id9 --><br><br>Kirin X90支持此参数。<!-- end id9 --> |
| isA2B2Shared | bool | 是否开启A2和B2的全局管理，即控制所有Matmul对象是否共用A2和B2的double buffer机制。该配置为全局配置，所有Matmul对象取值必须保持一致。注意，开启时，A矩阵、B矩阵的基本块大小均不能超过32KB。<br><br>参数取值如下：<br>true：开启。false：关闭（默认值）。<!-- npu="A3" id48 --><br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品支持该参数。<!-- end id48 --><!-- npu="910b" id49 --><br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品支持该参数。<!-- end id49 --><!-- npu="310p" id50 --><br><br>Atlas 推理系列产品AI Core不支持该参数。<!-- end id50 --><!-- npu="950" id51 --><br><br>除MxMatmul场景外，Ascend 950PR/Ascend 950DT支持该参数。<!-- end id51 --><!-- npu="x90" id10 --><br><br>Kirin X90支持此参数。<!-- end id10 --><br><br>该参数取值为true时，建议同时设置enUnitFlag参数为true，使搬运与计算流水并行，提高性能。 |
| isEnableChannelSplit | bool | 是否开启channel_split功能。正常情况下，Matmul计算出的CubeFormat::NZ格式的C矩阵分形为16\*16，假设此时的分形个数为x，channel_split功能是使获得的C矩阵分形为16\*8，同时分形个数变为2x。注意，当前仅在Matmul计算结果C矩阵的Format为CubeFormat::NZ，TYPE为float类型，矩阵乘结果L0C Buffer（CO1）为float类型，输出到Global Memory的场景，支持开启该参数。参数取值如下：<br>false：默认值，不开启channel_split功能，输出的分形为16\*16。<br>true：开启channel_split功能，输出的分形为16\*8。<!-- npu="950" id52 --><br><br>Ascend 950PR/Ascend 950DT支持该参数。<!-- end id52 --><!-- npu="A3" id53 --><br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品支持该参数。<!-- end id53 --><!-- npu="910b" id54 --><br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品支持该参数。<!-- end id54 --><!-- npu="310p" id55 --><br><br>Atlas 推理系列产品AI Core不支持该参数。<!-- end id55 --><!-- npu="310b" id56 --><br><br>Atlas 200I/500 A2 推理产品不支持该参数。<!-- end id56 --><!-- npu="x90" id11 --><br><br>Kirin X90不支持此参数。<!-- end id11 --> |
| enableKdimReorderLoad | bool | 是否开启K轴错峰加载数据。基于相同Tiling参数，执行Matmul计算时，如果多核的左矩阵或者右矩阵相同，且存储于Global Memory，多个核一般会同时访问相同地址以加载矩阵数据，引发同地址访问冲突，影响性能。开启K轴错峰加载数据后，多核执行Matmul时，将尽量在相同时间访问矩阵的不同Global Memory地址，减少地址访问冲突概率，提升性能。该参数功能只支持MDL模板，建议K轴较大且左矩阵和右矩阵均非全载场景开启该功能。参数取值如下。<br>false：默认值，关闭K轴错峰加载数据的功能。<br>true：开启K轴错峰加载数据的功能。<!-- npu="950" id57 --><br><br>除MxMatmul场景外，Ascend 950PR/Ascend 950DT支持该参数。<!-- end id57 --><!-- npu="A3" id58 --><br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品支持该参数。<!-- end id58 --><!-- npu="910b" id59 --><br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品支持该参数。<!-- end id59 --><!-- npu="310p" id60 --><br><br>Atlas 推理系列产品AI Core不支持该参数。<!-- end id60 --><!-- npu="310b" id61 --><br><br>Atlas 200I/500 A2 推理产品不支持该参数。<!-- end id61 --><!-- npu="x90" id12 --><br><br>Kirin X90支持此参数。<!-- end id12 --> |
| enableL1BankConflictOptimise | bool | 是否开启L1上的Bank冲突优化。在Tiling侧调用[EnableL1BankConflictOptimise](../Matmul_Tiling/EnableL1BankConflictOptimise.md)接口获取能否开启该优化的结果，并与TilingKey机制配合使用，在核函数（Kernel）侧增加代码实现分支。若开启该优化，基于相同Tiling参数执行Matmul计算时，对A、B矩阵和MxMatmul场景的ScaleA、ScaleB矩阵不再连续分配L1 Buffer的空间，在DoubleBuffer场景下，并行计算的数据分别被分配在L1 Buffer的上半部空间和下半部空间，非DoubleBuf场景，数据被分配在L1 Buffer的上半部空间；另外，Bias被分配在L1 Buffer的上半部空间，向量的量化/反量化场景的量化系数被分配在L1 Buffer的下半部空间。参数取值如下。<br>false：默认值，关闭L1 Bank冲突优化。<br>true：开启L1 Bank冲突优化。<!-- npu="950" id62 --><br><br>除MxMatmul场景外，Ascend 950PR/Ascend 950DT支持该参数。<!-- end id62 --><!-- npu="A3" id63 --><br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品不支持该参数。<!-- end id63 --><!-- npu="910b" id64 --><br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品不支持该参数。<!-- end id64 --><!-- npu="310p" id65 --><br><br>Atlas 推理系列产品AI Core不支持该参数。<!-- end id65 --><!-- npu="310b" id66 --><br><br>Atlas 200I/500 A2 推理产品不支持该参数。<!-- end id66 --><!-- npu="x90" id13 --><br><br>Kirin X90不支持此参数。<!-- end id13 --> |

## 返回值说明

[MatmulConfig结构体](MatmulConfig.md#matmulconfig-params)。

## 约束说明

无

## 调用示例

```
// 获取MatmulConfig模板为Norm模板
constexpr static MatmulConfigMode configMode = MatmulConfigMode::CONFIG_NORM;
// singleCoreM、singleCoreN、singleCoreK、basicM、basicN、basicK
constexpr static MatmulShapeParams shapeParams = {128, 128, 128, 64, 64, 64};
// B矩阵量化时为per channel且不使用offset系数
constexpr static MatmulQuantParams quantParams = {false, false};
// 不开启多Batch
constexpr static MatmulBatchParams batchParams{false};
// 不进行芯片指令搬运地址偏移量校验，开启通过vector进行ND2NZ
constexpr static MatmulFuncParams funcParams{false, true};
constexpr static MatmulConfig mmConfig = GetMMConfig<configMode>(shapeParams, quantParams, batchParams, funcParams);
```
