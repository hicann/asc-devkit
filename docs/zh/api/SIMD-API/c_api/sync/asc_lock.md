# asc_lock

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

头文件路径为：`"c_api/sync/sync.h"`。

与[asc_unlock](asc_unlock.md)配对使用，用于实现核内异步流水指令之间的同步处理，`asc_lock`和`asc_unlock`各自的功能如下：

- `asc_lock`：根据`mutex_id`获取Mutex，若Mutex已被锁定，将阻塞指定流水上后续指令的执行，直到所有流水中具有相同`mutex_id`的Mutex都已经被[asc_unlock](asc_unlock.md)释放。
- `asc_unlock`：指定流水的前序指令执行完成后，根据`mutex_id`释放对应Mutex。

> [!NOTE]说明
> `asc_lock`接口需与[asc_unlock](asc_unlock.md)接口配合使用，对同一组`pipe`、`mutex_id`和`mode`，`asc_lock`和`asc_unlock`必须严格成对出现，否则属于未定义行为。具体约束请参见[约束说明](#约束说明)。

## 函数原型

```c
__aicore__ inline void asc_lock(pipe_t pipe, uint8_t mutex_id)
__aicore__ inline void asc_lock(pipe_t pipe, uint8_t mutex_id, const asc_mutex_execute_mode mode)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :---  | :--- | :--- |
| pipe | 输入 | 标识阻塞哪条流水上后续指令，直到所有流水中具有相同`mutex_id`的Mutex都已经被[asc_unlock](asc_unlock.md)释放。<br>参数的类型是`pipe_t`枚举，各个枚举取值的含义请参考[硬件流水类型](./intra_core_sync_overview.md#硬件流水类型)。|
| mutex_id | 输入 | mutex标号，取值范围为[0, 31]。|
| mode | 输入 | 可选参数，用于指定是否阻塞流水，默认值为`ASC_LOCK_BLOCK`。<br>&bull; `ASC_LOCK_BLOCK`：阻塞`pipe`对应流水的执行，直到代码中位于当前`asc_lock`之前且`mutex_id`相同的所有`asc_unlock`调用均已执行完成。<br>&bull; `ASC_LOCK_NON_BLOCK`：不阻塞`pipe`对应流水的执行。|

`asc_mutex_execute_mode`枚举定义如下：

```c
enum asc_mutex_execute_mode {
    ASC_LOCK_BLOCK = 0,
    ASC_LOCK_NON_BLOCK = 1
};
```

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- AIC和AIV支持的`pipe`取值如下表所示。

  | 核类型 | 支持的pipe取值 |
  | :--- | :--- |
  | AIC | `PIPE_S`、`PIPE_M`、`PIPE_MTE1`、`PIPE_MTE2`、`PIPE_FIX` |
  | AIV | `PIPE_S`、`PIPE_MTE2`、`PIPE_MTE3`、`PIPE_V` |

- `asc_lock`与`asc_unlock`必须严格成对使用，并使用相同的`pipe`、`mutex_id`和`mode`。此外，对应的`asc_unlock`必须始终写在`asc_lock`之后，否则属于未定义行为。

    ```cpp
    // 反例：先asc_unlock再asc_lock，顺序颠倒。
    asc_unlock(PIPE_MTE2, 0);
    asc_lock(PIPE_MTE2, 0);
    ```

- 对于`mutex_id`相同的`asc_lock`与`asc_unlock`组合，无论`pipe`、`mode`是否相同，都不得在代码中嵌套使用，否则属于未定义行为。

    ```cpp
    // 反例：mutex_id相同，asc_lock与asc_unlock嵌套。
    asc_lock(PIPE_MTE2, 0);
    asc_lock(PIPE_MTE3, 0);
    asc_unlock(PIPE_MTE3, 0);
    asc_unlock(PIPE_MTE2, 0);
    ```

- 当具有相同`mutex_id`与`pipe`的两对`asc_lock`与`asc_unlock`连续调用时，第一次调用的`asc_lock`将由参数`pipe`指定的流水阻塞后，第二次调用的`asc_lock`不能再次阻塞该流水。换言之，连续调用的、具有相同`mutex_id`与`pipe`的两对`asc_lock`与`asc_unlock`不能实现单流水（参数`pipe`指定）内不同指令之间的同步，单流水内多个指令之间的同步请使用[asc_sync_pipe](asc_sync_pipe.md)接口。

    两次搬运的目的地址在Unified Buffer（UB）存在重叠时，开发者需要控制`PIPE_MTE2`上两条指令执行的先后顺序。在此场景中如下写法（具有相同`mutex_id`与`pipe`的两对`asc_lock`与`asc_unlock`连续调用）只能保证两条`PIPE_MTE2`上指令执行后，`PIPE_V`上指令才能执行，但并不能控制`PIPE_MTE2`上两条指令执行的先后顺序，正确写法是在`CopyInY`与`CopyInX`之间插入`asc_sync_pipe(PIPE_MTE2)`（参考多行注释）。

    ```cpp
    // x和y在UB上的地址存在重叠（重叠部分x覆盖y），预期先搬入y后搬入x，需要保证搬入顺序。
    void CopyInX(__ubuf__ float* x, __gm__ float* xGm, uint32_t len, uint8_t mutexId)
    {
        asc_lock(PIPE_MTE2, mutexId);
        asc_copy_gm2ub(x, xGm, len);
        asc_unlock(PIPE_MTE2, mutexId);
    }

    void CopyInY(__ubuf__ float* y, __gm__ float* yGm, uint32_t len, uint8_t mutexId)
    {
        asc_lock(PIPE_MTE2, mutexId);
        asc_copy_gm2ub(y, yGm, len);
        asc_unlock(PIPE_MTE2, mutexId);
    }

    void Process(__ubuf__ float* z, __ubuf__ float* x, __ubuf__ float* y, uint32_t len, uint8_t mutexId)
    {
        // 先搬入y，再搬入x（重叠部分x覆盖y），顺序错误会导致精度异常。
        CopyInY(y, yGm, len, mutexId);

        /*
        * // 必须在CopyInY与CopyInX之间插入asc_sync_pipe，保证前一次搬入完成后才执行下一次搬入。
        * asc_sync_pipe(PIPE_MTE2);
        */

        CopyInX(x, xGm, len, mutexId);
        asc_lock(PIPE_V, mutexId);
        asc_add(z, x, y, len);
        asc_unlock(PIPE_V, mutexId);
    }
    ```

## 调用示例

本示例通过`asc_lock`阻塞对应流水，确保数据搬入、矢量计算和数据搬出按顺序执行。

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

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

constexpr uint32_t ELEMENTS = 64;
constexpr uint32_t BYTES = ELEMENTS * sizeof(float);
constexpr uint8_t MUTEX_ID = 1;

__simd_vf__ inline void add_vf(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1)
{
    vector_float dst_reg;
    vector_float src0_reg;
    vector_float src1_reg;
    uint32_t count = ELEMENTS;
    vector_bool mask = asc_update_mask_b32(count);
    asc_loadalign(src0_reg, src0);
    asc_loadalign(src1_reg, src1);
    asc_add(dst_reg, src0_reg, src1_reg, mask);
    asc_storealign(dst, dst_reg, mask);
}

__global__ __vector__ void asc_lock_kernel(__gm__ float* output, __gm__ float* src0, __gm__ float* src1)
{
    asc_init();
    __ubuf__ float x[ELEMENTS], y[ELEMENTS], z[ELEMENTS];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(x, src0, BYTES);
    asc_copy_gm2ub_align(y, src1, BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    asc_lock(PIPE_V, MUTEX_ID);
    add_vf(z, x, y);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, z, BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync_pipe(PIPE_ALL);
}

} // namespace

int main()
{
    std::vector<float> src0(ELEMENTS), src1(ELEMENTS), output(ELEMENTS, 0.0f), golden(ELEMENTS);
    for (uint32_t i = 0; i < ELEMENTS; ++i) {
        src0[i] = static_cast<float>(i) * 0.25f;
        src1[i] = static_cast<float>(ELEMENTS - i) * 0.5f;
        golden[i] = src0[i] + src1[i];
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float *src0_device = nullptr, *src1_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, BYTES, src0.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, BYTES, src1.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_lock_kernel<<<1, 0>>>(output_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_lock passed." : "[Failed] asc_lock failed.") << std::endl;
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
