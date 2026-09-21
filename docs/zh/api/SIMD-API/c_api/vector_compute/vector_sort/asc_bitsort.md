# asc_bitsort

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"c_api/vector_compute/compute/vector_sort.h"`。

本接口实现一次迭代内对32个数的降序排列操作。支持通过`repeat`参数分别对多个迭代进行排列。每次迭代处理32个元素；下次迭代时，`src0`和`src1`各跳过32个元素，`dst`跳过256字节。不同迭代之间不进行排序。

如下图所示，src0和src1分别表示待排序的数据score和对应RP的索引index，排序完成后以（score, index）的结构存储在dst中。

src1固定为uint32\_t类型，无论src0是float还是half类型，dst中的（score, index）结构总是占据8B空间。其中：

- 当src0为float类型时，dst中的index存储在高4B，score存储在低4B；
- 当src0为half类型时，dst中index存储在高4B，score存储在低2B，中间的2B保留。

**图1**  asc_bitsort排序示意图

![](../../../../figures/sort32.png)<a id="图1-asc_bitsort排序示意图"></a>

## 函数原型

```cpp
// 占位符形式
__aicore__ inline void asc_bitsort(__ubuf__ <dtype>* dst, __ubuf__ <dtype>* src0, __ubuf__ uint32_t* src1, int32_t repeat)
```

### dtype支持数据类型

`dtype`支持的数据类型为`half`、`float`。

### 函数原型典型示例

```cpp
__aicore__ inline void asc_bitsort(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ uint32_t* src1, int32_t repeat)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数在UB中的起始地址。 |
| src0 | 输入 | score在UB中的起始地址。 |
| src1 | 输入 | index在UB中的起始地址。 |
| repeat | 输入 | 迭代次数，取值范围为[0, 255]。每次迭代处理32个元素；下次迭代时，`src0`和`src1`各跳过32个元素，`dst`跳过256字节。不同迭代之间不进行排序。 |

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- 操作数地址重叠约束请参考[通用地址重叠约束](../../general_description_and_constraints.md#通用地址重叠约束)。
- `dst`、`src0`、`src1`的起始地址需要32字节对齐。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR&950DT系列产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
template <typename T>
void print_data(const char* label, const std::vector<T>& values)
{
    std::cout << label << ":";
    const size_t count = values.size() < 8 ? values.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[i];
    if (values.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

constexpr uint32_t ELEMENT_COUNT = 32;
constexpr uint32_t INPUT_BYTES = ELEMENT_COUNT * sizeof(float);
constexpr uint32_t OUTPUT_BYTES = ELEMENT_COUNT * 8;

__global__ __vector__ void bitsort_kernel(__gm__ float* dst, __gm__ float* src0, __gm__ uint32_t* src1)
{
    asc_init();
    __ubuf__ float scores[ELEMENT_COUNT];
    __ubuf__ uint32_t indices[ELEMENT_COUNT];
    __ubuf__ float sorted[ELEMENT_COUNT * 2];

    asc_copy_gm2ub_align(scores, src0, INPUT_BYTES);
    asc_copy_gm2ub_align(indices, src1, ELEMENT_COUNT * sizeof(uint32_t));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_bitsort(sorted, scores, indices, 1);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(dst, sorted, OUTPUT_BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> scores(ELEMENT_COUNT), output_scores(ELEMENT_COUNT), golden_scores(ELEMENT_COUNT);
    std::vector<uint32_t> indices(ELEMENT_COUNT), output_indices(ELEMENT_COUNT), golden_indices(ELEMENT_COUNT);
    std::vector<float> result(ELEMENT_COUNT * 2, 0);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        scores[i] = static_cast<float>(i + 1);
        indices[i] = i;
        golden_scores[i] = static_cast<float>(ELEMENT_COUNT - i);
        golden_indices[i] = ELEMENT_COUNT - 1 - i;
    }

    if (aclInit(nullptr) != ACL_SUCCESS || aclrtSetDevice(0) != ACL_SUCCESS) {
        std::cerr << "Device initialization failed\n";
        return 1;
    }
    float* scores_device = nullptr;
    uint32_t* indices_device = nullptr;
    float* result_device = nullptr;
    if (aclrtMalloc(reinterpret_cast<void**>(&scores_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&indices_device), ELEMENT_COUNT * sizeof(uint32_t), ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS ||
        aclrtMalloc(reinterpret_cast<void**>(&result_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST) != ACL_SUCCESS) {
        std::cerr << "Device allocation failed\n";
        return 1;
    }

    bool ok = aclrtMemcpy(scores_device, INPUT_BYTES, scores.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE) == ACL_SUCCESS &&
              aclrtMemcpy(indices_device, ELEMENT_COUNT * sizeof(uint32_t), indices.data(),
                          ELEMENT_COUNT * sizeof(uint32_t), ACL_MEMCPY_HOST_TO_DEVICE) == ACL_SUCCESS;
    if (ok) {
        bitsort_kernel<<<1, 0>>>(result_device, scores_device, indices_device);
        ok = aclrtSynchronizeDevice() == ACL_SUCCESS &&
             aclrtMemcpy(result.data(), OUTPUT_BYTES, result_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST) == ACL_SUCCESS;
    }
    if (ok) {
        for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
            output_scores[i] = result[2 * i];
            std::memcpy(&output_indices[i], &result[2 * i + 1], sizeof(uint32_t));
        }
    }
    print_data("Input scores", scores);
    print_data("Input indices", indices);
    print_data("Output scores", output_scores);
    print_data("Output indices", output_indices);
    print_data("Golden scores", golden_scores);
    print_data("Golden indices", golden_indices);
    const bool passed = ok && output_scores == golden_scores && output_indices == golden_indices;
    std::cout << (passed ? "[Success] asc_bitsort passed." : "[Failed] asc_bitsort failed.") << std::endl;

    aclrtFree(scores_device);
    aclrtFree(indices_device);
    aclrtFree(result_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
