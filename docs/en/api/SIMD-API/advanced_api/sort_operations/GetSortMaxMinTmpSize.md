# GetSortMaxMinTmpSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T17:08:39.084Z -->

## Description

The Sort API with the SortConfig template parameter on the kernel side requires you to reserve/allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve/allocate on the host side. Based on this range, you can select an appropriate space size and pass it to the kernel side as a Tiling parameter.

-   To ensure correctness, the reserved/allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage.

## Prototype

```
void GetSortMaxMinTmpSize(const ge::Shape &srcShape, ge::DataType valueType, ge::DataType indexType, bool isReuseSource, const SortConfig &config, uint32_t &maxValue, uint32_t &minValue)
```

## Parameters

**Table 1**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. |
| valueType | Input | Data type of the input and output Value. Keep consistent with the template parameter T of the Sort API. |
| indexType | Input | Data type of the input and output Index. Keep consistent with the template parameter U of the Sort API. |
| isReuseSource | Input | Whether to reuse the space of the source operand input. Keep consistent with the isReuseSource parameter of the Sort API. |
| config | Input | Corresponding configuration of Sort: the selected sorting algorithm, the ascending/descending order of the sorting result, and whether the input and output carry index data. The data type is SortConfig, defined as shown in the following code. The parameters hasSrcIndex and hasDstIndex keep consistent with whether the Sort API used carries input indexes and output indexes. Currently, the combination of hasSrcIndex = true and hasDstIndex = false is not supported. |
| maxValue | Output | Maximum temporary space size required for the Sort API to complete computation. Space exceeding this value is not used by this API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br> Note: maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer. In this scenario, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the Sort API to complete computation. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

```
enum class SortType {
    RADIX_SORT,  // Implemented using the radix sort algorithm.
    MERGE_SORT   // Implemented using the merge sort algorithm.
};
struct SortConfig {
    SortType type = SortType::RADIX_SORT; // Sorting algorithm.
    bool isDescend = false; // Whether to sort in descending order. The default value is false, and the output result is sorted in ascending order.
    bool hasSrcIndex = false; // Whether to carry input indexes.
    bool hasDstIndex = false; // Whether to carry output indexes.
};
```

## Return Value

None

## Constraints

None

## Examples

For a complete invocation example, see [More Samples](../math_compute/more_samples_83.md).

```
// Input shape information is 1024; the operator input data type is uint32_t; modifying the source operand is not allowed.
std::vector<int64_t> shape_vec = {1024};
ge::Shape srcShape(shape_vec);
ge::DataType valueType = ge::DT_UINT32;
ge::DataType indexType = ge::DT_UINT32;
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
