# asc_copy_l12l0b_trans

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-28T10:27:09.702Z pushedAt=2026-08-29T09:20:22.267Z -->

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

This API loads 2D data with transpose from the L1 Buffer to the L0B Buffer.

The following examples illustrate the API functionality and key parameters: in the figures below, an N-shape or a Z-shape represents a fractal.

- For the uint8_t/int8_t data type, each iteration processes 32 × 32 × 1B data and can handle 2 fractals (a fractal is 512B). In each iteration, 2 consecutive 16 × 32 fractals in the source operand are merged into a 32 × 32 block matrix, transposed based on the block matrix, and then split into 2 16 × 32 fractals after transposition. Different layouts are possible depending on parameters such as the destination operand fractal gap.

As shown in the figure below:

- A total of 3072B of data is processed. Each iteration processes 32 × 32 × 1B data, requiring 3 iterations to complete, with **repeat_time** = 3.
- **src_stride** = 1, indicating that between adjacent iterations, the gap between the start address of the previous block matrix and the start address of the next block matrix in the source operand is 1 (unit: 32 × 32 × 1B). The unit here is actually the size of the merged block matrix.
- **dst_gap** = 1, indicating that between adjacent iterations, the gap from the end address of the first fractal of the previous iteration to the start address of the first fractal of the next iteration in the destination operand is 1 (unit: 512B).
- **dst_frac_gap** = 0, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal in the destination operand is 0 (unit: 512B).
![](../../figures/asc_copy_l12l0_trans_b8_example1.png)

As shown in the figure below:

- The explanations of **repeat_time** and **src_stride** are the same as those in the figure above.
- **dst_gap** = 0, indicating that between adjacent iterations, there is no gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand.
- **dst_frac_gap** = 2, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal of the destination operand is 2 (unit: 512B).
![](../../figures/asc_copy_l12l0_trans_b8_example2.png)

- For the half/bfloat16_t data type, each iteration processes 16 × 16 × 2B data and can handle one fractal (a fractal is 512B). In each iteration, one 16 × 16 fractal in the source operand will be transposed.
  - Because each iteration processes 16 × 16 × 2B data, 3 iterations are required to complete, so repeat_time = 3.
  - **src_stride** = 1, indicating that between adjacent iterations, the gap between the start addresses of the previous block matrix and the next block matrix of the source operand is 1 (unit: 16 × 16 × 2B).
  - **dst_gap** = 0, indicating that between adjacent iterations, the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand have no gap.
  - In this scenario, because the fractal is the block matrix itself and each iteration processes one fractal, there is no gap between fractals within an iteration, so this parameter setting is ineffective.
![](../../figures/asc_copy_l12l0_trans_b16_example.png)

- For the float/int32_t/uint32_t data type, each iteration processes 16 × 16 × 4B data and can handle 2 fractals (a fractal is 512B). In each iteration, 2 consecutive 16 × 8 fractals of the source operand are merged into 1 16 × 16 block matrix, transposed based on the block matrix, and then split into 2 16 × 8 fractals after transposition. Different layouts are possible depending on parameters such as the destination operand fractal gap.
As shown in the figure below:
  - Because each iteration processes 16 × 16 × 4B data, 3 iterations are required to complete, so repeat_time = 3;
  - src_stride = 1, indicating that between adjacent iterations, the gap between the start address of the previous block matrix and the next block matrix of the source operand is 1 (unit: 16 × 16 × 4B). Here the unit is actually the size of the merged block matrix;
  - **dst_gap** = 1, indicating that between adjacent iterations, the gap from the end address of the first fractal of the previous iteration to the start address of the first fractal of the next iteration in the destination operand is 1 (unit: 512B).
  - dst_frac_gap = 0, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal of the destination operand is 0 (unit: 512B).
![](../../figures/asc_copy_l12l0_trans_b32_example1.png)

As shown in the figure below:
- The explanations of **repeat_time** and **src_stride** are the same as those in the figure above.
- **dst_gap** = 0, indicating that between adjacent iterations, there is no gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand.
- **dst_frac_gap** = 2, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal of the destination operand is 2 (unit: 512B).
![](../../figures/asc_copy_l12l0_trans_b32_example2.png)

- For the int4b_t data type, each iteration processes 64\*64\*0.5B of data and can process 4 fractals (a fractal is 512B). In each iteration, 4 consecutive 16\*64 fractals in the source operand will be merged into a 64\*64 block matrix, transposed based on the block matrix, and split into 4 16\*64 fractals after transposition, and these fractals can have different arrangements according to the destination operand fractal gap and other parameters.

    For the int4b_t data type, two numbers need to be merged into an int8_t or uint8_t number. The combination rules are as follows:

    ![](../../figures/int4b_example.png)

  As shown in the following figure:
  - A total of 6144B of data needs to be processed. Each iteration processes 64\*64\*0.5B of data, so 3 iterations can complete the processing, with repeat_time = 3.
  - **src_stride = 1**, indicating that between adjacent iterations, the gap between the start address of the previous block matrix and the next block matrix of the source operand is 1 (unit: 64\*64\*0.5B). Here the unit is actually the size of the merged block matrix.
  - **dst_gap** = 1, indicating that between adjacent iterations, the gap from the end address of the first fractal of the previous iteration to the start address of the first fractal of the next iteration in the destination operand is 1 (unit: 512B).
  - **dst_frac_gap** = 0, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal in the destination operand is 0 (unit: 512B).

  ![](../../figures/asc_copy_l12l0_trans_b4_example1.png)

  As shown in the figure below:

  - The explanations of **repeat_time** and **src_stride** are the same as those in the figure above.
  - **dst_gap** = 0, indicating that between adjacent iterations, there is no gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand.
  - **dst_frac_gap** = 2, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal of the destination operand is 2 (unit: 512B).

  ![](../../figures/asc_copy_l12l0_trans_b4_example2.png)

