# asc_copy_gm2ub_align

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：不支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
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

提供数据非对齐搬运的功能，将数据从Global Memory (GM)搬运到Unified Buffer（UB），并支持8位/16位/32位数据类型搬运。

本接口支持以下两种数据搬运方式：

- 前n个数据搬运

    若搬运数据长度非32字节对齐，搬运数据会补齐至32字节对齐，支持以下两种填充方式：

    - 手动填充：搬运前调用[asc_set_copy_pad_val](../asc_set_copy_pad_val.md)配置填充值。
    - 自动填充：不调用`asc_set_copy_pad_val`时，由硬件自动填充dummy假数据，dummy假数据的值为数据块的第一个元素的值。

- 高维切分搬运

    高维切分搬运采用Normal模式。若`burst_len`指定的单个数据块长度非32字节对齐，则`burst_count`指定的每个数据块都会分别补齐至32字节对齐。非32字节对齐场景支持以下两种填充方式：

    - 手动填充：搬运前调用`asc_set_copy_pad_val`设置填充值。
    - 自动填充：不调用`asc_set_copy_pad_val`时，由硬件自动填充dummy假数据，dummy假数据的值为数据块的第一个元素的值。

## 函数原型

- 前n个数据搬运

    ```cpp
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ half* dst, __gm__ half* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ float* dst, __gm__ float* src, uint32_t size)
    ```

- 高维切分搬运

    ```cpp
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int16_t* dst, __gm__ int16_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ half* dst, __gm__ half* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int32_t* dst, __gm__ int32_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ float* dst, __gm__ float* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    ```

- 同步计算

    ```cpp
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ half* dst, __gm__ half* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ float* dst, __gm__ float* src, uint32_t size)
    ```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输出 | 目的UB的起始地址。需要32字节对齐。 |
| src | 输入 | 源GM的起始地址。需要1字节对齐。 |
| size | 输入 | 搬运数据大小，单位为字节。取值范围：[0, 2097151]。 |
| burst_count | 输入 | 待搬运的连续传输数据块个数。取值范围：[0, 4095]。 |
| burst_len | 输入 | 待搬运的每个连续传输数据块的长度，单位为字节。取值范围：[0, 2097151]。 |
| left_padding_num | 输入 | 连续搬运数据块左侧需要补充的元素个数。该参数对应的填充数据大小不能超过32字节。 |
| right_padding_num | 输入 | 连续搬运数据块右侧需要补充的元素个数。该参数对应的填充数据大小不能超过32字节。 |
| src_gap | 输入 | 源操作数相邻连续数据块的距离（前面一个数据块的尾与后面一个数据块的头的间隔），单位为字节。 |
| dst_gap | 输入 | 目的操作数相邻连续数据块的距离（前面一个数据块的尾与后面一个数据块的头的间隔），单位为DataBlock（32字节）。 |

## 返回值说明

无

## 流水类型

PIPE_MTE2

## 约束说明

- 各存储单元的空间大小和对齐要求请参考[存储单元说明](../../general_description_and_constraints.md#存储单元说明)。
- 当`burst_count`、`burst_len`中任意一个值为0时，该接口被视为NOP（空操作）。
- 当`size`值为0时，该接口被视为NOP（空操作）。
- 如果本指令与其他指令存在UB地址重叠，必须插入同步指令[asc_sync_notify](../../sync/asc_sync_notify.md)和[asc_sync_wait](../../sync/asc_sync_wait.md)，保证多个指令串行化，防止出现异常数据。
- 同步计算包含同步等待。
- `left_padding_num`、`right_padding_num`对应的填充数据大小均不能超过32字节。

## 调用示例

```cpp
// total_length指参与搬运的数据总长度。src是外部输入的GM内存。
constexpr uint64_t total_length = 128;
__ubuf__ uint8_t dst[total_length];
asc_copy_gm2ub_align(dst, src, total_length * sizeof(uint8_t));
```
