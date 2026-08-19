# GetSpecialMDLConfig

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
- Kirin X90：支持
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030：不支持
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetSpecialMDLConfig_res.md#id1 -->

## 功能说明

用于配置SpecialMDL模板的参数，获取自定义SpecialMDL模板。SpecialMDL模板的介绍请参考[表模板特性](MatmulConfig.md#template-features)。

## 函数原型

```
__aicore__ constexpr MatmulConfig GetSpecialMDLConfig(const bool intrinsicsLimit = false, const bool batchLoop = false, const uint32_t doMTE2Preload = 0, const bool isVecND2NZ = false, bool isPerTensor = false, bool hasAntiQuantOffset = false)
```

## 参数说明

本接口的所有参数用于设置[MatmulConfig结构体](MatmulConfig.md#matmulconfig-params)中的参数，其中互相对应的参数的功能作用相同。

**表1**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| intrinsicsLimit | 输入 | 用于设置参数intrinsicsCheck。<br><br>当左矩阵或右矩阵在单核上内轴（即尾轴）大于等于65535（元素个数）时，是否开启循环执行数据从Global Memory到L1 Buffer的搬入。例如，左矩阵A[M, K]，单核上的内轴数据singleCoreK大于65535，配置该参数为true后，API内部通过循环执行数据的搬入。参数取值如下：<br>false：当左矩阵或右矩阵在单核上内轴大于等于65535时，不开启循环执行数据的搬入（默认值）。<br>true：当左矩阵或右矩阵在单核上内轴大于等于65535时，开启循环执行数据的搬入。<!-- npu="950" id9 --><br><br>对于Ascend 950PR/Ascend 950DT，MxMatmul场景不支持此参数。<!-- end id9 --> |
| batchLoop | 输入 | 用于设置参数isNBatch。<br><br>是否多Batch输入多Batch输出。仅对BatchMatmul有效，开启多Batch后，仅支持Norm模板，且需调用[IterateNBatch](IterateNBatch.md)实现多Batch输入多Batch输出。参数取值如下：<br>false：不开启多Batch（默认值）。<br>true：开启多Batch。 |
| doMTE2Preload | 输入 | 用于设置参数doMTE2Preload。<br><br>在MTE2流水间隙较大，且M/N数值较大时可通过该参数开启对应M/N方向的预加载功能，开启后能减小MTE2间隙，提升性能。预加载功能仅在MDL模板有效（不支持SpecialMDL模板）。参数取值如下：<br>0：不开启（默认值）。<br>1：开启M方向preload。<br>2：开启N方向preload。<br><br>注意：开启M/N方向的预加载功能时需保证K全载且M/N方向开启DoubleBuffer；其中，M方向的K全载条件为：singleCoreK/baseK <= stepKa；N方向的K全载条件为：singleCoreK/baseK <= stepKb。 |
| isVecND2NZ | 输入 | 预留参数，保持默认值false即可。 |
| isPerTensor | 输入 | 用于设置参数isPerTensor。<br><br>A矩阵half类型输入且B矩阵int8_t类型输入场景，开启B矩阵量化时是否为per tensor。<br>true：per tensor量化。<br>false：per channel量化。<!-- npu="950" id10 --><br><br>对于Ascend 950PR/Ascend 950DT，MxMatmul场景不支持此参数。<!-- end id10 --> |
| hasAntiQuantOffset | 输入 | 用于设置参数hasAntiQuantOffset。<br><br>A矩阵half类型输入且B矩阵int8_t类型输入场景，开启B矩阵量化时是否使用offset系数。<!-- npu="950" id11 --><br><br>对于Ascend 950PR/Ascend 950DT，MxMatmul场景不支持此参数。<!-- end id11 --> |

## 返回值说明

[MatmulConfig结构体](MatmulConfig.md#matmulconfig-params)。

## 约束说明

无

## 调用示例

```
// 配置SpecialMDL模板的参数，获取自定义SpecialMDL模板
constexpr MatmulConfig MM_CFG = GetSpecialMDLConfig(
    /* intrinsicsLimit      */ false,
    /* batchLoop            */ false,
    /* doMTE2Preload        */ 0,
    /* isVecND2NZ           */ false,
    /* isPerTensor          */ false,
    /* hasAntiQuantOffset   */ false);
// 常规Matmul计算，最后输出使用自定义SpecialMDL模板的计算结果
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
