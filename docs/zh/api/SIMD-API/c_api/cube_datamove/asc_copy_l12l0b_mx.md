# asc_copy_l12l0b_mx

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

头文件路径为：`"c_api/cube_datamove/cube_datamove.h"`。

负责完成MX矩阵计算过程中所需的右矩阵对应的量化系数的搬运，数据通路为L1 Buffer->L0B_MX Buffer。其中右量化系数矩阵以32字节（固定数据类型为`fp8_e8m0_t`、分形大小为2×16的）的数据分形为单位进行搬运。

其中，L0B_MX Buffer的大小为4KB，和L0B Buffer地址的映射关系如下：

$$
L0B\_MX\ Buffer\ Address = L0B\ Buffer\ Address / 16
$$

本接口为MX矩阵计算量化系数搬运接口，需要与对应数据搬运接口[asc_copy_l12l0b](asc_copy_l12l0b/asc_copy_l12l0b_2d_arch_3510.md)配合使用。

本接口仅在AIC上执行有效。

## 函数原型

```cpp
__aicore__ inline void asc_copy_l12l0b_mx(uint64_t dst,
                                          __cbuf__ fp8_e8m0_t* src,
                                          uint16_t x_start_pos,
                                          uint16_t y_start_pos,
                                          uint8_t x_step,
                                          uint8_t y_step,
                                          uint16_t src_stride,
                                          uint16_t dst_stride)

__aicore__ inline void asc_copy_l12l0b_mx_sync(uint64_t dst,
                                               __cbuf__ fp8_e8m0_t* src,
                                               uint16_t x_start_pos,
                                               uint16_t y_start_pos,
                                               uint8_t x_step,
                                               uint8_t y_step,
                                               uint16_t src_stride,
                                               uint16_t dst_stride)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 含义 |
| --- | --- | --- |
| dst | 输出 | 目的操作数，存储位置为L0B_MX Buffer。起始地址需要按照32字节对齐。 |
| src | 输入 | 源操作数，存储位置为L1 Buffer。起始地址需要按照32字节对齐。 |
| x_start_pos | 输入 | 源矩阵X轴起始分形位置。单位为32字节，即1个2×16分形。 |
| y_start_pos | 输入 | 源矩阵Y轴起始分形位置。单位为32字节，对应分形的基本寻址粒度。 |
| x_step | 输入 | 源矩阵X轴方向搬运长度。单位为32字节，即1个2×16分形。取值范围：[1, 255]。 |
| y_step | 输入 | 源矩阵Y轴方向搬运长度。单位为32字节，对应分形的基本寻址粒度。取值范围：[1, 255]。 |
| src_stride | 输入 | 源矩阵X轴方向相邻分形起始地址的间隔。单位为32字节，即1个2×16分形。 |
| dst_stride | 输入 | 目的矩阵X轴方向相邻分形起始地址的间隔。单位为32字节，即1个2×16分形。 |

## 返回值说明

无

## 流水类型

PIPE_MTE1

## 约束说明

### 通用约束

- 本接口非AIC调用直接返回。
- `src`位于L1 Buffer，dst位于L0B_MX Buffer，二者位于不同的物理存储单元，不存在地址重叠。
- 如果本指令与其他指令存在目的地址重叠，需要插入同步指令（[asc_sync_notify](../sync/asc_sync_notify.md)和[asc_sync_wait](../sync/asc_sync_wait.md)），保证多个指令串行化，防止出现异常数据。
- L0B_MX Buffer容量上限：L0B_MX Buffer总容量4KB，`dst`偏移量与搬运大小之和不可越界，否则触发地址溢出异常。
- L1 Buffer容量上限：L1 Buffer总容量512KB，`src`偏移量与源矩阵占用大小之和不可越界，否则触发地址溢出异常。

### 搬运约束

- 本接口需要与对应数据搬运接口[asc_copy_l12l0b](asc_copy_l12l0b/asc_copy_l12l0b_2d_arch_3510.md)配合使用，使量化系数与右矩阵数据写入相互映射的L0B_MX Buffer和L0B Buffer地址。
- `x_step`或`y_step`配置为0时，本接口被视为NOP（空操作）。0不在有效取值范围内，不应在有效任务中配置为0，以免产生无效指令并影响性能。
- 量化系数矩阵的最小搬运粒度为1个2×16分形。矩阵边缘不足一个分形时，源矩阵需要补齐到完整分形。
- 量化系数矩阵的分形固定为2×16，对应L0B Buffer的分形为32×16，占L0B Buffer地址的1/16。dst与dst_stride需按地址映射关系设置，否则触发搬运异常。

## 调用示例

将代码保存为`examples.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[`__NPU_ARCH__`](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
Ascend 950PR/Ascend 950DT产品对应的NPU架构为`dav-3510`，编译运行命令如下：

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

样例单核占用33KB L1 Buffer、16KB L0A Buffer、16KB L0B Buffer和64KB L0C Buffer，并分别占用512字节L0A_MX Buffer和L0B_MX Buffer。样例依次复用`EVENT_ID0`完成MTE2到MTE1、MTE1到M、M到FIX的同步。

```cpp
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t M = 128;
constexpr uint32_t K = 128;
constexpr uint32_t N = 128;
constexpr uint32_t SCALE_K = K / 32;
constexpr uint32_t CUBE_M = 16;
constexpr uint32_t CUBE_K = 32;
constexpr uint8_t FP8_ONE = 0x38;
constexpr uint8_t FP8_NEG_ONE = 0xB8;

__global__ __cube__ void AscCopyL12l0bMxKernel(__gm__ uint8_t* a, __gm__ uint8_t* scale_a,
    __gm__ uint8_t* b, __gm__ uint8_t* scale_b, __gm__ float* output)
{
    asc_init();
    __cbuf__ fp8_e4m3fn_t a_l1[M * K], b_l1[N * K];
    __cbuf__ fp8_e8m0_t scale_a_l1[M * SCALE_K], scale_b_l1[N * SCALE_K];
    __ca__ fp8_e4m3fn_t a_l0[M * K];
    __cb__ fp8_e4m3fn_t b_l0[N * K];
    __cc__ float c_l0[M * N];

    // 将矩阵和量化系数从GM搬入L1 Buffer，并转换为对应的Nz格式。
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(a_l1, reinterpret_cast<__gm__ fp8_e4m3fn_t*>(a), K, 0, M, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 2, 0);
    asc_copy_gm2l1_dn2nz(reinterpret_cast<__cbuf__ half*>(scale_a_l1),
        reinterpret_cast<__gm__ half*>(scale_a), SCALE_K, 0, SCALE_K / 2, M, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(b_l1, reinterpret_cast<__gm__ fp8_e4m3fn_t*>(b), K, 0, N, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 2, 0);
    asc_copy_gm2l1_dn2nz(reinterpret_cast<__cbuf__ half*>(scale_b_l1),
        reinterpret_cast<__gm__ half*>(scale_b), SCALE_K, 0, SCALE_K / 2, N, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // 将左右矩阵及其量化系数分别搬入L0A、L0A_MX、L0B和L0B_MX Buffer。
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE_M, K / CUBE_K, M / CUBE_M, M / CUBE_M);
    asc_copy_l12l0a_mx(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(a_l0)) / 16,
        scale_a_l1, 0, 0, M / CUBE_M, SCALE_K / 2, SCALE_K / 2, SCALE_K / 2);
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / CUBE_M, K / CUBE_K, N / CUBE_M, N / CUBE_M);
    asc_copy_l12l0b_mx(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(b_l0)) / 16,
        scale_b_l1, 0, 0, N / CUBE_M, SCALE_K / 2, SCALE_K / 2, SCALE_K / 2);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    // 使用MX量化系数执行M=N=K=128的矩阵乘法。
    asc_mmad_mx(c_l0, a_l0, b_l0, M, K, N, 0, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    // 将L0C Buffer中的Nz结果转换为ND格式并搬出到GM。
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, N, M, N, M, 0, 0, 0,
        static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, true,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
    asc_sync_pipe(PIPE_FIX);
}

float DecodeScale(uint8_t value)
{
    return std::ldexp(1.0f, static_cast<int32_t>(value) - 127);
}

void PrintRow(const char* label, const std::vector<float>& data)
{
    std::cout << label << ':';
    for (uint32_t i = 0; i < 8; ++i) {
        std::cout << ' ' << data[i];
    }
    std::cout << " ..." << std::endl;
}
} // namespace

int main()
{
    // 构造FP8输入和量化系数，并在Host侧计算Golden结果。
    std::vector<uint8_t> a(M * K), b(N * K);
    std::vector<uint8_t> scale_a(M * SCALE_K, 127), scale_b(N * SCALE_K);
    std::vector<float> output(M * N), golden(M * N);
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t k = 0; k < K; ++k) {
            a[row * K + k] = ((row + 2 * k) % 5 < 2) ? FP8_ONE : FP8_NEG_ONE;
        }
    }
    for (uint32_t col = 0; col < N; ++col) {
        for (uint32_t k = 0; k < K; ++k) {
            b[col * K + k] = ((3 * col + k) % 7 < 3) ? FP8_ONE : FP8_NEG_ONE;
        }
        for (uint32_t block = 0; block < SCALE_K; ++block) {
            scale_b[col * SCALE_K + block] = 126 + (col + block) % 3;
        }
    }
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t col = 0; col < N; ++col) {
            for (uint32_t k = 0; k < K; ++k) {
                const float a_value = a[row * K + k] == FP8_ONE ? 1.0f : -1.0f;
                const float b_value = (b[col * K + k] == FP8_ONE ? 1.0f : -1.0f) *
                    DecodeScale(scale_b[col * SCALE_K + k / 32]);
                golden[row * N + col] += a_value * b_value;
            }
        }
    }

    // 分配Device内存、执行Kernel并取回结果。
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *a_device = nullptr, *b_device = nullptr;
    uint8_t *scale_a_device = nullptr, *scale_b_device = nullptr;
    float* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&scale_a_device), scale_a.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&scale_b_device), scale_b.size(), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size(), a.data(), a.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size(), b.data(), b.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(scale_a_device, scale_a.size(), scale_a.data(), scale_a.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(scale_b_device, scale_b.size(), scale_b.data(), scale_b.size(), ACL_MEMCPY_HOST_TO_DEVICE);
    AscCopyL12l0bMxKernel<<<1, 0>>>(a_device, scale_a_device, b_device, scale_b_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), output_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);

    PrintRow("Output row 0", output);
    PrintRow("Golden row 0", golden);
    bool passed = true;
    for (uint32_t i = 0; i < output.size(); ++i) {
        if (std::fabs(output[i] - golden[i]) > 1e-5f) {
            passed = false;
            break;
        }
    }
    std::cout << (passed ? "[Success] asc_copy_l12l0b_mx result is correct."
                         : "[Failed] asc_copy_l12l0b_mx result mismatch.") << std::endl;

    aclrtFree(a_device);
    aclrtFree(b_device);
    aclrtFree(scale_a_device);
    aclrtFree(scale_b_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
