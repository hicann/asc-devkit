# asc_copy_ub2gm_align_arch_3510（废弃）

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

**高维切分搬运接口已废弃，请使用`asc_store_l2_cache_mode`类型枚举值进行L2 Cache管理策略配置，新接口形式请参见[asc_copy_ub2gm_align高维切分数据搬运接口](../vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md#高维切分数据搬运)。**

提供数据非对齐搬运的功能，将数据从Unified Buffer (UB)搬运到Global Memory (GM)，并支持8位/16位/32位数据类型搬运。

本接口为高维切分搬运方式，不支持设置填充值。若搬运数据长度非32字节对齐，硬件在读取UB数据时补充dummy假数据，将搬运数据补齐至32字节对齐。写入GM时会丢弃补充的dummy假数据。可通过配置参数`src_stride`选择Normal模式或Compact模式：

- Compact模式：将`src_stride`设置为`len_burst`。多个数据块在UB中紧密排列，在32字节补齐处理上被视为一个整体；若所有数据块的总长度非32字节对齐，则仅在整体末尾补充dummy假数据至32字节对齐，写入GM时会丢弃补充的dummy假数据。
- Normal模式：将`src_stride`设置为不等于`len_burst`且满足32字节对齐的值，每个数据块单独进行32字节对齐处理。
    - 若`len_burst`为32字节对齐，不会读取dummy假数据，从UB读取的所有数据都会写入GM。
    - 若`len_burst`非32字节对齐，读取UB数据时会补充dummy假数据至32字节对齐，写入GM时会丢弃补充的dummy假数据。

例如，搬运3个长度为48字节的数据块时：

- Compact模式下将`src_stride`设置为48，各数据块在UB中的起始偏移依次为0、48、96，仅在144字节有效数据的末尾补充16字节dummy假数据。
- Normal模式下将`src_stride`设置为64，各数据块在UB中的起始偏移依次为0、64、128，每个数据块分别补充16字节dummy假数据。

两种模式下，dummy假数据均不会写入GM。当只搬运1个数据块，或`len_burst`已经32字节对齐时，两种模式的搬运结果相同。

## 函数原型

- 高维切分搬运

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

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输出 | 目的GM的起始地址。需要1字节对齐。 |
| src | 输入 | 源UB的起始地址。需要32字节对齐。 |
| n_burst | 输入 | 待搬运的连续传输数据块个数。取值范围：[0, 4095]。 |
| len_burst | 输入 | 待搬运的每个连续传输数据块的长度，单位为字节。取值范围：[0, 2097151]。 |
| l2_cache_mode | 输入 | L2 Cache管理策略配置。该参数类型为`uint8_t`，建议改用[asc_store_l2_cache_mode](../defs/enum/asc_store_l2_cache_mode.md)类型的枚举值。 |
| dst_stride | 输入 | 目的操作数相邻连续数据块的距离（前面一个数据块的头与后面一个数据块的头的间隔），单位为字节。<br>只搬运1个数据块，即`n_burst`设置为1时，可以将此参数设置为0。 |
| src_stride | 输入 | 源操作数相邻连续数据块的距离（前面一个数据块的头与后面一个数据块的头的间隔），单位为字节，用于选择数据搬运模式。<br>&bull; 等于`len_burst`：Compact模式，源数据块在UB中紧密排列，`src_stride`支持字节对齐。<br>&bull; 不等于`len_burst`：Normal模式，`src_stride`需要满足32字节对齐要求。 |

## 返回值说明

无

## 流水类型

PIPE_MTE3

## 约束说明

- 各存储单元的空间大小和对齐要求请参考[存储单元说明](../general_description_and_constraints.md#存储单元说明)。
- 当`n_burst`、`len_burst`中任意一个值为0时，该接口被视为NOP（空操作）。
- 如果需要执行多条`asc_copy_ub2gm_align`指令，且`asc_copy_ub2gm_align`指令的目的地址存在重叠，需要插入同步指令（[asc_sync_notify](../sync/asc_sync_notify.md)和[asc_sync_wait](../sync/asc_sync_wait.md)），保证多个`asc_copy_ub2gm_align`指令的串行化，防止出现异常数据。
- 当`src_stride`不等于`len_burst`时，`src_stride`要求32字节对齐。
