# asc_set_atomic_add

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

原子累加过程：将待搬运到GM的数据和GM上已有数据进行求和，然后将求和结果写入GM。本接口对后续目的地址为GM的数据搬运指令开启原子累加，不同产品支持的数据搬运通路请参考[约束说明](#约束说明)。

接口可选择不同的函数原型来设定不同的累加数据类型。

## 函数原型

```c
__aicore__ inline void asc_set_atomic_add_int8()
__aicore__ inline void asc_set_atomic_add_int16()
__aicore__ inline void asc_set_atomic_add_int32()
__aicore__ inline void asc_set_atomic_add_float16()
__aicore__ inline void asc_set_atomic_add_bfloat()
__aicore__ inline void asc_set_atomic_add_float()
```

## 参数说明

无

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 各个产品由于硬件架构不同，支持的数据通路也不同，具体情况如下：
    <!-- npu="950" id11 -->
    - Ascend 950PR/Ascend 950DT，支持的数据通路为UB/L0C Buffer->GM。
    <!-- end id11 -->
    <!-- npu="A3" id8 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据通路为UB/L0C Buffer/L1 Buffer->GM。
    <!-- end id8 -->
    <!-- npu="910b" id9 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据通路为UB/L0C Buffer/L1 Buffer->GM。
    <!-- end id9 -->
- 本接口调用后会对后续所有目的地址为GM的搬运指令开启原子操作，可以调用[asc_set_atomic_none](./asc_set_atomic_none.md)接口关闭原子操作。
- 该接口执行前不会自动将GM上已有数据置零。若开发者期望在原子累加前GM上的原始数据为零，则需手动清零。
- 本接口仅对后续目的地址为GM的搬运指令（通过MTE1/MTE2/MTE3单元搬运）生效，对于标量写GM的指令（例如[asc_store_dev](../scalar_compute/asc_store_dev.md)）不生效。
- 本接口与紧邻的后续搬运指令之间的同步由硬件保证，因此以下示例中插入的多流水同步是不必要的：

    ```c
    asc_set_atomic_add_int8();

    /*
    asc_set_atomic_add_int8与asc_copy_ub2gm之间的同步由硬件保证，因此以下同步是不必要的。
    asc_sync_notify(PIPE_S, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_S, PIPE_MTE3, EVENT_ID0);
    */

    asc_copy_ub2gm(dst, src1, total_length * sizeof(int8_t));
    // 关闭原子操作。
    asc_set_atomic_none();
    ```

- 后续搬运指令的操作数据类型需与所选接口设置的数据类型一致。
- 本接口不能保证后续搬运指令的执行顺序，若需保证确定性的执行顺序请参考[关键特性说明](key_features.md)。

## 调用示例

将代码保存为`examples.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id10 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng examples.asc -o main --npu-arch=dav-3510; ./main
```
<!-- end id10 -->

```c
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
template <typename T>
void PrintData(const char* label, const std::vector<T>& values)
{
    std::cout << label << ":";
    const size_t count = values.size() < 8 ? values.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[i];
    if (values.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

template <typename T>
bool CompareData(const std::vector<T>& actual, const std::vector<T>& expected, double tolerance = 0.0)
{
    if (actual.size() != expected.size()) return false;
    for (size_t i = 0; i < actual.size(); ++i) {
        if (actual[i] == expected[i]) continue;
        const double diff = static_cast<double>(actual[i]) - static_cast<double>(expected[i]);
        if (diff > tolerance || diff < -tolerance) return false;
    }
    return true;
}

template <typename T>
bool CompareRangeData(const std::vector<T>& actual, const std::vector<T>& expected,
    size_t begin, size_t count, double tolerance = 0.0)
{
    if (begin + count > actual.size() || begin + count > expected.size()) return false;
    for (size_t i = begin; i < begin + count; ++i) {
        if (actual[i] == expected[i]) continue;
        const double diff = static_cast<double>(actual[i]) - static_cast<double>(expected[i]);
        if (diff > tolerance || diff < -tolerance) return false;
    }
    return true;
}

constexpr uint32_t ELEMENTS = 8;

__global__ __vector__ void AscAtomicAddKernel(__gm__ int64_t* output)
{
    asc_init();
    __gm__ uint32_t* address = reinterpret_cast<__gm__ uint32_t*>(output);
    asc_dcci_entire_all();
    const uint32_t old_value = asc_atomic_add(address, 5U);
    asc_sync_data_barrier(mem_dsb_t::DSB_ALL);
    asc_dcci_entire_all();
    output[1] = static_cast<int64_t>(old_value);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<int64_t> input = {10, 0};
    std::vector<int64_t> golden = {15, 10};
    input.resize(ELEMENTS, 0);
    golden.resize(ELEMENTS, 0);
    std::vector<int64_t> output(ELEMENTS, -1);
    aclInit(nullptr);
    aclrtSetDevice(0);
    int64_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), (ELEMENTS) * sizeof(int64_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(output_device, input.size() * sizeof(int64_t), input.data(), input.size() * sizeof(int64_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    AscAtomicAddKernel<<<1, 0>>>(output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int64_t), output_device, output.size() * sizeof(int64_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    PrintData("Input", input);
    PrintData("Output", output);
    PrintData("Golden", golden);
    const bool passed = CompareData(output, golden);
    std::cout << (passed ? "[Success] asc_atomic_add passed." : "[Failed] asc_atomic_add failed.") << std::endl;
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
