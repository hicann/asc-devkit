# asc_sync_data_barrier

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

阻塞后续的指令执行，直到此前已发出但尚未完成的内存访问指令全部执行完成。可以通过`arg`参数指定阻塞作用的内存范围，取值的范围以及各个取值的含义见[参数说明](#参数说明)。

## 函数原型

```c
__aicore__ inline void asc_sync_data_barrier(mem_dsb_t arg)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :---  | :--- | :--- |
| arg | 输入 | 表示需要等待的内存位置，类型为mem_dsb_t，可取值为： <br>&bull; DSB_ALL：等待所有内存访问指令。 <br>&bull; DSB_DDR：等待GM访问指令。<br>&bull; DSB_UB：等待UB访问指令。<br>&bull; DSB_SEQ：预留参数，暂未启动，为后续的功能扩展做保留。|

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- mem_dsb_t类型的入参必须直接传递枚举值字面量。
- 等待的内存位置取值为DSB_ALL时，只能等待GM和Unified Buffer（UB）的访问指令，不包括其他内存位置，例如L0C Buffer、L1 Buffer。

## 调用示例

本示例启动两个AIV，用`asc_sync_data_barrier`保证跨核访问GM时数据的写入顺序。核0先写入数据，再调用`asc_sync_data_barrier(mem_dsb_t::DSB_DDR)`等待写操作完成，最后写入同步标记。核1轮询同步标记，读到标记后再读取数据，把计算结果写入GM。

将代码保存为`example.asc`后，用`bisheng`命令编译运行。`--npu-arch`参数需按实际产品型号指定对应的NPU架构，产品与NPU架构的映射关系见[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <iostream>

#include "acl/acl.h"
#include "c_api/scalar_compute/scalar_load.h"
#include "c_api/scalar_compute/scalar_store.h"
#include "c_api/asc_simd.h"
#include "c_api/utils/sys_init.h"

namespace {

constexpr int32_t SYNC_FLAG = 6;
constexpr int32_t DATA_VALUE = 7;
constexpr int32_t EXPECTED = 2 * DATA_VALUE;
constexpr size_t SHARED_BYTES = 2 * sizeof(int32_t);
constexpr size_t OUTPUT_BYTES = sizeof(int32_t);

/*
 * 两个核分别写入数据和同步标记，保证数据写入顺序。
 * 1. Host将shared[0]、shared[1]和output初始化为0，启动两个核。
 * 2. 核0先把DATA_VALUE写入shared[1]，这个写操作是同步标记要保护的数据。
 * 3. 核0调用DSB_DDR等shared[1]写入GM完成，再把SYNC_FLAG写入shared[0]发布数据。
 * 4. 核1绕过DCache轮询shared[0]，读到SYNC_FLAG后再读shared[1]。屏障保证此时数据已可见。
 * 5. 核1把数据乘以2写入output，等结果写入GM后退出，Host最终应读到EXPECTED。
 * 如果核0不用DSB_DDR，两次Scalar写GM的完成顺序没有保证，核1可能先看到同步标记，再从shared[1]读到尚未更新的数据。
 */
__global__ __vector__ void asc_sync_data_barrier_kernel(__gm__ int32_t* shared, __gm__ int32_t* output)
{
    asc_init();

    if (block_idx == 0) {
        // 先写入被依赖的数据。
        asc_store_dev(shared + 1, DATA_VALUE);
        // 等待数据写入GM后再发布同步标记，保证核1读到标记时数据已经可见。
        asc_sync_data_barrier(mem_dsb_t::DSB_DDR);
        asc_store_dev(shared, SYNC_FLAG);
    } else if (block_idx == 1) {
        // 持续轮询同步标记，asc_load_dev每次都绕过DCache直接读取GM。
        while (asc_load_dev(shared) != SYNC_FLAG) {
        }
        const int32_t value = asc_load_dev(shared + 1);
        asc_store_dev(output, 2 * value);
        // 等待结果写入GM后退出核函数。
        asc_sync_data_barrier(mem_dsb_t::DSB_DDR);
    }
}

} // namespace

int main()
{
    int32_t output = 0;
    int32_t* sharedDevice = nullptr;
    int32_t* output_device = nullptr;

    aclInit(nullptr);
    aclrtSetDevice(0);
    aclrtMalloc(reinterpret_cast<void**>(&sharedDevice), SHARED_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemset(sharedDevice, SHARED_BYTES, 0, SHARED_BYTES);
    aclrtMemset(output_device, OUTPUT_BYTES, 0, OUTPUT_BYTES);

    asc_sync_data_barrier_kernel<<<2, 0>>>(sharedDevice, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(&output, OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = output == EXPECTED;
    std::cout << (passed ? "[Success] asc_sync_data_barrier passed."
                          : "[Failed] asc_sync_data_barrier failed.") << std::endl;

    aclrtFree(sharedDevice);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
