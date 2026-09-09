# asc_copy_l12fb

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

将L1 Buffer中的数据搬运到Fixpipe Buffer，供后续Fixpipe流水的随路Vector量化与随路ReLU使用。搬运的数据为随路Vector量化和随路ReLU参数，两类参数各自独立存储，并拥有独立的地址空间。量化参数和ReLU参数通过dst地址的高4位（`dst[19:16]`）进行区分，具体如下：

**图1** Fixpipe Buffer图示

![](../../../../figures/fixpipe_buffer_02.png)

提供两种调用形式：

- **高维切分搬运**：指定`n_burst`、`len_burst`、`src_gap_size`、`dst_gap_size`，通过配置数据块个数、单个数据块长度以及源/目间隔实现连续与非连续搬运。

- **前n个数据搬运**：仅指定`size`，按照一个连续数据块搬运，源/目间隔均为0。

本接口仅在AIC上执行有效。

## 函数原型

- 高维切分搬运

    ```c
    __aicore__ inline void asc_copy_l12fb(__fbuf__ void* dst, __cbuf__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap_size, uint16_t dst_gap_size)
    ```

- 前n个数据搬运

    ```c
    __aicore__ inline void asc_copy_l12fb(__fbuf__ void* dst, __cbuf__ void* src, uint32_t size)
    ```

- 同步搬运

    ```c
    __aicore__ inline void asc_copy_l12fb_sync(__fbuf__ void* dst, __cbuf__ void* src, uint32_t size)
    ```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数，存储位置为Fixpipe Buffer。`dst[15:0]`为地址空间的偏移，各个地址空间相互独立，地址偏移需要各自计算，且目的地址按照功能不同对齐要求不同。<br>&nbsp;&nbsp;&bull; 随路量化场景：`dst[16]=0`代表随路量化参数地址空间，目的地址需要128B对齐；<br>&nbsp;&nbsp;&bull;随路Relu场景：`dst[16]=1`代表随路Relu参数地址空间，目的地址需要64B对齐。 |
| src | 输入 | 源操作数，存储位置为L1 Buffer。源地址需32字节对齐。 |
| n_burst | 输入 | 待搬运的连续数据块个数。取值范围[1, 4095]。 |
| len_burst | 输入 | 每个连续数据块的长度，单位64字节。取值范围[1, 65535]，对于随路量化场景取值必须为2的倍数。 |
| src_gap_size | 输入 | 源端（L1 Buffer）相邻两次搬运之间的步长系数，从上一次搬运结束位置到下一次搬运起始位置，单位32字节。取值范围[0, 65535]。 |
| dst_gap_size | 输入 | 目的端（Fixpipe Buffer）相邻两次搬运之间的步长系数，从上一次搬运结束位置到下一次搬运起始位置，单位32字节。取值范围[0, 65535]，对于随路量化场景取值必须为2的倍数。 |
| size | 输入 | 搬运数据总大小，单位字节。<br>**注：size需要32字节对齐，若未对齐，搬运量会向下取整到32字节对齐。** |

## 返回值说明

无

## 流水类型

PIPE_FIX

## 约束说明

### 通用约束

- 本接口非AIC调用直接返回。
- src起始地址需32字节对齐（L1 Buffer对齐要求），否则触发搬运异常。
- dst起始地址根据功能不同对齐要求不同，对于随路量化场景，起始地址需要128B对齐，对于随路Relu场景起始地址需要64B对齐，否则触发搬运异常。
- L1 Buffer总容量512KB，src偏移与搬运大小之和不可越界，否则触发地址越界异常。
- Fixpipe Buffer对于随路量化场景，容量上限为4KB；对于随路Relu场景，容量上限为2KB。dst偏移与搬运大小之和越界时会截断处理，导致搬运结果不符合预期。
- 如果本指令与其他指令存在目的地址重叠，需要插入同步指令（[asc_sync_notify](../../sync/asc_sync_notify.md)和[asc_sync_wait](../../sync/asc_sync_wait.md)），保证多个指令串行化，防止出现异常数据。
- 对于随路量化场景，`len_burst`与`dst_gap_size`场景取值必须为2的倍数，否则触发搬运异常。
- `n_burst`或`size`设置为`0`时，不执行搬运，本接口被视为NOP（空操作）。

## 调用示例

将代码保存为`examples.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <cstdio>
#include "acl/acl.h"
#include "c_api/asc_simd.h"

constexpr uint32_t M = 128, N = 128, K = 128;
constexpr uint32_t BYTES = N * sizeof(half);

__global__ __cube__ void CopyL12fbKernel(__gm__ half* input)
{
    asc_init();
    __cbuf__ half l1[N];
    __fbuf__ half fbuf[N];
    asc_copy_gm2l1(l1, input, 1, BYTES / 32, 0, 0, 0);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_copy_l12fb(fbuf, l1, BYTES);
    asc_sync_pipe(PIPE_MTE1);
}

int main()
{
    aclInit(nullptr); aclrtSetDevice(0);
    half* input = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    CopyL12fbKernel<<<1, 0>>>(input);
    const aclError ret = aclrtSynchronizeDevice();
    std::printf("M=%u N=%u K=%u asc_copy_l12fb=%s\n", M, N, K,
        ret == ACL_SUCCESS ? "PASS" : "FAIL");
    aclrtFree(input); aclrtResetDevice(0); aclFinalize();
    return ret == ACL_SUCCESS ? 0 : 1;
}
```
