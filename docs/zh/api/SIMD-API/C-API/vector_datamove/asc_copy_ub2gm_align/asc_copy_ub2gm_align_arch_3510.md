# asc_copy_ub2gm_align

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

将数据从Unified Buffer (UB)搬运到Global Memory (GM)，支持8位/16位/32位数据类型搬运。

本接口支持以下两种数据搬运方式，均不支持设置填充值：

- 前n个数据搬运

    若搬运数据长度非32字节对齐，搬出时会在读取UB数据时自动补充dummy假数据，将搬运数据补齐至32字节对齐。写入GM时会丢弃补充的dummy假数据。

- 高维切分搬运

    若搬运数据长度非32字节对齐，会在读取UB数据时补充dummy假数据，将搬运数据补齐至32字节对齐。写入GM时会丢弃补充的dummy假数据。可通过配置参数`src_stride`选择Normal模式或Compact模式：

    - Compact模式：将`src_stride`设置为`len_burst`。多个数据块在UB中紧密排列，在32字节补齐处理上被视为一个整体；若所有数据块的总长度非32字节对齐，则在读取UB数据时仅在整体末尾补充dummy假数据至32字节对齐，写入GM时会丢弃补充的dummy假数据。
    - Normal模式：将`src_stride`设置为不等于`len_burst`的32字节对齐值，每个数据块单独进行32字节对齐处理。
        - 若`len_burst`为32字节对齐，不会读取dummy假数据，从UB读取的所有数据都会写入GM。
        - 若`len_burst`非32字节对齐，读取UB数据时会补充dummy假数据至32字节对齐，写入GM时会丢弃补充的dummy假数据。

    例如，搬运3个长度为48字节的数据块时：
    - Compact模式下将`src_stride`设置为48，各数据块在UB中的起始偏移依次为0、48、96，仅在144字节有效数据的末尾补充16字节dummy假数据。
    - Normal模式下将`src_stride`设置为64，各数据块在UB中的起始偏移依次为0、64、128，每个数据块分别补充16字节dummy假数据。

    两种模式下，dummy假数据均不会写入GM。当只搬运1个数据块，或`len_burst`已经32字节对齐时，两种模式的搬运结果相同。

## 函数原型

- 前n个数据搬运

    ```cpp
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ half* dst, __ubuf__ half* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ float* dst, __ubuf__ float* src, uint32_t size)
    ```

- 同步搬运

    ```cpp
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ half* dst, __ubuf__ half* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ float* dst, __ubuf__ float* src, uint32_t size)
    ```

- 高维切分搬运

    ```cpp
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ int8_t* dst, __ubuf__ int8_t* src, uint16_t n_burst, uint32_t len_burst, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint16_t n_burst, uint32_t len_burst, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint16_t n_burst, uint32_t len_burst, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint16_t n_burst, uint32_t len_burst, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint16_t n_burst, uint32_t len_burst, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ int16_t* dst, __ubuf__ int16_t* src, uint16_t n_burst, uint32_t len_burst, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint16_t n_burst, uint32_t len_burst, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ half* dst, __ubuf__ half* src, uint16_t n_burst, uint32_t len_burst, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint16_t n_burst, uint32_t len_burst, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ int32_t* dst, __ubuf__ int32_t* src, uint16_t n_burst, uint32_t len_burst, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint16_t n_burst, uint32_t len_burst, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ float* dst, __ubuf__ float* src, uint16_t n_burst, uint32_t len_burst, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    ```

- **以下函数原型已废弃，请使用`asc_store_l2_cache_mode`类型的枚举值进行L2 Cache管理策略配置。**

    ```cpp
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ int8_t* dst, __ubuf__ int8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ int16_t* dst, __ubuf__ int16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ half* dst, __ubuf__ half* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ int32_t* dst, __ubuf__ int32_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ float* dst, __ubuf__ float* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    ```



## 参数说明

**表1**  参数说明
| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输出 | 目的GM的起始地址。需要1字节对齐。 |
| src | 输入 | 源UB的起始地址。需要32字节对齐。 |
| size | 输入 | 搬运数据大小，单位为字节。取值范围：[0, 2097151]。 |
| n_burst | 输入 | 待搬运的连续传输数据块个数。取值范围：[0, 4095]。 |
| len_burst | 输入 | 待搬运的每个连续传输数据块的长度，单位为字节。取值范围：[0, 2097151]。 |
| l2_cache_mode | 输入 | [asc_store_l2_cache_mode](../../enum/asc_store_l2_cache_mode.md)类型的枚举值，配置数据在L2 Cache中的管理策略。 |
| dst_stride | 输入 | 目的操作数相邻连续数据块的距离（前面一个数据块的头与后面一个数据块的头的间隔），单位为字节。<br> 只搬运1个数据块，即`n_burst`设置为1时，可以将此参数设置为0。 |
| src_stride | 输入 | 源操作数相邻连续数据块的距离（前面一个数据块的头与后面一个数据块的头的间隔），单位为字节，用于选择数据搬运模式。<br>&bull; 等于`len_burst`：Compact模式，源数据块在UB中紧密排列，`src_stride`支持字节对齐。<br>&bull; 不等于`len_burst`：Normal模式，`src_stride`需要满足32字节对齐要求。 |

