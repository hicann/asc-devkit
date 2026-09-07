# asc_dcci

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

头文件路径为：`"c_api/cache_ctrl/cache_ctrl.h"`。

如图1所示，在AI Core内部，访问Global Memory（GM）与Unified Buffer（UB）的方式分为以下两种：

- DMA搬运单元读写GM，数据通过[asc_copy_ub2gm](../vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm.md)和[asc_copy_gm2ub](../vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub.md)等接口在Unified Buffer（UB）等Local Memory和GM间交互，不经过DCache，因此不需要考虑Cache一致性问题。
- Scalar单元访问GM，若经过Dcache，首先会访问每个核内的DCache，因此存在DCache与GM的Cache一致性问题，具体原因请参考[缓存一致性](../../../../guide/programming_guide/advanced_programming/memory_model/cache_coherence.md)。

**图1**  DataCache内存层次示意图

![](../figures/dcci.png "DataCache内存层次示意图")

asc_dcci接口用于解决上述一致性问题，其功能可以拆解为两部分：

- Clean：将缓存中被修改过的数据（脏数据，Dirty Data）写回到GM中，避免数据丢失。
- Invalid：
    - 将指定地址范围的Cache Line标记为“无效”，使其从缓存中移除。
    - 保证下一次访问这些内存地址时，会从GM重新加载数据，而不是使用可能过期的缓存数据。

该接口的典型使用场景如下（以读写GM为例）：

- 读取GM的数据，但该数据可能在外部被其余核修改，此时需要使用asc_dcci接口，直接访问GM，获取最新数据。
- 用户通过Scalar单元写GM的数据，希望立刻写出，也需要使用asc_dcci接口。

> [!NOTE]说明
> - 标量单元读写GM时，若调用的是[asc_load_dev](../scalar_compute/scalar_load/asc_load_dev.md)和[asc_store_dev](../scalar_compute/scalar_store/asc_store_dev.md)这一对接口，则读写数据不经过DCache，无需调用asc_dcci接口。
> - Scalar单元读写UB时是否经过DCache，由[asc_set_ctrl](../spr/asc_set_ctrl.md)接口配置的CTRL[49]位控制，该bit位默认值为1'b0，即默认不经过DCache。CTRL[49]的详细说明请参考[asc_set_ctrl](../spr/asc_set_ctrl.md#ctrl_bit_desc)的常用CTRL寄存器比特位说明。

## 函数原型

```c
// 刷新传入GM地址所在的单个Cache Line
__aicore__ inline void asc_dcci_single(__gm__ void* dst)

// 刷新传入的UB地址所在的单个Cache Line
__aicore__ inline void asc_ub_dcci_single(__ubuf__ void* dst)

// 根据目标域刷新DCache中所有valid的Cache Line，目标域有以下四种取值：

// 刷新整个DCache中所有valid的Cache Line
__aicore__ inline void asc_dcci_entire_all()

// 刷新整个DCache中包含UB地址的所有valid的Cache Line
__aicore__ inline void asc_dcci_entire_ub()

// 刷新整个DCache中包含GM地址的所有valid的Cache Line
__aicore__ inline void asc_dcci_entire_out()

// 刷新整个DCache中包含与标量原子操作相关的GM地址的所有valid的Cache Line
__aicore__ inline void asc_dcci_entire_atomic()
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输入 | 待清理与失效的Cache Line所在地址。<br>硬件按Cache Line边界对齐定位目标行，传入地址无需满足Cache Line对齐。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

<!-- npu="950" id8 -->
- 针对Ascend 950PR/Ascend 950DT，不支持asc_dcci_entire_ub接口。
<!-- end id8 -->
- `dst`为`__ubuf__ void*`类型的函数原型仅支持在AIV中调用。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id9 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id9 -->

```cpp
// 本示例验证asc_dcci_entire_out对GM脏数据的回写效果。Scalar先在两个不同Cache Line中
// 写入101，再调用asc_dcci_entire_out将DCache中的脏数据回写到GM。Host回读后应得到101。
#include <array>
#include <cstdint>
#include <iostream>

#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr int32_t GM_VALUE = 11;
constexpr int32_t DIRTY_VALUE = 101;
constexpr uint32_t SECOND_LINE_OFFSET = 32;
constexpr uint32_t DATA_COUNT = 64;

__global__ __vector__ void AscDcciEntireOutKernel(__gm__ int32_t* data)
{
    asc_init();
    volatile __gm__ int32_t* data_gm = data;
    // 两个元素相隔128B，位于不同Cache Line。普通Scalar写GM会经过DCache。
    data_gm[0] = DIRTY_VALUE;
    data_gm[SECOND_LINE_OFFSET] = DIRTY_VALUE;

    // 刷新DCache中包含GM地址的Cache Line，并等待脏数据写回GM。
    asc_dcci_entire_out();
    asc_sync_data_barrier(mem_dsb_t::DSB_DDR);
}

bool CheckResult(const std::array<int32_t, DATA_COUNT>& output)
{
    return output[0] == DIRTY_VALUE && output[SECOND_LINE_OFFSET] == DIRTY_VALUE;
}
} // namespace

int main()
{
    std::array<int32_t, DATA_COUNT> input{};
    input.fill(GM_VALUE);
    aclInit(nullptr);
    aclrtSetDevice(0);

    int32_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), input.size() * sizeof(int32_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(int32_t), input.data(), input.size() * sizeof(int32_t), ACL_MEMCPY_HOST_TO_DEVICE);
    AscDcciEntireOutKernel<<<1, 0>>>(input_device);
    const aclError ret = aclrtSynchronizeDevice();
    aclrtMemcpy(input.data(), input.size() * sizeof(int32_t), input_device, input.size() * sizeof(int32_t), ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = ret == ACL_SUCCESS && CheckResult(input);
    std::cout << (passed ? "[Success] asc_dcci passed." : "[Failed] asc_dcci failed.") << std::endl;
    aclrtFree(input_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
