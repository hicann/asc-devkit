# 核函数与VF函数<a name="ZH-CN_TOPIC_0000002593909249"></a>

SIMD与SIMT混合编程涉及多种类型的函数，它们之间遵循严格的调用关系和层级约束。本章节将介绍整体函数类型和调用关系，再详细展开每种函数的定义、调用语法及约束。

SIMD与SIMT混合编程中涉及的函数类型如下表所示：

<a name="zh-cn_topic_0000002571578013_table1818191292017"></a>
| 修饰符 | 函数功能 | 调用方式 |
| --- | --- | --- |
| `__global__ __aicore__` | 算子入口，协调VF执行。若只有在AIV核内执行的SIMD与SIMT混合编程场景，可使用__global__ __vector__来标识只启动AIV核。 | Host侧通过`<<<...>>>`调用 |
| `__aicore__` | Device侧辅助函数 | 核函数或同级函数调用 |
| `__simt_vf__` | 线程级并行计算任务 | 通过SIMT提供的`asc_vf_call`接口调用 |
| `__simd_vf__` | 向量级并行计算任务 | 通过SIMD提供的`asc_vf_call`接口调用 |
| `__simt_callee__` | SIMT VF的子函数 | SIMT VF内部调用 |
| `__simd_callee__` | SIMD VF的子函数 | SIMD VF内部调用 |
| `__callee__` | SIMD VF和SIMT VF的公共子函数 | VF内部调用 |

各层函数间的调用关系如下：
- 核函数：是Device侧的入口函数，可以调用`__aicore__`函数和VF函数。
- simd vf函数：能被核函数或者`__aicore__`函数通过`asc_vf_call`接口调用。simd vf函数内只能调用`__simd_callee__`函数和`__callee__`函数。
- simt vf函数：使用__simt_vf__标记，能被核函数或者`__aicore__`函数通过`asc_vf_call`接口调用。simt vf函数内只能调用`__simt_callee__`函数和`__callee__`函数。
- `__simd_callee__`子函数：simd vf函数内部的子函数，子函数可能有返回值或者通过引用传参。__simd_callee__函数内只能调用`__simd_callee__`函数和`__callee__`函数。
- `__simt_callee__`子函数：simt vf函数内部的子函数，子函数可能有返回值或者通过引用传参。`__simt_callee__`函数内只能调用`__simt_callee__`函数和`__callee__`函数。
- `__callee__`公共函数：可供VF内部调用的公共子函数。`__callee__`函数内只能调用`__callee__`函数。

**图1** SIMD与SIMT混合编程函数调用层级  
<img src="../../../../figures/SIMT_混合_函数调用层级.png" title="SIMD与SIMT混合编程函数调用层级">

## 核函数的定义与执行配置<a name="zh-cn_topic_0000002571578013_section156822920311"></a>

核函数是SIMD与SIMT混合编程的Device侧入口函数，负责协调整个算子的执行流程，包括VF的调度和调用。函数定义语法为：

```
__global__ __vector__ void kernel_name(__gm__ type* param1, __gm__ type* param2, ...);
```

关键修饰符说明如下：

<a name="zh-cn_topic_0000002571578013_table4811406341"></a>
| 修饰符 | 作用 | 必需性 |
| --- | --- | --- |
| `__global__` | 标识核函数，表明可在Host侧通过`<<<...>>>`调用 | 必需 |
| `__vector__` | 标识函数是在Device侧AI Vector上执行 | 必需 |

核函数定义有以下几个约束：

-   返回值类型必须是void；
-   入参支持指针类型（需使用\_\_gm\_\_修饰）和Ascend C内置数据类型；
-   指针参数必须是指向Global Memory上的内存地址，使用\_\_gm\_\_修饰。

核函数的调用是通过<<<...\>\>\>核函数调用符在Host侧调用，语法如下：

```
kernel_name<<<block_num, dyn_ub_size, stream>>>(args...);
```

核函数调用符内的配置参数说明如下：

