# asc\_nop

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/memory_access_functions/asc_nop_res.md#id1 -->

## 功能说明

本接口的功能为生成一条空操作指令，占用当前线程15个时钟周期，不执行任何实际计算与访存操作。

## 函数原型

```cpp
inline void asc_nop()
```

## 参数说明

无

## 返回值说明

无

## 约束说明

无

## 需要包含的头文件

使用该接口需要包含`simt_api/device_functions.h`头文件。

```cpp
#include "simt_api/device_functions.h"
```

## 调用示例

在生产者-消费者同步场景中，消费者通过轮询标记位来判断生成者是否完成数据的写入，`asc_nop`接口可用于降低轮询GM的频率，从而减少带宽压力。

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(1024) void kernel_nop(uint32_t *data, uint32_t* flag)
    {
        ...
        while (asc_ldcg(flag) != 0) { // 轮询标记位
            asc_nop(); // 15个cycle后再发送下一个load指令
        }
        uint32_t x = *data; // 读取数据
        ...
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void kernel_nop(__gm__ uint32_t *data, __gm__ uint32_t* flag)
    {
        ...
        while (asc_ldcg(flag) != 0) { // 轮询标记位
            asc_nop(); // 15个cycle后再发送下一个load指令
        }
        uint32_t x = *data; // 读取数据
        ...
    }
    ```
