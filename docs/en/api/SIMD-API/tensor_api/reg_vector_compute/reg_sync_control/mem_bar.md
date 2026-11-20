# mem_bar

<!-- md-trans-meta sourceCommit=cd39cbc029c72c578e1c505b81bb7335b331ea4d translatedAt=2026-10-27T04:10:09.079Z pushedAt=2026-10-27T13:03:35.465Z nl=2 -->

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

The header file path is `"tensor_api/experimental/arch/vector/reg_sync_control.h"`, and the API namespace is `asc::te::experimental`.

`mem_bar` is a synchronization API between different pipelines within a Reg vector computation function. The API specifies the source pipeline and the destination pipeline through template parameters: Unified Buffer (UB) accesses in the destination pipeline that occur after `mem_bar` can be executed only after all UB accesses in the source pipeline that occur before `mem_bar` are complete.

In read/write scenarios, if the read and write instructions use the same register, the hardware can trigger register ordering, and the instructions are executed in code order without inserting a synchronization API. If different registers are used and the two pipelines must access the same UB address space in code order, call this API. The same applies to write-after-write scenarios.

This API takes effect only on the AIV and is not used for MTE pipeline synchronization, inter-core synchronization, or whole-core synchronization.

## Function Prototype

```cpp
template <mem_type src, mem_type dst>
__simd_callee__ inline void mem_bar()
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| src | Source pipeline, of type `mem_type`. UB accesses in this pipeline that occur before this API must complete first. For details, see [Table 2](#mem-type-values). |
| dst | Destination pipeline, of type `mem_type`. UB accesses in this pipeline that occur after this API wait for the source pipeline. For details, see [Table 2](#mem-type-values). |

<a id="mem-type-values"></a>

**Table 2** mem_type value description

| mem_type Value | Meaning |
| --- | --- |
| vec_store | Vector UB write pipeline within the `__simd_vf__` function, corresponding to the register-to-UB data move-out API, such as `store`. |
| vec_load | Vector UB read pipeline within the `__simd_vf__` function, corresponding to the UB-to-register data move-in API, such as `load`. |
| scalar_store | Scalar UB write pipeline within the `__simd_vf__` function. |
| scalar_load | Scalar UB read pipeline within the `__simd_vf__` function. |
| vec_all | All vector UB read/write pipelines within the `__simd_vf__` function. |
| scalar_all | All scalar UB read/write pipelines within the `__simd_vf__` function. |

<a id="mem-bar-supported-combinations"></a>

**Table 3** `src` and `dst` combination values

| src | dst |
| --- | --- |
| `mem_type::vec_store` | `mem_type::vec_load` |
| `mem_type::vec_load` | `mem_type::vec_store` |
| `mem_type::vec_store` | `mem_type::vec_store` |
| `mem_type::vec_store` | `mem_type::scalar_load` |
| `mem_type::vec_store` | `mem_type::scalar_store` |
| `mem_type::vec_load` | `mem_type::scalar_store` |
| `mem_type::scalar_store` | `mem_type::vec_load` |
| `mem_type::scalar_store` | `mem_type::vec_store` |
| `mem_type::scalar_load` | `mem_type::vec_store` |
| `mem_type::vec_all` | `mem_type::vec_all` |
| `mem_type::vec_all` | `mem_type::scalar_all` |
| `mem_type::scalar_all` | `mem_type::vec_all` |

## Return Value

None

## Data Type

None

## Constraints

- Only the template parameter combinations listed in [Table 3](#mem-bar-supported-combinations) are supported. Other combinations cause a compile-time error.
- In read/write dependency scenarios, if the read instruction and the write instruction use the same register, register ordering is triggered and you do not need to call this API.
- Call this API only when the UB addresses accessed by different pipelines have dependencies. Redundant synchronization degrades performance.
- This API should be called in a `__simd_vf__` function or a `__simd_callee__` function.

## Example

In the following example, `data` is both the source tensor for moving data in and the destination tensor for moving data out. The vector read UB operation in the later loop iteration and the vector write UB operation in the previous loop iteration access the same UB address space. Therefore, call `mem_bar` before each read so that the vector read pipeline waits for the vector write pipeline to complete.

```cpp
template <typename Tensor>
__simd_vf__ inline void Process(Tensor& data, uint16_t repeatTimes)
{
    using T = typename Tensor::data_type;
    const auto mask = asc::te::experimental::all_mask<T>();
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        asc::te::experimental::mem_bar<asc::te::experimental::mem_type::vec_store,
                                       asc::te::experimental::mem_type::vec_load>();
        auto value = asc::te::experimental::load(data, asc::te::make_coord(0));
        value.with_mask(mask);
        // Complete register computation using value.
        asc::te::experimental::store(data, asc::te::make_coord(0), value);
    }
}
```
