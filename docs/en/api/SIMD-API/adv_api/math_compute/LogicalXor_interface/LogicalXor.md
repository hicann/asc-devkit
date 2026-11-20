# LogicalXor

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-09T11:16:04.732Z pushedAt=2026-11-18T08:54:29.593Z nl=2 -->

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
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/LogicalXor_interface/LogicalXor_res.md#id1 -->

## Description

Performs element-wise logical XOR operation. When the input data type is not bool, zero is treated as False and non-zero data is treated as True.

![](../../../../figures/formulaimage_0000002475452485.png)

## Prototype

```
template <const LogicalXorConfig& config = DEFAULT_LOGICAL_XOR_CONFIG, typename T, typename U>
__aicore__ inline void LogicalXor(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, const uint32_t count)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| config | Configuration of the LogicalXor algorithm. This parameter is optional and is of the LogicalXorConfig type. Its definition is shown in the following code, where the parameter meanings are as follows:<br><br>isReuseSource: This parameter is reserved. Pass the default value false. |
| T | Data type of the destination operand. Supported data type: bool. |
| U | Data type of the source operands. Supported data types: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t. |

```
struct LogicalXorConfig {
    bool isReuseSource;
};
```

**Table 2**  Parameter description

| Parameter Name | Type | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| src0, src1 | Input | Source operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   For the operand address offset alignment requirements, see [General Description and Constraints](../../../general_description_and_constraints.md).

## Examples

```
AscendC::LocalTensor<uint32_t> src0, src1;
AscendC::LocalTensor<bool> dst;
uint32_t count = 512; // Number of elements involved in the computation.
AscendC::LogicalXor(dst, src0, src1, count);
```

The result is as follows:

```
Input data (src0):
[-4, 4, 1, -8, 0, ... 3]
Input data (src1):
[-1, 3, 1, 5, 1, ... -3]
Output data (dst):
[False, False, False, False, True, ... False]
```
