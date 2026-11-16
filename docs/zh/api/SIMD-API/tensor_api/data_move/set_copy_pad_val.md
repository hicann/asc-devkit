# set_copy_pad_val

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

设置Global Memory到Unified Buffer对齐搬运时使用的常量填充值。该配置对后续满足条件的对齐搬运持续生效。

## 函数原型

```cpp
template <typename T>
__aicore__ inline void set_copy_pad_val(T pad_value)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| pad_value | 输入 | 常量填充值，类型为`T`。 |

## 数据类型

`T`支持`int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`和`float`。

## 返回值说明

无

## 约束说明

- 本接口仅在AIV上生效。
- 本接口只设置填充值，须先调用本接口，再执行对应的[GM到UB搬运](../reg_vector_compute/vector_compute_load/copy_gm_to_ub.md)。
- 填充值用于[GM到UB搬运](../reg_vector_compute/vector_compute_load/copy_gm_to_ub.md)产生的左右Padding区域及32字节对齐补齐区域。
- 填充值配置对后续所有满足条件的GM到UB对齐搬运生效，直到再次调用本接口覆盖配置；不同搬运任务间若需不同填充值，应在调用搬运接口前重新调用本接口设置。
- 在[GM到UB搬运](../reg_vector_compute/vector_compute_load/copy_gm_to_ub.md)的`gm_to_ub_params`参数中，当`left_padding_count`和`right_padding_count`均为0时，`enable_constant_pad`为`true`才使用本接口配置的值填充补齐区域；为`false`时，由硬件自动填充dummy假数据，本接口设置不生效。
- 在[GM到UB搬运](../reg_vector_compute/vector_compute_load/copy_gm_to_ub.md)的`gm_to_ub_params`参数中，当`left_padding_count`或`right_padding_count`非0时，`enable_constant_pad`不生效，必须在搬运前调用本接口配置填充值。左右Padding区域及右侧为32字节对齐继续补齐的区域均使用该值。

## 调用示例

以下示例从Global Memory读取32行、每行24个int8_t数据，在每行左侧填充3个、右侧填充5个-9后写入Global Memory。示例展示set_copy_pad_val与gm_to_ub_params的配合使用，并使用mutex串行化GM到UB与UB到GM两个流水阶段。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

constexpr uint32_t rows = 32;
constexpr uint32_t source_columns = 24;
constexpr uint32_t padded_columns = 32;
constexpr uint32_t padded_length = rows * padded_columns;
constexpr uint8_t left_padding = 3;
constexpr uint8_t right_padding = 5;
constexpr int8_t padding_value = -9;
constexpr uint8_t buffer_lock_id = 0;

__global__ __vector__ void padded_copy_kernel(__gm__ int8_t* src, __gm__ int8_t* dst)
{
    using namespace asc::te;
    __ubuf__ int8_t dst_buffer[padded_length];

    auto src_layout = make_frame_layout<nd_ext_layout_ptn, layout_trait_default<int8_t>>(rows, source_columns);
    auto dst_layout = make_frame_layout<nd_ext_layout_ptn, layout_trait_default<int8_t>>(rows, padded_columns);
    auto src_gm = make_tensor(make_mem_ptr<location::gm>(src), src_layout);
    auto dst_ub = make_tensor(make_mem_ptr(dst_buffer), dst_layout);
    auto dst_gm = make_tensor(make_mem_ptr<location::gm>(dst), dst_layout);

    set_copy_pad_val<int8_t>(padding_value);
    constexpr gm_to_ub_params padding_params{left_padding, right_padding, true};

    mutex<pipe_t::PIPE_MTE2> copy_in_mutex(buffer_lock_id);
    copy_in_mutex.lock();
    copy(make_copy(copy_gm_to_ub{}).with(padding_params), dst_ub, src_gm);
    copy_in_mutex.unlock();

    mutex<pipe_t::PIPE_MTE3> copy_out_mutex(buffer_lock_id);
    copy_out_mutex.lock();
    copy(make_copy(copy_ub_to_gm{}), dst_gm, dst_ub);
    copy_out_mutex.unlock();
}
```
