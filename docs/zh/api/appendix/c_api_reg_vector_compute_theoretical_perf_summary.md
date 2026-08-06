# C-API-Reg矢量计算接口理论性能汇总

本节汇总介绍了C-API Reg矢量计算接口的理论性能，本节内容仅针对如下型号生效：

<!-- npu="950" id1 -->
Ascend 950PR/Ascend 950DT
<!-- end id1 -->

本节汇总的理论性能均为单指令理论性能，软仿实现的接口和数据类型不提供理论性能。

**表1**  基础算术

| 接口 | 源操作数数据类型 | 目的操作数数据类型 | 理论并行度（element/cycle） |
| -------- | ---------------- | ---------------- | ---------------- |
| asc_abs | int8_t | int8_t | 128 |
| asc_abs | int16_t | int16_t | 64 |
| asc_abs | half | half | 128 |
| asc_abs | int32_t | int32_t | 32 |
| asc_abs | float | float | 64 |
| asc_add_scalar | int8_t | int8_t | 128 |
| asc_add_scalar | uint8_t | uint8_t | 128 |
| asc_add_scalar | int16_t | int16_t | 64 |
| asc_add_scalar | uint16_t | uint16_t | 64 |
| asc_add_scalar | half | half | 64 |
| asc_add_scalar | bfloat16_t | bfloat16_t | 64 |
| asc_add_scalar | int32_t | int32_t | 32 |
| asc_add_scalar | uint32_t | uint32_t | 32 |
| asc_add_scalar | float | float | 32 |
| asc_ln | half | half | 16 |
| asc_ln | float | float | 16 |
| asc_max_scalar | int8_t | int8_t | 256 |
| asc_max_scalar | uint8_t | uint8_t | 256 |
| asc_max_scalar | int16_t | int16_t | 128 |
| asc_max_scalar | uint16_t | uint16_t | 128 |
| asc_max_scalar | half | half | 128 |
| asc_max_scalar | bfloat16_t | bfloat16_t | 128 |
| asc_max_scalar | int32_t | int32_t | 64 |
| asc_max_scalar | uint32_t | uint32_t | 64 |
| asc_max_scalar | float | float | 64 |
| asc_min_scalar | int8_t | int8_t | 256 |
| asc_min_scalar | uint8_t | uint8_t | 256 |
| asc_min_scalar | int16_t | int16_t | 128 |
| asc_min_scalar | uint16_t | uint16_t | 128 |
| asc_min_scalar | half | half | 128 |
| asc_min_scalar | int32_t | int32_t | 64 |
| asc_min_scalar | uint32_t | uint32_t | 64 |
| asc_min_scalar | float | float | 64 |
| asc_mul_scalar | int16_t | int16_t | 64 |
| asc_mul_scalar | uint16_t | uint16_t | 64 |
| asc_mul_scalar | half | half | 128 |
| asc_mul_scalar | int32_t | int32_t | 32 |
| asc_mul_scalar | uint32_t | uint32_t | 32 |
| asc_mul_scalar | float | float | 64 |
| asc_neg | int8_t | int8_t | 128 |
| asc_neg | int16_t | int16_t | 64 |
| asc_neg | half | half | 128 |
| asc_neg | int32_t | int32_t | 32 |
| asc_neg | float | float | 64 |
| asc_relu | half | half | 128 |
| asc_relu | int32_t | int32_t | 64 |
| asc_relu | float | float | 64 |

**表2**  逻辑算术

| 接口 | 源操作数数据类型 | 目的操作数数据类型 | 理论并行度（element/cycle） |
| -------- | ---------------- | ---------------- | ---------------- |
| asc_and | bool | bool | 128 |
| asc_and | int8_t | int8_t | 256 |
| asc_and | uint8_t | uint8_t | 256 |
| asc_and | fp8_e5m2_t | fp8_e5m2_t | 256 |
| asc_and | fp8_e4m3fn_t | fp8_e4m3fn_t | 256 |
| asc_and | int16_t | int16_t | 128 |
| asc_and | uint16_t | uint16_t | 128 |
| asc_and | half | half | 128 |
| asc_and | bfloat16_t | bfloat16_t | 128 |
| asc_and | int32_t | int32_t | 64 |
| asc_and | uint32_t | uint32_t | 64 |
| asc_and | float | float | 64 |
| asc_or | bool | bool | 128 |
| asc_or | int8_t | int8_t | 256 |
| asc_or | uint8_t | uint8_t | 256 |
| asc_or | int16_t | int16_t | 128 |
| asc_or | uint16_t | uint16_t | 128 |
| asc_or | half | half | 128 |
| asc_or | bfloat16_t | bfloat16_t | 128 |
| asc_or | int32_t | int32_t | 64 |
| asc_or | uint32_t | uint32_t | 64 |
| asc_or | float | float | 64 |

