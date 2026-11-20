# GetPowerMaxMinTmpSize

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-09T11:16:23.927Z pushedAt=2026-11-18T08:54:29.599Z nl=2 -->

## Description

The Power API on the kernel-side requires you to reserve or allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve or allocate on the host side. Based on this range, you can select an appropriate space size as a Tiling parameter to pass to the kernel-side.

-   To ensure correct functionality, the reserved or allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve or allocate space based on the actual memory usage.

Based on the srcShape1 and srcShape2 inputs, the API internally determines whether the API is of the Power\(dstTensor, srcTensor1, srcTensor2\), Power\(dstTensor, srcTensor1, scalarValue\), or Power\(dstTensor, scalarValue, srcTensor2\) type, and then returns the corresponding temporary space size.

## Prototype

```
void GetPowerMaxMinTmpSize(const AscendC::TensorShape& srcShape1, const AscendC::TensorShape& srcShape2, const bool typeIsInt, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape1 | Input | Shape information of the input srcTensor1. The parameter type is [AscendC::TensorShape](../../data_structures/TensorShape.md). |
| srcShape2 | Input | Shape information of the input srcTensor2. The parameter type is [AscendC::TensorShape](../../data_structures/TensorShape.md). |
| typeIsInt | Input | bool type. true indicates that the input is int32_t. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| isReuseSource | Input | Whether to reuse the space of the source operand, consistent with the Power API. |
| maxValue | Output | Maximum temporary space size required for the Power API to complete computation. Space exceeding this value is not used by this API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve or allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br><br>Note that maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer (UB). In this case, you need to select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Minimum temporary space size required for the Power API to complete computation. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

## Return Value

None

## Constraints

None

## Examples

For the complete call example, see [More Examples](../more_examples.md).

-   Power\(dstTensor, srcTensor1, srcTensor2\) example

    ```
    // The shape information of both srcTensor1 and srcTensor2 inputs is 512; the input data type of the operator is float; the source operand cannot be modified.
    std::vector<int64_t> shape_vec = {1024};
    AscendC::TensorShape shape(shape_vec);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetPowerMaxMinTmpSize(shape, shape, false, 4, false, maxValue, minValue);
    ```

-   Power\(dstTensor, srcTensor1, scalarValue\) example

    ```
    // The shape information of the srcTensor1 input is 128*128, and the shape of scalarValue is 1; the input data type of the operator is half; the source operand cannot be modified.
    std::vector<int64_t> shape1_vec = {128, 128};
    std::vector<int64_t> shape2_vec = {1};
    AscendC::TensorShape shape1(shape1_vec);
    AscendC::TensorShape shape2(shape2_vec);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetPowerMaxMinTmpSize(shape1, shape2, false, 2, false, maxValue, minValue);
    ```

-   Power\(dstTensor, scalarValue, srcTensor2\) example

    ```
    // The shape of scalarValue is 1, and the shape information of the srcTensor2 input is 128*128; the input data type of the operator is float; the source operand cannot be modified.
    std::vector<int64_t> shape1_vec = {1};
    std::vector<int64_t> shape2_vec = {128, 128};
    AscendC::TensorShape shape1(shape1_vec);
    AscendC::TensorShape shape2(shape2_vec);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetPowerMaxMinTmpSize(shape1, shape2, false, 4, false, maxValue, minValue);
    ```
