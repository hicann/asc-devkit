# GetLayoutPattern

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T18:07:20.197Z -->

> [!NOTE]
> This API is an experimental API and may be adjusted or improved in subsequent versions, with no guarantee of backward compatibility. Developers are advised to keep track of updates in subsequent versions.

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
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

The header file to include is `#include "tensor_api/tensor.h"`.

**GetLayoutPattern** extracts the **LayoutPattern** type information from a **Layout** type.

## Prototype

```cpp
template <typename T>
using GetLayoutPattern = typename GetPattern<Std::remove_cvref_t<T>>::type;
```

## Parameters

Template Parameters

| Parameter | Type | Description |
|--------|------|------|
| T | Input | **Layout** type from which the **LayoutPattern** is to be extracted. |

## Return Value

If **T** contains layout pattern information, the corresponding **LayoutPattern** type is returned.

If **T** is a **Layout** constructed by **MakeLayout**, **GetLayoutPattern** automatically derives a recognizable **LayoutPattern** based on the type structures of **Shape** and **Stride**; if it cannot be uniquely identified from the types, **Std::ignore_t** is returned.

## Example

```cpp
using namespace AscendC::Te;

using LayoutType = decltype(MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(16, 16));
using Pattern = GetLayoutPattern<LayoutType>;
// Pattern = NZLayoutPtn

using InferredLayout = decltype(MakeLayout(MakeShape(AscendC::Te::_16{}, AscendC::Te::_16{})));
using InferredPattern = GetLayoutPattern<InferredLayout>;
// InferredPattern = NDLayoutPtn

using PlainLayout = decltype(MakeLayout(MakeShape(16, 16), MakeStride(16, 1)));
using PlainPattern = GetLayoutPattern<PlainLayout>;
// PlainPattern = Std::ignore_t. It cannot be automatically deduced when runtime values are not encoded into the type.
```