## 返回值说明

无

## 流水类型

PIPE_MTE3

## 约束说明

- 各存储单元的空间大小和对齐要求请参考[存储单元说明](../../通用说明和约束.md#存储单元说明)。
- 当`n_burst`、`len_burst`中任意一个值为0时，该接口被视为NOP（空操作）。
- 当`size`值为0时，该接口被视为NOP（空操作）。
- 如果需要执行多条`asc_copy_ub2gm_align`指令，且`asc_copy_ub2gm_align`指令的目的地址存在重叠，需要插入同步指令（[asc_sync_notify](../../sync/asc_sync_notify.md)和[asc_sync_wait](../../sync/asc_sync_wait.md)），保证多个`asc_copy_ub2gm_align`指令的串行化，防止出现异常数据。
- 前n个数据搬运接口的`size`无需32字节对齐。若`size`非32字节对齐，硬件会自动补充dummy假数据至32字节对齐。dummy假数据的值为数据块的第一个元素的值。写入GM时会丢弃补充的dummy假数据。
- 当`src_stride`设置为0时，可能会出现**硬件未定义行为**。如需将同一个源数据块Broadcast到GM的多个连续位置，建议使用loop功能，具体请参考[关键特性说明](#关键特性说明)。
- 当`src_stride`不等于`len_burst`时，`src_stride`要求32字节对齐。

## 关键特性说明

### Broadcast场景

使用高维切分搬运接口时，不建议通过将`src_stride`设置为0实现Broadcast。可通过loop功能实现Broadcast，将`n_burst`设置为1，并通过[asc_set_ub2gm_loop_size](../asc_set_ub2gm_loop_size.md)、[asc_set_ub2gm_loop1_stride](../asc_set_ub2gm_loop1_stride.md)和[asc_set_ub2gm_loop2_stride](../asc_set_ub2gm_loop2_stride.md)配置循环搬运：

- 将`loop1_size`设置为Broadcast次数，将`loop2_size`设置为1。
- 将`loop1_src_stride`设置为0，使每次内层循环读取同一个UB源数据块。
- 将`loop1_dst_stride`设置为单个数据块的字节数，使每次内层循环写入GM的下一个连续位置。
- 将`loop2_src_stride`和`loop2_dst_stride`设置为0。外层循环只执行1次，这两个参数不产生地址偏移。
- 将`asc_copy_ub2gm_align`的`n_burst`设置为1，并将`src_stride`设置为非0值。循环搬运结束后，调用`asc_set_ub2gm_loop_size`将循环次数复位为1。

以下示例将UB中的256字节源数据块重复搬运2048次，在GM中生成512KB的连续数据。使用前需要保证`src`指向的UB空间至少包含256字节有效数据，`dst`指向的GM空间至少包含512KB可写空间。

```cpp
constexpr uint32_t block_bytes = 256;
constexpr uint32_t broadcast_count = 2048;

asc_set_ub2gm_loop_size(broadcast_count, 1);
asc_set_ub2gm_loop1_stride(0, block_bytes);
asc_set_ub2gm_loop2_stride(0, 0);
asc_copy_ub2gm_align(dst, src, 1, block_bytes,
    asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, block_bytes, block_bytes);
asc_set_ub2gm_loop_size(1, 1);
```

## 调用示例

```cpp
uint32_t loop1_size = 2;
uint32_t loop2_size = 2;
uint64_t loop1_src_stride = 96;
uint64_t loop1_dst_stride = 128;
uint64_t loop2_src_stride = 192;
uint64_t loop2_dst_stride = 288;
asc_set_ub2gm_loop_size(loop1_size, loop2_size);
asc_set_ub2gm_loop1_stride(loop1_src_stride, loop1_dst_stride);
asc_set_ub2gm_loop2_stride(loop2_src_stride, loop2_dst_stride);
asc_copy_ub2gm_align(dst, src, 2, 48 * sizeof(int8_t), asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, 48 * sizeof(int8_t), 48 * sizeof(int8_t));
asc_set_ub2gm_loop_size(1, 1);
```
