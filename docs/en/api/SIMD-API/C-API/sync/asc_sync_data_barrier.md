# asc_sync_data_barrier

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T12:50:04.573Z pushedAt=2026-09-08T03:45:05.387Z -->

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

Blocks the execution of subsequent instructions until all preceding memory access instructions (the memory locations to wait for can be controlled by parameters) have completed.

## Prototype

```cpp
__aicore__ inline void asc_sync_data_barrier(mem_dsb_t arg)
```

## Parameters

| Parameter | Input/Output | Description |
| :---  | :--- | :--- |
| arg | Input | Indicates the memory location to wait for. The type is **mem_dsb_t**, and the value can be: <br>&bull; **DSB_ALL**: Waits for all memory access instructions. <br>&bull; **DSB_DDR**: Waits for GM access instructions.<br>&bull; **DSB_UB**: Waits for UB access instructions.<br>&bull; **DSB_SEQ**: Reserved parameter, not yet enabled, reserved for future function extension.|

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

The input parameter of the **mem_dsb_t** type must directly pass an enumeration value literal.

## Example

```cpp
asc_sync_data_barrier(mem_dsb_t::DSB_ALL);
```