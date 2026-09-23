# asc_mrgsort4

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

本接口实现将2、3或4条已经按score从大到小排列的队列合并为一条有序队列，保持按照score从大到小排序。

该接口不负责将无序的输入队列排序，输入一般是经过[asc_bitsort](asc_bitsort.md)指令处理后的输出数据，队列详细结构请参考[asc_bitsort功能说明](asc_bitsort.md#功能说明)中的介绍。

## 函数原型

```cpp
// 占位符形式
__aicore__ inline void asc_mrgsort4(__ubuf__ <dtype>* dst,
                                    __ubuf__ <dtype>* src[ASC_C_API_MRGSORT_ELEMENT_LEN],
                                    uint8_t repeat,
                                    uint16_t element_length_0,
                                    uint16_t element_length_1,
                                    uint16_t element_length_2,
                                    uint16_t element_length_3,
                                    bool if_exhausted_suspension,
                                    uint8_t valid_bit)
```

### dtype支持数据类型

`dtype`支持的数据类型为`half`、`float`，源操作数与目的操作数的指针类型须一致。

### 函数原型典型示例

```cpp
__aicore__ inline void asc_mrgsort4(__ubuf__ float* dst,
                                    __ubuf__ float* src[ASC_C_API_MRGSORT_ELEMENT_LEN],
                                    uint8_t repeat,
                                    uint16_t element_length_0,
                                    uint16_t element_length_1,
                                    uint16_t element_length_2,
                                    uint16_t element_length_3,
                                    bool if_exhausted_suspension,
                                    uint8_t valid_bit)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | UB中的目的队列起始地址，要求32字节对齐。 |
| src | 输入 | 包含4条源队列起始地址的指针数组，`ASC_C_API_MRGSORT_ELEMENT_LEN`为`4`。每个队列起始地址要求8字节对齐，数组下标0～3分别对应4条队列。4个数组项均须初始化，无效队列可复用合法UB地址。 |
| repeat | 输入 | 迭代次数，执行归并时取值为[1, 255]。取值大于1时须满足[约束说明](#约束说明)中的多次迭代约束。多次迭代产生多条分别有序的结果队列，不会自动将它们合为一条。 |
| element_length_0 | 输入 | 起始地址为`src[0]`的队列的长度，即队列中数据单元的个数，每个数据单元占8字节，因此单位为8字节。<br>对于有效队列，取值范围为[1, 4095]；对于无效队列，取值为0。 |
| element_length_1 | 输入 | 起始地址为`src[1]`的队列的长度，单位及范围同`element_length_0`。 |
| element_length_2 | 输入 | 起始地址为`src[2]`的队列的长度，单位及范围同`element_length_0`。 |
| element_length_3 | 输入 | 起始地址为`src[3]`的队列的长度，单位及范围同`element_length_0`。 |
| if_exhausted_suspension | 输入 | 某个队列耗尽时，控制指令是否停止执行，类型为bool，取值说明如下：<br>&nbsp;&nbsp;&bull; `false`：归并排序过程中某条有效队列耗尽后继续处理其余队列，输出所有有效队列的值，单次迭代的输出长度为有效队列长度之和。<br>&nbsp;&nbsp;&bull; `true`：归并排序过程中任意一有效队列耗尽时停止排序，只输出已经完成排序的有序前缀，不能将有效队列长度之和作为单次迭代的实际输出长度。 |
| valid_bit | 输入 | 有效队列位掩码，有效取值为`3`、`7`、`15`，含义如下：<br>&nbsp;&nbsp;&bull; `3`（`0b0011`）：有效队列为起始地址为`src[0]`和`src[1]`的队列。<br>&nbsp;&nbsp;&bull; `7`（`0b0111`）：有效队列为起始地址为`src[0]`、`src[1]`和`src[2]`的队列。<br>&nbsp;&nbsp;&bull; `15`（`0b1111`）：全部4条队列均有效。 |

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- 不支持源操作数和目的操作数地址重叠。
- dst的起始地址要求32字节对齐，src数组中的起始地址要求8字节对齐。
- 如果存在score\[i\]==score\[j\]并且i\>j，将优先选取score\[j\]排在前面。
- 每次迭代内的数据会进行排序，不同迭代间的数据不会进行排序。
- src数组的4个元素均须初始化，即使对应队列未被`valid_bit`选中。
- 当`repeat = 1`时，有效源队列可以长度不同、存储不连续。当`repeat > 1`时，必须同时满足：
    - 4条源队列长度一致，均为`L`。
    - 每组4条队列按`src[0]`至`src[3]`顺序连续存储，各组也连续存储。
    - `if_exhausted_suspension = false`。
    - `valid_bit = 15`。

  第`r`次迭代（从0开始）的源、目的地址相对于初始地址均偏移`r * 4 * L * 8`字节，每次产生一条包含`4 * L`个数据单元的有序队列。
<!-- npu="A3,910b,950" id12 -->
- 当参数`repeat = 0`时，该接口的行为如下：
    <!-- npu="A3,910b" id10 -->
    - 针对如下型号，不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。
       <!-- npu="A3" id13 -->
       - Atlas A3系列产品
       <!-- end id13 -->
       <!-- npu="910b" id9 -->
       - Atlas A2系列产品
       <!-- end id9 -->
    <!-- end id10 -->
    <!-- npu="950" id11 -->
    - 针对Ascend 950PR&950DT系列产品，不保证该接口被视为NOP（空操作）。
    <!-- end id11 -->
<!-- end id12 -->

<!-- npu="950" id8 -->
## 调用示例

以下示例将256个乱序float类型score及其index分别作为输入，通过[asc_bitsort](asc_bitsort.md)与`asc_mrgsort4`配合完成全局降序排序，每个输出数据单元包含一个score及其对应的index，占8字节，总体计算流程为：

1. 调用`asc_bitsort`，以`repeat = 8`对每组32个元素分别排序，生成8条有序队列。此时各队列内部有序，全部数据尚未全局有序。
2. 调用`asc_mrgsort4`，以`repeat = 2`分别归并两组各4条队列，生成两条各包含128个数据单元的有序队列。
3. 再次调用`asc_mrgsort4`，以`repeat = 1`、`valid_bit = 0b0011`归并上述两条队列，得到包含256个数据单元的全局有序结果。

Host端使用CPU排序结果，逐一核对全部256个score及其对应的index。

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

以Ascend 950PR&950DT系列产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

```cpp
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "c_api/asc_simd.h"
#include "acl/acl.h"

#define ACL_CHECK(call) do { \
    aclError error = (call); \
    if (error != ACL_SUCCESS) { \
        std::cerr << #call << " failed: " << error << std::endl; \
        std::exit(1); \
    } \
} while (0)

