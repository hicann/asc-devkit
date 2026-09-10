# asc_duplicate_highest_merge

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
<!-- npu="910" id4 -->
- Atlas 训练系列产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="310b" id7 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"c_api/reg_compute/compute/reg_broadcast.h"`。

将源操作数`src`的最高位元素，即下标最大的元素，广播到目的操作数`dst`中被`mask`筛选的位置。`mask`仅筛选`dst`中的元素位置，不影响从`src`中读取的元素；`dst`中未被`mask`筛选的元素保留原值。

本接口与[asc_duplicate_highest](asc_duplicate_highest.md)的广播数据来源相同，区别是本接口采用合并模式保留`dst`中未参与计算的元素。

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

```cpp
// 占位符形式
__simd_callee__ inline void asc_duplicate_highest_merge(vector_<dtype>& dst,
                                                        vector_<dtype> src,
                                                        vector_bool mask)
```

**占位符说明如下：**

- `<dtype>`取值为：`int8_t`、`uint8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

### 函数原型典型示例

```cpp
__simd_callee__ inline void asc_duplicate_highest_merge(vector_float& dst,
                                                        vector_float src,
                                                        vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输入/输出 | 目的操作数（矢量数据寄存器）。被mask筛选的元素写入广播值，未被筛选的元素保留调用接口前的值。数据类型需要与src保持一致。 |
| src | 输入 | 源操作数（矢量数据寄存器），其最高位元素作为待广播的数据。数据类型需要与dst保持一致。 |
| mask | 输入 | 目的操作数元素操作的有效指示（掩码寄存器）。对应比特位为1时，dst中的元素写入src的最高位元素；为0时，dst中的元素保留原值。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- 本接口只能在使用`__simd_vf__`标记的Vector Function内调用，不支持在`__aicore__`函数中直接调用，仅在AIV上生效，在AIC上调用将直接返回。
- 使用`mask`前，需要通过掩码设置或搬入接口完成初始化；未初始化的掩码寄存器内容不确定。
- `mask`仅筛选`dst`中写入广播值的位置，不筛选`src`中的元素。无论`mask`的最高位是否有效，待广播的数据均为`src`的最高位元素。
- 调用接口前需要初始化`dst`，否则未被`mask`筛选的元素值不确定。
- 同一寄存器的数据依赖由硬件保序，无需额外插入同步指令。本接口与前后Reg数据搬运接口之间，如果不同寄存器访问同一UB地址且存在写后读或写后写依赖，需要调用[asc_mem_bar](../reg_sync/asc_mem_bar.md)进行同步。

## 调用示例

以下示例将`src`的最高位元素广播到`dst`的前4个元素，`dst`中的其余元素保留原值。

```cpp
__simd_vf__ inline void duplicate_highest_merge(__ubuf__ float* dst, __ubuf__ float* src)
{
    vector_float dst_reg;
    vector_float src_reg;
    uint32_t active_count = 4;
    uint32_t full_count = 64;
    vector_bool active_mask = asc_update_mask_b32(active_count);
    vector_bool full_mask = asc_update_mask_b32(full_count);

    asc_loadalign(dst_reg, dst);
    asc_loadalign(src_reg, src);
    asc_duplicate_highest_merge(dst_reg, src_reg, active_mask);
    asc_storealign(dst, dst_reg, full_mask);
}
```
