# print APIs

<!-- md-trans-meta sourceCommit=8d12fdc1392b422cc5d95088e74075f8f24a6135 translatedAt=2026-11-19T03:33:20.664Z pushedAt=2026-11-19T05:54:06.710Z nl=2 -->

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

The header file path is `tensor_api/utils/print.h`.

Tensor API provides three debug print APIs:

- `print`: Prints formatted strings, scalars, Tuple, Layout, or Tensor description information.
- `print_layout`: Prints the mapping from logical coordinates to physical indexes of a Layout in the form of a two-dimensional table.
- `print_tensor`: Prints element data in the logical coordinate order of a Tensor, and optionally prints the Tensor description information.

These APIs are used for operator debugging and are not recommended for use in formal performance tests or performance-sensitive paths. The printed data is written to the device debug output, and the output order may be interleaved when multiple cores print simultaneously.

## Prototype

### print

```cpp
__aicore__ inline void print(__gm__ const char* fmt)

template <typename Arg0, typename... Args>
__aicore__ inline void print(__gm__ const char* fmt, Arg0&& arg0, Args&&... args)

template <typename Value, typename Std::enable_if_t<!detail::is_print_string_v<Value>, int> = 0>
__aicore__ inline void print(const Value& value)
```

### print_layout

```cpp
template <typename Layout>
__aicore__ inline void print_layout(const Layout& layout)
```

### print_tensor

```cpp
template <typename Tensor>
__aicore__ inline void print_tensor(const Tensor& tensor, bool print_type = true)
```

## Parameters

**Table 1** Parameter description of the `print` formatting API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| fmt | Input | Format control string. Supports ordinary characters and conversion specifications such as `%d`, `%lld`, `%f`, `%u`, `%s`, and `%p`. The number and types of conversion specifications must match the subsequent arguments.|
| arg0 | Input | First formatting argument.|
| args... | Input | Remaining formatting arguments.|
| value | Input | Scalar, Tuple, Layout, or Tensor object to be printed.|

**Table 2** Parameter description of `print_layout`

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| layout | Input | Layout object to be printed. Its Shape must contain two elements at the top level; each top-level element can be an integer or a nested Shape structure.|

**Table 3** Parameter description of `print_tensor`

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| tensor | Input | Tensor object to be printed. The Shape of its Layout must contain 1 to 4 elements at the top level; each top-level element can be an integer or a nested Shape structure.|
| print_type | Input | Whether to print the pointer bit width, address, and Layout description of the Tensor. The default value is `true`; when set to `false`, only the element data is printed.|

## Return Value

None

## Constraints

- Output is generated only when debug printing is enabled. When `ASCENDC_DUMP=0` is defined, none of the three APIs outputs data.
- Scenarios supported by `print_layout`: The Shape and Stride of the Layout contain only two dimensions at the top level. The top-level dimensions can be integers or nested structures. Compile-time validation is triggered when the number of top-level dimensions is not two.
- Scenarios supported by `print_tensor`: The Shape and Stride of the Tensor contain only 1 to 4 dimensions at the top level. The top-level dimensions can be integers or nested structures. Compile-time validation is triggered when the number of top-level dimensions is out of range.
- The Tensor storage locations supported by `print_tensor` are as follows:

  | storage location | Supported |
  | :--- | :--- |
  | `location::gm` | Supported |
  | `location::ub` | Supported |
  | `location::l1` | Supported |
  | `location::l0c` | Supported |
  | `location::bias` | Supported |
  | `location::l0a` | Not supported |
  | `location::l0b` | Not supported |
  | `location::l0scalea` | Not supported |
  | `location::l0scaleb` | Not supported |
  | `location::fixbuf` | Not supported |
  | `location::ssbuf` | Not supported |
- Printing increases the operator execution time and the amount of debug output. Before printing a Tensor, control the number of logical elements in the Tensor. When multiple cores or multiple threads print simultaneously, logs may interleave or be limited by the output buffer size.
- When using the `print_tensor` API, the total amount of data printed on each core must not exceed 30KB. Developers must control the amount of content data to be printed.
- The `print_tensor` API supports printing the following element data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, and uint64_t.
- Because GM addresses cannot be directly accessed in the SIMD VF, this API cannot be used in the SIMD VF.

## Output Format

### print Output

`print` preserves the nested structure of a Tuple, with elements separated by commas and spaces; for a Layout, it outputs `shape` and `stride`; for a Tensor, it outputs the pointer bit width, address, and Layout description, but does not output the Tensor elements.

The common value types that can be printed directly are as follows:

| value type | output form |
| :--- | :--- |
| `int8_t`, `int16_t`, `int32_t`, `int64_t` | Decimal signed integer. |
| `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t` | Decimal unsigned integer. |
| `float`, `half`, `bfloat16_t`, and other floating-point types | Decimal floating-point number. |
| Tuple | Output using parentheses according to the original nesting relationship. |
| Layout | Output the `layout(shape=..., stride=...)` description. |
| Tensor | Output the `ptr[bit width](address) o layout(...)` description. |

