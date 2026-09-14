# asc_copy_gm2l1_dn2nz

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:39:49.684Z -->

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

Copies matrix data from Global Memory to the L1 Buffer, performing **DN->NZ/NCHW->NC1HWC0/NCHW->C1HWNC0** operations during the process. If D is not aligned to the size of C0, zeros are padded in the innermost dimension of the L1 Buffer. Data in GM is stored in DN format; data in L1 is stored in NZ format.

The DN->NZ data copy is illustrated as follows:

![ ](../figures/asc_copy_gm2l1_dn2nz.png)

## Prototype

- Regular copy

    ```cpp
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ int8_t* dst, __gm__ int8_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ int16_t* dst, __gm__ int16_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ uint16_t* dst, __gm__ uint16_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,  uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ half* dst, __gm__ half* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ int32_t* dst, __gm__ int32_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz(__cbuf__ float* dst, __gm__ float* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    ```

- Synchronous copy

    ```cpp
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ int8_t* dst, __gm__ int8_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ int16_t* dst, __gm__ int16_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ uint16_t* dst, __gm__ uint16_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value,  uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ half* dst, __gm__ half* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ int32_t* dst, __gm__ int32_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    __aicore__ inline void asc_copy_gm2l1_dn2nz_sync(__cbuf__ float* dst, __gm__ float* src, uint64_t loop1_src_stride, uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
    ```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| **dst** | Output | Starting address of the destination operand (vector). |
| **src** | Input | Starting address of the source operand (vector). |
| **loop1_src_stride** | Input | Interval, in bytes, between data blocks of the source operand in adjacent iterations of the innermost loop during the copy process. |
| **l2_cache_ctl** | Input | Configures the management policy for data in the L2 Cache. The values are described as follows:  <br>&bull; **0**: DISABLE mode, applicable to data that is accessed only once. <br>&bull; **1**: NORMAL mode, applicable to data whose reuse pattern is unknown or not extreme. <br>&bull; **2**: LAST mode, applicable to data that is accessed repeatedly at high frequency. <br>&bull; **4**: PERSISTENT mode, applicable to data that needs to reside in the cache for a long time. |
| **n_value** | Input | N value in the DN layout of the source operand. |
| **d_value** | Input | D value in the DN layout of the source operand. |
| **loop4_src_stride** | Input | Interval, in bytes, between data blocks of the source operand in adjacent iterations of the outermost loop during the copy process. |
| **smallc0_en** | Input | SmallC0 mode switch:  <br>&bull; **true**: C0_SIZE is padded to 32 bytes. <br>&bull; **false**: C0_SIZE is padded to 4 channels. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- Note that **smallc0_en** can be enabled only when **D** <= 4; otherwise, it becomes invalid.

## Example

```cpp
// The stride for the innermost loop is 4 bytes.
constexpr uint64_t loop1_src_stride = 4;
// Use NORMAL mode for L2 Cache.
constexpr uint8_t l2_cache_ctl = 1;
// Set the N value to 3.
constexpr uint16_t n_value = 3;
// Set the D value to 3.
constexpr uint32_t d_value = 3;
// The stride for the outermost loop is 4 bytes.
constexpr uint64_t loop4_src_stride = 2;
// Pad each C0 data block to 32 bytes.
constexpr bool smallc0_en = true;
__gm__ half src[256];
__cbuf__ half dst[256];
asc_copy_gm2l1_dn2nz(dst, src, loop1_src_stride, l2_cache_ctl, n_value, d_value, loop4_src_stride, smallc0_en);
```
