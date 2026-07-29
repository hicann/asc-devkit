# GetMDLConfig

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

用于配置MDL模板的参数，获取自定义MDL模板。MDL模板的介绍请参考[表 模板特性](MatmulConfig.md#table6981133810309)。

## 函数原型

```
__aicore__ constexpr MatmulConfig GetMDLConfig(const bool intrinsicsLimit = false, const bool batchLoop = false, const uint32_t doMTE2Preload = 0, const bool isVecND2NZ = false, bool isPerTensor = false, bool hasAntiQuantOffset = false, const bool enUnitFlag = false, const bool isMsgReuse = true, const bool enableUBReuse = true, const bool enableL1CacheUB = false, const bool enableMixDualMaster = false, const bool enableKdimReorderLoad = false)
```

## 参数说明

本接口的所有参数用于设置[MatmulConfig结构体](MatmulConfig.md#matmulconfig-params)中的参数，其中互相对应的参数的功能作用相同。

**表1**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| intrinsicsLimit | 输入 | 用于设置参数intrinsicsCheck。<br><br>当左矩阵或右矩阵在单核上内轴（即尾轴）大于等于65535（元素个数）时，是否开启循环执行数据从Global Memory到L1 Buffer的搬入。例如，左矩阵A[M, K]，单核上的内轴数据singleCoreK大于65535，配置该参数为true后，API内部通过循环执行数据的搬入。参数取值如下：<br>false：当左矩阵或右矩阵在单核上内轴大于等于65535时，不开启循环执行数据的搬入（默认值）。<br>true：当左矩阵或右矩阵在单核上内轴大于等于65535时，开启循环执行数据的搬入。<br><br>对于Ascend 950PR/Ascend 950DT，MxMatmul场景不支持此参数。 |
| batchLoop | 输入 | 用于设置参数isNBatch。<br><br>是否多Batch输入多Batch输出。仅对BatchMatmul有效，开启多Batch后，仅支持Norm模板，且需调用[IterateNBatch](IterateNBatch.md)实现多Batch输入多Batch输出。参数取值如下：<br>false：不开启多Batch（默认值）。<br>true：开启多Batch。 |
| doMTE2Preload | 输入 | 用于设置参数doMTE2Preload。<br><br>在MTE2流水间隙较大，且M/N数值较大时可通过该参数开启对应M/N方向的预加载功能，开启后能减小MTE2间隙，提升性能。预加载功能仅在MDL模板有效（不支持SpecialMDL模板）。参数取值如下：<br>0：不开启（默认值）。1：开启M方向preload。2：开启N方向preload。<br><br>注意：开启M/N方向的预加载功能时需保证K全载且M/N方向开启DoubleBuffer；其中，M方向的K全载条件为：singleCoreK/baseK <= stepKa；N方向的K全载条件为：singleCoreK/baseK <= stepKb。 |
| isVecND2NZ | 输入 | 用于设置参数enVecND2NZ。<br><br>开启通过vector指令进行ND2NZ。开启时需要设置[SetLocalWorkspace](SetLocalWorkspace.md)。参数取值如下：<br>false：不开启通过vector指令进行ND2NZ（默认值）。<br>true：开启通过vector指令进行ND2NZ。<br><br>针对Atlas 推理系列产品AI Core，在Unified Buffer空间足够的条件下（Unified Buffer空间大于2倍TCubeTiling的[transLength](../Matmul-Tiling类/TCubeTiling结构体.md#tcubetiling-struct)参数），建议优先开启，搬运性能更好。<br><br>对于Ascend 950PR/Ascend 950DT，MxMatmul场景不支持此参数。 |
| isPerTensor | 输入 | 用于设置参数isPerTensor。<br><br>A矩阵half类型输入且B矩阵int8_t类型输入场景，开启B矩阵量化时是否为per tensor。<br>true：per tensor量化。<br>false：per channel量化。<br><br>对于Ascend 950PR/Ascend 950DT，MxMatmul场景不支持此参数。 |
| hasAntiQuantOffset | 输入 | 用于设置参数hasAntiQuantOffset。<br><br>A矩阵half类型输入且B矩阵int8_t类型输入场景，开启B矩阵量化时是否使用offset系数。<br><br>对于Ascend 950PR/Ascend 950DT，MxMatmul场景不支持此参数。 |
| enUnitFlag | 输入 | 用于设置参数enUnitFlag。<br><br>开启UnitFlag功能，使计算与搬运流水并行，提高性能。Norm, IBShare下默认开启，MDL下默认不开启。参数取值如下：<br>false：不开启UnitFlag功能。<br>true：开启UnitFlag功能。<br><br>注意：对于Ascend 950PR/Ascend 950DT的MxMatmul场景，仅在NORM/MDL模板、A和scaleA不转置、 B和scaleB转置、C为ND格式，输出到GM场景下，开启UnitFlag功能有性能收益。 |
| isMsgReuse | 输入 | 用于设置参数enableReuse。<br><br>[SetSelfDefineData](SetSelfDefineData.md)函数设置的回调函数中的dataPtr是否直接传递计算数据。若未调用SetSelfDefineData设置dataPtr，该参数仅支持默认值true。参数取值如下：<br>true：直接传递计算数据，仅限单个值。<br>false：传递GM上存储的数据地址信息。<br><br>对于Ascend 950PR/Ascend 950DT，MxMatmul场景不支持该参数。 |
| enableUBReuse | 输入 | 用于设置参数enableUBReuse。<br><br>是否开启Unified Buffer复用。在Unified Buffer空间足够的条件下（Unified Buffer空间大于4倍TCubeTiling的[transLength](../Matmul-Tiling类/TCubeTiling结构体.md#tcubetiling-struct)参数），设置开启Unified Buffer复用后，Unified Buffer空间分为互不重叠的两份，分别存储Matmul计算相邻前后两轮迭代的数据，后一轮迭代数据的搬入将不必等待前一轮迭代的Unified Buffer空间释放，从而优化流水。参数取值如下：<br>true：开启Unified Buffer复用。<br>false：不开启Unified Buffer复用。<br><br>包括MxMatmul场景，Ascend 950PR/Ascend 950DT不支持该参数。<br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品不支持该参数。<br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品不支持该参数。<br><br>Atlas 推理系列产品AI Core支持该参数。<br><br>Atlas 200I/500 A2 推理产品不支持该参数。 |
| enableL1CacheUB | 输入 | 用于设置参数enableL1CacheUB 。<br><br>是否开启L1 Buffer缓存Unified Buffer计算块。建议在MTE3和MTE2流水串行较多的场景使用。参数取值如下：<br>true：开启L1 Buffer缓存Unified Buffer计算块。<br>false：不开启L1 Buffer缓存Unified Buffer计算块。<br><br>若要开启L1 Buffer缓存Unified Buffer计算块，必须在Tiling实现中调用[SetMatmulConfigParams](../Matmul-Tiling类/SetMatmulConfigParams.md)接口将参数enableL1CacheUBIn设置为true。<br><br>包括MxMatmul场景，Ascend 950PR/Ascend 950DT不支持该参数。<br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品不支持该参数。<br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品不支持该参数。<br><br>Atlas 推理系列产品AI Core支持该参数。<br><br>Atlas 200I/500 A2 推理产品不支持该参数。 |
| enableMixDualMaster | 输入 | 用于设置参数enableMixDualMaster。<br><br>是否开启MixDualMaster（双主模式）。区别于MIX模式（包含矩阵计算和矢量计算）通过消息机制驱动AIC运行，双主模式为AIC和AIV独立运行代码，不依赖消息驱动，用于提升性能。该参数默认值为false，仅能在以下场景设置为true：<br>核函数的类型为MIX，同时AIC核数 : AIV核数为1:1。<br>核函数的类型为MIX，同时AIC核数 : AIV核数为1:2，且A矩阵和B矩阵同时开启[IBSHARE](Matmul使用说明.md#table1188045714378)参数。<br><br>注意，开启MixDualMaster场景，需要满足：<br>同一算子中所有Matmul对象的该参数取值必须保持一致。<br>A/B/Bias矩阵只支持从GM搬入。<br>获取矩阵计算结果只支持调用[IterateAll](IterateAll.md)接口输出到GlobalTensor或者LocalTensor，即计算结果放置于Global Memory或者Local Memory的地址，不能调用[GetTensorC](GetTensorC.md)等接口获取结果。<br><br>除MxMatmul场景外，Ascend 950PR/Ascend 950DT支持该参数。<br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品支持该参数。<br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品支持该参数。<br><br>Atlas 推理系列产品AI Core不支持该参数。<br><br>Atlas 200I/500 A2 推理产品不支持该参数。 |
| enableKdimReorderLoad | 输入 | 用于设置参数enableKdimReorderLoad。<br><br>是否开启K轴错峰加载数据。基于相同Tiling参数，执行Matmul计算时，如果多核的左矩阵或者右矩阵相同，且存储于Global Memory，多个核一般会同时访问相同地址以加载矩阵数据，引发同地址访问冲突，影响性能。开启K轴错峰加载数据后，多核执行Matmul时，将尽量在相同时间访问矩阵的不同Global Memory地址，减少地址访问冲突概率，提升性能。该参数功能只支持MDL模板，建议K轴较大且左矩阵和右矩阵均非全载场景开启该功能。参数取值如下。<br>false：默认值，关闭K轴错峰加载数据的功能。<br>true：开启K轴错峰加载数据的功能。<br><br>除MxMatmul场景外，Ascend 950PR/Ascend 950DT支持该参数。<br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品支持该参数。<br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品支持该参数。<br><br>Atlas 推理系列产品AI Core不支持该参数。<br><br>Atlas 200I/500 A2 推理产品不支持该参数。 |

## 返回值说明

[MatmulConfig结构体](MatmulConfig.md#matmulconfig-params)。

## 约束说明

无

## 调用示例

```
// 配置MDL模板的参数，获取自定义MDL模板
constexpr MatmulConfig MM_CFG = GetMDLConfig(
    /* intrinsicsLimit      */ false,
    /* batchLoop            */ false,
    /* doMTE2Preload        */ 0,
    /* isVecND2NZ           */ false,
    /* isPerTensor          */ false,
    /* hasAntiQuantOffset   */ false,
    /* enUnitFlag           */ false,
    /* isMsgReuse           */ true,
    /* enableUBReuse        */ true,
    /* enableL1CacheUB      */ false,
    /* enableMixDualMaster  */ false,
    /* enableKdimReorderLoad*/ false
);
// 常规Matmul计算，最后输出使用自定义MDL模板的计算结果
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
