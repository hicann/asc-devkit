# asc_copy_gm2l1_arch_3510（2D矩阵搬运模式）

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
<!-- npu="910" id4 -->
- Atlas 训练系列产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="310b" id7 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"c_api/cube_datamove/cube_datamove.h"`。

将数据从Global Memory搬运到L1 Buffer。以512字节的分形为单位，从GM中按行列方向的起始位置、步长切分提取多个分形矩阵搬运到L1 Buffer，搬运过程中不支持分形格式转换，用于矩阵计算所需的2D格式数据加载。

本接口仅在AIC上执行有效。

## 函数原型

### 模板原型（占位符形式）

```cpp
__aicore__ inline void asc_copy_gm2l1(__cbuf__ <dtype>* dst,
                                      __gm__ <dtype>* src,
                                      uint32_t m_start_position,
                                      uint32_t k_start_position,
                                      uint16_t dst_stride,
                                      uint16_t m_step,
                                      uint16_t k_step,
                                      uint8_t decomp_mode,
                                      uint8_t l2_cache_ctl)
__aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ <dtype>* dst,
                                           __gm__ <dtype>* src,
                                           uint32_t m_start_position,
                                           uint32_t k_start_position,
                                           uint16_t dst_stride,
                                           uint16_t m_step,
                                           uint16_t k_step,
                                           uint8_t decomp_mode,
                                           uint8_t l2_cache_ctl)
```

### dtype支持的数据类型

dtype支持的数据类型为`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`、`void`。

### 函数原型典型示例

```cpp
// 示例：bfloat16_t类型的2D矩阵搬运。
__aicore__ inline void asc_copy_gm2l1(__cbuf__ bfloat16_t* dst,
                                      __gm__ bfloat16_t* src,
                                      uint32_t m_start_position,
                                      uint32_t k_start_position,
                                      uint16_t dst_stride,
                                      uint16_t m_step,
                                      uint16_t k_step,
                                      uint8_t decomp_mode,
                                      uint8_t l2_cache_ctl)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数，存储位置为L1 Buffer。目的地址需32字节对齐。 |