## Prototype

- Regular transpose transfer

  ```cpp
  __aicore__ inline void asc_copy_l12l0b_trans(__cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans(__cb__ fp8_e4m3fn_t* dst, __cbuf__ fp8_e4m3fn_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans(__cb__ fp8_e5m2_t* dst, __cbuf__ fp8_e5m2_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans(__cb__ half* dst, __cbuf__ half* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans(__cb__ float* dst, __cbuf__ float* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans(__cb__ hifloat8_t* dst, __cbuf__ hifloat8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans(__cb__ int32_t* dst, __cbuf__ int32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans(__cb__ int8_t* dst, __cbuf__ int8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans(__cb__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans(__cb__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans(__cb__ int4b_t* dst, __cbuf__ int4b_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap)
  __aicore__ inline void asc_copy_l12l0b_trans(__cb__ fp4x2_e2m1_t* dst, __cbuf__ fp4x2_e2m1_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans(__cb__ fp4x2_e1m2_t* dst, __cbuf__ fp4x2_e1m2_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  ```

- Synchronous transpose transfer

  ```cpp
  __aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ fp8_e4m3fn_t* dst, __cbuf__ fp8_e4m3fn_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ fp8_e5m2_t* dst, __cbuf__ fp8_e5m2_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ half* dst, __cbuf__ half* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ float* dst, __cbuf__ float* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ hifloat8_t* dst, __cbuf__ hifloat8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ int32_t* dst, __cbuf__ int32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ int8_t* dst, __cbuf__ int8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ int4b_t* dst, __cbuf__ int4b_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap)
  __aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ fp4x2_e2m1_t* dst, __cbuf__ fp4x2_e2m1_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);
  __aicore__ inline void asc_copy_l12l0b_trans_sync(__cb__ fp4x2_e1m2_t* dst, __cbuf__ fp4x2_e1m2_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap, uint16_t dst_frac_gap, uint16_t src_frac_gap);

  ```

## Parameters

| Parameter              | Input/Output | Description                                                                                                                                                                                               |
|:-----------------| :--- |:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| dst              | Output | Destination L0B Buffer address. |
| src              | Input | Source L1 Buffer address. |
| index_id | Input | Matrix ID, indicating which fractal in the source operand the transfer starts from. Value range: [0, 65535]. |
|repeat | Input | Number of iterations.<br> For data types with a bit width of 4, each iteration processes 4 fractals, with each fractal being 16*64*0.5B of data;<br> For data types with a bit width of 8, each iteration processes 2 fractals, with each fractal being 16*32*1B of data;<br>For data types with a bit width of 16, each iteration processes 1 fractal, with each fractal being 16*16*2B of data; For data types with a bit width of 32, each iteration processes 4 fractals, with each fractal being 16*8*4B of data. Value range: [1, 255]. |
| src_stride | Input | Gap between the start address of the previous fractal and the next fractal of the source operand between adjacent iterations. The unit is a single fractal of 512B. Value range: [0, 65535]. |
| dst_gap | Input | Gap between the end address of the previous fractal and the start address of the next fractal of the destination operand between adjacent iterations. The unit is a single fractal of 512B. Value range: [0, 65535]. |
| dst_frac_gap | Input | Gap between the end address of the previous fractal and the start address of the next fractal of the destination operand before transposition within each iteration. The unit is 512B. Valid only when the data bit width is 4, 8, or 32. |
| src_frac_gap | Input | Gap between the end address of the previous fractal and the start address of the next fractal of the source operand before transposition within each iteration. The unit is 512B. Valid only when the data bit width is 4, 8, or 32. |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- repeat=0 indicates that no transfer operation is performed.
- Developers must ensure that the transposed fractals of the destination operand do not overlap.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).

## Example

```cpp
//Set the source operand and destination operand. total_length indicates the data length involved in the computation.
constexpr uint64_t total_length = 512;
__cbuf__ int32_t src[total_length];
__cb__ int32_t dst[total_length];

// Set the configuration during the data transfer.
uint8_t n = 64;
uint8_t nBlockSize = 32;
uint8_t repeat = n / nBlockSize;
uint16_t index_id = 0;
uint16_t src_stride = 1;
uint16_t dst_gap = 1;
uint16_t dst_frac_gap = 15;
uint16_t src_frac_gap = 8;

asc_copy_l12l0b_trans(dst, src, index_id, repeat, src_stride, dst_gap, dst_frac_gap, src_frac_gap);
```
