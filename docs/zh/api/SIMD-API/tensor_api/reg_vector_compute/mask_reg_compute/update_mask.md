# update_mask

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

头文件路径为：`"tensor_api/experimental/arch/vector/mask_reg_compute.h"`，命名空间为`asc::te::experimental`。

根据当前剩余待处理元素数的值生成对应长度的有效位掩码，并自动将前剩余待处理元素数减去当前向量长度以更新剩余待处理元素数量。

## 函数原型

```cpp
template <typename DataType>
__simd_callee__ inline reg_tensor<bool> update_mask(uint32_t& remain)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| `DataType` | 输入 | 模板参数，支持的数据类型为b8/b16/b32。 |
| `remain` | 输入/输出 | 元素个数。调用后自动减去当前向量处理单元的元素个数（b8模式、b16模式、b32模式分别为256、128、64）。<br>执行完一次该接口后，`remain = (remain < VL_T) ? 0 : (remain - VL_T)`，`VL_T`表示位宽为VL的矢量数据寄存器中可存放数据类型T的元素个数。 |

## 返回值说明

返回`reg_tensor<bool>`，其`reg`成员是类型为vector_bool的掩码寄存器，其对应的元素为有效数据。

## 约束说明

- 返回对象只保证`reg`成员有效，并不会初始化成员变量`mask`。
- 掩码寄存器数量上限为8。超出限制上限的寄存器数据会写入预留的8KUB内存中，可能会引起性能劣化。编译器会自动复用生命周期结束的寄存器和预留内存，若寄存器与预留内存均存在可用空间，将优先复用寄存器。

## 调用示例

```cpp
uint32_t remain = count;
for (uint32_t offset = 0; offset < count; offset += elementsPerReg) {
    auto tailMask = asc::te::experimental::update_mask<float>(remain);
    auto value = asc::te::experimental::load(src, asc::te::make_coord(offset)).with_mask(tailMask);
    asc::te::experimental::store(dst, asc::te::make_coord(offset), value);
}
```
