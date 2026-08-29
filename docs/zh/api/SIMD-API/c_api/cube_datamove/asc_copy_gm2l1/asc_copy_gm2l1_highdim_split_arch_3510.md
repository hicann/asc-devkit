# asc_copy_gm2l1_arch_3510（高维切分数据搬运）

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

将数据从Global Memory搬运到L1 Buffer，数据搬运时格式和内容保持不变。高维切分是指能够通过配置数据块个数、单个数据块长度、地址偏移等搬运参数实现非连续搬运。并且此接口支持在搬运过程中按通道padding模式对32字节粒度的数据插入或移除padding。其中padding值需要通过[asc_set_gm2l1_pad](../asc_set_gm2l1_pad.md)设置。

本接口仅在AIC上执行有效。

## 函数原型

### 高维切分数据搬运

```cpp
__aicore__ inline void asc_copy_gm2l1(__cbuf__ void* dst, __gm__ void* src, uint32_t n_burst,
                                      uint32_t len_burst, uint8_t pad_func_mode, uint64_t src_stride,
                                      uint32_t dst_stride)
__aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t n_burst,
                                           uint32_t len_burst, uint8_t pad_func_mode, uint64_t src_stride,
                                           uint32_t dst_stride)
```

## 参数说明

### 高维切分数据搬运

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数，存储位置为L1 Buffer。目的地址需32字节对齐。 |
| src | 输入 | 源操作数，存储位置为GM。起始地址需要按照1字节对齐。 |
| n_burst | 输入 | 搬运的数据块的个数。取值范围：[1, $2^{17}-1$]。 |
| len_burst | 输入 | 搬运的每个数据块长度，单位32字节。取值范围：[1, $2^{17}-1$]。 |
| pad_func_mode | 输入 | 4 bit值，指示通道上的padding功能，在32字节数据中插入padding值，或从每个32字节的读取数据中移除padding值。取值范围 [0, 8]：<br>&nbsp;&nbsp;&bull; `0`：不做padding；<br>&nbsp;&nbsp;&bull; `1`：每有1个字节的数据，插入31个padding值；<br>&nbsp;&nbsp;&bull; `2`：每有2个字节的数据，插入15个padding值；<br>&nbsp;&nbsp;&bull; `3`：每有4个字节的数据，插入14个padding值；<br>&nbsp;&nbsp;&bull; `4`：每有8个字节的数据，插入12个padding值；<br>&nbsp;&nbsp;&bull; `5`：每有16个字节的数据，插入8个padding值；<br>&nbsp;&nbsp;&bull; `6`：每有32个字节的数据，移除28个最高有效位数据，只保留4个最低有效位元素；<br>&nbsp;&nbsp;&bull; `7`：每有32个字节的数据，移除24个最高有效位数据，只保留8个最低有效位元素；<br>&nbsp;&nbsp;&bull; `8`：每有32个字节的数据，移除16个最高有效位数据，只保留16个最低有效位元素。 |
| src_stride | 输入 | 源操作数相邻数据块之间的间隔（即前一个数据块起始地址与后一个数据块起始地址的差值），取值范围：[0, $2^{36}-1$]，单位32字节。<br>&nbsp;&nbsp;&bull; n_burst = 1时，`src_stride`无意义，设置为0即可。 |
| dst_stride | 输入 | 目的操作数相邻数据块之间的间隔（即前一个数据块起始地址与后一个数据块起始地址的差值），取值范围：[0, $2^{17}-1$]，单位32字节。<br>&nbsp;&nbsp;&bull; n_burst = 1时，`dst_stride`无意义，设置为0即可。 |

## 返回值说明

无

## 流水类型

PIPE_MTE2

## 约束说明

### 通用约束

- 本接口仅在AIC上生效，非AIC调用直接返回。
- `dst`起始地址需32字节对齐（L1 Buffer对齐要求），否则会导致搬运异常。
- `src`起始地址需1字节对齐（Global Memory对齐要求），否则会导致搬运异常。
- 如果本指令与其他指令存在目的地址重叠，需要插入同步指令（[asc_sync_notify](../../sync/asc_sync_notify.md)和[asc_sync_wait](../../sync/asc_sync_wait.md)），保证多个指令串行化，防止出现异常数据。
- L1 Buffer容量上限：L1 Buffer总容量512KB，dst偏移量与搬运大小之和不可越界，否则触发异常。

