# make_mask

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-27T03:53:32.955Z pushedAt=2026-10-27T13:03:35.438Z nl=2 -->

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
- AI Core in Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Vector Core in Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file path is `"tensor_api/experimental/arch/vector/mask_reg_compute.h"`, and the API namespace is `asc::te::experimental`.

Creates a mask register based on the pattern and the data element width, to indicate which elements participate in the computation.

## Function Prototype

```cpp
template <mask_pattern Pattern = mask_pattern::all, typename DataType>
__simd_callee__ inline reg_tensor<bool> make_mask()
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| `Pattern` | Input | Pattern for creating the mask register, of enum class type.<br><code>enum class mask_pattern {<br>    all,      // Set all elements as valid data.<br>    vl1,      // Set the lowest 1 element as valid data.<br>    vl2,      // Set the lowest 2 elements as valid data.<br>    vl3,      // Set the lowest 3 elements as valid data.<br>    vl4,      // Set the lowest 4 elements as valid data.<br>    vl8,      // Set the lowest 8 elements as valid data.<br>    vl16,     // Set the lowest 16 elements as valid data.<br>    vl32,     // Set the lowest 32 elements as valid data.<br>    vl64,     // Set the lowest 64 elements as valid data.<br>    vl128,    // Set the lowest 128 elements as valid data.<br>    every3,   // Set elements at multiples of 3 as valid data.<br>    every4,   // Set elements at multiples of 4 as valid data.<br>    half,     // Set the lowest half of the elements as valid data.<br>    quarter,  // Set the lowest quarter of the elements as valid data.<br>    none      // Set all elements as invalid data.<br>};</code> |
| `DataType` | Input | Template parameter. The supported data types are b8/b16/b32. |

## Return Value

Returns `reg_tensor<bool>`, whose `reg` member is a mask register of type vector_bool, and whose corresponding elements are valid data.

## Constraints

- The returned object only guarantees that the `reg` member is valid, and does not initialize the member variable `mask`.
- The maximum number of mask registers is 8. Register data exceeding this limit is written to the reserved 8KUB memory, which may cause performance degradation. The compiler automatically reuses registers and reserved memory whose lifecycles have ended. If both registers and reserved memory have available space, registers are reused first.

## Example

```cpp
auto mask = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::every3, float>();
auto value = asc::te::experimental::load(src, asc::te::make_coord(0)).with_mask(mask);
```
