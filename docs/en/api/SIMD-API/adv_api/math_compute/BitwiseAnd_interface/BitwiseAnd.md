# BitwiseAnd

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T02:16:16.279Z pushedAt=2026-10-29T02:15:02.530Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/BitwiseAnd_interface/BitwiseAnd_res.md#id1 -->

## Description

Performs a bitwise AND operation on two inputs.

![](../../../../figures/zh-cn_formulaimage_0000002386108598.png)

## Prototype

```
template <const BitwiseAndConfig& config = DEFAULT_BITWISE_AND_CONFIG, typename T>
__aicore__ inline void BitwiseAnd(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const uint32_t count)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| config | Configuration of the BitwiseAnd algorithm. This parameter is optional and of the BitwiseAndConfig type. Its definition is shown in the following code, where the parameters are described as follows:<br><br>isReuseSource: This parameter is reserved. Pass the default value false. |
| T | Data type of the operands. Supported data types: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t. |

```
struct BitwiseAndConfig {
    bool isReuseSource;
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| src0, src1 | Input | Source operands.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The data type of the source operands must be the same as that of the destination operand. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Source operands and destination operands must not overlap in address.**
-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
AscendC::LocalTensor<uint16_t> dst, src0, src1;
uint32_t count = 512; // Number of elements involved in the computation.
AscendC::BitwiseAnd(dst, src0, src1, count);
```

The result is as follows:

```
Input data (src0):
[93, 87, 99, 1, 87, 58, 6, 16, 85, 66, 56, 65, 24, 98, 96, 50, 18, 37, 0, ... 66]
Input data (src1):
[81, 12, 33, 47, 47, 18, 82, 44, 91, 34, 17, 99, 7, 82, 42, 85, 88, 99, 61, ... 81]
Output data (dst):
[81, 4, 33, 1, 7, 18, 2, 0, 81, 2, 16, 65, 0, 66, 32, 16, 16, 33, 0, ... 64]
```
