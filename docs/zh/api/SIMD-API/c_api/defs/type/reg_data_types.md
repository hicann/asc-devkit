# 寄存器类型

Ascend 950PR&950DT系列产品（NPU架构版本3510）的C API提供矢量数据寄存器、掩码寄存器、非对齐搬运寄存器和地址寄存器类型。寄存器类型主要在AIV的Vector Function（`__simd_vf__`标记的函数）内使用，最细粒度公共头文件为`c_api/defs/type.h`。

各类寄存器的用途如下。

| C API类型 | 用途 |
| --- | --- |
| `vector_<dtype>`，例如`vector_int8_t`、`vector_half`、`vector_float` | 保存矢量计算的数据操作数和结果。NPU架构版本3510的矢量数据寄存器长度为256字节。 |
| `vector_bool` | 按元素控制矢量指令是否生效。掩码寄存器长度为256 bit。 |
| `vector_load_unalign` | 保存UB非对齐加载的中间状态。 |
| `vector_store_unalign` | 保存UB非对齐存储的中间状态。 |
| `addr_reg` | 保存地址偏移，配合支持地址寄存器的Reg接口使用。 |

## 使用说明

- 矢量数据寄存器类型根据元素位宽分为b8、b16、b32和b64类别。完整类型列表和打包类型说明请参见[寄存器类型定义](data_type_definition.md)。
- `vector_bool`既可以由掩码生成接口构造，也可以通过[asc_get_mask_spr](../../reg_compute/reg_mask/asc_get_mask_spr.md)读取特殊寄存器中的掩码值。
- Reg与UB之间的数据交换通过[Reg加载](../../reg_compute/load/reg_load_overview.md)和[Reg存储](../../reg_compute/store/reg_store_overview.md)接口完成。GM中的数据不能直接加载到Reg，需要先搬运到UB。
- `vector_load_unalign`和`vector_store_unalign`只用于对应的非对齐加载、存储接口序列，不用于一般矢量计算。
- `addr_reg`可通过[asc_update_addr_reg](../../reg_compute/reg_addr_reg/asc_update_addr_reg.md)更新。寄存器对象的有效范围由Vector Function和具体接口约束共同决定。
