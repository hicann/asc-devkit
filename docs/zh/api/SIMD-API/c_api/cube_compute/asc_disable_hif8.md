# asc_disable_hif8

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

头文件路径为：`"c_api/cube_compute/cube_compute.h"`。

本接口用于设置Mmad计算关闭HiF8模式（与[asc_enable_hif8](./asc_enable_hif8.md)相反），调用此接口后L0A Buffer/L0B Buffer中的FP8数据在参与Mmad计算之前不会被转化为hifloat8_t类型数据。

本接口为矩阵计算相关配置接口，仅在AIC上生效。

## 函数原型

```cpp
__aicore__ inline void asc_disable_hif8()
```

## 参数说明

无

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 本接口非AIC调用直接返回。
- 需在[asc_mmad](./asc_mmad.md)执行前调用，以此来确保模式配置在矩阵乘加计算过程中生效。
- 与[asc_enable_hif8](./asc_enable_hif8.md)作用相反，二者不同时生效。

<!-- npu="950" id8 -->
## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

以下调用示例代码仅Ascend 950PR/Ascend 950DT产品支持。

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t M = 16;
constexpr uint32_t K = 32;
constexpr uint32_t N = 16;
constexpr uint32_t A_ELEMENTS = M * K;
constexpr uint32_t B_ELEMENTS = N * K;
constexpr uint32_t C_ELEMENTS = M * N;
constexpr uint8_t FP8_E4M3_18 = 0x59;
constexpr float FP8_RESULT = 324.0f;
constexpr float HIF8_RESULT = 25.0f / 65536.0f;

__global__ __cube__ void asc_disable_hif8_kernel(
    __gm__ uint8_t* a, __gm__ uint8_t* b, __gm__ float* hif8_output, __gm__ float* fp8_output)
{
    asc_init();
    __cbuf__ fp8_e4m3fn_t a_l1[A_ELEMENTS], b_l1[B_ELEMENTS];
    __ca__ fp8_e4m3fn_t a_l0[A_ELEMENTS];
    __cb__ fp8_e4m3fn_t b_l0[B_ELEMENTS];
    __cc__ float hif8_l0[C_ELEMENTS], fp8_l0[C_ELEMENTS];

    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(a_l1, reinterpret_cast<__gm__ fp8_e4m3fn_t*>(a), K, 0, M, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 16, 0);
    asc_copy_gm2l1_nd2nz(b_l1, reinterpret_cast<__gm__ fp8_e4m3fn_t*>(b), K, 0, N, K, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, 1, 1, 1, 1);
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, 1, 1, 1, 1);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    asc_enable_hif8();
    asc_mmad(hif8_l0, a_l0, b_l0, M, K, N, 0, true, false, true);
    asc_sync_pipe(PIPE_M);
    asc_disable_hif8();
    asc_mmad(fp8_l0, a_l0, b_l0, M, K, N, 0, true, false, true);
    asc_sync_pipe(PIPE_M);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(hif8_output, hif8_l0, N, M, N, M, 0, 0, 0,
        static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, true,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(fp8_output, fp8_l0, N, M, N, M, 0, 0, 0,
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
    std::vector<uint8_t> a(A_ELEMENTS), b(B_ELEMENTS);
    std::vector<float> hif8(C_ELEMENTS), fp8(C_ELEMENTS), hif8_golden(C_ELEMENTS), fp8_golden(C_ELEMENTS);
    for (uint32_t i = 0; i < M; ++i) {
        a[i * K + i] = FP8_E4M3_18;
        b[i * K + i] = FP8_E4M3_18;
        hif8_golden[i * N + i] = HIF8_RESULT;
        fp8_golden[i * N + i] = FP8_RESULT;
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *a_device = nullptr, *b_device = nullptr;
    float *hif8_device = nullptr, *fp8_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&hif8_device), hif8.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&fp8_device), fp8.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size(), a.data(), a.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size(), b.data(), b.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    asc_disable_hif8_kernel<<<1, 0>>>(a_device, b_device, hif8_device, fp8_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(hif8.data(), hif8.size() * sizeof(float), hif8_device, hif8.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(fp8.data(), fp8.size() * sizeof(float), fp8_device, fp8.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_row("HiF8 output row 0", hif8);
    print_row("Restored FP8 output row 0", fp8);
    print_row("HiF8 golden row 0", hif8_golden);
    print_row("FP8 golden row 0", fp8_golden);
    const bool passed = hif8 == hif8_golden && fp8 == fp8_golden;
    std::cout << (passed ? "[Success] asc_disable_hif8 restores FP8 MMAD."
                         : "[Failed] asc_disable_hif8 result mismatch.") << std::endl;
    aclrtFree(a_device); aclrtFree(b_device); aclrtFree(hif8_device); aclrtFree(fp8_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
<!-- end id8 -->
