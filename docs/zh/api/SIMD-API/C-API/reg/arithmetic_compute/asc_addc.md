# asc_addc

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

该接口根据mask，对源操作数src0、src1及输入进位carry_src进行按元素求和操作，将结果写入目的操作数dst，同时将每个元素的进位结果写入carry（存放进位的掩码寄存器）。
 	 
Carry flag（进位/借位标志）用于表示加法进位或者减法无借位。若src0，src1，carry_src输入按位相加后最高位有进位，在carry中对应位置每4bit设置1，否则写0。

计算公式如下：

$$
\{carry_i, dst_i\} = src0_i + src1_i + carry\_src_i
$$

![](../../figures/asc_addc_1.png)

以uint64_t类型数据计算0xFFFFFFFF FFFFFFFF + 0x00000000 00000008 = 0x00000000 00000007为例，asc_add/asc_addc接口的适用场景如下图：

![](../../figures/asc_addc_2.png)

## 函数原型

```cpp
__simd_callee__ inline void asc_addc(vector_bool& carry, vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool carry_src, vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| carry | 输出 | 目的操作数（掩码寄存器）。存储加法计算后的进位数据。 |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src0 | 输入 | 源操作数（矢量数据寄存器）。 |
| src1 | 输入 |源操作数（矢量数据寄存器）。 |
| carry_src | 输入 | 源操作数（掩码寄存器），进位数据。 |
| mask | 输入 | 源操作数掩码（掩码寄存器）。用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- mask控制源操作数是否参与计算，源操作数不参与计算的元素在输出对应位置置零。

- 运算输出完整计算结果（包含进位位），不受[asc_set_ctrl](../../sys_var/asc_set_ctrl.md)影响，硬件不会对输出进行饱和或截断。

## 调用示例

```cpp
__simd_vf__ inline void addc_vf(__ubuf__ uint32_t* dst_addr, __ubuf__ uint32_t* src0_addr, __ubuf__ uint32_t* src1_addr, __ubuf__ uint32_t* carry_src_addr, uint32_t count, uint32_t one_repeat_size, uint16_t one_block_size, uint16_t repeat_time)
{
    vector_uint32_t src0;
    vector_uint32_t src1;
    vector_uint32_t dst;
    vector_bool carry;
    vector_bool carry_src;
    vector_bool mask;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        mask = asc_update_mask_b32(count);
        asc_loadalign(src0, src0_addr, one_repeat_size);
        asc_loadalign(src1, src1_addr, one_repeat_size);
        asc_loadalign_postupdate(carry_src, carry_src_addr, one_repeat_size);
        asc_addc(carry, dst, src0, src1, carry_src, mask);
        asc_storealign(dst_addr, dst, one_block_size, mask);
    }
}
```
