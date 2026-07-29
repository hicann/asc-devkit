# WaitIterateBatch

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

等待[IterateBatch](IterateBatch.md)异步接口或[IterateNBatch](IterateNBatch.md)异步接口返回，支持连续输出到Global Memory。

## 函数原型

```
__aicore__ inline void WaitIterateBatch()
```

## 参数说明

无

## 返回值说明

无

## 约束说明

-   配套[IterateBatch](IterateBatch.md)或[IterateNBatch](IterateNBatch.md)异步接口使用。
-   仅支持连续输出至Global Memory。
-   当开启MixDualMaster（双主模式）场景时，即模板参数[enableMixDualMaster](MatmulConfig.md#matmulconfig-params)设置为true，不支持使用该接口。

## 调用示例

```
AscendC::Matmul<aType, bType, cType, biasType> mm;
mm.SetTensorA(gm_a[offsetA]);
mm.SetTensorB(gm_b[offsetB]);
if (tiling.isBias) {
    mm.SetBias(gm_bias[offsetBias]);
}
mm.IterateBatch(gm_c[offsetC], batchA, batchB, false);
// do some other compute tasks
mm.WaitIterateBatch(); // 等待IterateBatch完成
```
