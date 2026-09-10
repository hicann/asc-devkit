# asc_sync_intra_arrive

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
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

本接口与[asc_sync_intra_wait](asc_sync_intra_wait.md)配对使用，实现单AI Core内AIC与单个AIV之间的同步（[四种核间同步模式](system_sync_overview.md#fig_sync_control_mode)中的模式4），核间同步实现的原理如下：

- 单个AIV等待AIC的场景：
  - 单AI Core内AIC执行`asc_sync_intra_arrive`后向调度模块发送通知，接着调度模块将该AI Core内单个AIV对应`sync_id`的计数器增加1。
  - 单个AIV上配对的`asc_sync_intra_wait`检测到对应`sync_id`的计数器非0后解除阻塞并将计数器减1。
- AIC等待单个AIV的场景：
  - 单AI Core内单个AIV执行`asc_sync_intra_arrive`后向调度模块发送通知，接着调度模块将AIC对应`sync_id`的计数器增加1。
  - AIC上配对的`asc_sync_intra_wait`检测到对应`sync_id`的计数器非0后解除阻塞并将计数器减1。

以图1为例，演示1个AI Core中AIV0与AIC进行同步（AIV0-1发起asc_sync_intra_arrive）。

AIC 0中在执行asc_sync_intra_wait后，此时AIC 0 sync_id=0的计数器为0，后续所有指令全部被阻塞，需要等到1个AIV执行完asc_sync_intra_arrive。

- AIV 0-0不需要执行asc_sync_intra_arrive。
- AIV 0-1的PIPE_MTE3指令全部执行完毕后，asc_sync_intra_arrive生效。此时调度模块感知1个AIV已执行完asc_sync_intra_arrive，因此将AIC 0 sync_id=0的计数器值增加为1。AIC 0检测到对应的sync_id=0的计数器变为1，则AIC 0核解除阻塞，继续执行后续PIPE_FIX的指令，并且将计数器值减去1。

**图1**  block内同步时序图（AIV进行asc_sync_intra_arrive）
![](../figures/single_ai_core_aic_single_aiv_sync.png "block内同步时序图（AIV进行asc_sync_intra_arrive）")

核间同步具体使用方法，请参考[调用示例](#调用示例)。

该同步模式的具体执行原理，请参考[单个AI Core中AIC与单个AIV同步（模式4）](key_features.md#single_ai_core_aic_single_aiv_sync)中的代码片段及配套时序图。

## 函数原型

```c
__aicore__ inline void asc_sync_intra_arrive(pipe_t pipe,
                                             uint64_t sync_id)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :---  | :--- | :--- |
| pipe | 输入 | 标识在哪条流水的前序指令完成后才允许向调度模块发送通知。<br>参数的类型是`pipe_t`枚举，各个枚举取值的含义请参考[硬件流水类型](./intra_core_sync_overview.md#硬件流水类型)。<br>AIC和AIV支持的`pipe`取值请参考[约束说明](#supported_pipe_combinations)。 |
| sync_id | 输入 | 核间同步的标记，用于标识同一组同步信号。每个`sync_id`各自拥有独立的4位计数器。一个AI Core由1个AIC与2个AIV构成，AIC侧拥有32个`sync_id`（0~31），每个AIV侧各拥有16个`sync_id`（0~15）。<br>同步时`asc_sync_intra_arrive`与`asc_sync_intra_wait`的`sync_id`对应关系请参考[约束说明](#约束说明)。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 调用本接口的核函数不能使用`__cube__`或`__vector__`[函数执行空间限定符](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#函数执行空间限定符)。使用这两种函数执行空间限定符时，硬件不会开启调度模块，无法正常进行核间同步。对于`asc_sync_intra_arrive`和`asc_sync_intra_wait`这对接口，支持的函数执行空间限定符为`__mix__(1, 2)`。
- 针对`asc_sync_intra_arrive`接口，传入的`pipe`参数**生效**，AIC和AIV支持的`pipe`取值如[表2](#aic_aiv_supported_pipe)所示。<a id="supported_pipe_combinations"></a>

  **表2**  AIC和AIV支持的`pipe`取值<a id="aic_aiv_supported_pipe"></a>

  | 核类型 | 支持的pipe取值 |
  | :--- | :--- |
  | AIC | `PIPE_S`、`PIPE_M`、`PIPE_MTE1`、`PIPE_MTE2`、`PIPE_FIX` |
  | AIV | `PIPE_S`、`PIPE_MTE2`、`PIPE_MTE3`、`PIPE_V` |

- 与其它核间同步模式不同，本接口与`asc_sync_intra_wait`配对使用时，**不要求**`arrive`与`wait`传入的`sync_id`相同，而是要求二者符合如下跨核ID映射关系，否则会出现未定义行为。<a id="sync_id_mapping"></a>
    - AIV0中调用的`asc_sync_intra_arrive`接口的`sync_id`取值为0~15，分别与AIC中调用的`asc_sync_intra_wait`接口的`sync_id`取值0~15对应。
    - AIV1中调用的`asc_sync_intra_arrive`接口的`sync_id`取值为0~15，分别与AIC中调用的`asc_sync_intra_wait`接口的`sync_id`取值16~31对应。
    - AIC中调用的`asc_sync_intra_arrive`接口的`sync_id`取值为0~15，分别与AIV0中调用的`asc_sync_intra_wait`接口的`sync_id`取值0~15对应。
    - AIC中调用的`asc_sync_intra_arrive`接口的`sync_id`取值为16~31，分别与AIV1中调用的`asc_sync_intra_wait`接口的`sync_id`取值0~15对应。
- 每个计数器最多连续累加15次（此时计数器的值为15），必须保证计数器的值不超过15，否则触发异常。
- 本接口不阻塞`pipe`流水中的后续指令。
- AIC的`sync_id`取值范围为[0, 31]，超出范围值会被按位宽截断处理为低5位（例如，syncId=32时，截取后为0；syncId=33时，截取后为1）。
- AIV的`sync_id`取值范围为[0, 15]，超出范围值会被按位宽截断处理为低4位（例如，syncId=16时，截取后为0；syncId=17时，截取后为1）。

## 调用示例

本示例演示AIV0执行完数据搬运后，AIC才能开始执行。

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
constexpr uint64_t SYNC_ID = 8;

/*
 * 本示例说明：
 * - 采用__mix__(1, 2)标识核函数，核函数启动1个AI Core，共包含1个AIC和2个AIV。
 * - AIV0将矩阵从GM搬入UB。
 * - AIV0将矩阵从UB搬运至L1 Buffer（PIPE_MTE3）操作执行完后，才能执行asc_sync_intra_arrive向调度模块发送通知。
 * - AIC中从L1 Buffer搬运数据到L0A Buffer、L0B Buffer（PIPE_MTE1）的操作被asc_sync_intra_wait阻塞，直到AIV0执行了asc_sync_intra_arrive后才能执行。
 * - 通过asc_get_sub_block_id区分不同的AIV，仅AIV0参与该次同步，AIV1不参与该次同步。
 */
__global__ __mix__(1, 2) void asc_sync_intra_kernel(__gm__ half* input, __gm__ float* output)
{
    asc_init();
    __cbuf__ half matrix_a_l1[MATRIX_ELEMENTS], matrix_b_l1[MATRIX_ELEMENTS];
    __ubuf__ half input_ub[MATRIX_ELEMENTS];
    __ca__ half matrix_a_l0[MATRIX_ELEMENTS];
    __cb__ half matrix_b_l0[MATRIX_ELEMENTS];
    __cc__ float output_l0[MATRIX_ELEMENTS];

    if ASC_IS_AIV {
        if (asc_get_sub_block_id() == 0) {
            asc_copy_gm2ub_align(input_ub, input, MATRIX_BYTES);
            asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
            asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0);
            asc_copy_ub2l1(matrix_a_l1, input_ub, MATRIX_BYTES);
            asc_copy_ub2l1(matrix_b_l1, input_ub, MATRIX_BYTES);
            asc_sync_intra_arrive(PIPE_MTE3, SYNC_ID);
        }
    }

    if ASC_IS_AIC {
        asc_sync_intra_wait(PIPE_MTE1, SYNC_ID);
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
    asc_sync_intra_kernel<<<numBlocks, 0>>>(input_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    bool passed = true;
    for (float value : output) {
        passed = passed && (value == static_cast<float>(MATRIX_DIM));
    }
    std::cout << (passed ? "[Success] asc_sync_intra_arrive passed."
                          : "[Failed] asc_sync_intra_arrive failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
