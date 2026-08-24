# asc_sync_subblock_arrive

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
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas 训练系列产品：不支持
<!-- end id16 -->

## 功能说明

本接口与[asc_sync_subblock_wait](asc_sync_subblock_wait.md)配对使用，实现同一AI Core内全部AIV（[subblock](inter_core_sync_overview.md#fig_block_subblock_relationship)）之间的同步（[四种核间同步模式](system_sync_overview.md#fig_sync_control_mode)中的模式1），核间同步实现的原理如下：

- 同一AI Core内所有AIV（subblock）都执行`asc_sync_subblock_arrive`后向调度模块发送通知，接着调度模块将各AIV（subblock）对应`flag_id`的计数器增加1。
- 各AIV（subblock）上配对的`asc_sync_subblock_wait`检测到对应`flag_id`的计数器非0后解除阻塞并将计数器减1。

核间同步具体使用方法，请参考[调用示例](#调用示例)。

## 函数原型

```c
__aicore__ inline void asc_sync_subblock_arrive(pipe_t pipe,
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

- 用户需要确保配套使用（`flag_id`必须完全一致）`asc_sync_subblock_arrive`和`asc_sync_subblock_wait`，否则会出现未定义行为。
- 每个计数器最多连续累加15次（此时计数器的值为15），必须保证计数器的值不超过15，否则触发异常。
- `pipe`不支持的取值：`PIPE_ALL`和`PIPE_S`。
- 本接口不阻塞`pipe`流水中的后续指令。
- `flag_id`取值范围为[0, 15]，超出范围值会被按位宽截断处理为低4位（例如，flagId=16时，截取后为0；flagId=17时，截取后为1）。

## 调用示例

本示例演示两个AIV均执行完数据搬运后，各AIV才能开始读取数据。

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id17 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

<!-- end id17 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {

constexpr uint32_t ELEMENTS_PER_AIV = 8;
constexpr uint32_t TOTAL_ELEMENTS = 2 * ELEMENTS_PER_AIV;
constexpr uint32_t AIV_BYTES = ELEMENTS_PER_AIV * sizeof(float);
constexpr uint32_t TOTAL_BYTES = TOTAL_ELEMENTS * sizeof(float);
constexpr int64_t FLAG_ID = 8;

/*
 * 本示例说明：
 * - 采用__mix__(1, 2)标识核函数，同一AI Core内包含2个AIV。
 * - 两个AIV分别将各自的数据从GM搬入UB。
 * - 两个AIV分别将一组数据从UB搬运至GM（PIPE_MTE3）操作执行完后，才能执行asc_sync_subblock_arrive向调度模块发送通知。
 * - 各AIV中从GM读取数据（PIPE_MTE2）的操作被asc_sync_subblock_wait阻塞，直到两个AIV均执行了asc_sync_subblock_arrive后才能执行。
 * - 同步完成后，由AIV0读取并输出两个AIV生成的数据。
 */
__global__ __mix__(1, 2) void asc_sync_subblock_kernel(
    __gm__ float* input, __gm__ float* contributions, __gm__ float* output)
{
    asc_init();
    if ASC_IS_AIV {
        __ubuf__ float local[ELEMENTS_PER_AIV];
        __ubuf__ float snapshot[TOTAL_ELEMENTS];
        const uint32_t sub_block_num = asc_get_sub_block_num();
        const uint32_t subblock_id = asc_get_sub_block_id();
        const uint32_t total_elements = sub_block_num * ELEMENTS_PER_AIV;
        const uint32_t total_bytes = total_elements * sizeof(float);
        asc_copy_gm2ub_align(local, input + subblock_id * ELEMENTS_PER_AIV, AIV_BYTES);
        asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
        asc_copy_ub2gm_align(contributions + subblock_id * ELEMENTS_PER_AIV, local, AIV_BYTES);
        asc_sync_subblock_arrive(PIPE_MTE3, FLAG_ID);
        asc_sync_subblock_wait(PIPE_MTE2, FLAG_ID);
        if (subblock_id == 0) {
            asc_copy_gm2ub_align(snapshot, contributions, total_bytes);
            asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
            asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
            asc_copy_ub2gm_align(output, snapshot, total_bytes);
        }
    }
    asc_sync_pipe(PIPE_ALL);
}

} // namespace

int main()
{
    uint32_t numBlocks = 1;
    std::vector<float> input(TOTAL_ELEMENTS);
    std::vector<float> output(TOTAL_ELEMENTS, 0.0f);
    for (uint32_t i = 0; i < TOTAL_ELEMENTS; ++i) {
        input[i] = static_cast<float>(i / ELEMENTS_PER_AIV + 1);
    }
    float *input_device = nullptr, *contributions_device = nullptr, *output_device = nullptr;

    aclInit(nullptr);
    aclrtSetDevice(0);
    aclrtMalloc(reinterpret_cast<void**>(&input_device), TOTAL_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&contributions_device), TOTAL_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), TOTAL_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, TOTAL_BYTES, input.data(), TOTAL_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_sync_subblock_kernel<<<numBlocks, 0>>>(input_device, contributions_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), TOTAL_BYTES, output_device, TOTAL_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    bool passed = true;
    for (uint32_t i = 0; i < TOTAL_ELEMENTS; ++i) {
        const float expected = static_cast<float>(i / ELEMENTS_PER_AIV + 1);
        passed = passed && (output[i] == expected);
    }
    std::cout << (passed ? "[Success] asc_sync_subblock_arrive passed."
                          : "[Failed] asc_sync_subblock_arrive failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(contributions_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
