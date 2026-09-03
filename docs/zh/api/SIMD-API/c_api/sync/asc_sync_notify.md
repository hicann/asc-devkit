# asc_sync_notify

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

如图1所示，与[asc_sync_wait](asc_sync_wait.md)配对使用，用于实现AI Core内部不同流水之间的同步控制，`asc_sync_notify`和`asc_sync_wait`各自的功能如下：

- `asc_sync_notify`：当源流水的前序指令的所有读写操作都完成之后，当前指令开始执行，并将硬件中的对应标志位设置为1。`asc_sync_notify`只是设置硬件中的对应标志位，并不会阻塞源流水中的下一个指令。
- `asc_sync_wait`：当目的流水执行到该指令时，如果发现硬件中对应标志位为0，目的流水的后续指令将一直被阻塞；如果发现硬件中对应标志位为1，则将硬件中对应标志位设置为0，同时目的流水的后续指令开始执行。

**图1**  `asc_sync_notify`和`asc_sync_wait`接口功能示意图    
![](../figures/asc_sync_notify.png)

## 函数原型

```c
__aicore__ inline void asc_sync_notify(pipe_t pipe,
                                       pipe_t tpipe,
                                       event_t id)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| pipe | 输入 | 源流水类型，即“等待哪条流水的前序指令完成”。取值范围为`pipe_t`枚举：`PIPE_S`、`PIPE_V`、`PIPE_M`、`PIPE_MTE1`、`PIPE_MTE2`、`PIPE_MTE3`、`PIPE_FIX`。 |
| tpipe | 输入 | 目标流水类型，即“解除哪条流水的`asc_sync_wait`阻塞”。取值范围与`pipe`相同，为`pipe_t`枚举。 |
| id | 输入 | 同步事件ID，每对`pipe`与`tpipe`组合各自拥有8个独立的同步事件ID。取值范围为`event_t`枚举类型。 |

`event_t`枚举定义如下：

```c
typedef enum {
    EVENT_ID0 = 0,
    EVENT_ID1 = 1,
    EVENT_ID2 = 2,
    EVENT_ID3 = 3,
    EVENT_ID4 = 4,
    EVENT_ID5 = 5,
    EVENT_ID6 = 6,
    EVENT_ID7 = 7
} event_t;
```

## 返回值说明

无

## 流水类型

`PIPE_S`

## 约束说明

- **`pipe`与`tpipe`并非任意组合，两者组合的取值存在限制**：针对不同产品，AIC与AIV中支持的组合不同，具体请参考[核内同步分类](intra_core_sync_overview.md#核内同步分类)中的[表2](intra_core_sync_overview.md#aic_intra_core_sync_combinations)和[表3](intra_core_sync_overview.md#aiv_intra_core_sync_combinations)。
- 相同源流水、相同目标流水、相同`id`下，连续使用`asc_sync_notify`会引发未定义行为。
- 本接口需与`asc_sync_wait`配对使用，配对的两条调用其`pipe`、`tpipe`、`id`三个参数必须完全一致。
- `pipe`与`tpipe`均不可取`PIPE_ALL`，否则触发异常。
- 每对`pipe`与`tpipe`组合各自拥有8个独立的同步事件ID。例如`PIPE_M`与`PIPE_V`的组合和`PIPE_V`与`PIPE_MTE3`的组合可同时使用相同的`id`值而互不干扰。
- 本接口不会对`pipe`与`tpipe`两条流水的后续指令产生阻塞效果。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510; ./main
```

<!-- end id8 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {

constexpr uint32_t ELEMENTS = 64;
constexpr uint32_t BYTES = ELEMENTS * sizeof(float);

__simd_vf__ inline void add_vf(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1)
{
    vector_float dst_reg;
    vector_float src0_reg;
    vector_float src1_reg;
    uint32_t count = ELEMENTS;
    vector_bool mask = asc_update_mask_b32(count);
    asc_loadalign(src0_reg, src0);
    asc_loadalign(src1_reg, src1);
    asc_add(dst_reg, src0_reg, src1_reg, mask);
    asc_storealign(dst, dst_reg, mask);
}

void print_data(const char* label, const std::vector<float>& data)
{
    std::cout << label << ":";
    for (uint32_t i = 0; i < 8; ++i) std::cout << ' ' << data[i];
    std::cout << " ..." << std::endl;
}

__global__ __vector__ void asc_sync_notify_kernel(__gm__ float* output, __gm__ float* src0, __gm__ float* src1)
{
    asc_init();
    __ubuf__ float x[ELEMENTS], y[ELEMENTS], z[ELEMENTS];
    asc_copy_gm2ub_align(x, src0, BYTES);
    asc_copy_gm2ub_align(y, src1, BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    add_vf(z, x, y);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, z, BYTES);
    asc_sync_mte3(0);
}

} // namespace

int main()
{
    std::vector<float> src0(ELEMENTS), src1(ELEMENTS), output(ELEMENTS, 0.0f), golden(ELEMENTS);
    for (uint32_t i = 0; i < ELEMENTS; ++i) {
        src0[i] = static_cast<float>(i) * 0.25f;
        src1[i] = static_cast<float>(ELEMENTS - i) * 0.5f;
        golden[i] = src0[i] + src1[i];
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    float *src0_device = nullptr, *src1_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, BYTES, src0.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, BYTES, src1.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_sync_notify_kernel<<<1, 0>>>(output_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input src0", src0);
    print_data("Input src1", src1);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_sync_notify passed." : "[Failed] asc_sync_notify failed.") << std::endl;
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
