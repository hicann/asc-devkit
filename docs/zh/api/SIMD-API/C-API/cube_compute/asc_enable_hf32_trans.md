# asc_enable_hf32_trans

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
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
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

设置HF32模式取整方式，需要先使用[asc_enable_hf32](./asc_enable_hf32.md)开启HF32取整模式。

## 函数原型

 ```cpp
   __aicore__ inline void asc_enable_hf32_trans(uint32_t mode)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
|:-------|:----------|:------|
| mode | 输入 | HF32取整模式控制入参，uint32_t类型，支持如下2种取值：<br>0：FP32将以最接近偶数的方式四舍五入为HF32。<br>1：FP32将以向零靠近的方式四舍五入为HF32。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

mode仅支持如下2种取值：<br>0：FP32将以最接近偶数的方式四舍五入为HF32。<br>1：FP32将以向零靠近的方式四舍五入为HF32。

## 调用示例

```cpp
uint32_t mode = 0;
asc_enable_hf32_trans(mode);
```
