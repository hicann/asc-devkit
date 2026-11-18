# \_\_dp4a

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
- Atlas推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/integer_math_functions/__dp4a_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/math_functions.h"`。

将输入数据 `src_a` 和 `src_b` 各拆成四个8位分量，再将对应分量分别相乘并累加到32位输入数据 `c` 上。

## 函数原型

```cpp
inline int __dp4a(int src_a, int src_b, int c)
```

```cpp
inline unsigned int __dp4a(unsigned int src_a, unsigned int src_b, unsigned int c)
```

```cpp
inline int __dp4a(char4 src_a, char4 src_b, int c)
```

```cpp
inline unsigned int __dp4a(uchar4 src_a, uchar4 src_b, unsigned int c)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src_a | 输入 | 源操作数 |
| src_b | 输入 | 源操作数 |
| c | 输入 | 32位累加初值 |

## 返回值说明

```text
src_a:  31      24 23      16 15       8 7        0
        +---------+---------+---------+---------+
        |   a3    |   a2    |   a1    |   a0    |
        +---------+---------+---------+---------+

src_b:  31      24 23      16 15       8 7        0
        +---------+---------+---------+---------+
        |   b3    |   b2    |   b1    |   b0    |
        +---------+---------+---------+---------+
```

返回 `c + a0 * b0 + a1 * b1 + a2 * b2 + a3 * b3` 的32位点积结果。

## 约束说明

无

## 调用示例

-   SIMT编程场景：

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(1024) void kernel_dp4a_int(
        int* dst, int* src_a, int* src_b, int* c)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __dp4a(src_a[idx], src_b[idx], c[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_dp4a_int(
        __gm__ int* dst, __gm__ int* src_a, __gm__ int* src_b, __gm__ int* c)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __dp4a(src_a[idx], src_b[idx], c[idx]);
    }
    ```

输入输出示例如下：

```text
src_a: 0x04030201
src_b: 0x08070605
c: 0
result: 70
```
