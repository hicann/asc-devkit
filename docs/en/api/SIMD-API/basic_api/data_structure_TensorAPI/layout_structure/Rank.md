# Rank

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:58:11.409Z -->

> [!NOTE]NOTE
> This is an experimental API and may be adjusted or improved in subsequent versions, with no guarantee of backward compatibility. Developers are advised to keep track of updates in subsequent versions.

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

Returns the rank of **Layout**.

## Prototype

```cpp
template <size_t... Is, typename Shape, typename Stride>
__aicore__ inline constexpr auto Rank(const Layout<Shape, Stride>& layout);
```

### Parameters

**Table 1** Template parameter description

  | Parameter | Type | Description |
|--------|------|------|
| Shape | Input | The type of the shape that constitutes the Layout, that is, a tuple type. |
| Stride | Input | The type of the stride that constitutes the Layout, that is, a tuple type. |
| Is... | size_t | Index sequence used to recursively select the substructures of shape and stride at compile time. |

**Table 2** Parameter description

  | Parameter | Type | Description |
|--------|------|------|
| layout | Input | Layout used to describe the tensor layout. |

### Return Value

Returns the rank of the **Layout**, that is, the number of dimensions of the **Shape** of the **Layout** or of its substructure tuple.

### Example Code

   ```cpp
  using namespace AscendC::Te;
  auto layout = MakeLayout(MakeShape(10, 20), MakeStride(1, 100));
  auto rank1 = Rank(layout); //rank1 = 2
  auto rank2 = Rank<0>(layout); //rank2 = 1
  ```