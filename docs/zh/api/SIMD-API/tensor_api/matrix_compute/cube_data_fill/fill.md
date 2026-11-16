# fill

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
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

头文件路径为：`tensor_api/tensor.h`。

`fill`将标量填充值写入L1 Buffer，并将填充值广播到多个32字节分形块，可用于矩阵计算前的L1 Buffer初始化。

Tensor API提供两种填充形式：显式传入`fill_params<T>`描述填充配置；仅传入填充值时，接口根据目的Tensor的Layout推导填充参数，并完整填充该Tensor。两种形式均要求填充值类型`T`与目的Tensor的`data_type`完全一致。

## 函数原型

```cpp
template <typename T>
struct fill_params {
    uint16_t repeat_times = 0;
    uint16_t block_num = 0;
    uint16_t dst_gap = 0;
    T init_value = {};
};

template <typename Tensor, typename T>
__aicore__ inline void fill(const Tensor& dst, const fill_params<T>& params)

template <typename Tensor, typename T>
__aicore__ inline void fill(const Tensor& dst, const T& init_value)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| Tensor | 目的Tensor类型。 |
| T | 填充值类型，必须与`Tensor::data_type`一致。 |

**表2** `fill`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输出 | L1目的Tensor。 |
| params | 输入 | 物理块填充配置。 |
| init_value | 输入 | 标量填充值。接口根据目的Tensor的Layout推导完整填充配置。 |

**表3** `fill_params`成员说明

| 成员 | 默认值 | 描述 |
| :--- | :---: | :--- |
| repeat_times | `0` | 重复次数，即填充值按`block_num`个32字节块写入L1 Buffer后，再以`dst_gap`为间隔重复写入的总次数。取值范围为`[0, 32767]`。 |
| block_num | `0` | 单次重复写入的32字节物理块数。取值范围为`[0, 32767]`。 |
| dst_gap | `0` | 相邻两次重复之间的间隔，单位为32字节块。取值范围为`[0, 32767]`。 |
| init_value | `T{}` | 标量填充值。 |

## 数据类型

`T`支持`half`、`bfloat16_t`、`int16_t`、`uint16_t`、`int32_t`、`uint32_t`和`float`。

## 返回值说明

无

## 约束说明

- 本接口仅在AIC上生效。
- 目的Tensor必须位于L1 Buffer。
- `T`必须与`Tensor::data_type`一致，否则编译期报错。
- 目的Tensor起始地址需要按照32字节对齐。
- L1 Buffer总容量为512KB。目的Tensor偏移量与实际占用空间之和不可超过L1 Buffer容量。
- 如果本指令与其他指令存在数据依赖，需要插入同步指令，保证多个指令串行化，防止出现异常数据。

## 调用示例

以下示例先用fill将L1中的A矩阵初始化为2，再将该矩阵从L1搬入L0A，并将同一初始化数据作为B矩阵搬入L0B，执行矩阵乘后将L0C结果写回Global Memory。输出每个元素等于columns * 2 * 2。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

constexpr uint32_t rows = 128;
constexpr uint32_t columns = 128;
constexpr uint32_t element_count = rows * columns;

__global__ __cube__ void fill_l1_kernel(__gm__ half* output)
{
    using namespace asc::te;
    __cbuf__ half l1_buffer[element_count];
    __ca__ half l0_a_buffer[element_count];
    __cb__ half l0_b_buffer[element_count];
    __cc__ float l0_c_buffer[element_count];

    auto l1_layout = make_frame_layout<nz_layout_ptn, half>(rows, columns);
    auto l0_a_layout = make_frame_layout<nz_layout_ptn, half>(rows, columns);
    auto l0_b_layout = make_frame_layout<zn_layout_ptn, half>(rows, columns);
    auto l0_c_layout = make_frame_layout<nz_layout_ptn, float>(rows, columns);
    auto output_layout = make_frame_layout<nd_ext_layout_ptn, half>(rows, columns);
    auto l1_tensor = make_tensor(make_mem_ptr(l1_buffer), l1_layout);
    auto l0_a_tensor = make_tensor(make_mem_ptr(l0_a_buffer), l0_a_layout);
    auto l0_b_tensor = make_tensor(make_mem_ptr(l0_b_buffer), l0_b_layout);
    auto l0_c_tensor = make_tensor(make_mem_ptr(l0_c_buffer), l0_c_layout);
    auto output_tensor = make_tensor(make_mem_ptr<location::gm>(output), output_layout);

    fill_params<half> params{};
    params.repeat_times = rows;
    params.block_num = columns * sizeof(half) / 32;
    params.dst_gap = 0;
    params.init_value = static_cast<half>(2);

    mutex<PIPE_MTE2> fill_mutex(0);
    fill_mutex.lock();
    fill(l1_tensor, params);
    fill_mutex.unlock();

    mutex<PIPE_MTE1> l0_mutex(0);
    l0_mutex.lock();
    copy(make_copy(copy_l1_to_l0a{}, l1_to_l0a_trait_default{}), l0_a_tensor, l1_tensor);
    copy(make_copy(copy_l1_to_l0b{}, l1_to_l0b_trait_default{}), l0_b_tensor, l1_tensor);
    l0_mutex.unlock();

    mutex<PIPE_M> mmad_mutex(0);
    mmad_mutex.lock();
    mmad(make_mmad(mmad_operation{}, mmad_trait_default{}).with(mmad_params{rows, columns, columns,
                  unit_flag_mode::disable, true}),
        l0_c_tensor, l0_a_tensor, l0_b_tensor);
    mmad_mutex.unlock();

    mutex<PIPE_FIX> output_mutex(0);
    output_mutex.lock();
    copy(make_copy(copy_l0c_to_gm{}, l0c_to_gm_trait_default{}), output_tensor, l0_c_tensor);
    output_mutex.unlock();
    sync_pipe(PIPE_ALL);
}
```
