# SetTail

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
- Atlas 200I/500 A2 推理产品：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- npu="x90" id1 -->
- Kirin X90：支持
<!-- end id1 -->

## 功能说明

在不改变Tiling的情况下，重新设置本次计算的singleCoreM/singleCoreN/singleCoreK，以元素为单位。

## 函数原型

```
__aicore__ inline void SetTail(int tailM = -1, int tailN = -1, int tailK = -1)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| tailM | 输入 | 重新设置的singleCoreM值 |
| tailN | 输入 | 重新设置的singleCoreN值 |
| tailK | 输入 | 重新设置的singleCoreK值 |

## 返回值说明

无

## 约束说明

无

## 调用示例

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);

// tailM：M方向上剩余元素个数，tailN：N方向上剩余元素个数，tailK：K方向上剩余元素个数
// 如果是尾核，剩余元素可能会少于单核需要计算的元素。此时，需要使用SetTail重新设置本次计算的元素个数
if (tailM < tiling.singleCoreM || tailN < tiling.singleCoreN || tailK < tiling.singleCoreK) {
    matmulObj.SetTail(tailM, tailN, tailK);
}

mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(gmBias);
}
mm.IterateAll(gm_c);
mm.End();
```
