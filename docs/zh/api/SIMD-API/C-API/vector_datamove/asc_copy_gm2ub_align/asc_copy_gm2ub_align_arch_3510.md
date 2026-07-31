# asc_copy_gm2ub_align

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

提供数据非对齐搬运的功能，将数据从Global Memory (GM)搬运到Unified Buffer (UB)，并支持8位/16位/32位数据类型搬运。

本接口支持以下两种数据搬运方式：

- 前n个数据搬运

    若搬运数据长度非32字节对齐，搬运数据会补齐至32字节对齐，支持以下两种填充方式：

    - 手动填充：搬运前调用[asc_set_copy_pad_val](../asc_set_copy_pad_val.md)配置填充值。
    - 自动填充：由硬件自动填充dummy假数据，dummy假数据的值为数据块的第一个元素的值。

- 高维切分搬运

    若搬运数据长度非32字节对齐，会将搬运数据补齐至32字节对齐。可通过配置参数`dst_stride`选择Normal模式或Compact模式。非32字节对齐场景支持以下两种填充方式：

    - 手动填充：搬运前调用`asc_set_copy_pad_val`设置填充值，并将`enable_constant_pad`设置为`true`。
    - 自动填充：将`enable_constant_pad`设置为`false`，由硬件自动填充dummy假数据，dummy假数据的值为数据块的第一个元素的值。

    数据搬运模式说明如下：

    - Compact模式：将`dst_stride`设置为`len_burst`，并将`left_padding_num`和`right_padding_num`设置为0。多个数据块在UB中紧密排列，在32字节补齐处理上被视为一个整体；若所有数据块的总长度非32字节对齐，则仅在整体末尾通过上述手动或自动填充方式补齐至32字节对齐。
    - Normal模式：将`dst_stride`设置为不等于`len_burst`且满足32字节对齐的值。相邻目的数据块的首地址间隔由`dst_stride`指定，数据块之间可以预留间隔，无需紧密排列。
        - 当`left_padding_num`和`right_padding_num`均为0时，通过上述手动或自动填充方式将每个数据块分别补齐至32字节对齐。
        - 当`left_padding_num`或`right_padding_num`非0时，`enable_constant_pad`不生效，必须在搬运前调用`asc_set_copy_pad_val`配置填充值。若数据块长度与左右Padding的总字节数非32字节对齐，硬件会在右侧继续填充dummy假数据至32字节对齐，dummy假数据的值为`asc_set_copy_pad_val`配置的值。

    例如，搬运3个长度为48字节且无左右Padding的数据块时：
    - Compact模式下将`dst_stride`设置为48，各数据块在UB中的起始偏移依次为0、48、96，仅在144字节有效数据的末尾补齐16字节。
    - Normal模式下将`dst_stride`设置为64，各数据块在UB中的起始偏移依次为0、64、128，每个数据块分别补齐16字节。

    当只搬运1个数据块，或`len_burst`已经32字节对齐且无左右Padding时，两种模式的搬运结果相同。

## 函数原型

- 前n个数据搬运

    ```cpp
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ half* dst, __gm__ half* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ float* dst, __gm__ float* src, uint32_t size)
    ```

- 同步搬运

    ```cpp
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ half* dst, __gm__ half* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ float* dst, __gm__ float* src, uint32_t size)
    ```

- 高维切分搬运

    ```cpp
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int16_t* dst, __gm__ int16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ half* dst, __gm__ half* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int32_t* dst, __gm__ int32_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ float* dst, __gm__ float* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    ```

- **以下函数原型已废弃，请使用`asc_load_l2_cache_mode`类型枚举值进行L2 Cache管理策略配置。**

    ```cpp
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int16_t* dst, __gm__ int16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ half* dst, __gm__ half* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int32_t* dst, __gm__ int32_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ float* dst, __gm__ float* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    ```


## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输出 | 目的UB的起始地址。需要32字节对齐。 |
| src | 输入 | 源GM的起始地址。需要1字节对齐。 |
| size | 输入 | 搬运数据大小，单位为字节。取值范围：[0, 2097151]。 |
| n_burst | 输入 | 待搬运的连续传输数据块个数。取值范围：[0, 4095]。 |
| len_burst | 输入 | 待搬运的每个连续传输数据块的长度，单位为字节。取值范围：[0, 2097151]。 |
| left_padding_num | 输入 | 连续搬运数据块左侧需要补充的元素个数。该参数对应的填充数据大小不能超过32字节。Compact模式下需要设置为0。 |
| right_padding_num | 输入 | 连续搬运数据块右侧需要补充的元素个数。该参数对应的填充数据大小不能超过32字节。Compact模式下需要设置为0。 |
| enable_constant_pad | 输入 | 当`left_padding_num`和`right_padding_num`均为0时，配置非对齐场景的填充方式。取值说明如下：  <br>&bull; `true`：手动填充，填充值为接口`asc_set_copy_pad_val`设置的值。 <br>&bull; `false`：自动填充，由硬件填充dummy假数据，dummy假数据的值为数据块的第一个元素的值。<br>当`left_padding_num`或`right_padding_num`非0时，该参数不生效。 |
| l2_cache_mode | 输入 | [asc_load_l2_cache_mode](../../enum/asc_load_l2_cache_mode.md)类型的枚举值，配置数据在L2 Cache中的管理策略。 |
| src_stride | 输入 | 源操作数相邻连续数据块的距离（前面一个数据块的头与后面一个数据块的头的间隔），单位为字节。<br>只搬运1个数据块，即`n_burst`设置为1时，可以将此参数设置为0。 |
| dst_stride | 输入 | 目的操作数相邻连续数据块的距离（前面一个数据块的头与后面一个数据块的头的间隔），单位为字节，用于选择数据搬运模式。<br>&bull; 等于`len_burst`：Compact模式，目的数据块在UB中紧密排列，`dst_stride`支持字节对齐。<br>&bull; 不等于`len_burst`：Normal模式，`dst_stride`需要满足32字节对齐要求。<br>只搬运1个数据块，即`n_burst`设置为1时，可以将此参数设置为0。 |

## 返回值说明

无

## 流水类型

PIPE_MTE2

## 约束说明

- 各存储单元的空间大小和对齐要求请参考[存储单元说明](../../通用说明和约束.md#存储单元说明)。
- 当`n_burst`、`len_burst`中任意一个值为0时，该接口被视为NOP（空操作）。
- 当`size`值为0时，该接口被视为NOP（空操作）。
- 如果需要执行多条`asc_copy_gm2ub_align`指令，且`asc_copy_gm2ub_align`指令的目的地址存在重叠，需要插入同步指令（[asc_sync_notify](../../sync/asc_sync_notify.md)和[asc_sync_wait](../../sync/asc_sync_wait.md)），保证多个`asc_copy_gm2ub_align`指令的串行化，防止出现异常数据。
- 当`left_padding_num`或`right_padding_num`非0时，`enable_constant_pad`不生效，必须在搬运前调用`asc_set_copy_pad_val`配置填充值。`left_padding_num`、`right_padding_num`对应的填充数据大小均不能超过32字节。
- 前n个数据搬运接口：若`size`非32字节对齐，搬运数据会补齐至32字节对齐，目的UB需要预留补齐后的空间。手动填充时，调用`asc_set_copy_pad_val`配置填充值；自动填充时，由硬件填充dummy假数据，dummy假数据的值为数据块的第一个元素的值。
- 当`dst_stride`不等于`len_burst`时，`dst_stride`要求32字节对齐。

## 调用示例

```cpp
asc_set_gm2ub_loop_size(2, 2);
asc_set_gm2ub_loop1_stride(96, 128);
asc_set_gm2ub_loop2_stride(192, 288);
asc_copy_gm2ub_align(dst, src, 2, 48 * sizeof(int8_t), 0, 0, false, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, 48 * sizeof(int8_t), 48 * sizeof(int8_t));
asc_set_gm2ub_loop_size(1, 1);
```