**表3**  复合计算

| 接口 | 源操作数数据类型 | 目的操作数数据类型 | 理论并行度（element/cycle） |
| -------- | ---------------- | ---------------- | ---------------- |
| asc_abs_sub | half | half | 128 |
| asc_abs_sub | float | float | 64 |
| asc_axpy | half | half | 128 |
| asc_axpy | float | float | 64 |

**表4**  比较与选择

| 接口 | 源操作数数据类型 | 目的操作数数据类型 | 理论并行度（element/cycle） |
| -------- | ---------------- | ---------------- | ---------------- |
| asc_eq | int8_t | bool | 256 |
| asc_eq | uint8_t | bool | 256 |
| asc_eq | int16_t | bool | 128 |
| asc_eq | uint16_t | bool | 128 |
| asc_eq | half | bool | 128 |
| asc_eq | bfloat16_t | bool | 128 |
| asc_eq | int32_t | bool | 64 |
| asc_eq | uint32_t | bool | 64 |
| asc_eq | float | bool | 64 |
| asc_ge | int8_t | bool | 256 |
| asc_ge | uint8_t | bool | 256 |
| asc_ge | int16_t | bool | 128 |
| asc_ge | uint16_t | bool | 128 |
| asc_ge | half | bool | 128 |
| asc_ge | bfloat16_t | bool | 128 |
| asc_ge | int32_t | bool | 64 |
| asc_ge | uint32_t | bool | 64 |
| asc_ge | float | bool | 64 |
| asc_ge_scalar | int8_t | bool | 256 |
| asc_ge_scalar | uint8_t | bool | 256 |
| asc_ge_scalar | int16_t | bool | 128 |
| asc_ge_scalar | uint16_t | bool | 128 |
| asc_ge_scalar | half | bool | 128 |
| asc_ge_scalar | bfloat16_t | bool | 128 |
| asc_ge_scalar | int32_t | bool | 64 |
| asc_ge_scalar | uint32_t | bool | 64 |
| asc_ge_scalar | float | bool | 64 |
| asc_gt | int8_t | bool | 256 |
| asc_gt | uint8_t | bool | 256 |
| asc_gt | int16_t | bool | 128 |
| asc_gt | uint16_t | bool | 128 |
| asc_gt | half | bool | 128 |
| asc_gt | bfloat16_t | bool | 128 |
| asc_gt | int32_t | bool | 64 |
| asc_gt | uint32_t | bool | 64 |
| asc_gt | float | bool | 64 |
| asc_gt_scalar | int8_t | bool | 256 |
| asc_gt_scalar | uint8_t | bool | 256 |
| asc_gt_scalar | int16_t | bool | 128 |
| asc_gt_scalar | uint16_t | bool | 128 |
| asc_gt_scalar | half | bool | 128 |
| asc_gt_scalar | bfloat16_t | bool | 128 |
| asc_gt_scalar | int32_t | bool | 64 |
| asc_gt_scalar | uint32_t | bool | 64 |
| asc_gt_scalar | float | bool | 64 |
| asc_lt | int8_t | bool | 256 |
| asc_lt | uint8_t | bool | 256 |
| asc_lt | int16_t | bool | 128 |
| asc_lt | uint16_t | bool | 128 |
| asc_lt | half | bool | 128 |
| asc_lt | bfloat16_t | bool | 128 |
| asc_lt | int32_t | bool | 64 |
| asc_lt | uint32_t | bool | 64 |
| asc_lt | float | bool | 64 |
| asc_ne | int8_t | bool | 256 |
| asc_ne | uint8_t | bool | 256 |
| asc_ne | int16_t | bool | 128 |
| asc_ne | uint16_t | bool | 128 |
| asc_ne | half | bool | 128 |
| asc_ne | bfloat16_t | bool | 128 |
| asc_ne | int32_t | bool | 64 |
| asc_ne | uint32_t | bool | 64 |
| asc_ne | float | bool | 64 |
| asc_ne_scalar | int8_t | bool | 256 |
| asc_ne_scalar | uint8_t | bool | 256 |
| asc_ne_scalar | int16_t | bool | 128 |
| asc_ne_scalar | uint16_t | bool | 128 |
| asc_ne_scalar | half | bool | 128 |
| asc_ne_scalar | bfloat16_t | bool | 128 |
| asc_ne_scalar | int32_t | bool | 64 |
| asc_ne_scalar | uint32_t | bool | 64 |
| asc_ne_scalar | float | bool | 64 |
| asc_select | bool | bool | 256 |
| asc_select | int8_t | int8_t | 256 |
| asc_select | uint8_t | uint8_t | 256 |
| asc_select | hifloat8_t | hifloat8_t | 256 |
| asc_select | fp8_e5m2_t | fp8_e5m2_t | 256 |
| asc_select | fp8_e4m3fn_t | fp8_e4m3fn_t | 256 |
| asc_select | int16_t | int16_t | 128 |
| asc_select | uint16_t | uint16_t | 128 |
| asc_select | half | half | 128 |
| asc_select | bfloat16_t | bfloat16_t | 128 |
| asc_select | int32_t | int32_t | 64 |
| asc_select | uint32_t | uint32_t | 64 |
| asc_select | float | float | 64 |

