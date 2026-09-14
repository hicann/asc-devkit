# GetLogMaxMinTmpSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T16:10:24.301Z -->

## Description

Host-side API used to obtain the minimum temporary space size required for the Log API to complete computation. This space is reserved space, meaning that sufficient physical space must be reserved for computation.

## Prototype

```
void GetLogMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

```
void GetLog10MaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

```
void GetLog2MaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| isReuseSource | Input | Whether to reuse the space of the source operand input, consistent with the Log API. |
| maxValue | Output | Maximum temporary space size required for the Log API to complete computation. Space exceeding this value is not used by this API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, developers can reserve/allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br><br>Note that maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, developers need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the Log API to complete computation. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

## Return Value

None

## Constraints

None

## Examples

For the complete calling example, see [More Samples](../more_samples_83.md).

-   GetLogMaxMinTmpSize API example:

    ```
    // Input shape information is 1024; the operator input data type is half; source operands cannot be modified.
    std::vector<int64_t> shape_vec = {1024};
    ge::Shape shape(shape_vec);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetLogMaxMinTmpSize(shape, 2, false, maxValue, minValue);
    ```

-   GetLog10MaxMinTmpSize API example:

    ```
    // Input shape information is 1024; the operator input data type is half; source operands cannot be modified.
    std::vector<int64_t> shape_vec = {1024};
    ge::Shape shape(shape_vec);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetLog10MaxMinTmpSize(shape, 2, false, maxValue, minValue);
    ```

-   GetLog2MaxMinTmpSize API example:

    ```
    // The input shape information is 1024; the operator input data type is half; modifying the source operand is not allowed.
    std::vector<int64_t> shape_vec = {1024};
    ge::Shape shape(shape_vec);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetLog2MaxMinTmpSize(shape, 2, false, maxValue, minValue);
    ```

