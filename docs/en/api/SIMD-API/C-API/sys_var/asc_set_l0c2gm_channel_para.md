# asc_set_l0c2gm_channel_para

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T13:25:35.867Z pushedAt=2026-09-08T03:45:05.438Z -->

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

Sets the bits of the dedicated register for the channel stride parameter. It must be used together with [asc_copy_l0c2l1](../cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_3510.md) or [asc_copy_l0c2gm](../cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_3510.md). This function takes effect only when the NZ2DN mode is enabled upon calling the API.

## Prototype

```cpp
__aicore__ inline void asc_set_l0c2gm_channel_para(uint64_t config)
```

## Parameters

Table 1 Parameter description

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| config | Input | Register value to be set. For the description of the common channel stride register bits, see Table 2. |

Table 2 Description of common channel stride register bits

|CHANNEL_PARA Bit    |Function|
| :-------     | :---- |
| CHANNEL_PARA[48-0]    |  Reserved bits. Setting them has no effect.  | 
| CHANNEL_PARA[63-48]    |  Indicates the loop0 source stride, in C0_SIZE. It takes effect when the NZ2DN mode is enabled and cannot be 0.  | 

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

It must be called before the corresponding data movement API.

## Example

```cpp
uint64_t config = 0;
asc_set_l0c2gm_channel_para(config);
```
