# asc_and（废弃）

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

**该接口的浮点类型重载已废弃，请根据数据类型使用等位宽的[asc_and](../reg_compute/reg_logic/asc_and.md)整数重载替代。**

根据`mask`对源操作数`src0`、`src1`执行按位与（&）操作，将结果写入目的操作数`dst`。

计算公式如下：

$$
dst_i = src0_i \& src1_i
$$

本接口仅在AIV上生效。

## 函数原型

```c
__simd_callee__ inline void asc_and(vector_<dtype>& dst,
                                    vector_<dtype> src0,
                                    vector_<dtype> src1,
                                    vector_bool mask)
```

### dtype支持数据类型

`dtype`取值为：`fp8_e5m2_t`、`fp8_e4m3fn_t`、`half`、`bfloat16_t`、`float`。

### 函数原型典型示例

```c
__simd_callee__ inline void asc_and(vector_half& dst,
                                    vector_half src0,
                                    vector_half src1,
                                    vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :----- | :------- | :--- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src0 | 输入 | 源操作数（矢量数据寄存器）。 |
| src1 | 输入 | 源操作数（矢量数据寄存器）。 |
| mask | 输入 | 源操作数掩码（掩码寄存器）。对应位置为1时参与计算，为0时不参与计算，未参与计算的元素在输出中置0。 |

## 返回值说明

无

## 约束说明

- 本接口在非AIV上调用直接返回。
- `mask`需通过[掩码设置接口](../defs/type/data_type_definition.md#掩码寄存器)预先赋值后再传入；未赋值的掩码寄存器内容不确定，会导致有效元素位置错误。
- 参与计算的元素个数为VL除以`sizeof(dtype)`。
- `mask`比特位为0时，`dst`对应比特位写0。

## 调用示例

以下示例使用`half`类型的废弃重载，仅用于说明兼容调用。新代码请按功能说明迁移至等位宽的整数重载。

```cpp
vector_half dst;
vector_half src0;
vector_half src1;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_and(dst, src0, src1, mask);
```
