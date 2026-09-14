# asc_set_l0c2gm_relu_alpha

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T13:27:30.391Z pushedAt=2026-09-08T03:45:05.440Z -->

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

Sets the value in the **RELU_ALPHA** register. This is a 64-bit register that stores the alpha value used for Scalar ReLU in fixpipe or cube instructions.

The meaning of each bit is as follows:

| bit range |                                           Meaning                                           |
|:------|:--------------------------------------------------------------------------------------:|
| 31:13 | Indicates the M2 value of Scalar ReLU in ReLU_PRE, which takes effect only for Scalar ReLU. The hardware uses it for computation in the format of 1 sign bit, 8 exponent bits, and 10 mantissa bits. It cannot be INF/NAN.  |
| 63:45 | Indicates the M2 value of Scalar ReLU in ReLU_POST, which takes effect only for Scalar ReLU. The hardware uses it for computation in the format of 1 sign bit, 8 exponent bits, and 10 mantissa bits. It cannot be INF/NAN. |

## Prototype

```cpp
__aicore__ inline void asc_set_l0c2gm_relu_alpha(uint64_t config)
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
uint32_t pre_m2 = (0<<18)|(127<<10)|0;
uint32_t post_m2 = (0<<18)|(124<<10)|0;
uint64_t config = ((uint64_t)pre_m2<<13)|((uint64_t)post_m2<<45);
asc_set_l0c2gm_relu_alpha(config);
```