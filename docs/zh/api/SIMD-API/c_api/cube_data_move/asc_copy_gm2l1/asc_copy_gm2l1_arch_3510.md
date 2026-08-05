# asc_copy_gm2l1

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

将矩阵数据从Global Memory搬运到L1 Buffer中，支持以下两种搬运模式：
- **填充模式**：搬运过程中可以对数据进行填充或移除填充值。
- **2D搬运模式**：支持2D矩阵数据搬运，支持压缩能力。（NPU_ARCH 220x版本不支持.）

## 函数原型

- 填充模式

    ```cpp
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ void* dst, __gm__ void* src, uint32_t n_burst, uint32_t len_burst, uint8_t pad_func_mode, uint64_t src_stride, uint32_t dst_stride)
    ```

- 填充模式（同步）

    ```cpp
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t n_burst, uint32_t len_burst, uint8_t pad_func_mode, uint64_t src_stride, uint32_t dst_stride)
    ```

- 2D搬运模式

    ```cpp
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ bfloat16_t *dst, __gm__ bfloat16_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ float *dst, __gm__ float *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ fp8_e4m3fn_t *dst, __gm__ fp8_e4m3fn_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ fp8_e5m2_t *dst, __gm__ fp8_e5m2_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ half *dst, __gm__ half *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ hifloat8_t *dst, __gm__ hifloat8_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ int16_t *dst, __gm__ int16_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ int32_t *dst, __gm__ int32_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ int8_t *dst, __gm__ int8_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ uint16_t *dst, __gm__ uint16_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ uint32_t *dst, __gm__ uint32_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ uint8_t *dst, __gm__ uint8_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ fp4x2_e1m2_t *dst, __gm__ fp4x2_e1m2_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ fp4x2_e2m1_t *dst, __gm__ fp4x2_e2m1_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ void *dst, __gm__ void *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    ```

- 2D搬运模式（同步）

    ```cpp
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ bfloat16_t *dst, __gm__ bfloat16_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ float *dst, __gm__ float *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ fp8_e4m3fn_t *dst, __gm__ fp8_e4m3fn_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ fp8_e5m2_t *dst, __gm__ fp8_e5m2_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ half *dst, __gm__ half *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ hifloat8_t *dst, __gm__ hifloat8_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ int16_t *dst, __gm__ int16_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ int32_t *dst, __gm__ int32_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ int8_t *dst, __gm__ int8_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ uint16_t *dst, __gm__ uint16_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ uint32_t *dst, __gm__ uint32_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ uint8_t *dst, __gm__ uint8_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ fp4x2_e1m2_t *dst, __gm__ fp4x2_e1m2_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ fp4x2_e2m1_t *dst, __gm__ fp4x2_e2m1_t *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ void *dst, __gm__ void *src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t decomp_mode, uint8_t l2_cache_ctl)
    ```

## 参数说明

### 填充模式参数

**表1** 填充模式参数

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输出 | 目的操作数（矢量）的起始地址。 |
| src | 输入 | 源操作数（矢量）的起始地址。 |
| n_burst | 输入 | 待搬运的连续传输数据块个数。 |
| len_burst | 输入 | 待搬运的每个连续传输数据块的长度，单位为32个字节。 |
| pad_func_mode | 输入 | 一个4Bit的值，用于指示通道上的padding功能，他会在32个字节的数据中填充数据；或从每个32个字节的读取数据中移除padding值。<br>&bull; 0：不做padding。 <br>&bull; 1：每有1个字节的数据，插入31个padding值。 <br>&bull; 2：每有2个字节的数据，插入15个padding值。 <br>&bull; 3：每有4个字节的数据，插入14个padding值。<br>&bull; 4：每有8个字节的数据，插入12个padding值。 <br>&bull; 5：每有16个字节的数据，插入8个padding值。<br>&bull; 6：每有32个字节的数据，移除28个最高有效位数据，只保留4个最低有效位元素。<br>&bull; 7：每有32个字节的数据，移除24个最高有效位数据，只保留8个最低有效位元素。<br>&bull; 8：每有32个字节的数据，移除16个最高有效位数据，只保留16个最低有效位元素。 |
| src_stride | 输入 | 输入数据中两个相邻连续数据块之间的距离。 |
| dst_stride | 输入 | 输出数据中两个相邻连续数据块之间的距离。 |

