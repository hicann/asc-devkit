# asc_copy_gm2ub

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-08T04:16:48.260Z pushedAt=2026-09-08T13:17:42.502Z -->

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

Copies data from Global Memory (GM) to Unified Buffer (UB).

## Prototype

- Copies the first n data items

```c++
__aicore__ inline void asc_copy_gm2ub(__ubuf__ void* dst, __gm__ void* src, uint32_t size)
```

- High-dimensional split copy

```c++
__aicore__ inline void asc_copy_gm2ub(__ubuf__ void* dst, __gm__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
```

- Synchronous computation

```c++
__aicore__ inline void asc_copy_gm2ub_sync(__ubuf__ void* dst, __gm__ void* src, uint32_t size)
```

## Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Destination UB address. |
| src | Input | Source GM address. |
| size | Input | Size of the data to be copied (in bytes). |
| n_burst | Input | Number of consecutive data blocks to be copied. Value range: [1, 4095]. |
| len_burst | Input | Length of each consecutive data block to be copied. The unit is DataBlock (32 bytes). Value range: [1, 65535]. |
| src_gap | Input | Gap between adjacent consecutive data blocks of the source operand (the gap between the tail of the preceding data block and the head of the following data block).<br>The unit is DataBlock (32 bytes). |
| dst_gap | Input | Gap between adjacent consecutive data blocks of the destination operand (the gap between the end of the preceding data block and the start of the following data block).<br>The unit is DataBlock (32 bytes). |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- The start address of **src** must be aligned to the byte size of the corresponding data type.
- The start address of **dst** must be 32-byte aligned.
- If multiple **asc_copy_gm2ub** instructions need to be executed and their destination addresses overlap, a synchronization instruction must be inserted to ensure serialization of the multiple **asc_copy_gm2ub** instructions and prevent abnormal data.
- Synchronous computation includes synchronous waiting.
- When the first n data items are copied, the size of the copied data must be 32-byte aligned.

## Example

```cpp
// total_length indicates the total length of the data involved in the copy. src is the half-type GM memory input from outside.
constexpr uint64_t total_length = 128;
__ubuf__ half dst[total_length];
asc_copy_gm2ub(dst, src, total_length * sizeof(half));
```
