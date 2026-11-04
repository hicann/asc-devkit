# gather

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

头文件路径为：`"tensor_api/experimental/arch/vector/gather_compute.h"`，命名空间为`asc::te::experimental`。

根据`index`中逐元素保存的索引，从源寄存器`src`中收集元素，生成新的[reg_tensor](../reg_tensor/reg_tensor.md)。索引以元素个数为单位，不是字节偏移。

设`VL_T`为一个矢量数据寄存器可容纳的`DataType`元素个数，计算关系为`dst.reg[i] = src.reg[index.reg[i] % VL_T]`。例如，`src`的前四个元素为`{10, 20, 30, 40}`，`index`的前四个元素为`{3, 1, 0, 2}`，则结果的前四个元素为`{40, 20, 10, 30}`。

本接口仅在AIV上生效。

## 函数原型

```cpp
template <typename DataType, typename IndexType>
__simd_callee__ inline reg_tensor<DataType> gather(const reg_tensor<DataType>& src, const reg_tensor<IndexType>& index);
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| DataType | 源操作数及返回结果的元素类型，支持的类型组合请参见[数据类型](#数据类型)。 |
| IndexType | 索引寄存器的元素类型，须为与DataType位宽相同的无符号整型。 |

**表2** 函数参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src | 输入 | 源寄存器，类型为`reg_tensor<DataType>`。 |
| index | 输入 | 索引寄存器，类型为`reg_tensor<IndexType>`，其中，`index.reg`提供逐元素索引。 |

## 数据类型

| DataType | IndexType |
| --- | --- |
| int8_t、uint8_t、hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t | uint8_t |
| int16_t、uint16_t、half、bfloat16_t | uint16_t |
| int32_t、uint32_t、float | uint32_t |

## 返回值说明

返回`reg_tensor<DataType>`，其`reg`保存收集后的数据，`mask`为`all_mask<DataType>().reg`，即按结果元素类型设置的全有效掩码。

## 约束说明

- 索引超出源寄存器可容纳的元素范围时，按`index.reg[i] % VL_T`取模后访问。
- 仅需使用部分结果时，应在后续计算或写回前通过[with_mask](../reg_tensor/reg_tensor.md#with_mask)重新绑定有效掩码。

## 调用示例

以下代码片段在VF中调用。源数据和索引各占满一个寄存器，所有索引值及被索引的源元素均已初始化，目的Tensor具有容纳一个寄存器结果的空间。示例先将数据从UB搬入寄存器，再收集并写回全部结果；无需为输入绑定掩码，返回结果携带全有效掩码。

```cpp
template <typename SrcTensor, typename IndexTensor, typename DstTensor>
__simd_vf__ inline void gather_vf(SrcTensor src_tensor, IndexTensor index_tensor, DstTensor dst_tensor)
{
    const auto coord = asc::te::make_coord(0);
    auto src_reg = asc::te::experimental::load(src_tensor, coord);
    auto index_reg = asc::te::experimental::load(index_tensor, coord);

    auto result = asc::te::experimental::gather(src_reg, index_reg);
    asc::te::experimental::store(dst_tensor, coord, result);
}
```
