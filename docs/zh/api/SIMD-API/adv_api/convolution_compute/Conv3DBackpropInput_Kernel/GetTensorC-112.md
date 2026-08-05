# GetTensorC

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：不支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->

## 功能说明

在完成Iterate操作后调用本接口，获取结果矩阵块，完成数据从L0C到GM的搬运。此接口与[Iterate](Iterate-111.md)接口配合使用，用于在Iterate执行迭代计算后，获取结果矩阵。

## 函数原型

```
template <bool sync = true>
__aicore__ inline void GetTensorC(const AscendC::GlobalTensor<DstT>& output, uint8_t enAtomic = 0, bool enSequentialWrite = false)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| sync | 预留参数，用户无需感知。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| output | 输入 | 将计算结果搬至Global Memory的GM地址。 |
| enAtomic | 输入 | 预留参数，用户无需感知。 |
| enSequentialWrite | 输入 | 预留参数，用户无需感知。 |

## 返回值说明

无

## 约束说明

GetTensorC接口必须在Iterate后进行调用，完成卷积反向实现，调用顺序如下。

```
while (Iterate()) {
    GetTensorC();
}
```

## 调用示例

```
while (gradInput_.Iterate()) {
    gradInput_.GetTensorC(gradInputGm_[offsetC_]);
}
```
