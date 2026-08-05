# WaitIterateAll

## 产品支持情况

<!-- npu="950" id2 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id2 -->
<!-- npu="A3" id3 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id3 -->
<!-- npu="910b" id4 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id4 -->
<!-- npu="310b" id5 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- npu="x90" id1 -->
- Kirin X90：支持
<!-- end id1 -->

## 功能说明

等待IterateAll异步接口返回，支持连续输出到Global Memory。

## 函数原型

```
__aicore__ inline void WaitIterateAll()
```

## 参数说明

无

## 返回值说明

无

## 约束说明

-   配套[IterateAll](IterateAll.md)异步接口使用。
-   仅支持连续输出至Global Memory。

## 调用示例

```
AscendC::Matmul<aType, bType, cType, biasType> mm;
mm.SetTensorA(gm_a[offsetA]);
mm.SetTensorB(gm_b[offsetB]);
if (tiling.isBias) {
    mm.SetBias(gm_bias[offsetBias]);
}
mm.template IterateAll<false>(gm_c[offsetC], 0, false, true);
// do some others compute
mm.WaitIterateAll(); // 等待IterateAll完成
```
