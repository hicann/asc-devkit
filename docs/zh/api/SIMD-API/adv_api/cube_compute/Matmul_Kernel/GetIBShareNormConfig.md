# GetIBShareNormConfig

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
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas 训练系列产品：不支持
<!-- end id8 -->
<!-- npu="x90" id1 -->
- Kirin X90：不支持
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030：不支持
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetIBShareNormConfig_res.md#id1 -->

## 功能说明

用于配置IBShare模板的参数，获取自定义IBShare模板。IBShare模板的介绍请参考[表模板特性](MatmulConfig.md#template-features)。

## 函数原型

```
__aicore__ constexpr MatmulConfig GetIBShareNormConfig(const bool intrinsicsLimit = false, const bool batchLoop = false, const bool isVecND2NZ = false, const BatchMode bmmMode = BatchMode::BATCH_LESS_THAN_L1, const bool isDoubleCache = false, const bool enUnitFlag = true)
```

## 参数说明

本接口的所有参数用于设置[MatmulConfig结构体](MatmulConfig.md#matmulconfig-params)中的参数，其中互相对应的参数的功能作用相同。

**表1**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| intrinsicsLimit | 输入 | 用于设置参数intrinsicsCheck。<br><br>当左矩阵或右矩阵在单核上内轴（即尾轴）大于等于65535（元素个数）时，是否开启循环执行数据从Global Memory到L1 Buffer的搬入。例如，左矩阵A[M, K]，单核上的内轴数据singleCoreK大于65535，配置该参数为true后，API内部通过循环执行数据的搬入。参数取值如下：<br>false：当左矩阵或右矩阵在单核上内轴大于等于65535时，不开启循环执行数据的搬入（默认值）。<br>true：当左矩阵或右矩阵在单核上内轴大于等于65535时，开启循环执行数据的搬入。<!-- npu="950" id9 --><br><br>对于Ascend 950PR/Ascend 950DT，MxMatmul场景不支持此参数。<!-- end id9 --> |
| batchLoop | 输入 | 用于设置参数isNBatch。<br><br>是否多Batch输入多Batch输出。仅对BatchMatmul有效，开启多Batch后，仅支持Norm模板，且需调用[IterateNBatch](IterateNBatch.md)实现多Batch输入多Batch输出。参数取值如下：<br>false：不开启多Batch（默认值）。<br>true：开启多Batch。 |
| isVecND2NZ | 输入 | 用于设置参数enVecND2NZ。<br><br>开启通过vector指令进行ND2NZ。开启时需要设置[SetLocalWorkspace](SetLocalWorkspace.md)。参数取值如下：<br>false：不开启通过vector指令进行ND2NZ（默认值）。<br>true：开启通过vector指令进行ND2NZ。<!-- npu="310p" id13 --><br><br>针对Atlas 推理系列产品AI Core，在Unified Buffer（UB）空间足够的条件下（UB空间大于2倍TCubeTiling的[transLength](../Matmul_Tiling/TCubeTiling_struct.md#p1620315053211)参数），建议优先开启，搬运性能更好。<!-- end id13 --><!-- npu="950" id10 --><br><br>对于Ascend 950PR/Ascend 950DT，MxMatmul场景不支持此参数。<!-- end id10 --> |
| bmmMode | 输入 | 用于设置参数batchMode。该参数用于BatchMatmul场景。<br><br>BatchMatmul场景中Layout类型为NORMAL时，设置BatchMatmul输入A/B矩阵的多batch数据总和与L1 Buffer的大小关系。参数取值如下：<br>BatchMode::BATCH_LESS_THAN_L1：多batch数据总和<L1 Buffer Size；<br>BatchMode::BATCH_LARGE_THAN_L1：多batch数据总和>L1 Buffer Size；<br>BatchMode::SINGLE_LARGE_THAN_L1：单batch数据总和>L1 Buffer Size。 |
| isDoubleCache | 输入 | 用于设置参数enableDoubleCache。<br><br>开启IBShare模板后，在L1 Buffer上是否同时缓存两块数据。参数取值如下：<br>false：L1 Buffer上同时缓存一块数据（默认值）。<br>true：开启L1 Buffer上同时缓存两块数据。<br><br>注意：该参数取值为true时，需要控制基本块大小，防止两块数据的缓存超过L1 Buffer大小限制。<!-- npu="950" id11 --><br><br>包括MxMatmul场景，Ascend 950PR/Ascend 950DT不支持此参数。<!-- end id11 --> |
| enUnitFlag | 输入 | 用于设置参数enUnitFlag。<br><br>开启UnitFlag功能，使计算与搬运流水并行，提高性能。Norm, IBShare下默认开启，MDL下默认不开启。参数取值如下：<br>false：不开启UnitFlag功能。<br>true：开启UnitFlag功能。<!-- npu="950" id12 --><br><br>注意：对于Ascend 950PR/Ascend 950DT的MxMatmul场景，仅在NORM/MDL模板、A和scaleA不转置、 B和scaleB转置、C为ND格式，输出到GM场景下，开启UnitFlag功能有性能收益。<!-- end id12 --> |

## 返回值说明

[MatmulConfig结构体](MatmulConfig.md#matmulconfig-params)。

## 约束说明

IBShare模板当前仅适用于MIX场景，不支持纯CUBE场景。

## 调用示例

```
// 配置IBShare模板的参数，获取自定义IBShare模板。
constexpr MatmulConfig MM_CFG = GetIBShareNormConfig(
    /* intrinsicsLimit      */ false,
    /* batchLoop            */ false,
    /* isVecND2NZ           */ false,
    /* bmmMode              */ BatchMode::BATCH_LESS_THAN_L1,
    /* isMsgReuse           */ false,
    /* enableUBReuse        */ true);
// 常规Matmul计算，最后输出使用自定义IBShare模板的计算结果
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
typedef AscendC::MatmulType<
    AscendC::TPosition::GM, CubeFormat::ND, half, true /*开启矩阵转置*/, LayoutMode::NONE /*不开启BatchMatmul*/,
    true /*开启IBShare*/>
    bType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
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
