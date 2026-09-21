# addc

## 产品支持情况

- Ascend 950PR&950DT系列产品：支持
- Atlas A3训练系列产品/Atlas A3推理系列产品：不支持
- Atlas A2训练系列产品/Atlas A2推理系列产品：不支持
- Atlas 200I/500 A2推理产品：不支持
- Atlas推理系列产品AI Core：不支持
- Atlas推理系列产品Vector Core：不支持
- Atlas训练系列产品：不支持

## 功能说明

头文件路径：`"tensor_api/experimental/arch/vector/basic_arithmetic.h"`。

命名空间：`asc::te::experimental`。

对输入元素执行逐元素加法，并返回结果和进位；带`carry_src`的重载会额外加入输入进位：

$$
\{carry_i, dst_i\} = \{1'b0, src0_i\} + \{1'b0, src1_i\} + carry\_src_i
$$

当前接口仅支持`int32_t`和`uint32_t`。

## 函数原型

```cpp
template <typename T>
__simd_callee__ inline reg_pair<T, bool> addc(
    const reg_tensor<T>& src0,
    const reg_tensor<T>& src1)

template <typename T>
__simd_callee__ inline reg_pair<T, bool> addc(
    const reg_tensor<T>& src0,
    const reg_tensor<T>& src1,
    const reg_tensor<bool>& carry_src)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| T | 模板参数 | 操作数数据类型，支持`int32_t`、`uint32_t`。 |
| src0 | 输入 | 源操作数，类型为`reg_tensor<T>`；`src0.mask`控制参与计算的元素。 |
| src1 | 输入 | 源操作数，类型为`reg_tensor<T>`。 |
| carry_src | 输入 | 进位输入，类型为`reg_tensor<bool>`；对应元素为1时加1。 |

## 返回值说明

返回类型为`reg_pair<T, bool>`：`first`保存加法结果的低32位，`second`保存进位。两个成员的`mask`均设置为`src0.mask`。

## 约束说明

`src0.mask`需通过`with_mask`接口预先设置。`src1.mask`和`carry_src.mask`不参与本次计算。

## 调用示例

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/arch/vector/basic_arithmetic.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"

template <typename InputTensor, typename OutputTensor>
__simd_vf__ inline void add_c_example(InputTensor input0, InputTensor input1, OutputTensor output)
{
    auto mask = asc::te::experimental::all_mask<uint32_t>();
    auto src0 = asc::te::experimental::load(input0, asc::te::make_coord(0)).with_mask(mask);
    auto src1 = asc::te::experimental::load(input1, asc::te::make_coord(0));
    auto carry_src = asc::te::experimental::all_mask<uint32_t>();
    auto result = asc::te::experimental::addc(src0, src1, carry_src);
    asc::te::experimental::store(output, asc::te::make_coord(0), result.first);
}
```
