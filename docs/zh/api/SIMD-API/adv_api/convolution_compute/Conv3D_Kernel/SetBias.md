# SetBias

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/convolution_compute/Conv3D_Kernel/SetBias_res.md#id1 -->

## 功能说明

设置偏置矩阵Bias。

## 函数原型

```
__aicore__ inline void SetBias(const AscendC::GlobalTensor<BiasT>& bias)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| bias | 输入 | Bias在Global Memory上的地址。类型为[GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md)。偏置矩阵Bias支持的数据类型为：half、bfloat16_t。 |

## 返回值说明

无

## 约束说明

在卷积计算中，如果涉及偏置矩阵Bias，必须调用此接口；若卷积计算不涉及Bias，则不应调用此接口。

## 调用示例

```
GlobalTensor<float> biasGm;
biasGm.SetGlobalBuffer(reinterpret_cast<__gm__ half*>(bias));
if (biasFlag) {
    conv3dApi.SetBias(biasGm);
}
```
