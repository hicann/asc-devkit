# asc\_shfl

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
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/Warp_shfl_functions/asc_shfl_res.md#id1 -->

## 功能说明

Warp Shfl类接口主要实现Warp级数据交换，能够实现直接读取某个线程的数据，而不需要通过共享内存。这类接口主要通过Warp分组实现组内线程间的数据交换操作。

- **Warp分组**

    Warp内的线程可分为多个组，用户通过参数`width`配置分组宽度（分组的线程数），分组内的线程可进行数据交换，组内线程通过相对组内起始线程位置来标识索引，称为逻辑Lane ID。

- **数据交换**

    本接口主要是获取分组内指定线程持有的`var`值，用户通过参数`src_lane`指定线程。如果`src_lane`大于等于`width`，指定线程的逻辑Lane ID是`src_lane % width`。

**主要使用场景**

- 数据分发：将固定位置的线程数据广播给其他线程；
- 动态数据交换：每个线程从不同的源线程读取数据；

例如，Warp内32个活跃线程调用`asc_shfl(LaneId, 5, 16)`接口，每个线程的返回值为当前线程所在分组内线程编号为5的var值。

**图1**  asc\_shfl结果示意图  
![](../../../figures/asc_shfl_result.png "asc_shfl结果示意图")

## 函数原型

```cpp
inline int32_t asc_shfl(int32_t var, int32_t src_lane, int32_t width = warpSize)
```

```cpp
inline uint32_t asc_shfl(uint32_t var, int32_t src_lane, int32_t width = warpSize)
```

```cpp
inline float asc_shfl(float var, int32_t src_lane, int32_t width = warpSize)
```

```cpp
inline int64_t asc_shfl(int64_t var, int32_t src_lane, int32_t width = warpSize)
```

```cpp
inline uint64_t asc_shfl(uint64_t var, int32_t src_lane, int32_t width = warpSize)
```

```cpp
inline half asc_shfl(half var, int32_t src_lane, int32_t width = warpSize)
```

```cpp
inline half2 asc_shfl(half2 var, int32_t src_lane, int32_t width = warpSize)
```

```cpp
inline bfloat16_t asc_shfl(bfloat16_t var, int32_t src_lane, int32_t width = warpSize)
```

```cpp
inline bfloat16x2_t asc_shfl(bfloat16x2_t var, int32_t src_lane, int32_t width = warpSize)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| var | 输入 | 线程用于交换的输入操作数。 |
| src_lane | 输入 | 期望获取的var值所在线程的Lane ID。 |
| width | 输入 | Warp内参与交换的线程的分组宽度，默认值为32。width的取值范围为(0, 32]，width必须是2的倍数。 |

## 返回值说明

Warp内指定线程的`var`值。

## 约束说明

-   如果目标线程是非活跃状态，获取到寄存器中未初始化的值。
-   若入参`width`不是2的倍数或超出32，返回值异常。

## 需要包含的头文件

使用除half、half2、bfloat16_t、bfloat16x2_t类型之外的接口需要包含`simt_api/device_warp_functions.h`头文件，使用half和half2类型接口需要包含`simt_api/asc_fp16.h`头文件，使用bfloat16_t和bfloat16x2_t类型接口需要包含`simt_api/asc_bf16.h`头文件。

```cpp
#include "simt_api/device_warp_functions.h"
```

```cpp
#include "simt_api/asc_fp16.h"
```

```cpp
#include "simt_api/asc_bf16.h"
```

## 调用示例

完整样例请参考[InsertHashTable算子样例](../../../../../../examples/03_simt_api/02_features/01_api_features/00_memory_access/insert_hash_table/README.md)。

- SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(1024) void kernel_shfl(int32_t* dst, int32_t total_num)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_num) {
            return;
        }
        int32_t laneId = idx % 32;
        // 0-15线程返回值为1，16-31线程返回值为17
        int32_t result = asc_shfl(laneId, 1, 16);
        dst[idx] = result;
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void kernel_shfl(__gm__ int32_t* dst, int32_t total_num)
    {
        // asc_vf_call参数：dim3{1024, 1, 1}
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_num) {
            return;
        }
        int32_t laneId = idx % 32;
        // 0-15线程返回值为1，16-31线程返回值为17
        int32_t result = asc_shfl(laneId, 1, 16);
        dst[idx] = result;
    }
    ```
