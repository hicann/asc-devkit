# mask的用法

## 功能说明

`reg_tensor<DataType>`同时保存数据寄存器`reg`和与数据关联的掩码寄存器`mask`。计算接口根据输入`reg_tensor`的`mask`选择参与计算的有效元素，并将计算所使用的mask传递给返回的`reg_tensor`。`local_tensor::store`的单源`direct`、`pack`和`pack_quarter`模式继续使用源`reg_tensor`中保存的mask控制写回。

`reg_tensor`的默认构造函数不初始化`reg`和`mask`。作为计算接口的输入参数前，必须保证其`mask`已初始化；否则，参与计算和写回的有效元素不确定。若计算链中的有效范围发生变化，应在下一次计算或写回前重新调用`with_mask`绑定新的mask。

当`DataType`为`bool`时，`reg_tensor<bool>`同样包含`reg`和`mask`两个成员。`all_mask`、`make_mask`、`update_mask`及比较接口只保证返回对象的`reg`成员有效，不应直接读取其`mask`成员。

## 初始化mask

可通过以下接口生成mask，类型为`reg_tensor<bool>`：

| 接口 | 使用场景 |
| --- | --- |
| `all_mask<DataType>()` | 当前寄存器中的所有元素都有效。 |
| `make_mask<Pattern, DataType>()` | 按固定模式生成mask。 |
| `update_mask<DataType>(remain)` | 按剩余元素数量生成mask，通常用于处理循环中的尾块。 |

`DataType`必须与mask所服务的数据元素类型保持一致。例如，在给`reg_tensor<float>`赋值时应使用`all_mask<float>()`或`update_mask<float>(remain)`，不同元素位宽对应的有效元素数不同。

### 与load链式调用

`load`和`load_broadcast`只初始化返回对象的`reg`，不初始化`mask`。推荐通过链式计算`with_mask`或在搬入后立即绑定mask：

```cpp
auto mask = asc::te::experimental::update_mask<float>(remain);
auto src0Reg = asc::te::experimental::load(src0, asc::te::make_coord(offset)).with_mask(mask);
auto src1Reg = asc::te::experimental::load(src1, asc::te::make_coord(offset)).with_mask(mask);
```

对于返回`reg_pair`的`deintlv`搬入模式，需分别为两个成员绑定mask：

```cpp
auto srcReg = asc::te::experimental::load<asc::te::experimental::load_sideband_mode::deintlv>(
    src, asc::te::make_coord(offset));

auto mask = asc::te::experimental::all_mask<uint8_t>();
srcReg.first.with_mask(mask);
srcReg.second.with_mask(mask);
```

## 计算接口中的mask传递

常规计算接口使用以下规则：

| 计算形式 | 计算使用的mask | 返回的`dst_reg_tensor.mask` |
| --- | --- | --- |
| 单输入，例如`abs(src)`、`exp(src)`、`cast<DstType>(src)` | `src.mask` | 与`src.mask`保持一致。 |
| 一个`reg_tensor`与标量计算，例如`src + scalar`、`scalar * src` | `reg_tensor`操作数的`mask` | 与该`reg_tensor`的`mask`保持一致。 |
| 多个`reg_tensor`计算，例如`src0 + src1`、`fma(src0, src1, src2)` | 第一个`reg_tensor`操作数的`mask` | 与第一个`reg_tensor`操作数的`mask`保持一致。 |
| 带输出参数，例如`add(carry, src0, src1)`、`mull(high, src0, src1)` | `src0.mask` | 返回值以及`carry`或`high`的`mask`均与`src0.mask`保持一致。 |
| 显式传入mask，例如`fill(value, mask)` | `mask.reg` | 与显式传入的`mask.reg`保持一致。 |

对于多个`reg_tensor`输入，接口不会合并或校验各输入的mask，第二个及后续输入的`reg_tensor`的`mask`不参与确定有效元素。调用前应保证所有数据输入的mask与第一个`reg_tensor`操作数保持一致。例如：

```cpp
auto mask = asc::te::experimental::update_mask<float>(remain);
auto src0Reg = asc::te::experimental::load(src0, asc::te::make_coord(offset)).with_mask(mask);
auto src1Reg = asc::te::experimental::load(src1, asc::te::make_coord(offset)).with_mask(mask);

auto dstReg = src0Reg + src1Reg;
// dstReg.mask与src0Reg.mask保持一致，可直接用于单源store。
asc::te::experimental::store(dst, asc::te::make_coord(offset), dstReg);
```

## 特殊接口说明

- `deinterleave`等重排接口可能重新生成输出mask，不遵循普通多输入计算的传递规则，具体以对应接口说明为准。
