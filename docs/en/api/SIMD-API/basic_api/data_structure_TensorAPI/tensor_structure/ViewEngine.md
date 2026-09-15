# ViewEngine

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T18:06:31.443Z -->

> [!NOTE]
> This API is an experimental API and may be adjusted or improved in later versions, with no guarantee of backward compatibility. Developers are advised to stay informed of updates in subsequent versions.

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

## Function Description

**ViewEngine** is the view engine in the Tensor API. It holds a pointer object **Iterator** and returns the start position of this view through **Begin()**. In **LocalTensor**, **ViewEngine** and **Layout** together form the view of a **Tensor**.

```cpp
LocalTensor<TensorAttribute<ViewEngine<Iterator>, LayoutType>>
```

## Prototype

`ViewEngine` is defined as follows:

  ```cpp
  template <typename Iterator>
  struct ViewEngine
  {
      using iterator    = Iterator;
      using reference   = typename IterRef<iterator>::type;
      using elementType = typename IterEle<iterator>::type;
      using valueType   = typename IterVal<iterator>::type;

      __aicore__ inline constexpr iterator const& Begin() const;
      __aicore__ inline constexpr iterator& Begin();
      __aicore__ inline constexpr ViewEngine(iterator storage = {});
  };
  ```

## Member Function Description

### `ViewEngine()`

- Function Description

  Constructs a **ViewEngine** object based on the given pointer/iterator object.

- Function Prototype

  ```cpp
  __aicore__ inline constexpr ViewEngine(iterator storage = {});

  ```

- Parameters

- `storage`: Pointer/iterator object corresponding to the start position of the view.

- Return Value

  Returns nothing.

### `Begin()`

- Function Description

  Obtains the start position pointer/iterator used when constructing the **ViewEngine** object.

- Function Prototype

  ```cpp
  __aicore__ inline constexpr iterator const& Begin() const;

  __aicore__ inline constexpr iterator& Begin();

  ```

- Return Value

- Returns the **storage** saved during construction, that is, the start pointer/iterator of the current view.

## Constraints

The passed-in iterator object must be a valid random access iterator to ensure that data can be correctly accessed and manipulated.

## Example

```cpp
  using namespace AscendC::Te;

  constexpr int32_t size = 256;
  __ubuf__ float data[size];
  auto ptr = MakeMemPtr(data);

  auto layout = MakeLayout(MakeShape(2, 2), MakeStride(2, 1));
  auto tensor = MakeTensor(ptr, layout);

  auto engine = tensor.Engine();
  auto begin = engine.Begin();

  float a = begin[0];
  float b = tensor[MakeCoord(1, 1)];

```
