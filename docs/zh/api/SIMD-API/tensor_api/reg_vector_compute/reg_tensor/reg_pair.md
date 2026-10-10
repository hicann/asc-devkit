# reg_pair

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"tensor_api/experimental/arch/vector/reg_tensor.h"`，命名空间为`asc::te::experimental`。

保存两个元素类型可以相同或不同的`reg_tensor`，用于`deinterleave`等双结果接口。`first`的元素类型由`DataType`指定，`second`的元素类型由`CarryType`指定。`CarryType`默认与`DataType`相同。

## 定义原型

```cpp
template <typename DataType, typename CarryType = DataType>
struct reg_pair {
    reg_tensor<DataType> first;
    reg_tensor<CarryType> second;
};
```

## 参数说明

| 参数名 | 描述 |
| --- | --- |
| `DataType` | 第一个寄存器Tensor的元素类型。 |
| `CarryType` | 第二个寄存器Tensor的元素类型，默认值为`DataType`。 |

## 成员说明

| 成员名 | 描述 |
| --- | --- |
| `first` | 第一个寄存器Tensor，类型为`reg_tensor<DataType>`。 |
| `second` | 第二个寄存器Tensor，类型为`reg_tensor<CarryType>`。 |

## 调用示例

接收`deinterleave`接口返回的两个寄存器Tensor：

```cpp
auto result = asc::te::experimental::deinterleave<float>(mask0, mask1);
asc::te::experimental::reg_tensor<bool> first = result.first;
asc::te::experimental::reg_tensor<bool> second = result.second;
```

接收[add_carry](../basic_arithmetic/add_carry.md)接口返回的两种元素类型的寄存器Tensor，`first`保存`uint32_t`类型的加法结果低32位，`second`保存`bool`类型的进位。

```cpp
auto src0 = asc::te::experimental::fill(uint32_t{0xffffffff});
auto src1 = asc::te::experimental::fill(uint32_t{1});
asc::te::experimental::reg_pair<uint32_t, bool> result =
    asc::te::experimental::add_carry(src0, src1);
asc::te::experimental::reg_tensor<uint32_t> sum = result.first;
asc::te::experimental::reg_tensor<bool> carry = result.second;
```