**表5**  类型转换

| 接口 | 源操作数数据类型 | 目的操作数数据类型 | 理论并行度（element/cycle） |
| -------- | ---------------- | ---------------- | ---------------- |
| asc_int322float | int32_t | float | 32 |
| asc_int322int16 | int32_t | int16_t | 64 |
| asc_int322int64 | int32_t | int64_t | 32 |
| asc_int322uint16 | int32_t | uint16_t | 64 |

**表6**  归约计算

| 接口 | 源操作数数据类型 | 目的操作数数据类型 | 理论并行度（element/cycle） |
| -------- | ---------------- | ---------------- | ---------------- |
| asc_reduce_max | int16_t | int16_t | 128 |
| asc_reduce_max | uint16_t | uint16_t | 128 |
| asc_reduce_max | half | half | 128 |
| asc_reduce_max | int32_t | int32_t | 64 |
| asc_reduce_max | uint32_t | uint32_t | 64 |
| asc_reduce_max | float | float | 64 |
| asc_reduce_max_datablock | int16_t | int16_t | 128 |
| asc_reduce_max_datablock | uint16_t | uint16_t | 128 |
| asc_reduce_max_datablock | half | half | 128 |
| asc_reduce_max_datablock | int32_t | int32_t | 64 |
| asc_reduce_max_datablock | uint32_t | uint32_t | 64 |
| asc_reduce_max_datablock | float | float | 64 |
| asc_reduce_min | int16_t | int16_t | 128 |
| asc_reduce_min | uint16_t | uint16_t | 128 |
| asc_reduce_min | half | half | 128 |
| asc_reduce_min | int32_t | int32_t | 64 |
| asc_reduce_min | uint32_t | uint32_t | 64 |
| asc_reduce_min | float | float | 64 |
| asc_reduce_min_datablock | int16_t | int16_t | 128 |
| asc_reduce_min_datablock | uint16_t | uint16_t | 128 |
| asc_reduce_min_datablock | half | half | 128 |
| asc_reduce_min_datablock | int32_t | int32_t | 64 |
| asc_reduce_min_datablock | uint32_t | uint32_t | 64 |
| asc_reduce_min_datablock | float | float | 64 |
| asc_reduce_sum | int16_t | int32_t | 64 |
| asc_reduce_sum | uint16_t | uint32_t | 64 |
| asc_reduce_sum | half | half | 128 |
| asc_reduce_sum | int32_t | int32_t | 64 |
| asc_reduce_sum | uint32_t | uint32_t | 64 |
| asc_reduce_sum | float | float | 64 |
| asc_reduce_sum_datablock | int16_t | int32_t | 64 |
| asc_reduce_sum_datablock | uint16_t | uint32_t | 64 |
| asc_reduce_sum_datablock | half | half | 128 |
| asc_reduce_sum_datablock | int32_t | int32_t | 64 |
| asc_reduce_sum_datablock | uint32_t | uint32_t | 64 |
| asc_reduce_sum_datablock | float | float | 64 |

**表7**  直方图计算

| 接口 | 源操作数数据类型 | 目的操作数数据类型 | 理论并行度（element/cycle） |
| -------- | ---------------- | ---------------- | ---------------- |
| asc_cumulative_histogram_bin0 | uint8_t | uint16_t | 64 |
| asc_cumulative_histogram_bin1 | uint8_t | uint16_t | 64 |
