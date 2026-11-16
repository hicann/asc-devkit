# block、intra、inter和subblock同步

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

本组接口提供四类到达/等待同步能力：

- block同步：实现单个AI Core内AIC与全部AIV之间的同步。
- intra同步：实现单个AI Core内AIC与单个AIV之间的同步。
- inter同步：实现不同AI Core之间的同步。
- subblock同步：实现单个AI Core内subblock（一个group内的从核）之间的同步。

每类同步均提供`arrive`和`wait`两个接口。

## 函数原型

```cpp
__aicore__ inline void sync_block_arrive(pipe_t pipe, int64_t flag_id)
__aicore__ inline void sync_block_wait(pipe_t pipe, int64_t flag_id)

__aicore__ inline void sync_intra_arrive(pipe_t pipe, uint64_t sync_id)
__aicore__ inline void sync_intra_wait(pipe_t pipe, uint64_t sync_id)

__aicore__ inline void sync_inter_arrive(pipe_t pipe, int64_t flag_id)
__aicore__ inline void sync_inter_wait(pipe_t pipe, int64_t flag_id)

__aicore__ inline void sync_subblock_arrive(pipe_t pipe, int64_t flag_id)
__aicore__ inline void sync_subblock_wait(pipe_t pipe, int64_t flag_id)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| pipe | 输入 | 同步关联的流水，类型为`pipe_t`。 |
| flag_id | 输入 | block、inter或subblock同步标记，用于标识同一组同步信号。取值范围为`[0, 15]`，每个`flag_id`各自拥有独立的4位计数器。 |
| sync_id | 输入 | intra同步标记，用于标识同一组同步信号。每个`sync_id`各自拥有独立的4位计数器。AIC侧拥有32个`sync_id`（0~31），每个AIV侧各拥有16个`sync_id`（0~15）。 |

## 返回值说明

无

## 约束说明

- `sync_block_arrive`与`sync_block_wait`、`sync_inter_arrive`与`sync_inter_wait`、`sync_subblock_arrive`与`sync_subblock_wait`配对使用时，`flag_id`必须完全一致，否则会出现未定义行为。
- `sync_intra_arrive`与`sync_intra_wait`配对使用时，不要求二者传入相同的`sync_id`，而是要求符合以下跨核ID映射关系，否则会出现未定义行为：
    - AIV0中`sync_intra_arrive`的`sync_id`取值为`0~15`，分别对应AIC中`sync_intra_wait`的`sync_id`取值`0~15`。
    - AIV1中`sync_intra_arrive`的`sync_id`取值为`0~15`，分别对应AIC中`sync_intra_wait`的`sync_id`取值`16~31`。
    - AIC中`sync_intra_arrive`的`sync_id`取值为`0~15`，分别对应AIV0中`sync_intra_wait`的`sync_id`取值`0~15`。
    - AIC中`sync_intra_arrive`的`sync_id`取值为`16~31`，分别对应AIV1中`sync_intra_wait`的`sync_id`取值`0~15`。
- 核函数不能使用`__cube__`或`__vector__`函数执行空间限定符，否则硬件不会开启调度模块，无法正常进行同步。各同步接口支持的函数执行空间限定符如下：

    | 同步接口 | 支持的函数执行空间限定符 |
    | :--- | :--- |
    | `sync_block_arrive` / `sync_block_wait` | `__mix__(1, 1)`、`__mix__(1, 2)` |
    | `sync_intra_arrive` / `sync_intra_wait` | `__mix__(1, 2)` |
    | `sync_inter_arrive` / `sync_inter_wait` | `__mix__(0, 1)`、`__mix__(1, 0)`、`__mix__(1, 1)`、`__mix__(1, 2)` |
    | `sync_subblock_arrive` / `sync_subblock_wait` | `__mix__(1, 1)`、`__mix__(1, 2)` |

- 各接口支持的`pipe`取值如下：

    | 同步接口 | AIC支持的`pipe` | AIV支持的`pipe` |
    | :--- | :--- | :--- |
    | `sync_block_arrive` / `sync_inter_arrive` | `PIPE_M`、`PIPE_MTE1`、`PIPE_MTE2`、`PIPE_FIX` | `PIPE_MTE2`、`PIPE_MTE3`、`PIPE_V` |
    | `sync_block_wait` / `sync_inter_wait` | `PIPE_S`、`PIPE_M`、`PIPE_MTE1`、`PIPE_MTE2`、`PIPE_FIX` | `PIPE_S`、`PIPE_MTE2`、`PIPE_MTE3`、`PIPE_V` |
    | `sync_intra_arrive` / `sync_intra_wait` | `PIPE_S`、`PIPE_M`、`PIPE_MTE1`、`PIPE_MTE2`、`PIPE_FIX` | `PIPE_S`、`PIPE_MTE2`、`PIPE_MTE3`、`PIPE_V` |
    | `sync_subblock_arrive` | 不支持 | `PIPE_MTE2`、`PIPE_MTE3`、`PIPE_V` |
    | `sync_subblock_wait` | 不支持 | `PIPE_S`、`PIPE_MTE2`、`PIPE_MTE3`、`PIPE_V` |

- `sync_subblock_arrive`和`sync_subblock_wait`仅支持AIV调用。
- `arrive`接口不阻塞`pipe`流水的后续指令；`wait`接口阻塞`pipe`流水的后续指令，直到对应计数器非0。计数器非0时，`wait`接口解除阻塞并将计数器减1。
- `sync_block_arrive`、`sync_block_wait`、`sync_inter_arrive`、`sync_inter_wait`、`sync_subblock_arrive`和`sync_subblock_wait`的`flag_id`取值范围为`[0, 15]`，超出范围的值会被按位宽截断处理为低4位。
- `sync_intra_arrive`和`sync_intra_wait`中，AIC的`sync_id`取值范围为`[0, 31]`，超出范围的值会被按位宽截断处理为低5位；AIV的`sync_id`取值范围为`[0, 15]`，超出范围的值会被按位宽截断处理为低4位。
- 每个计数器最多连续累加15次，必须保证计数器的值不超过15，否则触发异常。

## 调用示例

以下示例在__mix__(1, 2)核函数中使用block同步：AIV填充UB并将结果写回Global Memory，随后调用sync_block_arrive通知AIC；AIC调用sync_block_wait等待通知后写入Global Memory。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

constexpr uint32_t total_length = 64;
constexpr int64_t sync_flag_id = 0;

__global__ __mix__(1, 2) void block_sync_kernel(__gm__ float* output)
{
    using namespace asc::te;
    init_soc_state();
    __ubuf__ float data_buffer[total_length];

    auto layout = make_layout(make_shape(total_length));
    auto data_ub = make_tensor(make_mem_ptr(data_buffer), layout);
    auto output_gm = make_tensor(make_mem_ptr<location::gm>(output), layout);

    if ASCEND_IS_AIV {
        for (uint32_t i = 0; i < total_length; ++i) {
            data_buffer[i] = 1.0f;
        }
        copy(make_copy(copy_ub_to_gm{}), output_gm, data_ub);
        sync_block_arrive(pipe_t::PIPE_MTE3, sync_flag_id);
    }

    if ASCEND_IS_AIC {
        sync_block_wait(pipe_t::PIPE_S, sync_flag_id);
        output[0] = 1.0f;
    }
    sync_pipe(pipe_t::PIPE_ALL);
}
```