constexpr uint16_t QUEUE_LENGTH = 32;
constexpr uint32_t QUEUE_COUNT = 8;
constexpr uint32_t RECORD_COUNT = QUEUE_LENGTH * QUEUE_COUNT;
// index存储uint32_t位模式，不是将索引数值转换成float。
struct Record {
    float score;
    uint32_t index;
};
static_assert(sizeof(Record) == 8, "Each record must occupy 8 bytes.");
constexpr uint32_t WORD_COUNT = RECORD_COUNT * 2;
constexpr uint32_t BYTE_COUNT = RECORD_COUNT * sizeof(Record);
constexpr uint32_t SCORE_BYTES = RECORD_COUNT * sizeof(float);
constexpr uint32_t INDEX_BYTES = RECORD_COUNT * sizeof(uint32_t);

__global__ __vector__ void mrgsort_kernel(
    __gm__ float* dst, __gm__ float* scores, __gm__ uint32_t* indices)
{
    asc_init();
    __ubuf__ float score_local[RECORD_COUNT];
    __ubuf__ uint32_t index_local[RECORD_COUNT];
    __ubuf__ float sorted_local[WORD_COUNT];
    __ubuf__ float tmp_local[WORD_COUNT];
    __ubuf__ float dst_local[WORD_COUNT];

    asc_copy_gm2ub_align(score_local, scores, SCORE_BYTES);
    asc_copy_gm2ub_align(index_local, indices, INDEX_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

    // 第一步：每次迭代排序32个score，8次迭代生成8条独立的有序队列。
    // 输出交错存放(score, index)，每个数据单元占8字节，即2个float位置。
    asc_bitsort(sorted_local, score_local, index_local, QUEUE_COUNT);
    asc_sync();

    // 第二步：每组归并4条队列，repeat=2得到两条128个数据单元的队列。
    __ubuf__ float* queues[ASC_C_API_MRGSORT_ELEMENT_LEN] = {
        sorted_local, sorted_local + 2 * QUEUE_LENGTH,
        sorted_local + 4 * QUEUE_LENGTH, sorted_local + 6 * QUEUE_LENGTH
    };
    asc_mrgsort4(tmp_local, queues, 2, QUEUE_LENGTH, QUEUE_LENGTH,
        QUEUE_LENGTH, QUEUE_LENGTH, false, 0b1111);
    // 下一次归并读取本次结果，先完成同步。
    asc_sync();

    // 第三步：将两条128个数据单元的队列归并为全局有序结果。
    queues[0] = tmp_local;
    queues[1] = tmp_local + 2 * (4 * QUEUE_LENGTH);
    // 无效队列也初始化为合法的UB地址，长度设为0。
    queues[2] = tmp_local;
    queues[3] = tmp_local;
    asc_mrgsort4(dst_local, queues, 1, 4 * QUEUE_LENGTH,
        4 * QUEUE_LENGTH, 0, 0, false, 0b0011);

    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(dst, dst_local, BYTE_COUNT);
    asc_sync();
}

int main()
{
    std::vector<float> scores(RECORD_COUNT);
    std::vector<uint32_t> indices(RECORD_COUNT);
    std::vector<Record> golden(RECORD_COUNT), output(RECORD_COUNT);
    for (uint32_t i = 0; i < RECORD_COUNT; ++i) {
        // 73与256互质，生成1～256的不重复乱序分数，每组32个元素也未排序。
        scores[i] = static_cast<float>((i * 73 + 19) % RECORD_COUNT + 1);
        indices[i] = i;
        golden[i] = {scores[i], indices[i]};
    }
    std::stable_sort(golden.begin(), golden.end(),
        [](const Record& a, const Record& b) { return a.score > b.score; });

    ACL_CHECK(aclInit(nullptr));
    ACL_CHECK(aclrtSetDevice(0));
    float* score_device = nullptr;
    uint32_t* index_device = nullptr;
    float* dst_device = nullptr;
    ACL_CHECK(aclrtMalloc(reinterpret_cast<void**>(&score_device), SCORE_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST));
    ACL_CHECK(aclrtMalloc(reinterpret_cast<void**>(&index_device), INDEX_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST));
    ACL_CHECK(aclrtMalloc(reinterpret_cast<void**>(&dst_device), BYTE_COUNT,
        ACL_MEM_MALLOC_HUGE_FIRST));
    ACL_CHECK(aclrtMemcpy(score_device, SCORE_BYTES, scores.data(), SCORE_BYTES,
        ACL_MEMCPY_HOST_TO_DEVICE));
    ACL_CHECK(aclrtMemcpy(index_device, INDEX_BYTES, indices.data(), INDEX_BYTES,
        ACL_MEMCPY_HOST_TO_DEVICE));
    mrgsort_kernel<<<1, 0>>>(dst_device, score_device, index_device);
    ACL_CHECK(aclrtSynchronizeDevice());
    ACL_CHECK(aclrtMemcpy(output.data(), BYTE_COUNT, dst_device, BYTE_COUNT,
        ACL_MEMCPY_DEVICE_TO_HOST));

    bool passed = true;
    for (uint32_t i = 0; i < RECORD_COUNT; ++i) {
        if (output[i].score != golden[i].score || output[i].index != golden[i].index) {
            std::cerr << "Mismatch at record " << i << std::endl;
            passed = false;
            break;
        }
    }
    std::cout << "First 8 (score, index):";
    for (uint32_t i = 0; i < 8; ++i) {
        std::cout << " (" << output[i].score << ", " << output[i].index << ")";
    }
    std::cout << '\n' << (passed ? "[Success] asc_mrgsort4 passed."
                                : "[Failed] asc_mrgsort4 failed.") << std::endl;
    ACL_CHECK(aclrtFree(dst_device));
    ACL_CHECK(aclrtFree(index_device));
    ACL_CHECK(aclrtFree(score_device));
    ACL_CHECK(aclrtResetDevice(0));
    ACL_CHECK(aclFinalize());
    return passed ? 0 : 1;
}
```
<!-- end id8 -->
