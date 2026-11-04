# pack

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

头文件路径为：`"tensor_api/experimental/arch/vector/reg_compute_sel.h"`，命名空间为`asc::te::experimental`。

将源寄存器各元素的低半位宽数据按原顺序压缩，写入返回寄存器的低半部分或高半部分。`part`选择的是结果的存放位置，不是源元素中待提取的高位或低位。

- T为int16_t或uint16_t时，提取各元素的低8位，结果元素类型为uint8_t。
- T为int32_t或uint32_t时，提取各元素的低16位，结果元素类型为uint16_t。
- T为bool时，从掩码寄存器每两个比特中提取低位比特，即索引为0、2、4等偶数位置的比特，结果仍为掩码寄存器。

有符号整型输入同样按位提取低半位宽，返回无符号整型寄存器，不保留源元素的符号。

本接口仅在AIV上生效。`part`为`high_low_part::low`时调用[asc_pack_to_low](../../../c_api/reg_compute/reg_permute_sel/asc_pack_to_low.md)，为`high_low_part::high`时调用[asc_pack_to_high](../../../c_api/reg_compute/reg_permute_sel/asc_pack_to_high.md)。

## 函数原型

```cpp
template <high_low_part part = high_low_part::low, typename T>
__simd_callee__ inline decltype(auto) pack(reg_tensor<T> src);
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| part | 压缩结果的存放位置。`high_low_part::low`表示结果寄存器的低半部分，`high_low_part::high`表示高半部分。默认值为`high_low_part::low`。 |
| T | 源寄存器的元素类型，支持的类型请参见[数据类型](#数据类型)。 |

**表2** 函数参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src | 输入 | 源操作数，类型为[reg_tensor&lt;T&gt;](../reg_tensor/reg_tensor.md)。 |

## 数据类型

| T | 返回值类型 |
| --- | --- |
| bool | reg_tensor&lt;bool&gt; |
| int16_t | reg_tensor<uint8_t> |
| uint16_t | reg_tensor<uint8_t> |
| int32_t | reg_tensor<uint16_t> |
| uint32_t | reg_tensor<uint16_t> |

## 返回值说明

返回类型由T推导，具体映射见数据类型表。`reg`的指定半部分保存压缩结果，`mask`为`all_mask<结果元素类型>().reg`。全有效掩码不代表未写入的另一半具有可用的结果数据。

## 约束说明

- 压缩计算不使用`src.mask`屏蔽元素，须保证参与压缩的源数据已初始化。
- 返回值的`mask`按结果元素类型设置为全有效掩码，不会根据`part`限制为指定半部分。后续计算或写回前，须根据结果所在的半部分重新绑定合适的掩码，避免使用未写入的另一半数据，参见[with_mask](../reg_tensor/reg_tensor.md#with_mask)。

## 调用示例

以下代码片段在VF中调用。`src.reg`已初始化，提取低8位并写入结果低半部分，无需为源寄存器绑定掩码。返回对象默认携带全有效掩码，因此通过[make_mask](../mask_reg_compute/make_mask.md)为结果重新绑定uint8_t类型的低半部分掩码，避免使用另一半的数据。

```cpp
__simd_callee__ inline asc::te::experimental::reg_tensor<uint8_t> pack_example(
    asc::te::experimental::reg_tensor<uint16_t> src)
{
    auto result = asc::te::experimental::pack<asc::te::experimental::high_low_part::low>(src);
    const auto low_mask = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::half, uint8_t>();
    result.with_mask(low_mask);
    return result;
}
```
