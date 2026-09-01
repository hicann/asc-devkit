# all_mask

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

头文件路径为：`"tensor_api/arch/vector/experimental/mask_reg_compute.h"`，命名空间为`asc::te::experimental`。

创建`reg_tensor<bool>`，其成员变量reg是与`DataType`元素类型宽度匹配的全有效的掩码寄存器，等价于`make_mask<mask_pattern::all, DataType>()`。

## 函数原型

```cpp
template <typename DataType>
__simd_callee__ inline reg_tensor<bool> all_mask()
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| `DataType` | 输入 | 模板参数，支持的数据类型为b8/b16/b32。 |

## 返回值说明

返回`reg_tensor<bool>`，其`reg`成员是类型为vector_bool的掩码寄存器，其对应的元素全为有效数据。

## 约束说明

- 返回对象只保证`reg`成员有效，并不会初始化成员变量`mask`。
- 掩码寄存器数量上限为8。超出限制上限的寄存器数据会写入预留的8KUB内存中，可能会引起性能劣化。编译器会自动复用生命周期结束的寄存器和预留内存，若寄存器与预留内存均存在可用空间，将优先复用寄存器。

## 调用示例

```cpp
auto fullMask = asc::te::experimental::all_mask<half>();
auto value = asc::te::experimental::load(src, asc::te::make_coord(0)).with_mask(fullMask);
```
