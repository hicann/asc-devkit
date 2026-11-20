# rank

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-27T02:36:10.365Z pushedAt=2026-10-27T13:03:35.313Z nl=2 -->

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

Returns the rank of a layout.

## Function Prototype

```cpp
template <size_t... Is, typename Layout>
__aicore__ inline constexpr auto rank(const Layout& layout);
```

### Parameters

**Table 1** Template parameter description

  | Parameter | Type | Description |
|--------|------|------|
| Is... | size_t | Index sequence used to recursively select the substructures of shape and stride at compile time. |
| Layout | Input | Type of the layout object. |

**Table 2**  Parameters

  | Parameter | Type | Description |
|--------|------|------|
| layout | Input | Layout used to describe the tensor layout. |

### Return Value

Returns the rank of the layout, that is, the number of dimensions of the tuple formed by the shape of the layout or its substructures.

### Example

   ```cpp
   #include "tensor_api/tensor.h"

   using namespace asc::te;

  auto layout = make_layout(make_shape(10, 20), make_stride(1, 100));
  auto rank1 = rank(layout); //rank1 = 2
  auto rank2 = rank<0>(layout); //rank2 = 1
  ```
