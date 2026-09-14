# asc_copy_ub2gm

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-08T04:28:35.615Z pushedAt=2026-09-08T13:17:42.512Z -->

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

Copies data from the Unified Buffer (UB) to Global Memory (GM).

## Prototype

- Copies the first n data items

```c++
__aicore__ inline void asc_copy_ub2gm(__gm__ void* dst, __ubuf__ void* src, uint32_t size)
```

- High-dimensional split copy

```c++
__aicore__ inline void asc_copy_ub2gm(__gm__ void* dst, __ubuf__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t dst_stride, uint16_t src_stride)
```

- Synchronous computation

```c++
__aicore__ inline void asc_copy_ub2gm_sync(__gm__ void* dst, __ubuf__ void* src, uint32_t size)
```

## Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Target GM address. |
| src | Input | Source UB address. |
| size | Input | Size of the data to be copied (in bytes). |
| n_burst | Input | Number of consecutive data blocks to be copied. Value range: [1, 4095]. |
| len_burst | Input | Length of each consecutive data block to be copied, in bytes. Value range: [1, 2097151]. |
| dst_stride | Input | Gap between adjacent consecutive data blocks of the destination operand (the gap between the head of the preceding data block and the head of the following data block).<br>The unit is bytes. |
| src_stride | Input | Gap between adjacent consecutive data blocks of the source operand (the gap between the head of the preceding data block and the head of the following data block).<br>The unit is bytes. |

## Return Value

None

## Pipeline Type

PIPE_MTE3

## Constraints

- The start address of **dst** must be aligned to the byte size of the corresponding data type.
- The start address of **src** must be 32-byte aligned.
- If multiple **asc_copy_ub2gm** instructions need to be executed and their destination addresses overlap, a synchronization instruction must be inserted to ensure that the multiple **asc_copy_ub2gm** instructions are serialized, preventing abnormal data.
- Synchronous computation includes synchronous waiting.
- When the first n data items are copied, the size of the copied data must be 32-byte aligned.
- When **src_stride** is not equal to **len_burst**, **src_stride** must be 32-byte aligned.

## Example

```cpp
// total_length refers to the total length of the data to be copied. dst is the half-type GM memory input externally.
constexpr uint64_t total_length = 128;
__ubuf__ half src[total_length];
asc_copy_ub2gm(dst, src, total_length * sizeof(half));
```
