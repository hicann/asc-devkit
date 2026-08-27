# asc_storealign_pack_v2（废弃）

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

**该接口已废弃。请使用[asc_storealign_pack_quarter](../reg_compute/store/asc_storealign_pack_quarter.md)实现此功能。**

将矢量数据寄存器中由`mask`指示的有效32bit元素的低8bit数据压缩搬出到Unified Buffer（UB）。本接口通过`int32_t`类型的`offset`传入偏移量，用户可以选择更新偏移量或目的操作数地址。

## 函数原型

```cpp
__simd_callee__ inline void asc_storealign_pack_v2(__ubuf__ int32_t* dst_align32b, vector_int32_t src, int32_t offset, vector_bool mask)
__simd_callee__ inline void asc_storealign_pack_v2(__ubuf__ uint32_t* dst_align32b, vector_uint32_t src, int32_t offset, vector_bool mask)
__simd_callee__ inline void asc_storealign_pack_v2(__ubuf__ float* dst_align32b, vector_float src, int32_t offset, vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst_align32b | 输出 | 目的操作数的起始地址，需32字节对齐。 |
| src | 输入 | 源操作数（矢量数据寄存器）。支持的数据类型为`int32_t`、`uint32_t`、`float`。 |
| offset | 输入 | 相对`dst_align32b`起始地址的偏移量，类型为`int32_t`，单位为元素。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示参与搬出的元素。对应位置为1时参与搬出，为0时不参与搬出。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../defs/type/data_type_definition.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- `dst_align32b`起始地址需32字节对齐。
- 通过`offset`偏移后的实际访问地址需落在UB地址范围内，且仍需32字节对齐。
- `mask`需通过掩码设置接口预先赋值后再传入。

## 调用示例

```cpp
__ubuf__ int32_t* dst_align32b = (__ubuf__ int32_t*)asc_get_phy_buf_addr(0);
vector_int32_t src;
int32_t offset = 0;
vector_bool mask = asc_create_mask_b32(PAT_ALL);
asc_storealign_pack_v2(dst_align32b, src, offset, mask);
```
