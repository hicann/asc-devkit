# 函数执行空间限定符

SIMD与SIMT混合编程涉及多种类型的函数，它们之间遵循严格的调用关系和层级约束。本章节将会介绍整体函数类型和调用关系，并详细展开SIMT相关函数（\_\_simt\_vf\_\_和\_\_simt\_callee\_\_）的定义、调用语法及约束，其余限定符请参见对应章节。

SIMD与SIMT混合编程中涉及的函数类型如下表所示：

| 函数执行空间限定符 | 函数功能 | 调用方式 |
| --- | --- | --- |
| \_\_global\_\_ \_\_aicore\_\_ | 算子入口，协调VF执行。若只有在AIV核内执行的SIMD与SIMT混合编程场景，可使用__global__ __vector__来标识只启动AIV核。 | Host侧通过<<<>>>调用 |
| \_\_aicore\_\_ | Device侧辅助函数 | 核函数（Kernel）或同级函数调用 |
| \_\_simt_vf\_\_ | 线程级并行计算任务 | 通过SIMT提供的asc_vf_call接口调用 |
| \_\_simd_vf\_\_ | 向量级并行计算任务 | 通过SIMD提供的asc_vf_call接口调用 |
| \_\_simt_callee\_\_ | SIMT VF的子函数 | SIMT VF内部调用 |
| \_\_simd_callee\_\_ | SIMD VF的子函数 | SIMD VF内部调用 |
| \_\_callee\_\_ | 编译期执行的Device侧辅助函数 | Device侧函数内部均可调用 |

函数调用关系如下图所示：

![](../../../figures/function_call_hierarchy_64.png)

## \_\_simt\_vf\_\_

\_\_simt\_vf\_\_用于标记SIMT VF入口函数，函数无返回值。使用asc\_vf\_call接口调用SIMT VF入口函数，启动VF子任务。函数内可使用SIMT内置变量：threadIdx、blockIdx、blockDim、gridDim。函数定义示例如下：

```cpp
__simt_vf__ inline void function_name(
    __gm__ uint32_t* gm_param,
    __ubuf__ float* ubuf_param,
    uint64_t scalar_param, ...);
```

SIMT VF函数定义中的关键修饰符说明如下：

| 修饰符 | 作用 |
| --- | --- |
| \_\_simt_vf\_\_ | 函数标识符，标识SIMT VF函数。 |
| inline | 建议内联，实际是否内联由编译器决定。 |
| \_\_gm\_\_ | 内存空间修饰符，标识内存空间为GM。 |
| \_\_ubuf\_\_ | 内存空间修饰符，标识内存空间为UB。 |

通过SIMT的[asc\_vf\_call](kernel_function_config.md#asc_vf_call调用)接口在核函数（Kernel）或`__aicore__`函数中调用，调用示例如下：

```cpp
asc_vf_call<function_name>(dim3(blockDim), arg1, arg2, ...);
```

SIMT VF函数有以下约束：

-   入参仅支持Ascend C的[内置数据类型](builtin_data_types.md)（int32\_t、uint32\_t、float、half等）及其组成的指针、数组、结构体类型，且指针类型必须指向GM或者Unified Buffer（UB）内存。
-   函数返回类型必须是void。
-   SIMT VF内只能调用\_\_simt\_callee\_\_函数或\_\_callee\_\_函数。

## \_\_simt\_callee\_\_

\_\_simt\_callee\_\_子函数是SIMT VF函数内部调用的辅助函数，函数内部可以使用SIMT内置变量。定义示例如下：

```cpp
__simt_callee__ uint32_t simt_helper(__gm__ uint32_t* gm_param, __ubuf__ float* ubuf_param, uint64_t scalar_param, ...);
```

调用示例如下：

```cpp
uint32_t result = simt_helper(arg1, arg2, ...);
```

该函数有以下约束：

-   入参仅支持Ascend C的[内置数据类型](builtin_data_types.md)（int32\_t、uint32\_t、float、half等）及对应的指针类型。
-   函数返回值只能是Ascend C的[内置数据类型](builtin_data_types.md)（int32\_t、uint32\_t、float、half等）及对应的指针类型。
-   函数内只能调用\_\_simt\_callee\_\_函数或\_\_callee\_\_函数。
