# GetNormalConfig

## 产品支持情况

<!-- npu="950" id6 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id6 -->
<!-- npu="A3" id7 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id7 -->
<!-- npu="910b" id8 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id8 -->
<!-- npu="310b" id9 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id9 -->
<!-- npu="310p" id10 -->
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id10 -->
<!-- npu="910" id11 -->
- Atlas 训练系列产品：不支持
<!-- end id11 -->
<!-- npu="x90" id1 -->
- Kirin X90：支持
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030：不支持
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetNormalConfig_res.md#id1 -->

## 功能说明

用于配置Norm模板的参数，获取自定义Norm模板。Norm模板的介绍请参考[表模板特性](MatmulConfig.md#template-features)。

## 函数原型

```
__aicore__ constexpr MatmulConfig GetNormalConfig(const bool intrinsicsLimit = false, const bool batchLoop = false, const bool isVecND2NZ = false, const BatchMode bmmMode = BatchMode::BATCH_LESS_THAN_L1, const bool isMsgReuse = true, const IterateOrder iterateOrder = IterateOrder::UNDEF, const ScheduleType scheduleType = ScheduleType::INNER_PRODUCT, const bool enUnitFlag = true, const bool enableMixDualMaster = false, const BatchOutMode bmmOutMode = BatchOutMode::SINGLE_BATCH)
```

## 参数说明

本接口的所有参数用于设置[MatmulConfig结构体](MatmulConfig.md#matmulconfig-params)中的参数，其中互相对应的参数的功能作用相同。

**表1**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| intrinsicsLimit | 输入 | 用于设置参数intrinsicsCheck。<br><br>当左矩阵或右矩阵在单核上内轴（即尾轴）大于等于65535（元素个数）时，是否开启循环执行数据从Global Memory到L1 Buffer的搬入。例如，左矩阵A[M, K]，单核上的内轴数据singleCoreK大于65535，配置该参数为true后，API内部通过循环执行数据的搬入。参数取值如下：<br>false：当左矩阵或右矩阵在单核上内轴大于等于65535时，不开启循环执行数据的搬入（默认值）。<br>true：当左矩阵或右矩阵在单核上内轴大于等于65535时，开启循环执行数据的搬入。<!-- npu="950" id12 --><br><br>对于Ascend 950PR/Ascend 950DT，MxMatmul场景不支持此参数。<!-- end id12 --> |
| batchLoop | 输入 | 用于设置参数isNBatch。<br><br>是否多Batch输入多Batch输出。仅对BatchMatmul有效，开启多Batch后，仅支持Norm模板，且需调用[IterateNBatch](IterateNBatch.md)实现多Batch输入多Batch输出。参数取值如下：<br>false：不开启多Batch（默认值）。<br>true：开启多Batch。 |
| isVecND2NZ | 输入 | 用于设置参数enVecND2NZ。<br><br>开启通过vector指令进行ND2NZ。开启时需要设置[SetLocalWorkspace](SetLocalWorkspace.md)。参数取值如下：<br>false：不开启通过vector指令进行ND2NZ（默认值）。<br>true：开启通过vector指令进行ND2NZ。<!-- npu="310p" id13 --><br><br>针对Atlas 推理系列产品AI Core，在Unified Buffer（UB）空间足够的条件下（UB空间大于2倍TCubeTiling的[transLength](../Matmul_Tiling/TCubeTiling_struct.md#p1620315053211)参数），建议优先开启，搬运性能更好。<!-- end id13 --><!-- npu="950" id14 --><br><br>对于Ascend 950PR/Ascend 950DT，MxMatmul场景不支持此参数。<!-- end id14 --> |
| bmmMode | 输入 | 用于设置参数batchMode。该参数用于BatchMatmul场景。<br><br>BatchMatmul场景中Layout类型为NORMAL时，设置BatchMatmul输入A/B矩阵的多batch数据总和与L1 Buffer的大小关系。参数取值如下：<br>BatchMode::BATCH_LESS_THAN_L1：多batch数据总和<L1 Buffer Size；<br>BatchMode::BATCH_LARGE_THAN_L1：多batch数据总和>L1 Buffer Size；<br>BatchMode::SINGLE_LARGE_THAN_L1：单batch数据总和>L1 Buffer Size。 |
| isMsgReuse | 输入 | 用于设置参数enableReuse。<br><br>[SetSelfDefineData](SetSelfDefineData.md)函数设置的回调函数中的dataPtr是否直接传递计算数据。若未调用SetSelfDefineData设置dataPtr，该参数仅支持默认值true。参数取值如下：<br>true：直接传递计算数据，仅限单个值。<br>false：传递GM上存储的数据地址信息。<!-- npu="950" id15 --><br><br>对于Ascend 950PR/Ascend 950DT，MxMatmul场景不支持该参数。<!-- end id15 --> |
| iterateOrder | 输入 | 用于设置参数iterateOrder。<br><br>Matmul做矩阵运算的循环迭代顺序，与[表1](../Matmul_Tiling/TCubeTiling_struct.md#tcubetiling-struct)中的iterateOrder参数含义相同。当ScheduleType参数取值为ScheduleType::OUTER_PRODUCT时，本参数生效。参数取值如下：<br><br>ORDER_M：先往M轴方向偏移再往N轴方向偏移。<br><br>ORDER_N：先往N轴方向偏移再往M轴方向偏移。<br><br>UNDEF：当前无效。<br><br>注：Norm模板的Matmul场景、MDL模板使用时，若IterateOrder取值ORDER_M，[TCubeTiling结构](../Matmul_Tiling/TCubeTiling_struct.md#tcubetiling-struct)中的stepN需要大于1，IterateOrder取值ORDER_N时，TCubeTiling结构中的stepM需要大于1。MxMatmul仅支持MDL模板。<!-- npu="A3" id16 --><br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品支持该参数。<!-- end id16 --><!-- npu="910b" id17 --><br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品支持该参数。<!-- end id17 --><!-- npu="310p" id18 --><br><br>Atlas 推理系列产品AI Core不支持该参数。<!-- end id18 --><!-- npu="310b" id19 --><br><br>Atlas 200I/500 A2 推理产品不支持该参数。<!-- end id19 --><!-- npu="x90" id3 --><br><br>Kirin X90支持此参数。<!-- end id3 --> |
| scheduleType | 输入 | 用于设置参数scheduleType。<br><br>配置Matmul数据搬运模式。参数取值如下：<br>ScheduleType::INNER_PRODUCT：默认模式，在K方向上做MTE1的循环搬运；<br>ScheduleType::OUTER_PRODUCT：在M或N方向上做MTE1的循环搬运；设置该取值后，需要与IterateOrder参数配合使用。该配置当前只在BatchMatmul场景（开启Norm模板）或Matmul场景（开启MDL模板或Norm模板）生效。<br>若IterateOrder取值ORDER_M，则N方向循环搬运（在singleCoreN大于baseN场景可能有性能提升），即B矩阵的MTE1搬运并行；<br>若IterateOrder取值ORDER_N，则M方向循环搬运（在singleCoreM大于baseM场景可能有性能提升），即A矩阵的MTE1搬运并行；<br>不能同时开启M方向和N方向循环搬运；<br><br>注：<br>Norm模板的Batch Matmul场景或者MDL模板中，singleCoreK>baseK时，不能设置为ScheduleType::OUTER_PRODUCT取值，需使用默认模式。<br>Norm模板或MDL模板的Matmul场景，仅支持在纯Cube模式（只有矩阵计算）下配置ScheduleType::OUTER_PRODUCT。<br>MDL模板仅在调用[IterateAll](IterateAll.md)计算的场景支持配置ScheduleType::OUTER_PRODUCT。<br>仅在C矩阵输出至GM时，支持配置ScheduleType::OUTER_PRODUCT。<!-- npu="950" id20 --><br><br>Ascend 950PR/Ascend 950DT支持该参数。<!-- end id20 --><!-- npu="A3" id21 --><br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品支持该参数。<!-- end id21 --><!-- npu="910b" id22 --><br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品支持该参数。<!-- end id22 --><!-- npu="310p" id23 --><br><br>Atlas 推理系列产品AI Core不支持该参数。<!-- end id23 --><!-- npu="310b" id24 --><br><br>Atlas 200I/500 A2 推理产品不支持该参数。<!-- end id24 --><!-- npu="x90" id4 --><br><br>Kirin X90支持此参数。<!-- end id4 --> |
| enUnitFlag | 输入 | 用于设置参数enUnitFlag。<br><br>开启UnitFlag功能，使计算与搬运流水并行，提高性能。Norm, IBShare下默认开启，MDL下默认不开启。参数取值如下：<br>false：不开启UnitFlag功能。<br>true：开启UnitFlag功能。<!-- npu="950" id30 --><br><br>注意：对于Ascend 950PR/Ascend 950DT的MxMatmul场景，仅在NORM/MDL模板、A和scaleA不转置、 B和scaleB转置、C为ND格式，输出到GM场景下，开启UnitFlag功能有性能收益。<!-- end id30 --> |
| enableMixDualMaster | 输入 | 用于设置参数enableMixDualMaster。<br><br>是否开启MixDualMaster（双主模式）。区别于MIX模式（包含矩阵计算和矢量计算）通过消息机制驱动AIC运行，双主模式为AIC和AIV独立运行代码，不依赖消息驱动，用于提升性能。该参数默认值为false，仅能在以下场景设置为true：<br>核函数（Kernel）的类型为MIX，同时AIC核数 : AIV核数为1:1。<br>核函数（Kernel）的类型为MIX，同时AIC核数 : AIV核数为1:2，且A矩阵和B矩阵同时开启[IBSHARE](Matmul_usage.md#table1188045714378)参数。<br><br>注意，开启MixDualMaster场景，需要满足：<br>同一算子中所有Matmul对象的该参数取值必须保持一致。<br>A/B/Bias矩阵只支持从GM搬入。<br>获取矩阵计算结果只支持调用[IterateAll](IterateAll.md)接口输出到GlobalTensor或者LocalTensor，即计算结果放置于Global Memory或者Local Memory的地址，不能调用[GetTensorC](GetTensorC.md)等接口获取结果。<!-- npu="950" id25 --><br><br>除MxMatmul场景外，Ascend 950PR/Ascend 950DT支持该参数。<!-- end id25 --><!-- npu="A3" id26 --><br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品支持该参数。<!-- end id26 --><!-- npu="910b" id27 --><br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品支持该参数。<!-- end id27 --><!-- npu="310p" id28 --><br><br>Atlas 推理系列产品AI Core不支持该参数。<!-- end id28 --><!-- npu="310b" id29 --><br><br>Atlas 200I/500 A2 推理产品不支持该参数。<!-- end id29 --><!-- npu="x90" id5 --><br><br>Kirin X90不支持此参数。<!-- end id5 --> |
| bmmOutMode | 输入 | 预留参数。 |

## 返回值说明

[MatmulConfig结构体](MatmulConfig.md#matmulconfig-params)。

## 约束说明

无

<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetNormalConfig_res.md#id2 -->

## 调用示例

```
// 配置Norm模板的参数，获取自定义Norm模板
constexpr MatmulConfig MM_CFG = GetNormalConfig(
    /* intrinsicsLimit   */ false,
    /* batchLoop         */ false,
    /* isVecND2NZ        */ false,
    /* bmmMode           */ BatchMode::BATCH_LESS_THAN_L1,
    /* isMsgReuse        */ true,
    /* iterateOrder      */ IterateOrder::UNDEF,
    /* scheduleType      */ ScheduleType::INNER_PRODUCT,
    /* enUnitFlag        */ true,
    /* enableMixDualMaster */ false,
    /* bmmOutMode        */ BatchOutMode::SINGLE_BATCH);
// 常规Matmul计算，最后输出使用自定义Norm模板的计算结果
AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG> mm;
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(gmBias);
}
mm.IterateAll(gm_c);
mm.End();
```
