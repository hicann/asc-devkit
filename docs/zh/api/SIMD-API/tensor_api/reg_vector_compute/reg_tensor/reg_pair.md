# reg_pair

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
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"tensor_api/arch/vector/experimental/reg_tensor.h"`，命名空间为`asc::te::experimental`。

保存两个相同元素类型的`reg_tensor`，用于`deinterleave`等双结果接口。

## 定义原型

```cpp
template <typename DataType>
struct reg_pair {
    reg_tensor<DataType> first;
    reg_tensor<DataType> second;
};
```

## 参数说明

| 参数名 | 描述 |
| --- | --- |
| `DataType` | `reg_tensor<DataType> first`和`reg_tensor<DataType> second`的元素类型。 |

## 成员说明

| 成员名 | 描述 |
| --- | --- |
| `first` | 第一个矢量寄存器。 |
| `second` | 第二个矢量寄存器。 |

## 调用示例

```cpp
auto result = asc::te::experimental::deinterleave<float>(mask0, mask1);
asc::te::experimental::reg_tensor<bool> first = result.first;
asc::te::experimental::reg_tensor<bool> second = result.second;
```
