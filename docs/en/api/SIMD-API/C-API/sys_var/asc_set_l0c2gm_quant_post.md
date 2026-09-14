# asc_set_l0c2gm_quant_post

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T13:25:52.897Z pushedAt=2026-09-08T03:45:05.439Z -->

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

Sets the value in the QUANT_POST register. QUANT_POST is a 64-bit register that stores the quantization information of the Fixpipe post-processing stage.
The meaning of each bit is as follows. For the description of each quantization mode, see [asc_copy_l0c2l1](../cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_3510.md).

| bit range |                                                           Meaning                                                           |
| :-----------|:----------------------------------------------------------------------------------------------------------------------:|
| 4:0 |                                Indicates the offset in s5 in the QS162S4_POST, QF162S4_POST, and SHIFT2S4_POST quantization modes.                                |
| 8:0 | Indicates the offset in s9 in the QS162B8_POST, QF162B8_POST, and SHIFT2B8_POST quantization modes, or the lower 9 bits of the s17 offset in the QS162S16_POST, QF162S16_POST, and SHIFT2S16_POST quantization modes. |
| 9 |                             Indicates whether the quantization result is signed in the QS162B8_POST, QF162B8_POST, and SHIFT2B8_POST quantization modes.                              |
| 31:13 |         Indicates M3 in the QS162B8_POST, QF162B8_POST, QS162S4_POST, QF162S4_POST, QS162S16_POST, and QF162S16_POST quantization modes. It cannot be INF/NAN.          |
| 39:32 |                           Indicates the upper 8 bits of the s17 offset in the QS162S16_POST, QF162S16_POST, SHIFT2S16_POST, and SHIFT2S32_POST quantization modes. |
| 44:40 | Indicates the shift value in the SHIFT2S4_POST, SHIFT2B8_POST, SHIFT2S16_POST, and SHIFT2S32_POST quantization modes. |


## Prototype

```cpp
__aicore__ inline void asc_set_l0c2gm_quant_post(uint64_t config)
```

## Parameters

|Parameter|Input/Output| Description        |
| :------ | :---  |:----------|
|config   |Input   | Register value to be set. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Example
```cpp
const uint64_t MASK_8_0 = 0x1FF;
const uint64_t MASK_BIT9 = 0x200;
const uint64_t MASK_31_13 = 0xFFFFE000;

uint64_t field_8_0 = 0x1AB;
uint64_t bit_9 = 1;
uint64_t field_31_13 = 0x000EF;

uint64_t config = 0;

config = (config & ~MASK_8_0) | (field_8_0 & MASK_8_0);
config = (config & ~MASK_BIT9) | ((bit_9<<9) & MASK_BIT9);
config = (config & ~MASK_31_13) | (field_31_13 & MASK_31_13);
asc_set_l0c2gm_quant_post(config);
```