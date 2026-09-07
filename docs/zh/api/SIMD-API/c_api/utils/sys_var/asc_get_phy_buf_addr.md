# asc_get_phy_buf_addr

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

头文件路径为：`"c_api/utils/sys_var.h"`。

基于偏移量offset获取片上实际物理地址，常用于在调试、地址推导或tiling数据地址计算场景下，把一个相对偏移量转换为可在硬件上直接使用的物理地址形式。

## 函数原型

```c
__aicore__ inline uint64_t asc_get_phy_buf_addr(uint64_t offset)
```

## 参数说明

**表1** 参数说明

|参数名|输入/输出|描述|
| :------ | :--- | :------------ |
|offset   |输入   |片上内存的地址偏移量，范围是0~物理内存大小。|

## 返回值说明

物理内存的地址。

## 流水类型

PIPE_S

## 约束说明

不能和[以数组方式申请内存的方法](../../general_description_and_constraints.md#以数组方式申请内存)混用。

## 调用示例

本示例模拟Kernel手工规划Unified Buffer的场景：通过0字节、32字节和64字节三个偏移分别获取两个源操作数和一个目的操作数的物理地址，在Unified Buffer中完成加法后将结果写入GM。该方法适用于需要自行管理片上内存布局的底层实现；不能与[以数组方式申请内存的方法](../../general_description_and_constraints.md#以数组方式申请内存)混用。

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

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

constexpr uint32_t ELEMENTS = 16;
constexpr uint32_t BYTES = ELEMENTS * sizeof(uint32_t);

/*
 * 本示例说明：
 * - 每段Unified Buffer空间按32字节对齐，避免不同操作数重叠。
 * - asc_get_phy_buf_addr根据偏移返回片上物理地址。
 * - Kernel在手工分配的Unified Buffer空间中完成10+32，并将结果写入GM。
 */
__global__ __vector__ void asc_get_phy_buf_addr_kernel(__gm__ uint32_t* output)
{
    asc_init();
    __ubuf__ uint32_t* src0 =
        reinterpret_cast<__ubuf__ uint32_t*>(asc_get_phy_buf_addr(0));
    __ubuf__ uint32_t* src1 =
        reinterpret_cast<__ubuf__ uint32_t*>(asc_get_phy_buf_addr(32));
    __ubuf__ uint32_t* dst =
        reinterpret_cast<__ubuf__ uint32_t*>(asc_get_phy_buf_addr(64));

    src0[0] = 10U;
    src1[0] = 32U;
    dst[0] = src0[0] + src1[0];
    output[0] = dst[0];
    asc_dcci_single(output);
}

} // namespace

int main()
{
    std::vector<uint32_t> output(ELEMENTS, 0);
    uint32_t* output_device = nullptr;

    aclInit(nullptr);
    aclrtSetDevice(0);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    asc_get_phy_buf_addr_kernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    std::cout << "Unified Buffer calculation result: " << output[0] << std::endl;
    const bool passed = output[0] == 42U;
    std::cout << (passed ? "[Success] asc_get_phy_buf_addr passed."
                          : "[Failed] asc_get_phy_buf_addr failed.") << std::endl;

    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
