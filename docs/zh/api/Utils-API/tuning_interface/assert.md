# assert

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
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- npu="x90" id7 -->
- Kirin X90：不支持
<!-- end id7 -->
<!-- npu="9030" id8 -->
- Kirin 9030：不支持
<!-- end id8 -->

## 功能说明

本接口在SIMD和SIMT调试场景下提供assert断言功能。在算子核函数（Kernel）侧的实现代码中，如果assert的内部条件判断不为真，则会输出assert条件，并将输入的信息格式化打印在屏幕上，同时算子运行失败。

在算子核函数（Kernel）侧代码的适当位置使用assert进行断言检查，并格式化输出一些调试信息。示例如下：

```cpp
int assertFlag = 10;

assert(assertFlag != 10);
```

打印信息示例如下：

```
[ASSERT] /home/.../add_custom.cpp:44: : Assertion `assertFlag != 10' failed.
```

请注意，assert接口的打印功能对算子的实际运行性能有影响。

## 函数原型

```cpp
assert(expr)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| expr | 输入 | assert断言是否终止程序的条件。条件为true则程序继续执行，条件为false则终止程序。 |

## 返回值说明

无

## 约束说明

-   该接口当前仅支持融合编译场景。
-   SIMD场景，如果开发者需要包含标准库头文件<cassert\>，请在"utils/debug/asc\_assert.h"头文件之前包含，避免assert符号冲突。
<!-- npu="950,A3,910b" id13 -->
-   SIMT场景的支持情况如下：
      <!-- npu="950" id10 -->
    - Ascend 950PR/Ascend 950DT：支持。
      <!-- end id10 -->
      <!-- npu="A3" id11 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持。
      <!-- end id11 -->
      <!-- npu="910b" id12 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持。
      <!-- end id12 -->
<!-- end id13 -->

## 需要包含的头文件

使用该接口需要包含"utils/debug/asc\_assert.h"头文件。

```cpp
#include "utils/debug/asc_assert.h"
```

## SIMD调用示例

```cpp
// SIMD
__global__ __cube__ void simp_test_equal(int a)
{
    // input a is 7
    assert(a == 6);
}
```

程序运行时会触发assert，打印效果如下：

```
[ASSERT] test_assert.asc:41: : Assertion `a == 6' failed.
```

## SIMT调用示例

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(1024) inline void simt_kernel(float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        assert(!isnan(x[idx]));
    }
    ```

    程序运行时会触发assert，打印效果如下：

    ```
    [ASSERT] /home/.../simt_kernel.asc:44: void simt_kernel(float *): Assertion `!isnan(x[idx])' failed.
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void simt_kernel(__gm__ float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        assert(!isnan(x[idx]));
    }
    ```

    程序运行时会触发assert，打印效果如下：

    ```
    [ASSERT] /home/.../simt_kernel.asc:44: void simt_kernel(__gm__ float *): Assertion `!isnan(x[idx])' failed.
    ```
