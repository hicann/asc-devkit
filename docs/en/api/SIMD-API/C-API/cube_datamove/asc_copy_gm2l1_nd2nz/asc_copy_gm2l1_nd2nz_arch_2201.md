# asc_copy_gm2l1_nd2nz

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-28T10:10:39.214Z pushedAt=2026-08-29T07:20:51.792Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
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

Copies data from Global Memory (GM) to Level 1 cache (L1), supporting conversion from ND format to NZ format during data transfer.

## Prototype

- Regular transfer

    ```cpp
    __aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ int8_t* dst, __gm__ int8_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ half* dst, __gm__ half* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ int16_t* dst, __gm__ int16_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ float* dst, __gm__ float* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ int32_t* dst, __gm__ int32_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    ```

- Synchronous transfer

    ```cpp
    __aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ int8_t* dst, __gm__ int8_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ half* dst, __gm__ half* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ int16_t* dst, __gm__ int16_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ float* dst, __gm__ float* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ int32_t* dst, __gm__ int32_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    __aicore__ inline void asc_copy_gm2l1_nd2nz_sync(__cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t nd_num,
        uint16_t n_value, uint16_t d_value, uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride,
        uint16_t dst_nz_n_stride, uint16_t dst_nz_matrix_stride)
    ```

## Parameters


| Parameter            | Input/Output | Description                                                                                                                                                                                                                                                          |
|:---------------------|:------|:---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| dst                  | Output | Start address of the destination operand (vector).                                                                                                                                                                                                                |
| src                  | Input  | Start address of the source operand.                                                                                                                                                                                                                              |
| nd_num               | Input  | Number of ND matrices to transfer. Value range: nd_num∈[0, 4095].                                                                                                                                                                                                    |
| n_value              | Input  | Number of rows in the ND matrix. Value range: n_value∈[0, 16384].                                                                                                                                                                                                    |
| d_value              | Input  | Number of columns in the ND matrix. Value range: d_value∈[0, 65535].                                                                                                                                                                                                 |
| src_nd_matrix_stride | Input  | Offset between the start addresses of adjacent ND matrices in the source operand. Value range: src_nd_matrix_stride∈[0, 65535], in elements.                                                                                                                       |
| src_d_value          | Input  | Offset between the start addresses of adjacent rows in the same ND matrix of the source operand. Value range: src_d_value∈[1, 65535], in elements.                                                                                                                 |
| dst_nz_c0_stride     | Input  | After ND-to-NZ conversion, one row in the source operand is converted into multiple rows in the destination operand. dst_nz_c0_stride indicates the offset between the start addresses of adjacent rows in the destination NZ matrix that originate from the same row of the source operand. Value range: dst_nz_c0_stride∈[1, 16384], in C0_SIZE (32B). |
| dst_nz_n_stride      | Input  | Offset between the start addresses of adjacent rows of the Z-shaped matrix in the destination NZ matrix. Value range: dst_nz_n_stride∈[1, 16384], in C0_SIZE (32B).                                                                                               |
| dst_nz_matrix_stride | Input  | Offset between the start addresses of adjacent NZ matrices in the destination NZ matrix. Value range: dst_nz_matrix_stride∈[1, 65535], in elements.                                                                                                                |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- The start address of **src** must be aligned to the byte size of the corresponding data type.
- The start address of **dst** must be 32-byte aligned.
- If multiple **asc_copy_gm2l1_nd2nz** instructions need to be executed and their destination addresses overlap, a synchronization instruction must be inserted to ensure serialization of the multiple **asc_copy_gm2l1_nd2nz** instructions and prevent abnormal data.

## Example

```cpp
// total_length indicates the total length of the data to be transferred.
constexpr uint64_t total_length = 128;
__cbuf__ int8_t dst[total_length];
__gm__ int8_t src[total_length];
// Number of ND matrices to transfer.
constexpr uint16_t nd_num = 2;
// Number of rows in the ND matrix.
constexpr uint16_t n_value = 4;
// Number of columns in the ND matrix.
constexpr uint16_t d_value = 24;
// Starting offset of adjacent ND matrices.
constexpr uint16_t src_nd_matrix_stride = 144;
// Number of elements in one row.
constexpr uint16_t src_d_value = 48;
// Offset between the start addresses of multiple rows after the format is converted from ND to NZ.
constexpr uint16_t dst_nz_c0_stride = 11;
// Offset in dst between row x and row x+1 of an ND matrix in src after conversion to NZ format.
constexpr uint16_t dst_nz_n_stride = 2;
// Offset in dst between the start of the x-th ND matrix and the start of the (x+1)-th ND matrix.
constexpr uint16_t dst_nz_matrix_stride = 96;
asc_copy_gm2l1_nd2nz(dst, src, nd_num, n_value, d_value, src_nd_matrix_stride, src_d_value, dst_nz_c0_stride, dst_nz_n_stride, dst_nz_matrix_stride);
```