### 高维切分数据搬运约束

- `n_burst`取值范围为[1, $2^{17}-1$]，超出取值范围的值会被截断，导致搬运结果不符合预期。
- `len_burst`取值范围为[1, $2^{17}-1$]，超出取值范围的值会被截断，导致搬运结果不符合预期。
- `src_stride`取值范围为[0, $2^{36}-1$]，超出取值范围的值会被截断，导致搬运结果不符合预期。
- `dst_stride`取值范围为[0, $2^{17}-1$]，超出取值范围的值会被截断，导致搬运结果不符合预期。
- `pad_func_mode`取值范围为[0, 8]，未定义值会导致搬运结果异常，`pad_func_mode`中使用的padding值需要通过[asc_set_gm2l1_pad](../asc_set_gm2l1_pad.md)设置。
- 对于插入padding值的场景：
  - `len_burst`的单位为32个字节的指的是写入L1 Buffer的实际数据数，它必须设置为`1`，此时一次仅从GM里读取1/2/4/8/16个字节的数据。
  - 每一次burst都是从GM中连续读取，`src_stride`设置为`0`即可。
- 对于移除padding值的场景：
  - 源操作数中一次burst操作的数据为`len_burst * 32`个字节，目的操作数中一次burst的实际数据为`len_burst * 4/8/16`个字节。
  - 每一次`burst`后都连续地往L1 Buffer中写入，`dst_stride`设置为`0`即可。

## 调用示例

