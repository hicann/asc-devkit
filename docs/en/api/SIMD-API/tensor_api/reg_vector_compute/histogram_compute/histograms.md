# histograms

<!-- md-trans-meta sourceCommit=db86c75495b5d041b217977e24030960f4f2178e translatedAt=2026-11-19T02:51:20.285Z pushedAt=2026-11-19T05:54:06.664Z nl=2 -->

## Supported Products

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
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Function Description

Header file path: `"tensor_api/experimental/arch/vector/histogram_compute.h"`.

This API computes the frequency distribution or cumulative distribution of `uint8_t` input values based on `src.mask`, accumulates the statistics result in place into `dst.reg`, retains `dst.mask` unchanged, and returns the updated `dst` by value. The calculation formula is as follows:

For the $i$-th element in the return value ($i\in[0,127]$), the two statistical modes are respectively:

Frequency mode:

$$
\left[\operatorname{histograms}(dst, src)\right]_i =
\begin{cases}
dst_i + \displaystyle\sum_j (\mathrm{src.mask})_j\mathbf{1}\{src_j=i\},
    & \mathrm{options.half}=\mathrm{histogram\_half::low} \\
dst_i + \displaystyle\sum_j (\mathrm{src.mask})_j\mathbf{1}\{src_j=128+i\},
    & \mathrm{options.half}=\mathrm{histogram\_half::high}
\end{cases}
$$

Cumulative mode:

$$
\left[\operatorname{histograms}(dst, src)\right]_i =
\begin{cases}
dst_i + \displaystyle\sum_j (\mathrm{src.mask})_j\mathbf{1}\{src_j\le i\},
    & \mathrm{options.half}=\mathrm{histogram\_half::low} \\
dst_i + \displaystyle\sum_j (\mathrm{src.mask})_j\mathbf{1}\{src_j\le 128+i\},
    & \mathrm{options.half}=\mathrm{histogram\_half::high}
\end{cases}
$$

Here, when $(\mathrm{src.mask})_j$ is `1`, the $j$-th element of `src` participates in the statistics; when it is `0`, it does not participate. When `high` is used in combination with `cumulative`, the $i$-th output counts all input values not greater than $128+i$, and therefore includes the input values in the range `[0, 127]`.

**Table 1** Configuration item description

| Configuration Item | Value | Meaning |
| --- | --- | --- |
| `options.half` | `histogram_half::low` | Counts input values `[0, 127]`. |
| `options.half` | `histogram_half::high` | Counts input values `[128, 255]`. |
| `options.mode` | `histogram_mode::frequency` | Each output element represents the number of occurrences of the corresponding input value. |
| `options.mode` | `histogram_mode::cumulative` | Each output element represents the cumulative count of the current value and all preceding values. |

## Prototype

```cpp
template <const histogram_options& options = default_histogram_options, typename T, typename U>
__simd_callee__ inline reg_tensor<T> histograms(reg_tensor<T>& dst, const reg_tensor<U>& src)
```

## Parameters

**Table 2** Template parameter description

| Parameter | Description |
| --- | --- |
| options | Histogram configuration, whose type is histogram_options. By default, the number of occurrences of each value in the range [0, 127] is counted. |
| T | Output element type. For supported data types, see [data type](#data-type). |
| U | Input element type. For supported data types, see [data type](#data-type). |

**Table 3** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Input/Output | Initial value for statistics and accumulation result, whose type is `reg_tensor<T>`. The API updates `dst.reg` and retains `dst.mask` unchanged. |
| src | Input | Data to be counted, whose type is `reg_tensor<U>`. `src.mask` marks the elements that participate in the statistics. A position with a value of `1` participates in the statistics, and a position with a value of `0` does not. |

## Return Value

Returns the updated `dst`, whose type is `reg_tensor<T>`. In frequency mode, the occurrence count of each input value is accumulated onto the original value of `dst`. In cumulative mode, the number of inputs not greater than the corresponding value is accumulated onto the original value of `dst`. The `mask` of the return value is the same as `dst.mask` before calling.

## Data Type

**Table 4** Data type combinations

| src | dst |
| --- | --- |
| uint8_t | uint16_t |

## Constraints

- `src.mask` must be set in advance through the `with_mask` API. If it is not set, the content of the mask is undefined, which may cause incorrect positions of the elements participating in the statistics.
- The statistics result is accumulated on top of the original data in `dst`. `dst.reg` and `dst.mask` must be initialized before calling. For multiple calls, the same `dst` can be passed in directly to continue accumulating, without needing to reassign the return value to `dst`. The API does not modify `dst.mask`; before writing back, ensure that it is consistent with the required output range.
- The element type of `dst` is `uint16_t`, and the maximum value of a single statistics result is 65535. The user must ensure that the accumulated result does not exceed this range; otherwise, overflow occurs.
- When a non-default configuration is used, a named `constexpr histogram_options` object must be defined at namespace scope and passed as a template argument.

## Example

The following example computes the cumulative distribution of the upper half of the input data and accumulates the statistics result on the initial value `3`.

```cpp
#include "c_api/asc_simd.h"
#include "tensor_api/experimental/vector_ops.h"

constexpr uint16_t initial_value = 3;
constexpr asc::te::experimental::histogram_options histogram_config {
    asc::te::experimental::histogram_half::high,
    asc::te::experimental::histogram_mode::cumulative};

template <typename SrcTensor, typename DstTensor>
__simd_vf__ inline void histograms_vf(
    const SrcTensor src, DstTensor dst, uint16_t repeat_times, uint32_t total, uint32_t one_repeat_size)
{
    uint32_t remain = total;
    asc::te::experimental::reg_tensor<uint16_t> dst_reg;
    auto dst_mask = asc::te::experimental::make_mask<asc::te::experimental::mask_pattern::all, uint16_t>();
    dst_reg.mask = dst_mask.reg;
    asc_duplicate_scalar(dst_reg.reg, initial_value, dst_reg.mask);
    for (uint16_t i = 0; i < repeat_times; ++i) {
        const uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto mask = asc::te::experimental::update_mask<uint8_t>(remain);
        auto src_reg = asc::te::experimental::load(src, coord).with_mask(mask);
        asc::te::experimental::histograms<histogram_config>(dst_reg, src_reg);
    }
    asc::te::experimental::store(dst, asc::te::make_coord(0), dst_reg);
}
```
