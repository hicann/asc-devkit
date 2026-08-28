# asc_exp_sub（废弃）

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

**输入数据类型为`half`且不带`src_pos`参数的`asc_exp_sub`原型以及`asc_exp_sub_v2`原型已废弃。新接口请参见[asc_exp_sub](../reg_compute/reg_fused/asc_exp_sub.md)。**

这些接口将`src0`与`src1`按元素相减，并计算以差值为指数的自然指数函数，将计算结果写入`dst`。

### 迁移说明

**表1** 新旧原型对应关系

| 旧原型 | 新原型 |
| ------ | ------ |
| `asc_exp_sub(dst, vector_half src0, vector_half src1, mask)` | `asc_exp_sub_half2float(dst, src0, src1, mask, ASC_POSITION_EVEN)` |
| `asc_exp_sub_v2(dst, vector_half src0, vector_half src1, mask)` | `asc_exp_sub_half2float(dst, src0, src1, mask, ASC_POSITION_ODD)` |
| `asc_exp_sub_v2(dst, vector_float src0, vector_float src1, mask)` | `asc_exp_sub(dst, src0, src1, mask)` |

## 函数原型

```c
__simd_callee__ inline void asc_exp_sub(vector_float& dst,
                                        vector_half src0,
                                        vector_half src1,
                                        vector_bool mask)

__simd_callee__ inline void asc_exp_sub_v2(vector_float& dst,
                                           vector_half src0,
                                           vector_half src1,
                                           vector_bool mask)

__simd_callee__ inline void asc_exp_sub_v2(vector_float& dst,
                                           vector_float src0,
                                           vector_float src1,
                                           vector_bool mask)

```

## 参数说明

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| ------ | --------- | ---- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src0 | 输入 | 源操作数0（矢量数据寄存器）。 |
| src1 | 输入 | 源操作数1（矢量数据寄存器）。 |
| mask | 输入 | 掩码寄存器，用于指示参与计算的元素。对应位置为1时参与计算，为0时不参与计算；未参与计算的元素在输出中置0。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../defs/type/data_type_definition.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

无

## 调用示例

```cpp
vector_float dst;
vector_half src0;
vector_half src1;
vector_bool mask;

// 旧接口：读取half源操作数的偶数索引元素。
asc_exp_sub(dst, src0, src1, mask);

// 替代接口：显式指定源操作数读取位置。
asc_exp_sub_half2float(dst, src0, src1, mask, ASC_POSITION_EVEN);
```
