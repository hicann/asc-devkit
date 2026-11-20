# mask Usage

<!-- md-trans-meta sourceCommit=db86c75495b5d041b217977e24030960f4f2178e translatedAt=2026-11-19T02:53:00.044Z pushedAt=2026-11-19T05:54:06.669Z nl=2 -->

## Function Description

`reg_tensor<DataType>` simultaneously stores the data register `reg` and the mask register `mask` associated with the data. The mask used for computation is not necessarily the same as the mask kept by the returned object. For details, see [Mask Passing in Computation APIs](#mask-passing-in-computation-apis). The single-source `direct`, `pack`, and `pack_quarter` modes of `asc::te::experimental::store` continue to use the mask kept in the source `reg_tensor` to control write back.

The default constructor of `reg_tensor` does not initialize `reg` and `mask`. Before an API needs to read or reserve its `mask`, the member must be guaranteed to be initialized; otherwise, the valid elements that participate in computation and write back are indeterminate. If the valid range in the computation chain changes, call `with_mask` again to bind a new mask before the next computation or write back.

When `DataType` is `bool`, `reg_tensor<bool>` also contains the two members `reg` and `mask`. `all_mask`, `make_mask`, `update_mask`, and the comparison APIs only guarantee that the `reg` member of the returned object is valid; the `mask` member should not be read directly.

## Initializing Mask

You can generate a mask of type `reg_tensor<bool>` through the following APIs:

| API | Use Scenario |
| --- | --- |
| `all_mask<DataType>()` | All elements in the current register are valid. |
| `make_mask<Pattern, DataType>()` | Generates a mask according to a fixed pattern. |
| `update_mask<DataType>(remain)` | Generates a mask according to the number of remaining elements, typically used to process the tail block in a loop. |

`DataType` must remain consistent with the type of the data elements served by the mask. For example, when assigning a value to `reg_tensor<float>`, use `all_mask<float>()` or `update_mask<float>(remain)`. Different element widths correspond to different numbers of valid elements.

### Chained Call with Load

`load` and `load_broadcast` initialize only the `reg` of the returned object, not the `mask`. It is recommended to bind the mask through chained computation with `with_mask` or immediately after the data is moved in:

```cpp
auto mask = asc::te::experimental::update_mask<float>(remain);
auto src0Reg = asc::te::experimental::load(src0, asc::te::make_coord(offset)).with_mask(mask);
auto src1Reg = asc::te::experimental::load(src1, asc::te::make_coord(offset)).with_mask(mask);
```

For the `deintlv` move-in mode that returns `reg_pair`, bind the mask to the two members separately:

```cpp
auto srcReg = asc::te::experimental::load<asc::te::experimental::load_sideband_mode::deintlv>(
    src, asc::te::make_coord(offset));

auto mask = asc::te::experimental::all_mask<uint8_t>();
srcReg.first.with_mask(mask);
srcReg.second.with_mask(mask);
```

## Mask Passing in Computation APIs

The following table lists the mask used for computation and the mask kept by the returned object. If an API satisfies multiple conditions simultaneously, determine the rule in the order of "existing `dst` → all-valid mask result → explicit mask → regular API". Comparison operators always follow their special rules.

| Rule or category | API or computation form | Mask used for computation | Mask of the returned object |
| --- | --- | --- | --- |
| Existing dst as an input | `axpy(dst, src, scalar)`, `madd(dst, src0, src1)`, `mula(dst, src0, src1)`, `fill(dst, value, mask)`, `fill(dst, src, mask)`, `move(dst, src)`, `histograms(dst, src)` | `axpy`, `move`, and `histograms` use `src.mask`; `madd` and `mula` use `dst.mask`; `fill` uses the input or explicit mask specified by its operation mode. | The returned object keeps the `dst.mask` from before the call. This rule takes precedence over the explicit mask rule. |
| APIs whose semantics are handled according to an all-valid mask | `pack<part>(src)`, `unpack<part>(src)`, register `gather(src, index)`, `expand<T>(mask)`, `iota<order>(start)`, data or mask `interleave(src0, src1)` and `deinterleave(src0, src1)`, `move_mask<Offset>(src)` | None of them uses the validity mask of the input object; `mask.reg` in `expand` is used to generate prefix count indices, not as the output validity mask. | Set the mask of the returned object to the all-valid mask of the corresponding element type; when a `reg_pair` is returned, both members are set. |
| Explicitly passed mask | `fill(value, mask)` and `fill(src, mask)` without `dst`, `load_gather` | The explicitly passed `mask.reg` or `gather_mask.reg`. | Set the mask of the returned object to the `reg` of the explicit mask. |
| Regular API | `cast<DstType>(src)`, `muls_cast<DstType>(src, scalar)`, `exp_diff` with `position`, `reduce_sum(src)`, `reduce_max(src)`, `reduce_min(src)`, `abs(src)`, `exp(src)`, `compress(src)`, single-argument mask `move(src)`, `src + scalar`, `scalar * src`, `src0 + src1`, `fma(src0, src1, src2)`, `select(condition, src0, src1)`, `add_carry`, `sub_carry`, and `mull` | Single-input APIs use `src.mask`; multiple-input APIs use the first data input `src0.mask`. | Single-input APIs copy `src.mask`; multiple-input APIs copy `src0.mask`. When a `reg_pair` is returned, both `first.mask` and `second.mask` are set according to the same rule. When the type or layout changes, the mask is not remapped according to the target bit width, result position, or the number of results after reduction. |
| Special: comparison operators | Register and scalar overloads of `operator==`, `operator!=`, `operator<`, `operator<=`, `operator>`, and `operator>=` | Register-to-register comparison uses `src0.mask`; comparison with a scalar uses the mask of the register operand. | The mask of the returned object is not initialized. |

- The mask of the object returned by APIs handled according to an all-valid mask is all-valid, which does not guarantee that all positions of the returned object contain usable values. For example, the other half of the register not written by `pack` should still not be used as a valid result; the valid result range after type conversion, reduction, and compression should also follow the corresponding API constraints, and the mask should be re-bound before writing back if necessary.
- The returned objects of comparison APIs and mask generation APIs are used to provide the mask data in `reg`, and their uninitialized `mask` should not be read. When they need to be used as inputs that depend on a validity mask, bind a mask through `with_mask` first.
- For APIs that follow the regular multiple-input rule, the masks of the inputs are not merged or validated, and the masks of the second and subsequent data inputs do not participate in determining valid elements. Before calling, guarantee that all data inputs are valid at the positions selected by the mask of the first operand; binding the same mask to all inputs is a common practice. For example:

```cpp
auto mask = asc::te::experimental::update_mask<float>(remain);
auto src0Reg = asc::te::experimental::load(src0, asc::te::make_coord(offset)).with_mask(mask);
auto src1Reg = asc::te::experimental::load(src1, asc::te::make_coord(offset)).with_mask(mask);

auto dstReg = src0Reg + src1Reg;
// dstReg.mask remains consistent with src0Reg.mask and can be used directly for single-source store.
asc::te::experimental::store(dst, asc::te::make_coord(offset), dstReg);
```
