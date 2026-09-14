# asc_set_l12l0_padding_val

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T13:27:56.211Z pushedAt=2026-09-08T03:45:05.441Z -->

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

Sets the value in the **PADDING_B** register, which is a 64-bit register used to store the padding value during data transfer. The meaning of each bit is as follows:

| Bit Range |                                                                           Meaning                                                                           | 
| :-----------|:------------------------------------------------------------------------------------------------------------------------------------------------------:|
| 31:0 | Stores the padding value. When the data bit width is 32, bits 31:0 are used directly. When the data bit width is 16, bits 15:0 are used and bits 31:16 are ignored. When the data bit width is 8, bits 15:8 must be the same as bits 7:0, and bits 31:16 are ignored. When the data bit width is 4, bits 15:12, 11:8, 7:4, and 3:0 must be the same, and bits 31:16 are ignored. |
| 33:32 | Indicates the padding mode used. Three modes are currently supported. 2'b00: fixed padding. 2'b01: per-channel padding. 2'b10/2'b11: currently reserved. |

## Prototype

```cpp
__aicore__ inline void asc_set_l12l0_padding_val(uint64_t config)
```

## Parameters

| Parameter | Input/Output | Description        |
| :------ | :---  |:----------|
| **config**   | Input   | Register value to be set. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Example
```cpp
uint64_t config = 0;
asc_set_l12l0_padding_val(config);
```
