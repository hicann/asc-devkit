# Size

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T18:01:20.765Z -->

> [!NOTE] Note
> This API is experimental and may be adjusted or improved in future versions. Backward compatibility is not guaranteed. Developers are advised to keep track of updates in future versions.

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

Returns the number of valid elements in **Layout**.

## Prototype

```cpp
template <size_t... Is, typename Shape, typename Stride>
__aicore__ inline constexpr auto Size(const Layout<Shape, Stride>& layout);
```

### Parameters

**Table 1** Template parameter description

  | Parameter | Type | Description |
|--------|------|------|
| Shape | Input | Type of the shape that constitutes the Layout, that is, a tuple type. |
| Stride | Input | Type of the stride that constitutes the Layout, that is, a tuple type. |
| Is... | size_t | Index sequence used to recursively select the substructures of shape and stride at compile time. |

**Table 2** Parameter description

  | Parameter | Type | Description |
|--------|------|------|
| layout | Input | Used to describe the layout of a tensor. |

### Return Value

Returns the number of valid elements in **Layout**.

### Constraints

**Is...** must be an index within the valid range.

### Example

   ```cpp
  using namespace AscendC::Te;
  auto layout = MakeLayout(MakeShape(10, 20), MakeStride(1, 100));
  auto size = Size(layout); //size = 200
  ```
