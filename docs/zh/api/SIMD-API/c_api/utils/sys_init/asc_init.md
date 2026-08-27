# asc_init

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

本接口将当前AI Core上可能由前序算子遗留的全局状态恢复为接口约定的初始状态，避免这些状态影响当前算子的计算结果。本接口仅初始化控制状态，不会清零Global Memory、Local Memory或用户数据。

不同产品的初始化范围和初始化后的状态如下。

<!-- npu="950" id8 -->
**表 1**  Ascend 950PR/Ascend 950DT初始化结果

| 状态类别 | 初始化项                                           | 初始化后的状态                                                                                                                 |
| -------- | -------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------ |
| 公共状态 | 原子累加状态                                       | 清空原子累加状态，后续数据搬运不执行原子累加。                                                                                 |
| 公共状态 | `Mask`工作模式                                     | 设置为`Normal`模式。                                                                                                           |
| 公共状态 | `CTRL`寄存器                                       | 将`CTRL`寄存器恢复为接口约定的初始状态，其中保持位的值保持调用本接口前的值。                                                   |
| 公共状态 | 原子操作开启位与原子操作类型                       | 寄存器配置值设置为`0b00100100`，其中数据类型字段为`half`，操作类型字段为求和。原子累加状态已清空，因此初始化后不执行原子累加。 |
| AIC      | `asc_copy_l12l0a`进行3D格式搬运时的`Padding`填充值 | 设置为0。                                                                                                                      |
| AIV      | `Mask`                                             | 高位`Mask`和低位`Mask`均设置为`0xffffffffffffffff`。在`Normal`模式下，单次迭代内的所有元素均参与计算。                         |
| AIV      | `GM->UB`与`UB->GM`搬运的循环次数                   | 内层循环次数和外层循环次数均设置为1，恢复为普通搬运模式。                                                                      |
<!-- end id8 -->

<!-- npu="A3" id9 -->
**表 2**  Atlas A3 训练系列产品/Atlas A3 推理系列产品初始化结果

| 状态类别 | 初始化项       | 初始化后的状态                                                                                         |
| -------- | -------------- | ------------------------------------------------------------------------------------------------------ |
| AIC      | 全局状态       | 不修改Cube Core的状态。                                                                                |
| AIV      | 原子累加状态   | 清空原子累加状态，后续数据搬运不执行原子累加。                                                         |
| AIV      | `Mask`工作模式 | 设置为`Normal`模式。                                                                                   |
| AIV      | `Mask`         | 高位`Mask`和低位`Mask`均设置为`0xffffffffffffffff`。在`Normal`模式下，单次迭代内的所有元素均参与计算。 |
<!-- end id9 -->

<!-- npu="910b" id10 -->
**表 3**  Atlas A2 训练系列产品/Atlas A2 推理系列产品初始化结果

| 状态类别 | 初始化项       | 初始化后的状态                                                                                         |
| -------- | -------------- | ------------------------------------------------------------------------------------------------------ |
| AIC      | 全局状态       | 不修改Cube Core的状态。                                                                                |
| AIV      | 原子累加状态   | 清空原子累加状态，后续数据搬运不执行原子累加。                                                         |
| AIV      | `Mask`工作模式 | 设置为`Normal`模式。                                                                                   |
| AIV      | `Mask`         | 高位`Mask`和低位`Mask`均设置为`0xffffffffffffffff`。在`Normal`模式下，单次迭代内的所有元素均参与计算。 |
<!-- end id10 -->

## 函数原型

```cpp
__aicore__ inline void asc_init()
```

## 参数说明

无

## 返回值说明

无

## 流水类型

`PIPE_S`

## 约束说明

在实际运行中，这些值可能被前序执行的算子修改，若不调用该接口进行初始化，非预期的值可能导致计算结果出现精度错误。

例如前序算子使用`Counter`模式但未重置为`Normal`模式，当前算子以默认的`Normal`模式设置`Mask`时，会导致`Mask`设置不符合预期，进而引发精度错误。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id11 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510; ./main
```
<!-- end id11 -->

```c
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t ELEMENTS = 16;
constexpr uint32_t BYTES = ELEMENTS * sizeof(uint64_t);

void print_data(const char* label, const std::vector<uint64_t>& data)
{
    std::cout << label << ":";
    const size_t count = data.size() < 8 ? data.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << data[i];
    if (data.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

__global__ __vector__ void asc_init_kernel(__gm__ uint64_t* output, __gm__ uint64_t* input)
{
    asc_init();
    __ubuf__ uint32_t local[BYTES / sizeof(uint32_t)];
    asc_copy_gm2ub_align(local, reinterpret_cast<__gm__ uint32_t*>(input), BYTES);
    asc_sync_mte2(0);
    asc_set_atomic_add_int();
    // Reinitialize the core and clear persistent state such as the atomic mode.
    asc_init();
    // asc_init resets the atomic mode, so this is an ordinary Unified Buffer（UB）-to-GM copy.
    asc_copy_ub2gm(reinterpret_cast<__gm__ uint32_t*>(output), local, BYTES);
    asc_sync_mte3(0);
}

} // namespace

int main()
{
    std::vector<uint64_t> input(ELEMENTS), output(ELEMENTS, 0);
    for (uint32_t i = 0; i < ELEMENTS; ++i) { input[i] = i + 1; output[i] = 10; }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint64_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, BYTES, input.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, BYTES, output.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_init_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output after asc_init reset", output);
    print_data("Golden ordinary copy", input);
    const bool passed = output == input;
    std::cout << (passed ? "[Success] asc_init passed." : "[Failed] asc_init failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
