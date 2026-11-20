# GetSortMaxMinTmpSize

<!-- md-trans-meta sourceCommit=aa6d602ebc3dbc8f7b50f91dc72aceefdfa67b49 translatedAt=2026-11-02T05:10:25.258Z pushedAt=2026-11-18T04:58:51.367Z nl=2 -->

## Description

The computation of the kernel-side Sort API with the SortConfig template parameter requires developers to reserve/allocate temporary space. This API is used to obtain the maximum and minimum temporary space sizes to be reserved/allocated on the host side. Based on this range, developers select an appropriate space size as the Tiling parameter and pass it to the kernel side for use.

-   To ensure functional correctness, the reserved/allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, developers can reserve/allocate space based on the actual memory usage.

## Prototype

```
void GetSortMaxMinTmpSize(const AscendC::TensorShape& srcShape, AscendC::TensorDataType valueType, AscendC::TensorDataType indexType, bool isReuseSource, const SortConfig& config, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). |
| valueType | Input | Data type of the input and output Value. The parameter type is [AscendC::TensorDataType](../data_structures/TensorDataType.md). It must be consistent with the template parameter T of the Sort API. |
| indexType | Input | Data type of the input and output Index. The parameter type is [AscendC::TensorDataType](../data_structures/TensorDataType.md). It must be consistent with the template parameter U of the Sort API. |
| isReuseSource | Input | Whether to reuse the space of the source operand input. It must be consistent with the isReuseSource parameter of the Sort API. |
| config | Input | Corresponding configuration of Sort: the selected sorting algorithm, the ascending/descending order of the sorting result, and whether the input and output carry index data. The data type is SortConfig, defined as shown in the following code. The parameters hasSrcIndex and hasDstIndex must be consistent with whether the Sort API used carries input indexes and output indexes. Currently, the combination hasSrcIndex = true, hasDstIndex = false is not supported. |
| maxValue | Output | Maximum temporary space size required for the Sort API to complete the computation, in bytes. Space exceeding this value is not used by this API. Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, developers can reserve/allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for the computation.<br> Note: maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer (UB). In this case, developers need to select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Minimum temporary space size required for the Sort API to complete the computation, in bytes. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for the computation. |

```
enum class SortType {
    RADIX_SORT, // Use the radix sort algorithm.
    MERGE_SORT  // Use the merge sort algorithm.
};
struct SortConfig {
    SortType type = SortType::RADIX_SORT; // Sorting algorithm.
    bool isDescend = false;               // Whether to sort in descending order. The default value is false, which means the output is sorted in ascending order.
    bool hasSrcIndex = false;             // Whether to carry input indexes.
    bool hasDstIndex = false;             // Whether to carry output indexes.
};
```

## Return Value

None

## Constraints

None

## Examples

For the complete call example, see [More Examples](../math_compute/more_examples.md).

```
// The input shape information is 1024; the input data type of the operator is uint32_t; the source operand cannot be modified.
std::vector<int64_t> shape_vec = {1024};
AscendC::TensorShape srcShape(shape_vec);
AscendC::TensorDataType valueType = AscendC::TensorDataType::DT_UINT32;
AscendC::TensorDataType indexType = AscendC::TensorDataType::DT_UINT32;
bool isDescend = true;
bool hasSrcIndex = false;
bool hasDstIndex = false;
bool isReuseSource = false;
AscendC::SortConfig config;
config.type = AscendC::SortType::RADIX_SORT;
config.isDescend = isDescend;
config.hasSrcIndex = hasSrcIndex;
config.hasDstIndex = hasDstIndex;
uint32_t maxValue = 0;
uint32_t minValue = 0;
AscendC::GetSortMaxMinTmpSize(srcShape, valueType, indexType, isReuseSource, config, maxValue, minValue);
```
