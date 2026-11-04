# mask的用法

## 功能说明

`reg_tensor<DataType>`同时保存数据寄存器`reg`和与数据关联的掩码寄存器`mask`。计算接口使用的mask与返回对象保存的mask不一定相同，具体规则见[计算接口中的mask传递](#计算接口中的mask传递)。`asc::te::experimental::store`的单源`direct`、`pack`和`pack_quarter`模式继续使用源`reg_tensor`中保存的mask控制写回。

`reg_tensor`的默认构造函数不初始化`reg`和`mask`。在接口需要读取或保留其`mask`前，必须保证该成员已初始化；否则，参与计算和写回的有效元素不确定。若计算链中的有效范围发生变化，应在下一次计算或写回前重新调用`with_mask`绑定新的mask。

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

下表列举计算使用的mask和返回对象保存的mask。若一个接口同时满足多个条件，按“已有`dst`→满mask结果→显式mask→常规接口”的顺序判定；比较运算符始终按其特殊规则处理。

| 规则或类别 | 接口或计算形式 | 计算使用的mask | 返回对象的mask |
| --- | --- | --- | --- |
| 已有dst作为入参 | `axpy(dst, src, scalar)`、`madd(dst, src0, src1)`、`mula(dst, src0, src1)`、`fill(dst, value, mask)`、`fill(dst, src, mask)`、`move(dst, src)`、`histograms(dst, src)` | `axpy`、`move`和`histograms`使用`src.mask`；`madd`和`mula`使用`dst.mask`；`fill`使用其操作模式规定的输入或显式mask。 | 返回对象保留调用前的`dst.mask`。该规则优先于显式mask规则。 |
| 语义为按照全有效mask处理的接口 | `pack<part>(src)`、`unpack<part>(src)`、寄存器`gather(src, index)`、`expand<T>(mask)`、`iota<order>(start)`、数据或掩码`interleave(src0, src1)`和`deinterleave(src0, src1)`、`move_mask<Offset>(src)` | 均不使用输入对象的有效性mask；`expand`中的`mask.reg`用于生成前缀计数索引，不作为输出有效性mask。 | 将返回对象的mask设置为对应元素类型的全有效mask；返回`reg_pair`时两个成员均设置。 |
| 显式传入mask | 不带`dst`的`fill(value, mask)`和`fill(src, mask)`、`load_gather` | 显式传入的`mask.reg`或`gather_mask.reg`。 | 将返回对象的mask设置为显式mask的`reg`。 |
| 常规接口 | `cast<DstType>(src)`、`muls_cast<DstType>(src, scalar)`、带`position`的`exp_diff`、`reduce_sum(src)`、`reduce_max(src)`、`reduce_min(src)`、`abs(src)`、`exp(src)`、`compress(src)`、掩码单参数`move(src)`、`src + scalar`、`scalar * src`、`src0 + src1`、`fma(src0, src1, src2)`、`select(condition, src0, src1)`、`add_carry`、`sub_carry`和`mull` | 单输入接口使用`src.mask`；多输入接口使用第一个数据输入`src0.mask`。 | 单输入接口复制`src.mask`；多输入接口复制`src0.mask`。返回`reg_pair`时，`first.mask`和`second.mask`均按相同规则设置。类型或布局变化时，不按目标位宽、结果位置或归约后的结果数量重新映射。 |
| 特殊：比较运算符 | `operator==`、`operator!=`、`operator<`、`operator<=`、`operator>`、`operator>=`的寄存器和标量重载 | 寄存器与寄存器比较使用`src0.mask`；与标量比较使用寄存器操作数的mask。 | 不初始化返回对象的mask。 |

- 按照全有效mask处理的接口返回的对象的mask为全有效，不保证返回对象的所有位置都具有可使用的数值。例如，`pack`未写入的另一半寄存器仍不应作为有效结果使用；类型转换、归约和压缩后的有效结果范围也应遵循对应接口约束，必要时在写回前重新绑定mask。
- 比较接口及mask生成接口的返回对象用于提供`reg`中的掩码数据，不应读取其未初始化的`mask`。需要把它们作为依赖有效性mask的输入时，应先通过`with_mask`绑定mask。
- 对于遵循常规多输入规则的接口，各输入的mask不会合并或校验，第二个及后续数据输入的mask不参与确定有效元素。调用前应保证所有数据输入在第一个操作数mask选中的位置均有效；为各输入绑定相同mask是常见做法。例如：

```cpp
auto mask = asc::te::experimental::update_mask<float>(remain);
auto src0Reg = asc::te::experimental::load(src0, asc::te::make_coord(offset)).with_mask(mask);
auto src1Reg = asc::te::experimental::load(src1, asc::te::make_coord(offset)).with_mask(mask);

auto dstReg = src0Reg + src1Reg;
// dstReg.mask与src0Reg.mask保持一致，可直接用于单源store。
asc::te::experimental::store(dst, asc::te::make_coord(offset), dstReg);
```