| src | 输入 | 源操作数，存储位置为GM。源地址需1字节对齐。 |
| m_start_position | 输入 | 源矩阵中首个分形在行方向的起始位置，单位为16个元素。取值范围：$[0, 2^{32}-1]$。 |
| k_start_position | 输入 | 源矩阵中首个分形在列方向的起始位置，单位32字节。取值范围：$[0, 2^{32}-1]$。 |
| dst_stride | 输入 | 目的矩阵列方向前一个分形起始地址与后一个分形起始地址的间隔，单位512字节。取值范围：[0, 4095]。 |
| m_step | 输入 | 源矩阵行方向分形步长，即搬运的分形行数，单位为16个元素。取值范围：[1, 4095]。 |
| k_step | 输入 | 源矩阵列方向分形步长，即搬运的分形列数，单位32字节。取值范围：[1, 4095]。 |
| decomp_mode | 输入 | 预留参数，当前须设置为0。 |
| l2_cache_ctl | 输入 | 配置数据在L2 Cache中的管理策略。取值说明请参见[表2](#table2)。 |

**表2** l2_cache_ctl取值说明 <a id="table2"></a>

| 取值 | 模式 | 含义 |
|------|------|------|
| 0 | Normal模式 | 启用L2 Cache，并将分配的Cache Line标记为高替换优先级。 |
| 1 | Last模式 | &bull; 启用L2 Cache，并将分配的Cache Line标记为低替换优先级。<br>&bull; **Last模式暂不支持。**|
| 2 | Persistent模式 | &bull; 启用L2 Cache。已存入L2 Cache中的数据可能被替换，若需确保特定GM的数据始终保留在L2 Cache中，可采用驻留模式。<br>&bull; 被标记为驻留模式的Cache Line只能被其他同样标记为驻留模式的Cache Line替换。<br>&bull; **Persistent模式暂不支持。**|
| 4 | Disable模式 | 不启用L2 Cache，每次都直接从GM中读取，并保持已有Cache Line的状态不变。 |

## 返回值说明

无

## 流水类型

PIPE_MTE2

## 约束说明

### 通用约束

- 本接口非AIC调用直接返回。
- dst起始地址需32字节对齐（L1 Buffer对齐要求），否则会导致搬运异常。
- src起始地址需1字节对齐（Global Memory对齐要求），否则会导致搬运异常。
- 如果本指令与其他指令存在目的地址重叠，需要插入同步指令（[asc_sync_notify](../../sync/asc_sync_notify.md)和[asc_sync_wait](../../sync/asc_sync_wait.md)），保证多个指令串行化，防止出现异常数据。
- L1 Buffer容量上限：L1 Buffer总容量512KB，dst偏移量与搬运大小之和不可越界，否则触发异常。

### 2D矩阵搬运模式约束

- m_step或k_step为0时不执行搬运，本接口被视为NOP（空操作）。
- 接口以512字节的完整分形为最小搬运粒度，不支持仅搬运分形内的部分元素，且不支持填充功能。
- dst_stride取值范围为[0, 4095]，超出取值范围的值会被截断，导致搬运结果不符合预期。
- m_step取值范围为[1, 4095]，超出取值范围的值会被截断，导致搬运结果不符合预期。
- k_step取值范围为[1, 4095]，超出取值范围的值会被截断，导致搬运结果不符合预期。

## 调用示例

将代码保存为`examples.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

样例中A、B、C矩阵的shape均为[128,128]，数据格式均为Nz。A和B在GM中以Nz格式存储，分别按8个分形列循环调用本接口的2D矩阵搬运重载搬运到L1 Buffer，每次搬运一个分形列。随后A经[asc_copy_l12l0a](../asc_copy_l12l0a/asc_copy_l12l0a_2d_arch_3510.md)搬运到L0A Buffer，B经[asc_copy_l12l0b_transpose](../asc_copy_l12l0b/asc_copy_l12l0b_2d_arch_3510.md)搬运到L0B Buffer，调用[asc_mmad](../../cube_compute/asc_mmad.md)完成计算。C从L0C Buffer搬出到GM时关闭Nz2ND和Nz2DN，保持Nz格式。样例单核占用64KB L1 Buffer、32KB L0A Buffer、32KB L0B Buffer和64KB L0C Buffer。样例依次复用`EVENT_ID0`完成MTE2到MTE1、MTE1到M、M到FIX的同步。

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t M = 128;
constexpr uint32_t K = 128;
constexpr uint32_t N = 128;
constexpr uint32_t ELEMENTS = M * N;
constexpr uint32_t CUBE = 16;
constexpr uint16_t HALF_ONE = 0x3c00;
constexpr uint16_t HALF_NEG_ONE = 0xbc00;
constexpr uint16_t M_FRACTALS = M / CUBE;
constexpr uint16_t K_FRACTALS = K / CUBE;
constexpr uint16_t N_FRACTALS = N / CUBE;

__global__ __cube__ void AscCopyGm2l1TwoDimNzKernel(
    __gm__ half* a, __gm__ half* b, __gm__ float* output)
{
    asc_init();
    __cbuf__ half a_l1[M * K], b_l1[K * N];
    __ca__ half a_l0[M * K];
    __cb__ half b_l0[K * N];
    __cc__ float c_l0[ELEMENTS];

    // A、B在GM中已按Nz格式排布，2D搬运过程中不进行格式转换。
    for (uint16_t k_block = 0; k_block < K_FRACTALS; ++k_block) {
        const uint32_t offset = k_block * M * CUBE;
        asc_copy_gm2l1(a_l1 + offset, a + offset, 0, 0, M_FRACTALS, M_FRACTALS, 1, 0, 0);
    }
    for (uint16_t n_block = 0; n_block < N_FRACTALS; ++n_block) {
        const uint32_t offset = n_block * K * CUBE;
        asc_copy_gm2l1(b_l1 + offset, b + offset, 0, 0, K_FRACTALS, K_FRACTALS, 1, 0, 0);
    }
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    // A从Nz转换为L0A Buffer所需Zz排布，B从Nz转换为L0B Buffer所需Zn排布。
    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M_FRACTALS, K_FRACTALS, M_FRACTALS, M_FRACTALS);
    asc_copy_l12l0b_transpose(
        b_l0, b_l1, 0, 0, K_FRACTALS, N_FRACTALS, K_FRACTALS, N_FRACTALS);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    asc_mmad(c_l0, a_l0, b_l0, M, K, N, 0, true, false, true);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    // 关闭Nz2ND和Nz2DN，保持L0C Buffer中的Nz排布直接搬出至GM。
    asc_copy_l0c2gm(output, c_l0, N, M, M * CUBE, M, 0, 0, 0,
        static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, false,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
    asc_sync_pipe(PIPE_ALL);
}

uint16_t ToHalfBits(int32_t value)
{
    return value > 0 ? HALF_ONE : (value < 0 ? HALF_NEG_ONE : 0);
}

size_t NzIndex(uint32_t row, uint32_t col, uint32_t rows)
{
    return static_cast<size_t>(col / CUBE) * rows * CUBE + row * CUBE + col % CUBE;
}

void PrintNzBlock(const char* label, const std::vector<float>& data)
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
    std::vector<uint16_t> a(ELEMENTS), b(ELEMENTS);
    std::vector<float> output(ELEMENTS), golden(ELEMENTS);
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t k = 0; k < K; ++k) {
            const int32_t value = static_cast<int32_t>((row + 2 * k) % 3) - 1;
            a[NzIndex(row, k, M)] = ToHalfBits(value);
        }
    }
    for (uint32_t k = 0; k < K; ++k) {
        for (uint32_t col = 0; col < N; ++col) {
            const int32_t value = static_cast<int32_t>((2 * k + col) % 3) - 1;
            b[NzIndex(k, col, K)] = ToHalfBits(value);
        }
    }
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t col = 0; col < N; ++col) {
            int32_t sum = 0;
            for (uint32_t k = 0; k < K; ++k) {
                const int32_t a_value = static_cast<int32_t>((row + 2 * k) % 3) - 1;
                const int32_t b_value = static_cast<int32_t>((2 * k + col) % 3) - 1;
                sum += a_value * b_value;
            }
            golden[NzIndex(row, col, M)] = static_cast<float>(sum);
        }
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    half *a_device = nullptr, *b_device = nullptr;
    float* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size() * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size() * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size() * sizeof(uint16_t), a.data(), a.size() * sizeof(uint16_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size() * sizeof(uint16_t), b.data(), b.size() * sizeof(uint16_t),
        ACL_MEMCPY_HOST_TO_DEVICE);

    AscCopyGm2l1TwoDimNzKernel<<<1, 0>>>(a_device, b_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), output_device,
        output.size() * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);

    PrintNzBlock("Output Nz first block", output);
    PrintNzBlock("Golden Nz first block", golden);
    bool passed = true;
    for (size_t i = 0; i < output.size(); ++i) {
        if (output[i] != golden[i]) {
            std::cerr << "Mismatch at Nz offset " << i << ": output=" << output[i]
                      << ", golden=" << golden[i] << std::endl;
            passed = false;
            break;
        }
    }

    aclrtFree(output_device);
    aclrtFree(b_device);
    aclrtFree(a_device);
    aclrtResetDevice(0);
    aclFinalize();
    if (!passed) {
        return 1;
    }
    std::cout << "[Success] asc_copy_gm2l1 2D Nz matrix multiplication passed." << std::endl;
    return 0;
}
```
