# asc_shiftright

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

根据掩码mask对输入数据src0，按照src1对应元素进行右移操作，完成后将结果写入dst中。根据源操作数的数据类型，右移操作分为以下两种情况：
    
- **数据类型为无符号类型：执行逻辑右移。**

逻辑右移会将二进制数整体向右移动指定的位数，最低位被丢弃，最高位用0填充。例如，二进制数1010101010101010（uint16\_t类型）逻辑右移1位后，结果为0101010101010101。
- **数据类型为有符号类型：执行算术右移。**

算术右移会将二进制数整体向右移动指定的位数，最低位被丢弃，最高位复制符号位。例如，二进制数1010101010101010（int16\_t类型）算术右移1位后，结果为1101010101010101；算术右移3位后，结果为1111010101010101。

$$
dst_i = src0_i \gg src1_i
$$

## 函数原型

```cpp
__simd_callee__ inline void asc_shiftright(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
__simd_callee__ inline void asc_shiftright(vector_uint8_t& dst, vector_uint8_t src0, vector_int8_t src1, vector_bool mask)
__simd_callee__ inline void asc_shiftright(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
__simd_callee__ inline void asc_shiftright(vector_uint16_t& dst, vector_uint16_t src0, vector_int16_t src1, vector_bool mask)
__simd_callee__ inline void asc_shiftright(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
__simd_callee__ inline void asc_shiftright(vector_uint32_t& dst, vector_uint32_t src0, vector_int32_t src1, vector_bool mask)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。|
| src0 | 输入 | 源操作数（矢量数据寄存器）。|
| src1 | 输入 | 源操作数（矢量数据寄存器）。|
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[data_type_definition.md](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- 对于逻辑位移（无符号数据类型），如果位移量大于数据类型位宽，则输出为0。
- 对于算术位移（有符号数据类型），如果src0小于0，位移量大于数据类型位宽，则输出-1；如果src0大于0，位移量大于数据类型位宽，则输出0。
- src1不支持设置为负数，负数行为未定义。

## 调用示例

```cpp
vector_uint32_t dst;
vector_uint32_t src0;
vector_int32_t src1;
vector_bool mask = asc_create_mask_b32(PAT_ALL);
asc_loadalign(src0, src0_addr); // src0_addr是外部输入的UB内存空间地址。
asc_loadalign(src1, src1_addr); // src1_addr是外部输入的UB内存空间地址。
asc_shiftright(dst, src0, src1, mask);
```