# asc_get_phy_buf_addr

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T13:11:59.368Z pushedAt=2026-09-08T03:45:05.420Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
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

Obtains the actual on-chip physical address based on the **offset**.

## Prototype

```cpp
__aicore__ inline uint64_t asc_get_phy_buf_addr(uint64_t offset)
```

## Parameters

|Parameter|Input/Output|Description|
| :------ | :--- | :------------ |
|offset   |Input   |Address offset of the on-chip memory, ranging from 0 to the physical memory size.|

## Return Value

Returns the physical memory address.

## Pipeline Type

PIPE_S

## Constraints

This method cannot be mixed with the [method of allocating memory as an array](../general_description_and_constraints.md#allocating-memory-as-an-array).

## Example

```cpp
// Assume that the three operands src0, src1, and dst each contain 64 half-type data elements.
uint64_t offset = 0;                                   // First apply for memory for src0, starting from 0.
__ubuf__ half* src0 = (__ubuf__ half*)asc_get_phy_buf_addr(offset);    // Obtain the address of src0, and use the __ubuf__ keyword to specify that the address points to UB memory.
offset += 64 * sizeof(half);                           // Use offset to place the start address of src1 after src0.
__ubuf__ half* src1 = (__ubuf__ half*)asc_get_phy_buf_addr(offset);    // Obtain the address of src1, and use the __ubuf__ keyword to specify that the address points to UB memory.
offset += 64 * sizeof(half);                           // Set the start address of dst after src1 by using offset.
__ubuf__ half* dst = (__ubuf__ half*)asc_get_phy_buf_addr(offset);     // Obtain the address of dst, and use the __ubuf__ keyword to specify that the address points to UB memory.
...... // Use the data in src0, src1, and dst for subsequent computation or data movement operations.
```
