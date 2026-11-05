# BitwiseOr

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T02:21:28.087Z pushedAt=2026-10-29T02:15:02.556Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/BitwiseOr_interface/BitwiseOr_res.md#id1 -->

## Description

Performs a bitwise OR operation on two inputs.

![](../../../../figures/zh-cn_formulaimage_0000002442050326.png)

## Prototype

```
template <const BitwiseOrConfig& config = DEFAULT_BITWISE_OR_CONFIG, typename T>
__aicore__ inline void BitwiseOr(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const uint32_t count)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| config | Configuration of the BitwiseOr algorithm. This parameter is optional and of the BitwiseOrConfig type. Its definition is shown in the following code, where the parameters are described as follows:<br><br>isReuseSource: This parameter is reserved. Pass the default value false. |
| T | Data type of the source operands and the destination operand. Supported data types: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, and uint64_t. |

```
struct BitwiseOrConfig {
    bool isReuseSource;
};
```

**Table 2**  Parameter description

| Parameter name | Type | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| src0, src1 | Input | Source operands.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type must be the same as that of the destination operand. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Address overlap between the source operands and the destination operand is not supported.**
-   For the address offset alignment requirements of operands, see [General Description and Constraints](../../../general_description_and_constraints.md).

## Examples

```
AscendC::LocalTensor<uint32_t> src0, src1;
AscendC::LocalTensor<uint32_t> dst;
uint32_t count = 512; // Number of elements involved in the computation.
AscendC::BitwiseOr(dst, src0, src1, count);
```

The result is as follows:

```
Input data (src0):
[-111, -83, 95, -52, ... -102]
Input data (src1):
[64, -44, 45, -117, ... -107]
Input data (dst):
[-47, -3, 127, -49, ... -97]
```
