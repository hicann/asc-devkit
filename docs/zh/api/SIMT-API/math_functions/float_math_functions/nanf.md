# nanf

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/nanf_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/math_functions.h"`。

根据输入的字符串生成一个float类型的安静NaN值（浮点尾数最高位为1），并将`tagp`解析值的低22位写入该NaN值的低22位。

![](../../../figures/nanf.png)

## 函数原型

```cpp
inline float nanf(const char* tagp)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| tagp | 输入 | 源操作字符串首地址。支持十进制、八进制和十六进制格式；字符串无法按支持的进制解析时按0处理。 |

## 返回值说明

返回float类型的安静NaN值。返回值的低22位为`tagp`解析值的低22位；`tagp`字符串无法按支持的进制解析时，低22位为0；`tagp`解析溢出时，低22位为全1。

## 约束说明

无

## 调用示例

- SIMT编程场景：

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(1024) void kernel_nanf(float* dst)
    {
        dst[0] = nanf("0x1A");
        dst[1] = nanf("123");
        dst[2] = nanf("abc");
        dst[3] = nanf("0");
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_nanf(__gm__ float* dst)
    {
        dst[0] = nanf("0x1A");
        dst[1] = nanf("123");
        dst[2] = nanf("abc");
        dst[3] = nanf("0");
    }
    ```

输入输出示例如下：

```text
tagp："0x1A", "123", "abc", "0"
result: nan(0x7fc0001a) nan(0x7fc0007b) nan(0x7fc00000) nan(0x7fc00000)
```
