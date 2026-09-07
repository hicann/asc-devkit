# asc_set_store_atomic_config_v1

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：不支持
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

头文件路径为：`"c_api/atomic/scalar_atomic.h"`。

设置原子操作启用位与原子操作类型的值。

## 函数原型

```c
__aicore__ inline void asc_set_store_atomic_config_v1(uint16_t type,
                                                      uint16_t op)
```

## 参数说明

**表1** 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| type | 输入 | 原子操作启用位。<br>0：无原子操作。<br>1：开启原子操作，进行原子操作的数据类型为float。<br>2：开启原子操作，进行原子操作的数据类型为half。<br>3：开启原子操作，进行原子操作的数据类型为int16_t。<br>4：开启原子操作，进行原子操作的数据类型为int32_t。<br>5：开启原子操作，进行原子操作的数据类型为int8_t。<br>6：开启原子操作，进行原子操作的数据类型为bfloat16_t。 |
| op | 输入 | 原子操作类型。<br>0：求和操作。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 使用完成后，建议通过[asc_disable_dma_atomic](../datamove_atomic/asc_disable_dma_atomic.md)清空原子操作的状态，以免影响后续相关指令功能。
- 该指令执行前不会对GM的数据做清零操作，开发者需在需要时手动添加清零操作。

## 调用示例

将以下代码保存为`example.asc`后，执行如下编译运行命令：

<!-- npu="A3,910b" id9 -->
以[NPU架构版本2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-2201 && ./main
```
<!-- end id9 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t ELEMENTS = 64;
constexpr uint32_t BYTES = ELEMENTS * sizeof(int32_t);

template <typename T>
void PrintData(const char* label, const std::vector<T>& data)
{
    std::cout << label << ":";
    const size_t count = data.size() < 8 ? data.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +data[i];
    if (data.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

__global__ __vector__ void AscGetStoreAtomicConfigKernel(__gm__ int32_t* output, __gm__ int32_t* input,
    __gm__ uint64_t* raw_config)
{
    asc_init();
    // 开启float类型的原子求和操作，再读取原子配置寄存器的原始值。
    asc_set_store_atomic_config_v1(1, 0);
    asc_store_atomic_config config;
    asc_get_store_atomic_config(config);
    raw_config[0] = config.config;
    asc_dcci_single(raw_config);
    asc_sync_data_barrier(mem_dsb_t::DSB_DDR);
    asc_disable_dma_atomic();
}

} // namespace

int main()
{
    std::vector<int32_t> input(ELEMENTS), output(ELEMENTS, 10), golden(ELEMENTS);
    for (uint32_t i = 0; i < ELEMENTS; ++i) {
        input[i] = static_cast<int32_t>(i % 8 + 1);
        golden[i] = 10;
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    int32_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    uint64_t* config_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&config_device), sizeof(uint64_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, BYTES, input.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, BYTES, output.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    AscGetStoreAtomicConfigKernel<<<1, 0>>>(output_device, input_device, config_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    std::vector<uint64_t> raw_config(1);
    aclrtMemcpy(raw_config.data(), sizeof(uint64_t), config_device, sizeof(uint64_t), ACL_MEMCPY_DEVICE_TO_HOST);
    PrintData("Input", input);
    PrintData("Output", output);
    PrintData("Golden", golden);
    PrintData("Raw atomic config", raw_config);
    aclrtFree(config_device);
    bool passed = output == golden && (raw_config[0] == 1);
    std::cout << (passed ? "[Success] asc_get_store_atomic_config passed." : "[Failed] asc_get_store_atomic_config failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
