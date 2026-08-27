# asc_copy_gm2l1_align

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

对矩阵数据进行实时padding，完成padding后将数据从Global Memory搬运到L1 Buffer。需要与[asc_set_gm2l1_loop_size](asc_set_gm2l1_loop_size.md)、[asc_set_gm2l1_loop1_stride](asc_set_gm2l1_loop1_stride.md)、[asc_set_gm2l1_loop2_stride](asc_set_gm2l1_loop2_stride.md)和[asc_set_gm2l1_pad](asc_set_gm2l1_pad.md)配合使用。

## 函数原型

- 常规搬运

    ```cpp
    __aicore__ inline void asc_copy_gm2l1_align(__cbuf__ int8_t* dst, __gm__ int8_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align(__cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align(__cbuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align(__cbuf__ int16_t* dst, __gm__ int16_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align(__cbuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align(__cbuf__ half* dst, __gm__ half* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align(__cbuf__ int32_t* dst, __gm__ int32_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align(__cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count,uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align(__cbuf__ float* dst, __gm__ float* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    ```

- 同步搬运

    ```cpp
    __aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ int8_t* dst, __gm__ int8_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ int16_t* dst, __gm__ int16_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ half* dst, __gm__ half* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ int32_t* dst, __gm__ int32_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count,uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ float* dst, __gm__ float* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    ```

## 参数说明

**表1** 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输出 | 目的操作数（矢量）的起始地址。 |
| src | 输入 | 源操作数（矢量）的起始地址。 |
| n_burst | 输入 | 待搬运的连续传输数据块个数。 |
| len_burst | 输入 | 待搬运的每个连续传输数据块的长度，单位为字节。 |
| left_padding_count | 输入 | 数据左侧的padding元素数。对位宽为8的数据类型，该参数的最大值为32，对位宽为16的数据类型，该参数的最大值为16，对位宽为32的数据类型，该参数的最大值为8。 |
| right_padding_count | 输入 | 数据右侧的padding元素数。对位宽为8的数据类型，该参数的最大值为32，对位宽为16的数据类型，该参数的最大值为16，对位宽为32的数据类型，该参数的最大值为8。 |
| data_select_bit | 输入 | padding数据选择位，当前只支持设置为false，将padding值设置为数据块的第一个元素。 |
| l2_cache_ctl | 输入 | 配置数据在L2 Cache中的管理策略。取值说明请参见[表2](#table2)。 |
| burst_src_stride |输入| 源操作数相邻连续数据块的距离（前面一个数据块的头与后面一个数据块的头的间隔），单位为字节。<br>只搬运1个数据块，即n_burst=1时，可以将此参数设置为0。 |
| burst_dst_stride | 输入 | 目的操作数相邻连续数据块的距离（前面一个数据块的头与后面一个数据块的头的间隔），单位为字节。<br>只搬运1个数据块，即n_burst=1时，可以将此参数设置为0。 |

**表2** l2_cache_ctl取值说明 <a id="table2"></a>

| 取值 | 模式 | 含义 |
|------|------|------|
| 0    | NORMAL模式 | 启用L2 Cache，并且将分配的Cache Line标记为高替换优先级。|
| 1    | LAST模式 | &bull; 启用L2 Cache，并且将分配的Cache Line标记为低替换优先级。<br>&bull; **LAST模式功能，暂不支持。**|
| 2    | PERSISTENT模式 | &bull; 启用L2 Cache。已存入L2 Cache中的数据可能被替换，若需确保特定GlobalTensor的数据始终保留在L2 Cache中，可采用驻留模式。<br>&bull; 注意，被标记为驻留模式的Cache Line只能被其他同样被标记为驻留模式的Cache Line替换。<br>&bull; **PERSISTENT模式功能，暂不支持。**|
| 4    | DISABLE模式 | 不启用L2 Cache，每次都直接从GM中读取，并且保持已有Cache Line的状态不变。 |

## 返回值说明

无

## 流水类型

PIPE_MTE2

## 约束说明

- 各存储单元的空间大小和对齐要求请参考[存储单元说明](../general_description_and_constraints.md#存储单元说明)。
- 目的操作数不得存在地址重叠。对于存在地址重叠的操作，硬件将不返回任何警告或错误，也无法保证地址重叠数据的正确性。

## 调用示例

```cpp
// 待搬运的连续传输数据块个数为2
constexpr uint32_t n_burst = 2;
// 待搬运的每个连续传输数据块的长度为64个字节
constexpr uint32_t len_burst = 64;
// 数据左右侧padding的元素数为0
constexpr uint8_t left_padding_count = 0;
constexpr uint8_t right_padding_count = 0;
// padding值取数据块的第一个元素
constexpr bool data_select_bit = false;
// L2 Cache采用DISABLE模式
constexpr uint8_t l2_cache_ctl = 4;
// 输入输出数据中两个相邻连续数据块之间的距离为64字节
constexpr uint64_t burst_src_stride = 64;
constexpr uint32_t burst_dst_stride = 64;
// src为从GM传入的地址指针
__cbuf__ half dst[256];
asc_copy_gm2l1_align(dst, src, n_burst, len_burst,  left_padding_count, right_padding_count, data_select_bit, l2_cache_ctl, burst_src_stride, burst_dst_stride);
```
