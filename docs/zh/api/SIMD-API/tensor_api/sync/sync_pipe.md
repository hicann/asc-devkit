# sync_pipe

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

完成同一流水内的同步控制，用于约束单条流水或全部流水的执行顺序。传入`pipe_t::PIPE_ALL`时等待所有流水的前序指令完成后才对后序指令解除阻塞；传入其他合法流水值时仅阻塞该流水的后续指令，直至该流水前序指令完成，不影响其他流水的指令执行。

## 函数原型

```cpp
__aicore__ inline void sync_pipe(pipe_t pipe)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| pipe | 输入 | 需要同步的流水，类型为`pipe_t`。支持的取值为`pipe_t::PIPE_V`、`pipe_t::PIPE_M`、`pipe_t::PIPE_MTE1`、`pipe_t::PIPE_MTE2`、`pipe_t::PIPE_MTE3`、`pipe_t::PIPE_FIX`和`pipe_t::PIPE_ALL`，不支持`pipe_t::PIPE_S`。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- Scalar流水之间的同步由硬件自动保证，本接口不支持`PIPE_S`。
- `PIPE_V`之间的同步由硬件自动保证。
- `sync_pipe(pipe_t::PIPE_ALL)`会等待所有流水中此前提交的指令完成，可能影响性能。若只需等待单条流水，应传入对应流水值。
- `pipe_t::PIPE_MTE2`和`pipe_t::PIPE_MTE3`在搬运目的地址重叠时，开发者需要插入同步，保证多条搬运指令串行执行。

## 调用示例

以下示例先通过同ID的MTE2/MTE3 mutex完成GM到UB搬入和UB到GM搬出，最后调用sync_pipe等待所有流水中的前序指令完成。

```cpp
#include "tensor_api/tensor.h"

constexpr uint32_t total_length = 64;
constexpr uint8_t buffer_lock_id = 0;

__global__ __vector__ void sync_pipe_kernel(__gm__ float* src, __gm__ float* dst)
{
    using namespace asc::te;
    __ubuf__ float buffer[total_length];

    auto layout = make_layout(make_shape(total_length));
    auto src_gm = make_tensor(make_mem_ptr<location::gm>(src), layout);
    auto dst_gm = make_tensor(make_mem_ptr<location::gm>(dst), layout);
    auto ub_tensor = make_tensor(make_mem_ptr(buffer), layout);

    mutex<pipe_t::PIPE_MTE2> copy_in_mutex(buffer_lock_id);
    copy_in_mutex.lock();
    copy(make_copy(copy_gm_to_ub{}), ub_tensor, src_gm);
    copy_in_mutex.unlock();

    mutex<pipe_t::PIPE_MTE3> copy_out_mutex(buffer_lock_id);
    copy_out_mutex.lock();
    copy(make_copy(copy_ub_to_gm{}), dst_gm, ub_tensor);
    copy_out_mutex.unlock();
    sync_pipe(pipe_t::PIPE_ALL);
}
```
