# asc_set_l0c2gm_config

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:35:37.564Z -->

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

During in-path quantization in the data transfer process, call this API to set the vector quantization parameters in the quantization flow.

## Prototype

```cpp
__aicore__ inline void asc_set_l0c2gm_config(uint64_t relu_pre, uint64_t quant_pre, bool enable_unit_flag)
```

## Parameters

|Parameter|Input/Output|Description|
|------------|------------|-----------|
| **relu_pre** | Input     | Start address of the vector before the ReLU operation.|
| **quant_pre** | Input     | Start address of the vector before the quantization operation.|
| **enable_unit_flag** | Input     | Whether to enable **unit_flag**. **unit_flag** is a fine-grained parallelism between matrix computation instructions and matrix transfer instructions. When this feature is enabled, the computation result is transferred out as soon as the hardware finishes computing each fractal. This feature is not applicable to **L0C Buffer** accumulation scenarios.|
## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

The following three parameter passing forms are supported:
- Set both **relu_pre** and **quant_pre**.
- Pass only **relu_pre**, with **quant_pre** set to **0**.
- Pass only **quant_pre**, with **relu_pre** set to **0**.

## Example

```cpp
constexpr uint64_t relu_pre = 0;
constexpr uint64_t quant_pre = 0x1000;// Assume the effective address of the quantization operation is 0x1000.
asc_set_l0c2gm_config(relu_pre, quant_pre, true);
```
