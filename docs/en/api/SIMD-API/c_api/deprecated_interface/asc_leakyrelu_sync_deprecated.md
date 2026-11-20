---
okftype: reference.api
description: "Deprecated synchronous activation interface that performs Leaky ReLU operation element-wise and computes along the PIPE_V pipeline, providing overloads for half and float source types. The replacement is asc_leakyrelu used together with asc_sync."
tag:
  - roles: [compute]
  - operations: [leaky_relu]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "For operand address overlap constraints, see the generic address overlap constraint section on that page"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Works with the non-synchronous interface to complete synchronization"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_fused/asc_leakyrelu.md
    kind: replaced_by
    condition: "The non-synchronous interface that replaces this synchronous activation interface"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Both source-type overloads carry the ASC_DEPRECATED(9.2.0, 2027/09/07, asc_leakyrelu and asc_sync) marker (the macro line before the corresponding @override_id in include/c_api/composite/vector_compute_composite.h), and the replacement is the non-synchronous activation interface asc_leakyrelu plus asc_sync. All prototypes on this page fall within the deprecation scope, which is a page-wide deprecation with no upper availability limit."
---
# asc_leakyrelu_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:05:38.952Z pushedAt=2026-11-18T03:12:16.588Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
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

## Description

The header file path is `"c_api/composite/vector_compute_composite.h"`.

**The `asc_leakyrelu_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_fused/asc_leakyrelu.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Performs vector Leaky ReLU computation. The calculation formula is as follows:

$$
dst_i =
\begin{cases}
src_i ,\quad src_i>0\\
\alpha src_i, \quad src_i\le0&
\end{cases}
$$

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_leakyrelu_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
    __aicore__ inline void asc_leakyrelu_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --------- | ----- | ----------------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector), which is the data to be processed. |
| value | Input | Source operand (scalar), the value of alpha in leaky_relu. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst and src must be 32-byte aligned.

## Examples

```cpp
//total_length refers to the data length involved in the computation
constexpr uint64_t total_length = 64;
half alpha = 0.1;
__ubuf__ half src[total_length];
__ubuf__ half dst[total_length];
asc_leakyrelu_sync(dst, src, alpha, total_length);
```
