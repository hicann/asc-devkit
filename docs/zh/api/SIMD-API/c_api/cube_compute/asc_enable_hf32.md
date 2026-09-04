# asc_enable_hf32

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

头文件路径为：`"c_api/cube_compute/cube_compute.h"`。

本接口用于设置Mmad计算开启HF32模式，开启该模式后，Mmad计算FP32数据的性能将得到提升，但会带来一定的精度损失。

是否开启HF32对Mmad计算理论性能的影响见下表：

<!-- npu="A3,910b" id8 -->
**表1** HF32对Mmad计算理论性能的影响（[NPU架构版本2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）<a id="table1877123815311"></a>

| 接口 | 左矩阵A | 右矩阵B | $cube_m$ | $cube_n$ | $cube_k$ | $k_0$ |
| --- | --- | --- | --- | --- | --- | --- |
| asc_mmad（不开启HF32） | float | float | 16 | 16 | 4 | 8 |
| asc_mmad（开启HF32） | float | float | 16 | 16 | 8 | 8 |

<!-- end id8 -->

<!-- npu="950" id9 -->
**表2** HF32对Mmad计算理论性能的影响（[NPU架构版本3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）<a id="table1877123815915"></a>

| 接口 | 左矩阵A | 右矩阵B | $cube_m$ | $cube_n$ | $cube_k$ | $k_0$ |
| --- | --- | --- | --- | --- | --- | --- |
| asc_mmad（不开启HF32） | float | float | 16 | 16 | 1 | 8 |
| asc_mmad（开启HF32） | float | float | 16 | 16 | 8 | 8 |

<!-- end id9 -->

性能计算公式如下：

$$
\begin{gathered}
{ceil_m} = \left\lceil \frac{m}{16} \right\rceil \times 16 \\[12pt]
{ceil_n} = \left\lceil \frac{n}{16} \right\rceil \times 16 \\[12pt]
{ceil_k} = \left\lceil \frac{k}{16} \right\rceil \times k_0 \\[16pt]
\text{cube利用率} =
\frac{ (m \times n \times k) / ({cube_m} \times {cube_n} \times {cube_k}) }
{ \Delta t + ({ceil_m} \times {ceil_n} \times {ceil_k}) / ({cube_m} \times {cube_n} \times {cube_k}) }
\end{gathered}
$$

关键变量及常量说明：

- $m, n, k$：mmad入参实际计算的大小。
- $ceil_m, ceil_n, ceil_k$：$m, n, k$ 根据分形大小向上对齐后的值。
- $cube_m, cube_n, cube_k$：硬件真实并行度（单位：elements/cycle）。
- $k_0$：L0 Buffer上最小分形K方向大小。
- $\Delta t$：头开销cycle数。

开启HF32模式后，L0A Buffer/L0B Buffer中的FP32数据将在参与Mmad计算之前被舍入为HF32格式，舍入模式使用[asc_set_hf32_round_mode](asc_set_hf32_round_mode.md)接口配置，中间计算使用HF32格式，最终的运算结果仍以FP32格式输出，以保证后续处理的兼容性。

FP32与HF32格式的精度对比如下图所示：

<!-- npu="A3,910b" id10 -->
**图1** FP32与HF32格式精度示意图（[NPU架构版本2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）<a id="zh_cn_topic_hf32_figure1"></a>

![FP32与HF32格式精度示意图（NPU架构版本2201）](../../../figures/mmad_hf32.png "FP32与HF32格式精度示意图（NPU架构版本2201）")
<!-- end id10 -->

<!-- npu="950" id11 -->
**图2** FP32与HF32格式精度示意图（[NPU架构版本3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）<a id="zh_cn_topic_hf32_figure2"></a>

![FP32与HF32格式精度示意图（NPU架构版本3510）](../../../figures/mmad_hf32_950.png "FP32与HF32格式精度示意图（NPU架构版本3510）")
<!-- end id11 -->

## 函数原型

```c
__aicore__ inline void asc_enable_hf32()
```

## 参数说明

无

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 本接口需在矩阵乘加指令（[asc_mmad](asc_mmad.md)）执行前调用，以此来确保模式配置在矩阵乘加计算过程中生效。
- HF32模式启用后会持续生效，不会自动关闭。后续矩阵乘加指令若不显式重新配置，将沿用当前模式。如需关闭HF32模式，请重新调用[asc_disable_hf32](asc_disable_hf32.md)接口。
- HF32模式启用后，FP32转换为HF32格式的舍入模式由配套的[asc_set_hf32_round_mode](asc_set_hf32_round_mode.md)接口配置，须在本接口之后调用该配套接口配置舍入模式，否则舍入模式沿用上次配置或默认值。
- HF32模式仅对输入矩阵数据类型为`float`的矩阵乘加运算场景生效。

<!-- npu="950" id12 -->
## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

以下调用示例代码仅Ascend 950PR/Ascend 950DT产品支持。

```cpp
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t DIM = 16;
constexpr uint32_t ELEMENTS = DIM * DIM;

__global__ __cube__ void asc_enable_hf32_kernel(
    __gm__ float* a, __gm__ float* b, __gm__ float* fp32_output, __gm__ float* hf32_output)
{
    asc_init();
    __cbuf__ float a_l1[ELEMENTS];
    __cbuf__ float b_l1[ELEMENTS];
    __ca__ float a_l0[ELEMENTS];
    __cb__ float b_l0[ELEMENTS];
    __cc__ float fp32_l0[ELEMENTS];
    __cc__ float hf32_l0[ELEMENTS];

    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(a_l1, a, DIM * sizeof(float), 0, DIM, DIM, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(b_l1, b, DIM * sizeof(float), 0, DIM, DIM, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, 1, 2, 1, 1);
    asc_copy_l12l0b_transpose(b_l0, b_l1, 0, 0, 1, 2, 1, 1);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_disable_hf32();
    asc_mmad(fp32_l0, a_l0, b_l0, DIM, DIM, DIM, 0, true, false, true);
    asc_sync_pipe(PIPE_M);
    asc_enable_hf32();
    asc_set_hf32_round_mode(asc_hf32_round_mode::NEAREST_EVEN);
    asc_mmad(hf32_l0, a_l0, b_l0, DIM, DIM, DIM, 0, true, false, true);
    asc_sync_pipe(PIPE_M);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(fp32_output, fp32_l0, DIM, DIM, DIM, DIM, 0, 0, 0,
        static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, true,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(hf32_output, hf32_l0, DIM, DIM, DIM, DIM, 0, 0, 0,
        static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, true,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
    asc_sync_pipe(PIPE_ALL);
}

void print_row(const char* label, const std::vector<float>& data)
{
    std::cout << label << ':';
    for (uint32_t i = 0; i < 8; ++i) std::cout << ' ' << data[i];
    std::cout << " ..." << std::endl;
}
} // namespace

int main()
{
    std::vector<float> a(ELEMENTS, 0.0f), b(ELEMENTS, 0.0f);
    std::vector<float> fp32(ELEMENTS, 0.0f), hf32(ELEMENTS, 0.0f), golden(ELEMENTS, 0.0f);
    for (uint32_t i = 0; i < DIM; ++i) {
        a[i * DIM + i] = 1.0003f;
        b[i * DIM + i] = 1.0f;
        golden[i * DIM + i] = 1.0003f;
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float *a_device = nullptr, *b_device = nullptr, *fp32_device = nullptr, *hf32_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&fp32_device), fp32.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&hf32_device), hf32.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size() * sizeof(float), a.data(), a.size() * sizeof(float), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size() * sizeof(float), b.data(), b.size() * sizeof(float), ACL_MEMCPY_HOST_TO_DEVICE);
    asc_enable_hf32_kernel<<<1, 0>>>(a_device, b_device, fp32_device, hf32_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(fp32.data(), fp32.size() * sizeof(float), fp32_device, fp32.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(hf32.data(), hf32.size() * sizeof(float), hf32_device, hf32.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);

    print_row("Input A row 0", a);
    print_row("FP32 output row 0", fp32);
    print_row("HF32 output row 0", hf32);
    print_row("FP32 golden row 0", golden);
    bool passed = true;
    for (uint32_t i = 0; i < ELEMENTS; ++i) passed &= std::fabs(fp32[i] - golden[i]) < 1e-6f;
    passed &= std::fabs(hf32[0] - fp32[0]) > 1e-5f;
    std::cout << (passed ? "[Success] asc_enable_hf32 shows HF32 rounding."
                         : "[Failed] asc_enable_hf32 result mismatch.") << std::endl;
    aclrtFree(a_device);
    aclrtFree(b_device);
    aclrtFree(fp32_device);
    aclrtFree(hf32_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
<!-- end id12 -->
