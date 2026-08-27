# asc\_dcci\_single

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/memory_access_functions/asc_dcci_single_res.md#id1 -->

## 功能说明

该接口用于刷新指定地址所在的Cache Line，保证数据读取时Cache的一致性。

在SIMT编程中，写入数据时会立即写入Global Memory，使其他核可见，因此不存在核间一致性问题。当从Global Memory读取数据时，该数据可能已被其他核修改，此时应使用dcci接口直接访问Global Memory，以获取最新数据。

## 函数原型

```cpp
inline void asc_dcci_single(void *dst)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输入 | Global Memory的地址。 |

## 返回值说明

无

## 约束说明

输入必须为有效的Global Memory地址。传入非Global Memory地址或无效地址时，不能保证Cache刷新行为有效。

## 需要包含的头文件

使用该接口需要包含`simt_api/device_functions.h`头文件。

```cpp
#include "simt_api/device_functions.h"
```

## 调用示例

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(1024) void kernelDcci_single(uint32_t* data, uint32_t* output) {
        // 线程块0的线程0进行数据写操作
        if (blockIdx.x == 0 && threadIdx.x == 0) {
            data[1] = 10;      // 写入数据
            asc_threadfence(); // 确保数据写入完成
            data[0] = 1;       // 标志位，数据已就绪
        }

        // 线程块1的线程0进行数据读操作
        if (blockIdx.x == 1 && threadIdx.x == 0) {
            while (data[0] != 1) {
                asc_dcci_single((void*)(data)); // 从Global Memory读标志位
            }
            output[0] = data[1]; // 数据就绪，获取数据
        }
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void kernelDcci_single(__gm__ uint32_t* data, __gm__ uint32_t* output)
    {
        // 线程块0的线程0进行数据写操作
        if (blockIdx.x == 0 && threadIdx.x == 0) {
            data[1] = 10;      // 写入数据
            asc_threadfence(); // 确保数据写入完成
            data[0] = 1;       // 标志位，数据已就绪
        }

        // 线程块1的线程0进行数据读操作
        if (blockIdx.x == 1 && threadIdx.x == 0) {
            while (data[0] != 1) {
                asc_dcci_single((__gm__ void*)(data)); // 从Global Memory读标志位
            }
            output[0] = data[1]; // 数据就绪，获取数据
        }
    }
    ```
