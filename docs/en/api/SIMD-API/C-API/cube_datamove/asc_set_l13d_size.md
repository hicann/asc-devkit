# asc_set_l13d_size

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T11:00:50.929Z -->

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

Sets the boundary value of the 3D format data transfer API of [asc_copy_l12l0a](asc_copy_l12l0a/asc_copy_l12l0a_arch_2201.md)/[asc_copy_l12l0b](asc_copy_l12l0b/asc_copy_l12l0b_arch_2201.md) in the L1 Buffer.

## Prototype

```cpp
__aicore__ inline void asc_set_l13d_size(uint64_t value)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| **value** | Input | Boundary value used to set the boundary of the 3D format data transfer API of **asc_copy_l12l0a**/**asc_copy_l12l0b** in the L1 Buffer, in bytes. In non-zero scenarios, the boundary value cannot be less than 1024.<br>If **value** is set to 0, it indicates no boundary, and the **asc_copy_l12l0a**/**asc_copy_l12l0b** API can use the entire L1 Buffer. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Must be used together with the 3D format data transfer API of **asc_copy_l12l0a**/**asc_copy_l12l0b**.
- If the boundary value is set through the **asc_set_l13d_size** API, the initial L1 Buffer address of the 3D format data transfer API of **asc_copy_l12l0a**/**asc_copy_l12l0b** must fall within the set boundary (value).
- If the address of the source operand of the 3D format data transfer API of **asc_copy_l12l0a**/**asc_copy_l12l0b** in the L1 Buffer exceeds the set boundary value (value), data is read starting from the L1 Buffer start address.
- For operand address alignment constraints, see [General Address Alignment Constraints](../general_description_and_constraints.md#general-address-alignment-constraints).



## Example

```cpp
uint64_t value = 1024;
asc_set_l13d_size(value);    // Set the boundary value of the L1 Buffer to 1024 (unit: byte).
```
