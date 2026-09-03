# asc_sync_block_arrive

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"c_api/sync/sync.h"`。

本接口与[asc_sync_block_wait](asc_sync_block_wait.md)配对使用，实现单AI Core内AIC与全部AIV之间的同步（[四种核间同步模式](system_sync_overview.md#fig_sync_control_mode)中的模式2），核间同步实现的原理如下：

- 全部AIV等待单个AIC的场景：
  - 单AI Core内AIC执行`asc_sync_block_arrive`后向调度模块发送通知，接着调度模块将该AI Core内M个AIV各自对应`flag_id`的计数器增加1。
  - 各AIV上配对的`asc_sync_block_wait`检测到对应`flag_id`的计数器非0后解除阻塞并将计数器减1。
- 单个AIC等待全部AIV的场景：
  - 单AI Core内M个AIV都执行`asc_sync_block_arrive`后向调度模块发送通知，接着调度模块将AIC对应`flag_id`的计数器增加1。
  - AIC上配对的`asc_sync_block_wait`检测到对应`flag_id`的计数器非0后解除阻塞并将计数器减1。

核间同步具体使用方法，请参考[调用示例](#调用示例)。

## 函数原型

```c
__aicore__ inline void asc_sync_block_arrive(pipe_t pipe,
                                             int64_t flag_id)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :---  | :--- | :--- |
| pipe | 输入 | 标识在哪条流水的前序指令完成后才允许向调度模块发送通知。 |
| flag_id | 输入 | 核间同步的标记，用于标识同一组同步信号。取值范围为[0, 15]，每个`flag_id`各自拥有独立的4位计数器。 |

## 返回值说明

无

## 流水类型

`PIPE_S`

## 约束说明

- 用户需要确保配套使用（`flag_id`必须完全一致）`asc_sync_block_arrive`和`asc_sync_block_wait`，否则会出现未定义行为。
- 每个计数器最多连续累加15次（此时计数器的值为15），必须保证计数器的值不超过15，否则触发异常。
- `pipe`不支持的取值：`PIPE_ALL`和`PIPE_S`。
- 本接口不阻塞`pipe`流水中的后续指令。
- `flag_id`取值范围为[0, 15]，超出范围值会被按位宽截断处理为低4位（例如，flagId=16时，截取后为0；flagId=17时，截取后为1）。

## 调用示例

本示例演示两个AIV均执行完数据搬运后，AIC才能开始执行。

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

<!-- end id8 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {

constexpr uint32_t MATRIX_DIM = 16;
constexpr uint32_t MATRIX_ELEMENTS = MATRIX_DIM * MATRIX_DIM;
constexpr uint32_t MATRIX_BYTES = MATRIX_ELEMENTS * sizeof(half);
constexpr uint32_t OUTPUT_BYTES = MATRIX_ELEMENTS * sizeof(float);
constexpr int64_t FLAG_ID = 8;

/*
 * 本示例说明：
 * - 采用__mix__(1, 2)标识核函数，同一AI Core内包含1个AIC和2个AIV。
 * - AIV0和AIV1分别将矩阵从GM搬入UB，通过asc_get_sub_block_id区分不同的AIV。
 * - 两个AIV将矩阵从UB搬运至L1 Buffer（PIPE_MTE3）操作执行完后，才能执行asc_sync_block_arrive向调度模块发送通知。
 * - AIC中从L1 Buffer搬运数据到L0A Buffer、L0B Buffer（PIPE_MTE1）的操作被asc_sync_block_wait阻塞，直到两个AIV均执行了asc_sync_block_arrive后才能执行。
 */
__global__ __mix__(1, 2) void asc_sync_block_kernel(__gm__ half* input, __gm__ float* output)
{
    asc_init();
    __cbuf__ half matrix_a_l1[MATRIX_ELEMENTS], matrix_b_l1[MATRIX_ELEMENTS];
    __ubuf__ half input_ub[MATRIX_ELEMENTS];
    __ca__ half matrix_a_l0[MATRIX_ELEMENTS];
    __cb__ half matrix_b_l0[MATRIX_ELEMENTS];
    __cc__ float output_l0[MATRIX_ELEMENTS];

    if ASC_IS_AIV {
        asc_copy_gm2ub_align(input_ub, input, MATRIX_BYTES);
        asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
        if (asc_get_sub_block_id() == 0) {
            asc_copy_ub2l1(matrix_a_l1, input_ub, MATRIX_BYTES);
        } else {
            asc_copy_ub2l1(matrix_b_l1, input_ub, MATRIX_BYTES);
        }
        asc_sync_block_arrive(PIPE_MTE3, FLAG_ID);
    }

    if ASC_IS_AIC {
        asc_sync_block_wait(PIPE_MTE1, FLAG_ID);
        asc_copy_l12l0a(matrix_a_l0, matrix_a_l1, 0, 0, 1, 1, 1, 1);
        asc_copy_l12l0b_transpose(matrix_b_l0, matrix_b_l1, 0, 0, 1, 1, 1, 1);
        asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
        asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
        asc_mmad(output_l0, matrix_a_l0, matrix_b_l0, MATRIX_DIM, MATRIX_DIM, MATRIX_DIM, 0, false, false, true);
        asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
        asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
        asc_set_l0c_copy_nz_para(1, 2, MATRIX_ELEMENTS);
        asc_copy_l0c2gm(output, output_l0, MATRIX_DIM, MATRIX_DIM, MATRIX_DIM, MATRIX_DIM, 0, 0, 0,
                        static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, true,
                        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
    }
    asc_sync_pipe(PIPE_ALL);
}

} // namespace

int main()
{
    uint32_t numBlocks = 1;
    std::vector<half> input(MATRIX_ELEMENTS, static_cast<half>(1.0f));
    std::vector<float> output(MATRIX_ELEMENTS, 0.0f);
    half* input_device = nullptr;
    float* output_device = nullptr;

    aclInit(nullptr);
    aclrtSetDevice(0);
    aclrtMalloc(reinterpret_cast<void**>(&input_device), MATRIX_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, MATRIX_BYTES, input.data(), MATRIX_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_sync_block_kernel<<<numBlocks, 0>>>(input_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    bool passed = true;
    for (float value : output) {
        passed = passed && (value == static_cast<float>(MATRIX_DIM));
    }
    std::cout << (passed ? "[Success] asc_sync_block_arrive passed."
                          : "[Failed] asc_sync_block_arrive failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
