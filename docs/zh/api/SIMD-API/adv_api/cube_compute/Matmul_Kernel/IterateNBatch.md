# IterateNBatch

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/IterateNBatch_res.md#id1 -->

## 功能说明

调用一次IterateNBatch，会进行N次IterateBatch计算，计算出N个多Batch的singleCoreM \* singleCoreN大小的C矩阵。在调用该接口前，需将MatmulConfig中的[isNBatch](MatmulConfig.md#matmulconfig-params)参数设为true，开启多Batch输入多Batch输出功能，并调用[SetWorkspace](SetWorkspace.md)接口申请临时空间，用于缓存计算结果，即IterateNBatch的结果输出至[SetWorkspace](SetWorkspace.md)指定的Global Memory内存中。

对于BSNGD、SBNGD、BNGS1S2的Layout格式，调用该接口之前需要在tiling中使用SetALayout/SetBLayout/SetCLayout/SetBatchNum设置A/B/C的Layout轴信息和最大BatchNum数；对于Normal数据格式则需使用[SetBatchInfoForNormal](../Matmul_Tiling/SetBatchInfoForNormal.md)设置A/B/C的M/N/K轴信息和A/B矩阵的BatchNum数。实例化Matmul时，通过MatmulType设置Layout类型，当前支持3种Layout类型：BSNGD、SBNGD、BNGS1S2。

## 函数原型

```
template <bool sync = true, bool waitIterateBatch = false>
__aicore__ inline void IterateNBatch(const uint32_t batchLoop, uint32_t batchA, uint32_t batchB, bool enSequentialWrite, const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| sync | 获取C矩阵过程分为同步和异步两种模式：<br>同步：需要同步等待IterateNBatch执行结束，后续由开发者自行获取输出到Global Memory上的计算结果。<br>异步：不需要同步等待IterateNBatch执行结束。<br><br>通过该参数设置同步或者异步模式：同步模式设置为true；异步模式设置为false。默认为同步模式。 |
| waitIterateBatch | 是否需要通过[WaitIterateBatch](WaitIterateBatch.md)接口等待IterateNBatch执行结束，仅在异步场景下使用。默认为false。<br><br>true：需要通过WaitIterateBatch接口等待IterateNBatch执行结束，然后由开发者自行获取输出到Global Memory上的计算结果。<br><br>false：不需要通过WaitIterateBatch接口等待IterateNBatch执行结束。调用本接口后，需要调用[GetBatchTensorC](GetBatchTensorC.md)接口获取C矩阵，或者由开发者自行处理等待IterateNBatch执行结束的过程。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| batchLoop | 输入 | 当前计算的BMM个数。 |
| batchA | 输入 | 当前单次BMM调用计算左矩阵的batch数。 |
| batchB | 输入 | 当前单次BMM调用计算右矩阵的batch数，brc场景batchA/B不相同。 |
| enSequentialWrite | 输入 | 输出是否连续存放数据。 |
| matrixStrideA | 输入 | A矩阵源操作数相邻nd矩阵起始地址间的偏移，默认值是0。 |
| matrixStrideB | 输入 | B矩阵源操作数相邻nd矩阵起始地址间的偏移，默认值是0。 |
| matrixStrideC | 输入 | 该参数预留，保持默认值0即可。 |

## 返回值说明

无

## 约束说明

-   单BMM内计算遵循之前的约束条件。
-   对于BSNGD、SBNGD、BNGS1S2 Layout格式，输入A、B矩阵多Batch数据总和应小于L1 Buffer的大小。
-   当开启MixDualMaster（双主模式）场景时，即模板参数[enableMixDualMaster](MatmulConfig.md#matmulconfig-params)设置为true，不支持使用该接口。
-   当使用ND格式输入，且单个BMM内A或B矩阵的[源矩阵内轴](IterateBatch.md#row_width)大于65535时，需在MatmulConfig中设置intrinsicsCheck为true。开启此选项后，API会将一次从Global Memory到L1 Buffer的ND2NZ搬运操作拆分为多次循环搬运，从而避免单次搬运指令参数超限。

<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/IterateNBatch_res.md#id2 -->

## 调用示例

实例功能：完成aGM、bGM矩阵乘，结果保存到cGm上，其中aGM数据的layout格式为BSNGD，bGM数据的layout格式为BSNGD，cGM的layout格式为BNGS1S2，左矩阵每次计算batchA个SD数据，右矩阵每次计算batchB个SD数据。

```
// 创建Matmul实例
AscendC::Matmul<aType, bType, cType, biasType> mm1;
AscendC::TPipe pipe;
g_cubeTPipePtr = &pipe;

REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm1);
mm1.Init(&tiling);
int g_lay = tiling.ALayoutInfoG > tiling.BLayoutInfoG ? tiling.ALayoutInfoG : tiling.BLayoutInfoG;
int for_extent = tiling.ALayoutInfoB * tiling.ALayoutInfoN * g_lay / tiling.BatchNum;
mm1.SetTensorA(gm_a[0], isTransposeAIn);
mm1.SetTensorB(gm_b[0], isTransposeBIn);
mm1.SetWorkspace(workspaceGM, 0);
if (tiling.isBias) {
    mm1.SetBias(gm_bias[0]);
}
// 多batch Matmul计算
mm1.IterateNBatch(for_extent, batchA, batchB, false);
```
