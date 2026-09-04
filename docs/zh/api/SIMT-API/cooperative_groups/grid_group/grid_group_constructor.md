# grid_group_constructor

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/grid_group/grid_group_constructor_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/cooperative_groups.h"`。

`grid_group`不提供默认的构造函数，用户使用`this_grid`函数获取当前Grid对应的`grid_group`对象。

## 函数原型

```cpp
grid_group this_grid()
```

## 参数说明

无

## 返回值说明

当前Grid对应的`grid_group`对象。

## 约束说明

- 仅支持SIMT编程场景，不支持SIMD与SIMT混合编程场景。

## 调用示例

参考[调用示例](sync.md#调用示例)