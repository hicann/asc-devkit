# asc_copy_l12ub

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

头文件路径为：`"c_api/cube_datamove/cube_datamove.h"`。

将数据从L1 Buffer搬运到Unified Buffer，按数据块个数、单个数据块长度以及源/目间隔实现连续与非连续搬运。数据搬运时格式和内容保持不变。

本接口仅在AIC上执行有效。

## 函数原型

```c
__aicore__ inline void asc_copy_l12ub(__ubuf__ void* dst_addr, __cbuf__ void* src_addr, int8_t sub_blockid, uint16_t burst_count, uint16_t burst_len, uint16_t src_gap, uint16_t dst_gap)
__aicore__ inline void asc_copy_l12ub_sync(__ubuf__ void* dst_addr, __cbuf__ void* src_addr, int8_t sub_blockid, uint16_t burst_count, uint16_t burst_len, uint16_t src_gap, uint16_t dst_gap)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst_addr | 输出 | 目的操作数，存储位置为UB。目的地址需32字节对齐。 |
| src_addr | 输入 | 源操作数，存储位置为L1 Buffer。源地址需32字节对齐。 |
| sub_blockid | 输入 | 选择目的Vector Core。<br>&nbsp;&nbsp;&bull; `0`：AIV0；<br>&nbsp;&nbsp;&bull; `1`：AIV1。 |
| burst_count | 输入 | 待搬运的连续数据块个数。取值范围 [1, 4095]。 |
| burst_len | 输入 | 每个连续数据块的长度，单位32字节。取值范围 [1, 65535]。 |
| src_gap | 输入 | 源操作数（L1 Buffer）相邻两次搬运之间的间隔，单位32字节，从上一次搬运结束位置到下一次搬运起始位置。取值范围 [0, 65535]。 |
| dst_gap | 输入 | 目的操作数（UB Buffer）相邻两次搬运之间的间隔，单位32字节，从上一次搬运结束位置到下一次搬运起始位置。取值范围 [0, 65535]。 |

## 返回值说明

无

## 流水类型

PIPE_MTE1

## 约束说明

- 本接口非AIC调用直接返回。
- dst_addr起始地址需32字节对齐（UB对齐要求），否则会导致搬运异常。
- src_addr起始地址需32字节对齐（L1 Buffer对齐要求），否则会导致搬运异常。
- 如果本指令与其他指令存在目的地址重叠，需要插入同步指令（[asc_sync_notify](../sync/asc_sync_notify.md)和[asc_sync_wait](../sync/asc_sync_wait.md)），保证多个指令串行化，防止出现异常数据。
- L1 Buffer容量上限：L1 Buffer总容量512KB，src偏移量与搬运大小之和不可超过L1 Buffer容量，否则触发读溢出异常。
- UB容量上限：UB总容量为256KB，默认预留6KB SIMD VF栈与2KB Ascend C预留空间后可用248KB；SIMD+SIMT混编时再划分32KB~128KB作Data Cache，可用容量进一步减少。dst偏移量与搬运大小之和不可超过实际可用容量，否则触发写溢出异常。
- `burst_count`、`burst_len`、`src_gap`、`dst_gap`取值需满足参数说明中取值范围，不满足导致搬运结果不符合预期。

## 调用示例

将代码保存为`examples.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[`__NPU_ARCH__`](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

本示例使用`M=N=K=128`，将128×128个`uint8_t`元素从GM搬入L1 Buffer，再通过本接口搬至UB并回传GM，与Host侧输入进行比较。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <cstdio>
#include <vector>
#include "acl/acl.h"
#include "c_api/asc_simd.h"

namespace {
constexpr uint32_t M = 128;
constexpr uint32_t N = 128;
constexpr uint32_t K = 128;
constexpr uint32_t BYTES = M * N;
constexpr int64_t READY_FLAG = 11;

__global__ __mix__(1, 2) void CopyL12ubKernel(__gm__ uint8_t* input, __gm__ uint8_t* output)
{
    asc_init();
    __cbuf__ uint8_t l1[BYTES];
    __ubuf__ uint8_t* ub = reinterpret_cast<__ubuf__ uint8_t*>(0);
    if ASC_IS_AIC {
        asc_copy_gm2l1(l1, input, 1, BYTES / 32, 0, 0, 0);
        asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_copy_l12ub(ub, l1, 0, 1, BYTES / 32, 0, 0);
        asc_sync_pipe(PIPE_MTE1);
        asc_sync_block_arrive(PIPE_MTE1, READY_FLAG);
    }
    if ASC_IS_AIV {
        asc_sync_block_wait(PIPE_MTE3, READY_FLAG);
        if (asc_get_sub_block_id() == 0) {
            asc_copy_ub2gm_align(output, ub, BYTES);
        }
    }
    asc_sync_pipe(PIPE_ALL);
}
} // namespace

int main()
{
    std::vector<uint8_t> input(BYTES), output(BYTES);
    for (uint32_t i = 0; i < BYTES; ++i) input[i] = static_cast<uint8_t>(i % 251);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *inputDevice = nullptr, *outputDevice = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&inputDevice), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&outputDevice), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(inputDevice, BYTES, input.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    CopyL12ubKernel<<<1, 0>>>(inputDevice, outputDevice);
    const aclError ret = aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, outputDevice, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    const bool passed = ret == ACL_SUCCESS && input == output;
    std::printf("M=%u N=%u K=%u asc_copy_l12ub=%s\n", M, N, K, passed ? "PASS" : "FAIL");
    aclrtFree(inputDevice);
    aclrtFree(outputDevice);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