将代码保存为`examples.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[`__NPU_ARCH__`](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng examples.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

样例中A、B、C矩阵的shape均为[128,128]，Bias矩阵的shape为[1,128]。Bias通过高维切分搬运接口按两个256字节块从GM搬运到L1 Buffer，再调用[asc_copy_l12bt](../asc_copy_l12bt/asc_copy_l12bt_arch_3510.md)搬运到BiasTable Buffer，最后由MMAD完成`A × B + Bias`计算。样例单核占用64.5KB L1 Buffer、32KB L0A Buffer、32KB L0B Buffer、64KB L0C Buffer和512字节BiasTable Buffer。样例依次复用`EVENT_ID0`完成MTE2到MTE1、MTE1到M、M到FIX的同步。

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
constexpr uint32_t A_ELEMENTS = M * K;
constexpr uint32_t B_ELEMENTS = N * K;
constexpr uint32_t C_ELEMENTS = M * N;
constexpr uint32_t BIAS_ELEMENTS = N;
constexpr uint32_t CUBE = 16;
constexpr uint16_t HALF_ONE = 0x3c00;
constexpr uint16_t HALF_NEG_ONE = 0xbc00;
constexpr uint32_t BIAS_BLOCK_BYTES = 256;
constexpr uint32_t BIAS_BLOCKS = 2;
constexpr uint32_t BIAS_BLOCK_LEN = BIAS_BLOCK_BYTES / 32;

__global__ __cube__ void AscCopyGm2l1HighdimBiasKernel(
    __gm__ uint16_t* a, __gm__ uint16_t* b, __gm__ float* bias, __gm__ float* output)
{
    asc_init();
    __cbuf__ half a_l1[A_ELEMENTS], b_l1[B_ELEMENTS];
    __cbuf__ float bias_l1[BIAS_ELEMENTS];
    __ca__ half a_l0[A_ELEMENTS];
    __cb__ half b_l0[B_ELEMENTS];
    __cc__ float c_l0[C_ELEMENTS];

    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(a_l1, reinterpret_cast<__gm__ half*>(a), K * sizeof(half), 0, M, K, 0, false);
    asc_set_gm2l1_nz_para(1, 1, 128, 0);
    asc_copy_gm2l1_nd2nz(b_l1, reinterpret_cast<__gm__ half*>(b), K * sizeof(half), 0, N, K, 0, false);

    // Bias的1 x 128数据被切分为两个256字节块，由GM搬运至L1 Buffer, 也可以1次连续搬运512字节块。
    asc_copy_gm2l1(reinterpret_cast<__cbuf__ void*>(bias_l1), reinterpret_cast<__gm__ void*>(bias), BIAS_BLOCKS,
        BIAS_BLOCK_LEN, 0, BIAS_BLOCK_LEN, BIAS_BLOCK_LEN);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    asc_copy_l12l0a(a_l0, a_l1, 0, 0, M / CUBE, K / CUBE, M / CUBE, M / CUBE);
    asc_copy_l12l0b(b_l0, b_l1, 0, 0, N / CUBE, K / CUBE, N / CUBE, N / CUBE);
    asc_copy_l12bt(0, bias_l1, BIAS_ELEMENTS * sizeof(float));
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);

    // 从BiasTable Buffer读取Bias初始化C矩阵，再执行A x B + Bias计算。
    asc_mmad(c_l0, a_l0, b_l0, M, K, N, 0, false, true, false);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_set_l0c_copy_nz_para(1, 0, 0);
    asc_copy_l0c2gm(output, c_l0, N, M, N, M, 0, 0, 0,
        static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, true,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
    asc_sync_pipe(PIPE_ALL);
}

uint16_t ToHalfBits(int32_t value)
{
    return value > 0 ? HALF_ONE : (value < 0 ? HALF_NEG_ONE : 0);
}

void PrintRow(const char* label, const std::vector<float>& data)
{
    std::cout << label << ':';
    for (uint32_t i = 0; i < 8; ++i) std::cout << ' ' << data[i];
    std::cout << " ..." << std::endl;
}
} // namespace

int main()
{
    std::vector<uint16_t> a(A_ELEMENTS), b(B_ELEMENTS);
    std::vector<float> bias(BIAS_ELEMENTS), output(C_ELEMENTS), golden(C_ELEMENTS);
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t k = 0; k < K; ++k) {
            const int32_t value = static_cast<int32_t>((row + 2 * k) % 3) - 1;
            a[row * K + k] = ToHalfBits(value);
        }
    }
    for (uint32_t col = 0; col < N; ++col) {
        for (uint32_t k = 0; k < K; ++k) {
            const int32_t value = static_cast<int32_t>((2 * k + col) % 3) - 1;
            b[col * K + k] = ToHalfBits(value);
        }
        bias[col] = static_cast<float>(static_cast<int32_t>(col % 5) - 2);
    }
    for (uint32_t row = 0; row < M; ++row) {
        for (uint32_t col = 0; col < N; ++col) {
            int32_t sum = 0;
            for (uint32_t k = 0; k < K; ++k) {
                const int32_t a_value = static_cast<int32_t>((row + 2 * k) % 3) - 1;
                const int32_t b_value = static_cast<int32_t>((2 * k + col) % 3) - 1;
                sum += a_value * b_value;
            }
            golden[row * N + col] = static_cast<float>(sum) + bias[col];
        }
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t *a_device = nullptr, *b_device = nullptr;
    float *bias_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&a_device), a.size() * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&b_device), b.size() * sizeof(uint16_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&bias_device), bias.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), output.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(a_device, a.size() * sizeof(uint16_t), a.data(), a.size() * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(b_device, b.size() * sizeof(uint16_t), b.data(), b.size() * sizeof(uint16_t), ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(bias_device, bias.size() * sizeof(float), bias.data(), bias.size() * sizeof(float), ACL_MEMCPY_HOST_TO_DEVICE);
    AscCopyGm2l1HighdimBiasKernel<<<1, 0>>>(a_device, b_device, bias_device, output_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), output_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);

    PrintRow("Bias", bias);
    PrintRow("Output row 0", output);
    PrintRow("Golden row 0", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_copy_gm2l1 high-dimensional bias copy passed."
                         : "[Failed] asc_copy_gm2l1 high-dimensional bias copy mismatch.") << std::endl;

    aclrtFree(a_device);
    aclrtFree(b_device);
    aclrtFree(bias_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