<a name="zh-cn_topic_0000002571578013_table942016184315"></a>
| 参数 | 类型 | 说明 | 约束 |
| --- | --- | --- | --- |
| `block_num` | `uint32_t` | 设置核函数启用的核数 | 取值范围[1, 65535] |
| `dyn_ub_size` | `uint32_t` | 指定动态内存大小，单位为字节 | 不超过最大可配置值：256KB - 8KB - 32KB - 静态内存 |
| `stream` | `aclrtStream` | 用于维护异步操作执行顺序 | 无 |

## SIMT VF函数<a name="zh-cn_topic_0000002571578013_section1780955884616"></a>

SIMT VF函数用于实现线程级并行计算任务，处理不规则访问和复杂控制逻辑，函数内可使用SIMT内置变量：threadIdx、blockIdx、blockDim、gridDim。函数定义示例如下：

```
__simt_vf__ __launch_bounds__(MAX_THREAD_COUNT) inline void function_name(
    __gm__ type* gm_param,
    __ubuf__ type* ubuf_param,
    type scalar_param, ...);
```

SIMT VF函数定义中的关键修饰符说明如下：

<a name="zh-cn_topic_0000002571578013_table7661145014492"></a>
| 修饰符 | 作用 |
| --- | --- |
| `__simt_vf__` | 函数标识符，标识SIMT VF函数 |
| `__launch_bounds__(N)` | 指定最大线程数（可选，默认1024） |
| `inline` | 建议内联，实际是否内联由编译器决定 |
| `__gm__` | 内存空间修饰符，标识内存空间为GM |
| `__ubuf__` | 内存空间修饰符，标识内存空间为UB |

通过SIMT的[asc\_vf\_call](../../../../../api/SIMT-API/SIMD与SIMT混合编程简介/扩展语法/核函数配置-147.md#asc-vf-call)接口在核函数或\_\_aicore\_\_函数中调用，调用示例如下：

```cpp
uint32_t thread_num = 1024;
asc_vf_call<function_name>(dim3(thread_num), arg1, arg2, ...);
```

> [!NOTE]说明 
> 与SIMT编程场景中在核函数调用符内配置线程数不同，混合编程场景中SIMT线程配置通过asc_vf_call接口的第一个参数配置。

SIMT VF函数有以下约束：

-   入参仅支持Ascend C的[内置数据类型](../../../语言扩展层/SIMD与SIMT混合编程BuiltIn关键字.md#zh-cn_topic_0000002571575581_section1880403364916)（int32\_t、uint32\_t、float、half等）及其组成的指针、数组、结构体类型，且指针类型必须指向GM或者UB内存。
-   不支持将核函数中局部变量的地址或引用传递给VF函数。
-   函数返回类型必须是void。
-   SIMT VF内只能调用\_\_simt\_callee\_\_函数或\_\_callee\_\_函数。

## \_\_simt\_callee\_\_子函数<a name="zh-cn_topic_0000002571578013_section4680126514"></a>

\_\_simt\_callee\_\_子函数是SIMT VF函数内部调用的辅助函数，函数内部可以使用SIMT内置变量。定义示例如下：

```
__simt_callee__ return_type function_name(__gm__ type* gm_param, __ubuf__ type* ubuf_param, type scalar_param, ...);
```

调用示例如下：

```
return_type result = function_name(arg1, arg2, ...);
```

该函数有以下约束：

-   入参仅支持Ascend C的[内置数据类型](../../../语言扩展层/SIMD与SIMT混合编程BuiltIn关键字.md#zh-cn_topic_0000002571575581_section1880403364916)（int32\_t、uint32\_t、float、half等）和指针类型。
-   函数返回值只能是Ascend C的[内置数据类型](../../../语言扩展层/SIMD与SIMT混合编程BuiltIn关键字.md#zh-cn_topic_0000002571575581_section1880403364916)（int32\_t、uint32\_t、float、half等）及对应的指针类型。
-   函数内只能调用\_\_simt\_callee\_\_函数或\_\_callee\_\_函数。
