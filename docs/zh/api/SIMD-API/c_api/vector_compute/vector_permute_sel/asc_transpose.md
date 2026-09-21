# asc_transpose

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

头文件路径为：`"c_api/vector_compute/compute/vector_permute_sel.h"`。

用于实现16*16的二维矩阵数据块转置。

## 函数原型

```c
// 占位符形式
__simd_callee__ inline void asc_transpose(__ubuf__ <dtype>* dst,
                                          __ubuf__ <dtype>* src)
```

### dtype支持数据类型

`dtype`取值为：`int16_t`、`uint16_t`。

### 函数原型典型示例

```c
// 示例：实现16*16的二维矩阵，int16_t类型数据块转置
__simd_callee__ inline void asc_transpose(__ubuf__ int16_t* dst,
                                          __ubuf__ int16_t* src)
```

## 参数说明

**表1** 参数说明

|参数名|输入/输出|描述|
| ------------ | ------------ | ------------ |
|dst|输出|目的操作数（矢量）的起始地址。|
|src|输入|源操作数（矢量）的起始地址。|

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- 操作数地址重叠约束请参考[通用地址重叠约束](../../general_description_and_constraints.md#通用地址重叠约束)。
- dst、src的起始地址需要32字节对齐。

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
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"
namespace {

constexpr uint32_t SIDE = 16;
constexpr uint32_t ELEMENTS = SIDE * SIDE;
constexpr uint32_t BYTES = ELEMENTS * sizeof(uint16_t);

__global__ __vector__ void AscTransposeKernel(__gm__ uint16_t* output, __gm__ uint16_t* input)
{
    asc_init();
    __ubuf__ uint16_t src[ELEMENTS], dst[ELEMENTS];
    asc_copy_gm2ub_align(src, input, BYTES);
    asc_sync_mte2(0);
    // Transpose one 16x16 uint16_t matrix in UB.
    asc_transpose(dst, src);
    asc_sync_notify(PIPE_V, PIPE_S, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_S, EVENT_ID0);
    asc_copy_ub2gm_align(output, dst, BYTES);
    asc_sync_mte3(0);
}
} // namespace

int main()
{
    std::vector<uint16_t> input(ELEMENTS), output(ELEMENTS), golden(ELEMENTS);
    for (uint32_t row = 0; row < SIDE; ++row) {
        for (uint32_t column = 0; column < SIDE; ++column) {
            input[row * SIDE + column] = row * 16 + column;
            golden[row * SIDE + column] = column * 16 + row;
        }
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, BYTES, input.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    AscTransposeKernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    std::cout << "Input row 0:";
    for (uint32_t i = 0; i < SIDE; ++i) std::cout << ' ' << input[i];
    std::cout << " ...\nOutput row 0:";
    for (uint32_t i = 0; i < SIDE; ++i) std::cout << ' ' << output[i];
    std::cout << " ..." << std::endl;
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_transpose passed." : "[Failed] asc_transpose failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}

```
