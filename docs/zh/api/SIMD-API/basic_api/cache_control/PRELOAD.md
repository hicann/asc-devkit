# PRELOAD

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
<!-- npu="910" id4 -->
- Atlas 训练系列产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品 AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品 Vector Core：不支持
<!-- end id6 -->
<!-- npu="310b" id7 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"utils/base/sys_macros.h"`。

从当前指令所在的GM地址开始，将后续指令预加载到ICache中。

## 函数原型

```cpp
#define PRELOAD(len)
```

## 参数说明

**表 1**  参数说明

| 参数名 | 输入/输出 | 描述 |
|--------|-----------|------|
| len | 输入 | 预取长度。 |

<!-- npu="950,A3,910b" id8 -->
针对如下型号：
<!-- npu="950" id9 -->
- Ascend 950PR/Ascend 950DT
<!-- end id9 -->
<!-- npu="A3" id10 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品
<!-- end id10 -->
<!-- npu="910b" id11 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品
<!-- end id11 -->

`len`的单位为2K Byte，取值应小于ICache大小除以2K。其中，AIC和AIV的ICache大小分别为32KB和16KB。
<!-- end id8 -->

<!-- npu="310p" id12 -->
- 针对Atlas 推理系列产品 AI Core，`len`不控制预取长度，预取长度固定为128Byte。
<!-- end id12 -->

## 返回值说明

无

## 约束说明

仅支持在Kernel侧调用。

## 调用示例

```cpp
// 从当前指令地址开始预加载4个单位的指令。
PRELOAD(4);
```
