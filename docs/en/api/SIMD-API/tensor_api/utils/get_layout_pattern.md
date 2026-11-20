# get_layout_pattern

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-27T04:27:07.694Z pushedAt=2026-10-27T13:03:35.494Z nl=2 -->

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

The header file path is `tensor_api/tensor.h`.

get_layout_pattern is used to extract the LayoutPattern type information from a layout type.

## Prototype

```cpp
template <typename Layout>
using get_layout_pattern = typename get_pattern<Std::remove_cvref_t<Layout>>::type;
```

## Parameters

Template Parameter Description

**Table 1** Template parameter description

| Parameter | Type | Description |
|--------|------|------|
| Layout | Input | The layout type from which the LayoutPattern is to be extracted. |

## Return Value

If Layout contains layout pattern information, the corresponding LayoutPattern type is returned.

If Layout is a layout constructed by make_layout, get_layout_pattern automatically deduces the recognizable LayoutPattern based on the type structures of shape and stride; if the pattern cannot be uniquely identified from the types, Std::ignore_t is returned.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

using layout_type = decltype(make_frame_layout<nz_layout_ptn, layout_trait_default<float>>(16, 16));
using pattern = get_layout_pattern<layout_type>;
// pattern = nz_layout_ptn

using inferred_layout = decltype(make_layout(make_shape(asc::te::_16{}, asc::te::_16{})));
using inferred_pattern = get_layout_pattern<inferred_layout>;
// inferred_pattern = nd_layout_ptn

using plain_layout = decltype(make_layout(make_shape(16, 16), make_stride(16, 1)));
using plain_pattern = get_layout_pattern<plain_layout>;
// plain_pattern = Std::ignore_t, cannot be automatically deduced when the runtime value is not encoded into the type.
```