### 2D搬运模式参数

**表2** 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输出 | 目的操作数（矢量）的起始地址。 |
| src | 输入 | 源操作数（矢量）的起始地址。 |
| m_start_position | 输入 | M方向上的起始位置。 |
| k_start_position | 输入 | K方向上的起始位置。 |
| dst_stride | 输入 | 目的操作数步长。<br>单位为512字节。 |
| m_step | 输入 | M方向上的步长。 |
| k_step | 输入 | K方向上的步长。 |
| decomp_mode | 输入 | 设置不同的压缩模式。 |
| l2_cache_ctl | 输入 | 配置数据在L2 Cache中的管理策略。取值说明请参见[表3](#table3)。 |

**表3** l2_cache_ctl取值说明 <a id="table3"></a>

| 取值 | 模式 | 含义 |
|------|------|------|
| 0    | NORMAL模式 | 启用L2 Cache，并且将分配的Cache Line标记为高替换优先级。|
| 1    | LAST模式 | &bull; 启用L2 Cache，并且将分配的Cache Line标记为低替换优先级。<br>&bull; **LAST模式功能，暂不支持。**|
| 2    | PERSISTENT模式 | &bull; 启用L2 Cache。已存入L2 Cache中的数据可能被替换，若需确保特定GlobalTensor的数据始终保留在L2 Cache中，可采用驻留模式。<br>&bull; 注意，被标记为驻留模式的Cache Line只能被其他同样被标记为驻留模式的Cache Line替换。<br>&bull; **目前该驻留模式功能尚在开发中，暂不支持，计划于Ascend 950PR/Ascend 950DT上提供支持。**|
| 4    | DISABLE模式 | 不启用L2 Cache，每次都直接从GM中读取，并且保持已有Cache Line的状态不变。 |

## 返回值说明

无

## 流水类型

PIPE_MTE2

## 约束说明

各存储单元的空间大小和对齐要求请参考[存储单元说明](../../general_description_and_constraints.md#存储单元说明)。

### 填充模式约束

- 对于插入padding值的场景：
    - len_burst的单位为32个字节的指的是写入L1 Buffer的实际数据数，它必须设置为1，此时一次仅从GM里读取1/2/4/8/16个字节的数据。
    - 每一次burst都是从GM中连续读取，因此不需要设置src_stride。
    - dst_stride以32个字节为单位。
- 对于移除padding值的场景：
    - 源操作数中一次burst操作的数据为len_burst * 32个字节，目的操作数中一次burst的实际数据为len_burst * 4/8/16个字节。
    - 每一次burst后都连续地往L1 Buffer中写入，因此不需要设置dst_stride。
    - src_stride以32个字节为单位。

### 2D搬运模式约束

- 为了兼容性，需要设置m_step为1。

## 调用示例

```cpp
//待搬运的连续传输数据块个数为1
constexpr uint32_t n_burst = 1;
//待搬运的每个连续传输数据块的长度为128个字节
constexpr uint32_t len_burst = 4;
//不对数据做padding
constexpr uint8_t pad_func_mode = 0;
//输入输出数据中两个相邻连续数据块之间的距离为0
constexpr uint64_t src_stride = 0;
constexpr uint32_t dst_stride = 0;
// src表示源操作数的起始地址
__cbuf__ half dst[256];
asc_copy_gm2l1(dst, src, n_burst, len_burst, pad_func_mode, src_stride, dst_stride);
```
