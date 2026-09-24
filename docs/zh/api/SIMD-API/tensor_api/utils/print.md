# print打印接口

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3训练系列产品/Atlas A3推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2训练系列产品/Atlas A2推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`tensor_api/utils/print.h`。

Tensor API提供三个调试打印接口：

- `print`：打印格式化字符串、标量、Tuple、Layout或Tensor描述信息。
- `print_layout`：以二维表格形式打印Layout的逻辑坐标到物理索引的映射。
- `print_tensor`：按照Tensor的逻辑坐标顺序打印元素数据，并可选择是否打印Tensor描述信息。

接口用于算子调试，不建议在正式性能测试或性能敏感路径中调用。打印数据会写入设备侧调试输出，多个核同时打印时输出顺序可能交错。

## 函数原型

### print

```cpp
__aicore__ inline void print(__gm__ const char* fmt);

template <typename Arg0, typename... Args>
__aicore__ inline void print(__gm__ const char* fmt, Arg0&& arg0, Args&&... args);

template <typename Value, typename Std::enable_if_t<!detail::is_print_string_v<Value>, int> = 0>
__aicore__ inline void print(const Value& value);
```

### print_layout

```cpp
template <typename Layout>
__aicore__ inline void print_layout(const Layout& layout);
```

### print_tensor

```cpp
template <typename Tensor>
__aicore__ inline void print_tensor(const Tensor& tensor, bool print_type = true);
```

## 参数说明

**表1** `print`格式化接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| fmt | 输入 | 格式控制字符串。支持普通字符和`%d`、`%lld`、`%f`、`%u`、`%s`、`%p`等转换说明，转换说明的数量和类型必须与后续参数匹配。|
| arg0 | 输入 | 第一个格式化参数。|
| args... | 输入 | 其余格式化参数。|
| value | 输入 | 待打印的标量、Tuple、Layout或Tensor对象。|

**表2** `print_layout`参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| layout | 输入 | 待打印的Layout对象。其Shape必须在顶层包含两个元素；每个顶层元素可以是整数或嵌套Shape结构。|

**表3** `print_tensor`参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| tensor | 输入 | 待打印的Tensor对象。其Layout的Shape必须在顶层包含1到4个元素；每个顶层元素可以是整数或嵌套Shape结构。|
| print_type | 输入 | 是否打印Tensor的指针位宽、地址和Layout描述。默认值为`true`；设置为`false`时只打印元素数据。|

## 返回值说明

无

## 约束说明

- 只有开启调试打印时才会产生输出。定义`ASCENDC_DUMP=0`时，三个接口均不输出数据。
- `print_layout`只支持Shape在顶层包含两个元素的Layout。顶层元素可以是整数或嵌套Shape结构；顶层元素数量不为两个时会触发编译期校验。
- `print_tensor`只支持Shape在顶层包含1到4个元素的Tensor。顶层元素可以是整数或嵌套Shape结构。顶层元素数量超出范围时会触发编译期校验。元素按逻辑坐标顺序访问，实际物理地址由Tensor的Layout决定；ND、NZ等Layout的内部对齐区域不会作为额外逻辑元素打印。
- `print_tensor`支持打印的Tensor存储位置如下：

  | 存储位置 | 是否支持 |
  | :--- | :--- |
  | `location::gm` | 支持 |
  | `location::ub` | 支持 |
  | `location::l1` | 支持 |
  | `location::l0c` | 支持 |
  | `location::bias` | 支持 |
  | `location::l0a` | 不支持 |
  | `location::l0b` | 不支持 |
  | `location::l0scalea` | 不支持 |
  | `location::l0scaleb` | 不支持 |
  | `location::fixbuf` | 不支持 |
  | `location::ssbuf` | 不支持 |
- 打印功能会增加算子运行时间和调试输出量。打印Tensor前应控制Tensor的逻辑元素数量。多个核或多个线程同时打印时，日志可能交错或受输出缓存大小限制。
- 使用`print_tensor`接口时，在每个核上打印的数据总量不能大于30KB，请开发者自行控制打印的内容数据量，超出则不会打印。
- `print_tensor`接口支持打印的元素数据类型：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。
- 由于在SIMD VF中不能直接访问GM地址，因此不能在SIMD VF中使用。

## 输出格式

### print输出

`print`对Tuple保留嵌套结构，元素之间使用逗号和空格分隔；对Layout输出`shape`和`stride`；对Tensor输出指针位宽、地址和Layout描述，不输出Tensor元素。

支持直接打印的常用值类型如下：

| 值类型 | 输出形式 |
| :--- | :--- |
| `int8_t`、`int16_t`、`int32_t`、`int64_t` | 十进制有符号整数。 |
| `uint8_t`、`uint16_t`、`uint32_t`、`uint64_t` | 十进制无符号整数。 |
| `float`、`half`、`bfloat16_t`及其他浮点类型 | 十进制浮点数。 |
| Tuple | 按原有嵌套关系使用圆括号输出。 |
| Layout | 输出`layout(shape=..., stride=...)`描述。 |
| Tensor | 输出`ptr[位宽](地址) o layout(...)`描述。 |

字符串通过第一个参数`fmt`传入；需要输出字符串参数时，在`fmt`中使用`%s`并传入对应的字符串参数，例如`print((__gm__ const char*)"name=%s\n", name)`。

Tensor描述的格式为：

```text
ptr[<element_bits>b](<address>) o layout(shape=<shape>, stride=<stride>)
```

其中`<element_bits>`为元素位宽，例如普通`float`为`32`，4-bit数据类型为`4`。`o`用于分隔Tensor指针信息和Layout信息。

### print_layout输出

`print_layout`先输出Layout描述，再输出二维逻辑坐标到物理索引的表格。表格的行、列对应Layout的Shape顶层两个元素；每个元素可以是整数或嵌套Shape结构。单元格中的数值为调用`layout(coord)`得到的线性索引。

### print_tensor输出

当`print_type`为`true`时，先输出Tensor描述和冒号，再按逻辑坐标打印元素。不同维度的Tensor使用以下分隔方式：

- 1维：每行一个元素。
- 2维：每行一个逻辑行。
- 3维：不同的第三维平面之间使用`-`分隔。
- 4维：不同的第四维切片之间使用`=`分隔，第三维平面之间使用`-`分隔。

## 调用示例

### 打印标量和Tuple

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

NPU模式下的输出示例：

```text
[AIC Block 0/1] -7
[AIC Block 0/1] (2, (3, 4))
[AIC Block 0/1] value=7, text=ok
```

### 打印Layout描述

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

输出示例：

```text
[AIC Block 0/1] layout(shape=(2, 3), stride=(3, 1))
```

### 打印二维Layout表格

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

输出示例：

```text
[AIC Block 0/1] layout(shape=(2, 3), stride=(3, 1))
      0   1   2
    +---+---+---+
 0  | 0 | 1 | 2 |
    +---+---+---+
 1  | 3 | 4 | 5 |
    +---+---+---+
```

### 打印Tensor描述和元素

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

输出示例（地址因运行环境而异）：

```text
[AIC Block 0/1] ptr[32b](0x...) o layout(shape=(2, 2), stride=(2, 1)):
1  2
3  4
```

### 打印嵌套Shape的Tensor

`print_tensor`会保留Shape和Stride的嵌套关系，并按照展开后的逻辑坐标顺序打印元素。以下示例分别展示两层嵌套和三层嵌套的Shape：

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

调用方应在调用`print_nested_tensor_example`前准备至少覆盖示例Layout访问范围的GM数据。对应的Tensor描述和元素排布如下（地址因运行环境而异）：

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
