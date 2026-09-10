# asc_pack_to_low

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

头文件路径为：`"c_api/reg_compute/compute/reg_permute_sel.h"`。

将源操作数中每个元素的低半部分比特压缩后，依次写入目的操作数的低半部分，目的操作数的高半部分置0。对于b16源操作数，选取每个元素的低8位；对于b32源操作数，选取每个元素的低16位。对于掩码寄存器，选取源操作数中偶数索引的比特，依次写入目的操作数的低半部分。

支持通过引用参数或函数返回值输出结果。与[asc_pack_to_high](asc_pack_to_high.md)的区别是，本接口将压缩结果写入目的操作数的低半部分。

**图1** asc_pack_to_low压缩过程

![asc_pack_to_low压缩过程](../../../../figures/pack_low_draft.png)

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

```cpp
// 通过引用参数输出结果（占位符形式）
__simd_callee__ inline void asc_pack_to_low(vector_<dtype_dst>& dst,
                                            vector_<dtype_src> src)

// 通过函数返回值返回结果（占位符形式）
__simd_callee__ inline vector_<dtype_dst> asc_pack_to_low(vector_<dtype_src> src)
```

**占位符说明如下：**

- `<dtype_src>`取值为：`bool`、`int16_t`、`uint16_t`、`int32_t`、`uint32_t`。
- `<dtype_dst>`由`<dtype_src>`确定：`bool`对应`bool`，`int16_t`和`uint16_t`对应`uint8_t`，`int32_t`和`uint32_t`对应`uint16_t`。

### 函数原型典型示例

```cpp
// 通过引用参数输出结果
__simd_callee__ inline void asc_pack_to_low(vector_uint16_t& dst,
                                            vector_uint32_t src)

// 通过函数返回值返回结果
__simd_callee__ inline vector_uint16_t asc_pack_to_low(vector_uint32_t src)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数，类型为矢量数据寄存器或掩码寄存器，仅通过引用参数输出结果的函数原型包含该参数。低半部分保存压缩结果，高半部分置0。数据类型需要与src满足函数原型中规定的对应关系。 |
| src | 输入 | 源操作数，类型为矢量数据寄存器或掩码寄存器。对于矢量数据寄存器，提取每个元素的低半部分比特；对于掩码寄存器，提取偶数索引的比特。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

- 通过引用参数输出结果的函数原型无返回值，压缩结果写入`dst`。
- 通过函数返回值输出结果的函数原型返回保存压缩结果的矢量数据寄存器或掩码寄存器，类型为与`dtype_dst`对应的`vector_<dtype_dst>`。

## 流水类型

PIPE_V

## 约束说明

- 通过引用参数输出结果的函数原型在非AIV上调用时直接返回。
- 通过函数返回值输出结果的函数原型在非AIV上调用时返回对应矢量类型的默认构造值。
- 本接口只能在使用`__simd_vf__`标记的Vector Function内调用，不支持在`__aicore__`函数中直接调用。
- 对矢量数据寄存器进行压缩时，仅保留每个源元素的低半部分比特，源元素的高半部分比特被丢弃，不进行饱和处理。
- 同一寄存器的数据依赖由硬件保序，无需额外插入同步指令。本接口与前后Reg数据搬运接口之间，如果不同寄存器访问同一UB地址且存在写后读或写后写依赖，需要调用[asc_mem_bar](../reg_sync/asc_mem_bar.md)进行同步。

## 调用示例

以下示例从`vector_uint32_t`源操作数的每个元素中提取低16位，并通过函数返回值将结果写入`vector_uint16_t`目的操作数的低半部分。

```cpp
__simd_vf__ inline void pack_to_low(__ubuf__ uint16_t* dst, __ubuf__ uint32_t* src)
{
    vector_uint32_t src_reg;
    asc_loadalign(src_reg, src);
    vector_uint16_t dst_reg = asc_pack_to_low(src_reg);

    vector_bool mask = asc_create_mask_b16(PAT_ALL);
    asc_storealign(dst, dst_reg, mask);
}
```

完整的算子样例请参见[数据重排样例](../../../../../../../examples/02_simd_c_api/03_c_api/02_reg_vector_compute/data_relayout)。
