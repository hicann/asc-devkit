# GetPowerMaxMinTmpSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T09:08:35.915Z -->

## Description

The **Power** API on the kernel side requires you to reserve or apply for temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve or apply for on the host side. Based on this range, you can select an appropriate space size and pass it to the kernel side as a **Tiling** parameter.

-   To ensure correct functionality, the reserved or applied temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve or apply for space based on the actual memory usage.

Internally, the API determines, based on the **srcShape1** and **srcShape2** inputs, which of the following types the API belongs to: Power\(dstTensor, srcTensor1, srcTensor2\), Power\(dstTensor, srcTensor1, scalarValue\), or Power\(dstTensor, scalarValue, srcTensor2\), and then returns the corresponding temporary space size.

## Function Prototype

```
void GetPowerMaxMinTmpSize(const ge::Shape& srcShape1, const ge::Shape& srcShape2, const bool typeIsInt, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1** API parameter list

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| srcShape1 | Input | Shape information of the input **srcTensor1**. |
| srcShape2 | Input | Shape information of the input **srcTensor2**. |
| typeIsInt | Input | **bool** type. **true** indicates that the input is **int32_t**. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is **half**, pass 2. |
| isReuseSource | Input | Whether to reuse the space of the source operand input, consistent with the **Power** API. |
| maxValue | Output | Maximum temporary space size required for the **Power** API to complete computation. Space exceeding this value is not used by the API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve or apply for space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br><br>Note that **maxValue** is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the **Power** API to complete computation. To ensure correct functionality, the temporary space reserved or applied for during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

## Return Value

None

## Constraints

None

## Examples

For complete examples, see [More Samples](../more_samples_83.md).

-   Power\(dstTensor, srcTensor1, srcTensor2\) example

    ```
    // The input shape of both srcTensor1 and srcTensor2 is 512; the input data type of the operator is float; modifying the source operands is not allowed.
    std::vector<int64_t> shape_vec = {1024};
    ge::Shape shape(shape_vec);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetPowerMaxMinTmpSize(shape, shape, false, 4, false, maxValue, minValue);
    ```

-   Power\(dstTensor, srcTensor1, scalarValue\) example

    ```
    // The input shape of srcTensor1 is 128*128, and the shape of scalarValue is 1; the input data type of the operator is half; modifying the source operand is not allowed.
    std::vector<int64_t> shape1_vec = {128,128};
    std::vector<int64_t> shape2_vec = {1};
    ge::Shape shape1(shape1_vec);
    ge::Shape shape2(shape2_vec);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetPowerMaxMinTmpSize(shape1, shape2, false, 2, false, maxValue, minValue);
    ```

-   Power\(dstTensor, scalarValue, srcTensor2\) example

    ```
    // The shape of scalarValue is 1, and the input shape of srcTensor2 is 128*128; the input data type of the operator is float; modifying the source operand is not allowed.
    std::vector<int64_t> shape1_vec = {1};
    std::vector<int64_t> shape2_vec = {128,128};
    ge::Shape shape1(shape1_vec);
    ge::Shape shape2(shape2_vec);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetPowerMaxMinTmpSize(shape1, shape2, false, 4, false, maxValue, minValue);
    ```

