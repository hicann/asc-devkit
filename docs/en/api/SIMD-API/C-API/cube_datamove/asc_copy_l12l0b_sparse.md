# asc_copy_l12l0b_sparse

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:45:06.886Z -->

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

Copies a 512B dense weight matrix stored in the L1 Buffer to the L0B Buffer, and simultaneously reads a 128B index matrix to sparsify the dense matrix.

The index matrix is arranged in reverse order within an `int8_t` address. For example, the index matrix 1 2 0 1 0 2 1 0 is arranged in the address as 1 0 2 1 0 1 2 0, where 1 0 2 1 (corresponding to the first four elements 1 2 0 1 of the index matrix) is combined into one `int8_t` value, and 0 1 2 0 (corresponding to the last four elements 0 2 1 0 of the index matrix) is combined into another `int8_t` value.

## Prototype

- High-dimensional split copy
  ```cpp
  __aicore__ inline void asc_copy_l12l0b_sparse(__cb__ int8_t* dst, __cbuf__ int8_t* src, __cbuf__ int8_t* index, uint16_t start_index, uint8_t repeat)
  ```

- Synchronous copy
  ```cpp
  __aicore__ inline void asc_copy_l12l0b_sparse_sync(__cb__ int8_t* dst, __cbuf__ int8_t* src, __cbuf__ int8_t* index, uint16_t start_index, uint8_t repeat)
  ```

## Parameters

Table 1 Parameters
| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Starting address of the destination operand in the **L0B Buffer**. |
| src | Input | Starting address of the source operand in the **L1 Buffer**. |
| index | Input | Starting address of the index matrix in the **L1 Buffer**. |
| start_index | Input | Fractal matrix ID, indicating the starting position of the copy as the ordinal number of the fractal in the source operand (0 indicates the first fractal matrix in the source operand). Value range: [0, 65535]. Unit: 512B. |
| repeat | Input | Number of iterations, where each iteration processes 512B of data. Value range: [1, 255]. |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of **dst** and **src** must be 32-byte aligned.
- If **repeat** is 0, the operation is not executed.
- **start_index** cannot be less than 0.
- The transpose function is not supported.

## Example

```cpp
__cb__ int8_t dst[256];
__cbuf__ int8_t src[256];
__cbuf__ int8_t index[64];
uint16_t start_index = 1;
uint8_t repeat = 8;
asc_copy_l12l0b_sparse(dst, src, index, start_index, repeat);
```
