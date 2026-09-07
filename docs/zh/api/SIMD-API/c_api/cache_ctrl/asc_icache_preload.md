# asc_icache_preload

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

头文件路径为：`"c_api/cache_ctrl/cache_ctrl.h"`。

开发者手动调用本接口，从调用本接口所在的Global Memory地址预加载指令到ICache（指令缓存）中，避免后续取指时出现ICache未命中。ICache用于缓存Scalar单元最近使用或频繁使用的指令，通过本接口预取后，Scalar单元读取GM指令时优先从ICache中读取。

预加载地址通常通过[asc_get_program_counter](../utils/sys_var/asc_get_program_counter.md)接口获取。

## 函数原型

```c
__aicore__ inline void asc_icache_preload(const void* addr, int64_t prefetch_len)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| addr | 输入 | 预取指令的Global Memory起始地址。 |
| prefetch_len | 输入 | 预加载数据的长度，单位为2K Byte。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- `addr`预取地址硬件按128B对齐取有效位，调用方传入非128B对齐地址不会报错，但实际预取起始Cache Line由对齐结果决定。
- `prefetch_len`的取值范围为[0, 15]（AIC，对应32KB ICache）或[0, 7]（AIV，对应16KB ICache）。
- 调用本接口时的预加载地址需通过[asc_get_program_counter](../utils/sys_var/asc_get_program_counter.md)接口获取当前程序计数器（PC）值，获取后必须通过掩码`0xFFFFFFFFFFFF`（低48位全1）将高位无效位清零，以得到当前指令在GM中的有效48位地址。这是预加载地址的固定获取流程，示例代码如下：

    ```cpp
    int64_t prefetch_length = 6;
    // 通过asc_get_program_counter获取当前程序计数器（PC）值。
    // PC为64位返回值，有效地址位为0~47位，通过掩码0xFFFFFFFFFFFF（低48位全1）将高位无效位清零，
    // 得到当前指令在GM中的有效48位地址，作为预加载的起始地址。
    int64_t pc = asc_get_program_counter() & 0xFFFFFFFFFFFF;
    asc_icache_preload(reinterpret_cast<void *>(pc), prefetch_length);
    ```

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
// 本示例仅用于演示asc_icache_preload接口功能，不代表实际算子场景。
// 本示例展示在`__mix__(1, 2)`修饰的融合算子中，AIC完成矩阵乘法并将float结果写入GM后，两个AIV分别读取部分结果，
// 将结果乘以常数后写入输出GM的场景。当AIV中ICache miss性能时，在AIV等待AIC期间通过asc_icache_preload接口
// 预取指令，可以减少AIV中ICache miss，进而提升性能。
#include <cstdint>
#include <iostream>
#include <vector>

#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t MATRIX_DIM = 16;
constexpr uint32_t MATRIX_ELEMENTS = MATRIX_DIM * MATRIX_DIM;
constexpr uint32_t ELEMENTS_PER_AIV = MATRIX_ELEMENTS / 2;
constexpr uint32_t INPUT_BYTES = MATRIX_ELEMENTS * sizeof(half);
constexpr uint32_t FP32_RESULT_BYTES = MATRIX_ELEMENTS * sizeof(float);
constexpr uint32_t FP32_BYTES_PER_AIV = ELEMENTS_PER_AIV * sizeof(float);
constexpr uint32_t ELEMENTS_PER_VF = 64;
constexpr uint32_t AIV_REPEAT_TIMES = ELEMENTS_PER_AIV / ELEMENTS_PER_VF;
constexpr uint32_t DATA_BLOCK_BYTES = 32;
constexpr int64_t OUTPUT_READY_FLAG_ID = 0;
constexpr float OUTPUT_SCALE = 2.0F;

// 需要根据AIV中的指令总量、ICache容量等多方面因素选取合适的预取长度（单位为2KB）。
constexpr int64_t PREFETCH_LENGTH = 2;
constexpr int64_t PC_ADDRESS_MASK = 0xFFFFFFFFFFFF;

__simd_vf__ inline void ScaleResult(__ubuf__ float* output, __ubuf__ float* input)
{
    for (uint32_t i = 0; i < AIV_REPEAT_TIMES; ++i) {
        vector_float input_reg;
        vector_float output_reg;
        uint32_t count = ELEMENTS_PER_VF;
        vector_bool mask = asc_update_mask_b32(count);
        asc_loadalign(input_reg, input + i * ELEMENTS_PER_VF);
        asc_mul_scalar(output_reg, input_reg, OUTPUT_SCALE, mask);
        asc_storealign(output + i * ELEMENTS_PER_VF, output_reg, mask);
    }
}

__global__ __mix__(1, 2) void MatmulScaleKernel(
    __gm__ half* input, __gm__ float* matmul_result, __gm__ float* output, __gm__ int64_t* preload_status)
{
    asc_init();
    __cbuf__ half matrix_a_l1[MATRIX_ELEMENTS];
    __cbuf__ half matrix_b_l1[MATRIX_ELEMENTS];
    __ca__ half matrix_a_l0[MATRIX_ELEMENTS];
    __cb__ half matrix_b_l0[MATRIX_ELEMENTS];
    __cc__ float matmul_result_l0[MATRIX_ELEMENTS];

    if ASC_IS_AIC {
        // AIC直接将两个输入矩阵从GM搬入L1 Buffer。
        asc_copy_gm2l1(matrix_a_l1, input, 1, INPUT_BYTES / DATA_BLOCK_BYTES, 0, 0, 0);
        asc_copy_gm2l1(matrix_b_l1, input, 1, INPUT_BYTES / DATA_BLOCK_BYTES, 0, 0, 0);
        asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
        asc_copy_l12l0a(matrix_a_l0, matrix_a_l1, 0, 0, 1, 1, 1, 1);
        asc_copy_l12l0b_transpose(matrix_b_l0, matrix_b_l1, 0, 0, 1, 1, 1, 1);
        asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
        asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
        asc_mmad(
            matmul_result_l0, matrix_a_l0, matrix_b_l0, MATRIX_DIM, MATRIX_DIM, MATRIX_DIM, 0, false, false, true);
        asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
        asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
        asc_set_l0c2gm_nz2nd(1, 0, 0);
        asc_copy_l0c2gm(matmul_result, matmul_result_l0, MATRIX_DIM, MATRIX_DIM, MATRIX_DIM, MATRIX_DIM, 0, 0, 0,
            static_cast<uint64_t>(QuantMode_t::NoQuant), 0, false, true,
            static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, 0, false, false, false, false);
        asc_sync_block_arrive(PIPE_FIX, OUTPUT_READY_FLAG_ID);
    }

    if ASC_IS_AIV {
        // AIV即将等待AIC写出矩阵乘法结果，在等待前预取后续缩放计算所需的指令。
        const int64_t pc = asc_get_program_counter() & PC_ADDRESS_MASK;
        asc_icache_preload(reinterpret_cast<void*>(pc), PREFETCH_LENGTH);
        // 预取状态仅用于调试，查询结果为0表示完成，为1表示仍在进行。
        asc_store_dev(preload_status + asc_get_sub_block_id(), asc_get_icache_preload_status());

        // 等待AIC完成矩阵乘法结果的写出。
        asc_sync_block_wait(PIPE_MTE2, OUTPUT_READY_FLAG_ID);

        const uint32_t aiv_index = asc_get_sub_block_id();
        __ubuf__ float result_ub[ELEMENTS_PER_AIV];
        __ubuf__ float output_ub[ELEMENTS_PER_AIV];
        asc_copy_gm2ub_align(result_ub, matmul_result + aiv_index * ELEMENTS_PER_AIV, FP32_BYTES_PER_AIV);
        asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
        asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
        ScaleResult(output_ub, result_ub);
        asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
        asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
        asc_copy_ub2gm_align(output + aiv_index * ELEMENTS_PER_AIV, output_ub, FP32_BYTES_PER_AIV);
    }

    asc_sync_pipe(PIPE_ALL);
}

bool CheckResult(const std::vector<float>& matmul_result, const std::vector<float>& output)
{
    for (uint32_t i = 0; i < MATRIX_ELEMENTS; ++i) {
        if (matmul_result[i] != static_cast<float>(MATRIX_DIM) ||
            output[i] != static_cast<float>(MATRIX_DIM) * OUTPUT_SCALE) {
            return false;
        }
    }
    return true;
}
} // namespace

int main()
{
    std::vector<half> input(MATRIX_ELEMENTS, static_cast<half>(1.0F));
    std::vector<float> matmul_result(MATRIX_ELEMENTS, 0.0F);
    std::vector<float> output(MATRIX_ELEMENTS, 0.0F);
    half* input_device = nullptr;
    float* matmul_result_device = nullptr;
    float* output_device = nullptr;
    int64_t* preload_status_device = nullptr;
    std::vector<int64_t> preload_status(2, -1);

    aclInit(nullptr);
    aclrtSetDevice(0);
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&matmul_result_device), FP32_RESULT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), FP32_RESULT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&preload_status_device), preload_status.size() * sizeof(int64_t), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    MatmulScaleKernel<<<1, 0>>>(input_device, matmul_result_device, output_device, preload_status_device);
    const aclError ret = aclrtSynchronizeDevice();
    aclrtMemcpy(matmul_result.data(), FP32_RESULT_BYTES, matmul_result_device, FP32_RESULT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(output.data(), FP32_RESULT_BYTES, output_device, FP32_RESULT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(preload_status.data(), preload_status.size() * sizeof(int64_t), preload_status_device,
        preload_status.size() * sizeof(int64_t), ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = ret == ACL_SUCCESS && CheckResult(matmul_result, output);
    std::cout << "ICache preload status: " << preload_status[0] << ", " << preload_status[1] << std::endl;
    std::cout << (passed ? "[Success] asc_icache_preload passed." : "[Failed] asc_icache_preload failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(matmul_result_device);
    aclrtFree(output_device);
    aclrtFree(preload_status_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