The string is passed in through the first parameter `fmt`; when a string parameter needs to be output, use `%s` in `fmt` and pass in the corresponding string parameter, for example, `print((__gm__ const char*)"name=%s\n", name)`.

The format of the Tensor description is:

```text
ptr[<element_bits>b](<address>) o layout(shape=<shape>, stride=<stride>)
```

Here, `<element_bits>` is the element width, for example, `32` for a regular `float` and `4` for a 4-bit data type. `o` is used to separate the Tensor pointer information from the Layout information.

### print_layout Output

`print_layout` first outputs the Layout description, then outputs a table mapping two-dimensional logical coordinates to physical indexes. The rows and columns of the table correspond to the top two elements of the Layout's Shape; each element can be an integer or a nested Shape structure. The value in each cell is the linear index obtained by calling `layout(coord)`.

### print_tensor Output

When `print_type` is `true`, the Tensor description and a colon are output first, followed by the elements printed by logical coordinates. Tensors of different dimensions use the following separators:

- 1D: one element per line.
- 2D: one logical row per line.
- 3D: different third-dimension planes are separated by `-`.
- 4D: different fourth-dimension slices are separated by `=`, and third-dimension planes are separated by `-`.

## Example

### Printing Scalars and Tuples

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/utils/print.h"

using namespace asc::te;

__aicore__ inline void print_scalar_and_tuple_example()
{
    print(-7);
    print(make_coord(2, make_coord(3, 4)));
    print((__gm__ const char*)"value=%d, text=%s\n", 7, (__gm__ const char*)"ok");
}
```

Output example in NPU mode:

```text
[AIC Block 0/1] -7
[AIC Block 0/1] (2, (3, 4))
[AIC Block 0/1] value=7, text=ok
```

### Printing Layout Description

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/utils/print.h"

using namespace asc::te;

__aicore__ inline void print_layout_description_example()
{
    auto layout = make_layout(make_shape(2, 3), make_stride(3, 1));
    print(layout);
}
```

Output example:

```text
[AIC Block 0/1] layout(shape=(2, 3), stride=(3, 1))
```

### Print 2D Layout Table

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/utils/print.h"

using namespace asc::te;

__aicore__ inline void print_layout_table_example()
{
    auto layout = make_layout(make_shape(2, 3), make_stride(3, 1));
    print_layout(layout);
}
```

Output example:

```text
[AIC Block 0/1] layout(shape=(2, 3), stride=(3, 1))
      0   1   2
    +---+---+---+
 0  | 0 | 1 | 2 |
    +---+---+---+
 1  | 3 | 4 | 5 |
    +---+---+---+
```

### Printing Tensor Description and Elements

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/utils/print.h"

using namespace asc::te;

__aicore__ inline void print_gm_tensor_example(__gm__ int32_t* data)
{
    auto tensor = make_tensor(
        make_mem_ptr<location::gm>(data), make_layout(make_shape(2, 2), make_stride(2, 1)));

    print_tensor(tensor);
}
```

Output example (addresses vary depending on the runtime environment):

```text
[AIC Block 0/1] ptr[32b](0x...) o layout(shape=(2, 2), stride=(2, 1)):
1  2
3  4
```

### Printing a Tensor with Nested Shape

`print_tensor` preserves the nesting relationship of Shape and Stride, and prints elements in the order of the expanded logical coordinates. The following examples show two-level and three-level nested Shapes:

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/utils/print.h"

using namespace asc::te;

__aicore__ inline void print_nested_tensor_example(__gm__ int32_t* data)
{
    auto shape_2d_nested = make_shape(make_shape(2, 2), make_shape(2, 2));
    auto stride_2d_nested = make_stride(make_stride(4, 8), make_stride(1, 2));
    auto tensor_2d_nested = make_tensor(
        make_mem_ptr<location::gm>(data), make_layout(shape_2d_nested, stride_2d_nested));
    print_tensor(tensor_2d_nested);

    auto shape_3d_nested = make_shape(2, make_shape(make_shape(2, 2), make_shape(2, 2)));
    auto stride_3d_nested = make_stride(16, make_stride(make_stride(1, 2), make_stride(4, 8)));
    auto tensor_3d_nested = make_tensor(
        make_mem_ptr<location::gm>(data), make_layout(shape_3d_nested, stride_3d_nested));
    print_tensor(tensor_3d_nested);
}
```

Before calling `print_nested_tensor_example`, the caller should prepare GM data that at least covers the access range of the example Layout. The corresponding Tensor description and element arrangement are as follows (addresses vary depending on the runtime environment):

```text
[AIC Block 0/1] ptr[32b](0x...) o layout(shape=((2, 2), (2, 2)), stride=((4, 8), (1, 2))):
1  2  3  4
5  6  7  8
9  10  11  12
13  14  15  16
[AIC Block 0/1] ptr[32b](0x...) o layout(shape=(2, ((2, 2), (2, 2))), stride=(16, ((1, 2), (4, 8)))):
1  2  3  4  5  6  7  8  9  10  11  12  13  14  15  16
17  18  19  20  21  22  23  24  25  26  27  28  29  30  31  32
```
