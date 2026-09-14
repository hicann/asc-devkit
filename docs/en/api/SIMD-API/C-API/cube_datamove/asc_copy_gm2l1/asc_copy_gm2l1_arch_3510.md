# asc_copy_gm2l1

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-28T10:10:38.474Z pushedAt=2026-08-29T07:20:47.161Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

Copies matrix data from Global Memory to the L1 Buffer, supporting the following two move modes:
- **Padding mode**: Data can be filled or padding values can be removed during the data transfer.
- **2D copy mode**: Supports 2D matrix data copy and compression. (Not supported on NPU_ARCH 220x.)

## Prototype

- Padding mode

    ```cpp
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ void* dst, __gm__ void* src, uint32_t n_burst, uint32_t len_burst, uint8_t pad_func_mode, uint64_t src_stride, uint32_t dst_stride)
    ```

- Padding mode (synchronous)

    ```cpp
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t n_burst, uint32_t len_burst, uint8_t pad_func_mode, uint64_t src_stride, uint32_t dst_stride)
    ```

- 2D copy mode

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

- 2D copy mode (synchronous)

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

## Parameters

### Fill Mode Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| n_burst | Input | Number of consecutive data blocks to be moved. |
| len_burst | Input | Length of each consecutive data block to be moved, in units of 32 bytes. |
| pad_func_mode | Input | A 4-bit value that indicates the padding function on the channel. It inserts padding values into 32 bytes of data, or removes padding values from each 32-byte read. <br>&bull; 0: No padding. <br>&bull; 1: For every 1 byte of data, insert 31 padding values. <br>&bull; 2: For every 2 bytes of data, insert 15 padding values. <br>&bull; 3: For every 4 bytes of data, insert 14 padding values. <br>&bull; 4: For every 8 bytes of data, insert 12 padding values. <br>&bull; 5: For every 16 bytes of data, insert 8 padding values. <br>&bull; 6: For every 32 bytes of data, remove the 28 most significant bits and retain only 4 least significant bits. <br>&bull; 7: For every 32 bytes of data, remove the 24 most significant bits and retain only 8 least significant bits. <br>&bull; 8: For every 32 bytes of data, remove the 16 most significant bits and retain only 16 least significant bits. |
| src_stride | Input | Distance between two adjacent consecutive data blocks in the input data. |
| dst_stride | Input | Distance between two adjacent consecutive data blocks in the output data. |

### 2D Copy Mode Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Starting address of the destination operand (vector). |
| src | Input | Starting address of the source operand (vector). |
| m_start_position | Input | Starting position in the M direction. |
| k_start_position | Input | Starting position in the K direction. |
| dst_stride | Input | Destination operand stride.<br>Unit: 512 bytes. |
| m_step | Input | Stride in the M direction. |
| k_step | Input | Stride in the K direction. |
| decomp_mode | Input | Sets different compression modes. |
| l2_cache_ctl | Input | Configures the management policy for data in the L2 cache. Value description:  <br>&bull; **0**: DISABLE mode, suitable for data that is accessed only once. <br>&bull; **1**: NORMAL mode, suitable for data whose reuse pattern is unknown or not extreme. <br>&bull; **2**: LAST mode, suitable for data that is accessed frequently and repeatedly. <br>&bull; **4**: PERSISTENT mode, suitable for data that needs to reside in the cache for a long time. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

### Fill Mode Constraints

- For the scenario of inserting padding values:
    - The unit of **len_burst** is 32 bytes, which refers to the actual amount of data written to the L1 Buffer. It must be set to 1, in which case only 1/2/4/8/16 bytes of data are read from GM at a time.
    - Each burst reads data continuously from GM, so **src_stride** does not need to be set.
    - **dst_stride** is in units of 32 bytes.
- For the scenario of removing padding values:
    - The data of one burst operation in the source operand is **len_burst** * 32 bytes, and the actual data of one burst in the destination operand is **len_burst** * 4/8/16 bytes.
    - After each burst, data is written continuously to the L1 Buffer, so **dst_stride** does not need to be set.
    - **src_stride** is in units of 32 bytes.

### 2D Copy Mode Constraints

- The start addresses of **dst** and **src** must be 32-byte aligned.
- For compatibility, **m_step** must be set to 1.

## Example

```cpp
//The number of consecutive data blocks to be copied is 1.
constexpr uint32_t n_burst = 1;
//The length of each consecutive data block to be copied is 128 bytes.
constexpr uint32_t len_burst = 4;
//No padding is applied to the data.
constexpr uint8_t pad_func_mode = 0;
//The distance between two adjacent consecutive data blocks in the input and output data is 0.
constexpr uint64_t src_stride = 0;
constexpr uint32_t dst_stride = 0;
__gm__ half src[256];
__cbuf__ half dst[256];
asc_copy_gm2l1(dst, src, n_burst, len_burst, pad_func_mode, src_stride, dst_stride);
```
