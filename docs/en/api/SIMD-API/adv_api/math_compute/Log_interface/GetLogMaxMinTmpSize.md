# GetLogMaxMinTmpSize

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-09T11:13:20.947Z pushedAt=2026-11-18T08:54:29.511Z nl=2 -->

## Description

Host-side API used to obtain the minimum temporary space size required for the Log API to complete computation. This space is reserved space, meaning that sufficient physical space must be reserved for computation.

## Prototype

```
void GetLogMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

```
void GetLog10MaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

```
void GetLog2MaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. The parameter type is [AscendC::TensorShape](../../data_structures/TensorShape.md). |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| isReuseSource | Input | Whether to reuse the space of the source operand input, consistent with the Log API. |
| maxValue | Output | Maximum temporary space size required for the Log API to compete the computation, in bytes. Space beyond this value is not used by this API. Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve or allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br><br>Note that maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer (UB). In this case, you need to select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Minimum temporary space size required for the Log API to compete the computation, in bytes. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

## Return Value

None

## Constraints

None

## Examples

For the complete call example, see [More Examples](../more_examples.md).

-   GetLogMaxMinTmpSize API example:

    ```
    // Input shape information is 1024; the operator input data type is half; the source operand cannot be modified.
    std::vector<int64_t> shape_vec = {1024};
    AscendC::TensorShape shape(shape_vec);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetLogMaxMinTmpSize(shape, 2, false, maxValue, minValue);
    ```

-   GetLog10MaxMinTmpSize API example:

    ```
    // Input shape information is 1024; the operator input data type is half; the source operand cannot be modified.
    std::vector<int64_t> shape_vec = {1024};
    AscendC::TensorShape shape(shape_vec);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetLog10MaxMinTmpSize(shape, 2, false, maxValue, minValue);
    ```

-   GetLog2MaxMinTmpSize API example:

    ```
    // Input shape information is 1024; the operator input data type is half; the source operand cannot be modified.
    std::vector<int64_t> shape_vec = {1024};
    AscendC::TensorShape shape(shape_vec);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetLog2MaxMinTmpSize(shape, 2, false, maxValue, minValue);
    ```
