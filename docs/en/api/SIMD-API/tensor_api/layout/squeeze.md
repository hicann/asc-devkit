# squeeze

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-27T02:38:44.202Z pushedAt=2026-10-27T13:03:35.321Z nl=2 -->

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
<!-- npu="910" id4 -->
- Atlas training products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- AI Core in Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Vector Core in Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="310b" id7 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id7 -->

## Description

The header file path is `tensor_api/tensor.h`.

**squeeze** deletes the specified dimension of size 1 in the shape of a layout and synchronously deletes the corresponding dimension in the stride. When the input is a Tensor, the API updates the layout of the Tensor, while the Engine, storage location, and data start address of the Tensor remain unchanged.

The API supports the following two ways to specify the dimensions to be deleted:

- Specify the index of the top-level dimension of the shape through a template parameter.
- Specify the dimensions through a Pattern that has the same structure as the shape. In the Pattern, `_1` marks the dimension to be deleted, and `_` marks the dimension to be retained. Nested Patterns recursively process nested shapes.

The returned layout or Tensor retains the LayoutPattern and layout_trait information carried by the input layout.

## Function Prototype

- Delete the dimension of size 1 by dimension index.

    ```cpp
    template <size_t... SqueezeDims, typename Input>
    __aicore__ inline auto squeeze(const Input& x)
    ```

- Delete the dimension of size 1 through a Pattern.

    ```cpp
    template <typename Pattern, typename Input>
    __aicore__ inline auto squeeze(const Input& x, const Pattern& pattern)
    ```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| :--- | :--- |
| SqueezeDims | Index of the dimension to be deleted in the top-level structure of the shape. Multiple indexes can be specified at the same time, and the order of the indexes does not affect the result. |
| Input | Type of the input object, which must be a layout type or a Tensor type. |
| Pattern | Pattern type, which must be a tuple type with the same structure as the shape of the input layout. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| x | Input | The layout or Tensor object to be processed. |
| pattern | Input | The dimension selection Pattern. `_1` indicates deleting the corresponding dimension of size 1, and `_` indicates retaining the corresponding dimension. The Pattern can be constructed by `make_coord`. |

## Return Value

When the input is a layout, the layout object with the specified dimensions deleted is returned; when the input is a Tensor, the Tensor object described by the new layout is returned. The returned object retains the LayoutPattern and layout_trait information of the original layout.

## Constraints

- `SqueezeDims` must contain at least one dimension index and point to a top-level dimension of the input shape.
- `pattern` must have the same nested structure as the shape of the input layout.
- For compile-time constant dimensions, only dimensions of size 1 are deleted; specified dimensions whose size is not 1 are retained.
- For runtime dimensions, the API does not verify whether the corresponding shape value is 1. The user must ensure that the specified runtime dimension to be deleted has a size of 1; otherwise, the returned layout or Tensor cannot correctly describe the original data.
- After deleting dimensions, if only one nested tuple remains at the current level, the API expands that level. For example, the shape changes from `(1, (M, N))` to `(M, N)`.
- Deleting dimensions only changes the structure of the shape and stride, and does not move or modify the data in memory.

## Example

### Delete by Dimension Index

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto layout = make_layout(make_shape(_1{}, _4{}, _1{}), make_stride(_4{}, _1{}, _1{}));
auto squeezed_layout = squeeze<0, 2>(layout);

// The shape of squeezed_layout is (4), and its stride is (1).
```

### Delete by Pattern

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto layout = make_layout(make_shape(_1{}, _4{}, _1{}), make_stride(_4{}, _1{}, _1{}));
auto pattern = make_coord(_1{}, _, _1{});
auto squeezed_layout = squeeze(layout, pattern);

// The shape of squeezed_layout is (4), and its stride is (1).
```

### Delete the Batch Dimension of Tensor

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

__aicore__ inline void squeeze_tensor_example()
{
    constexpr uint32_t batch = 1;
    constexpr uint32_t m = 32;
    constexpr uint32_t n = 64;

    __cbuf__ half l1_buffer[batch * m * n];
    auto batch_layout = make_frame_layout<nz_layout_ptn, half>(batch, m, n);
    auto batch_tensor = make_tensor(make_mem_ptr(l1_buffer), batch_layout);

    auto matrix_tensor = squeeze<0>(batch_tensor);
    // matrix_tensor and batch_tensor point to the same data, and the layout no longer contains the Batch dimension.
}
```
