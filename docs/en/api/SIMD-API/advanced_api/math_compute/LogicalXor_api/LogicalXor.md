# LogicalXor

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-10T09:03:55.012Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs an element-wise logical XOR operation. When the input data type is not bool, zero is treated as False and non-zero data is treated as True.

![](../../../../figures/zh-cn_formulaimage_0000002475452485.png)

## Prototype

```
template <const LogicalXorConfig& config = DEFAULT_LOGICAL_XOR_CONFIG, typename T, typename U>
__aicore__ inline void LogicalXor(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, const uint32_t count)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| config | Configuration of the LogicalXor algorithm. This parameter is optional and of the LogicalXorConfig type. Its definition is shown in the following code, where the parameter meanings are as follows:<br><br>**isReuseSource**: This parameter is reserved. Pass the default value **false**. |
| T | Data type of the destination operand. Supported data type: bool. |
| U | Data type of the source operand. Supported data types: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, and uint64_t. |

```
struct LogicalXorConfig {
    bool isReuseSource;
};
```

**Table 2** Parameter description

| Parameter | Type | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| src0, src1 | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   For operand address offset alignment requirements, see [General Description and Constraints](../../../general_description_and_constraints.md).

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
