# Kernel Functions and VF Functions<a name="ZH-CN_TOPIC_0000002593909249"></a>

<!-- md-trans-meta sourceCommit=ca3b72f6b4f376702407e982cf2cc174087341f5 translatedAt=2026-10-23T14:06:16.432Z pushedAt=2026-11-03T07:05:13.414Z nl=2 -->

SIMD and SIMT mixed programming involves multiple types of functions, which follow strict call relationships and level constraints. This section introduces the overall function types and call relationships, and then describes the definition, call syntax, and constraints of each function type in detail.

The function types involved in SIMD and SIMT mixed programming are as follows:

<a name="zh-cn_topic_0000002571578013_table1818191292017"></a>

| Modifier | Function | Call Mode |
| --- | --- | --- |
| `__global__ __aicore__` | Operator entry that coordinates VF execution. If the SIMD and SIMT mixed programming scenario is executed only on the AIV core, __global__ __vector__ can be used to indicate that only the AIV core is started. | Called by the host through `<<<...>>>` |
| `__aicore__` | Device-side helper function | Called by a kernel function or a function at the same level |
| `__simt_vf__` | Thread-level parallel computing task | Called through the `asc_vf_call` interface provided by SIMT |
| `__simd_vf__` | Vector-level parallel computing task | Called through the `asc_vf_call` interface provided by SIMD |
| `__simt_callee__` | Subfunction of a SIMT VF | Called inside a SIMT VF |
| `__simd_callee__` | Subfunction of a SIMD VF | Called inside a SIMD VF |
| `__callee__` | Common subfunction of SIMD VFs and SIMT VFs | Called inside a VF |

The call relationships between functions at each level are as follows:
- Kernel function: an entry function on the device side. It can call `__aicore__` functions and VF functions.
- SIMD VF function: can be called by a kernel function or an `__aicore__` function through the `asc_vf_call` interface. Inside a SIMD VF function, only `__simd_callee__` functions and `__callee__` functions can be called.
- SIMT VF function: marked with __simt_vf__. It can be called by a kernel function or an `__aicore__` function through the `asc_vf_call` interface. Inside a SIMT VF function, only `__simt_callee__` functions and `__callee__` functions can be called.
- `__simd_callee__` subfunction: a subfunction inside a SIMD VF function. The subfunction may have a return value or pass parameters by reference. Inside a `__simd_callee__` function, only `__simd_callee__` functions and `__callee__` functions can be called.
- `__simt_callee__` subfunction: a subfunction inside a SIMT VF function. The subfunction may have a return value or pass parameters by reference. Inside a `__simt_callee__` function, only `__simt_callee__` functions and `__callee__` functions can be called.
- `__callee__` common function: a common subfunction that can be called inside a VF. Inside a `__callee__` function, only `__callee__` functions can be called.

**Figure 1** SIMD and SIMT mixed programming function call hierarchy  
<img src="../../../../figures/simt_call.png" title="SIMD and SIMT mixed programming function call hierarchy">

## Kernel Function Definition and Execution Configuration<a name="zh-cn_topic_0000002571578013_section156822920311"></a>

A kernel function is the device-side entry function of SIMD and SIMT mixed programming. It coordinates the execution flow of the entire operator, including the scheduling and calling of VFs. The function definition syntax is as follows:

```
__global__ __vector__ void kernel_name(__gm__ type* param1, __gm__ type* param2, ...);
```

The key modifiers are described as follows:

<a name="zh-cn_topic_0000002571578013_table4811406341"></a>

| Modifier | Description | Mandatory |
| --- | --- | --- |
| `__global__` | Identifies a kernel function, indicating that it can be called on the host side through `<<<...>>>` | Yes |
| `__vector__` | Identifies that the function is executed on the AI Vector on the device side. | Yes |

A kernel function definition has the following constraints:

-   The return value type must be void;
-   The input parameters support pointer types (which must be modified with \_\_gm\_\_) and Ascend C built-in data types;
-   A pointer parameter must point to a memory address in Global Memory and be modified with \_\_gm\_\_.

A kernel function is called on the host side through the <<<...\>\>\> kernel function call operator. The syntax is as follows:

```
kernel_name<<<block_num, dyn_ub_size, stream>>>(args...);
```

The configuration parameters in the kernel function call operator are described as follows:

<a name="zh-cn_topic_0000002571578013_table942016184315"></a>

| Parameter | Type | Description | Constraint |
| --- | --- | --- | --- |
| `block_num` | `uint32_t` | Sets the number of cores enabled for the kernel function | Value range [1, 65535] |
| `dyn_ub_size` | `uint32_t` | Specifies the dynamic memory size, in bytes. | Does not exceed the maximum configurable value: 256KB - 8KB - 32KB - static memory. |
| `stream` | `aclrtStream` | Used to maintain the execution order of asynchronous operations. | None |

## SIMT VF Function<a name="zh-cn_topic_0000002571578013_section1780955884616"></a>

A SIMT VF function is used to implement thread-level parallel computing tasks and handle irregular access and complex control logic. Inside the function, the following SIMT built-in variables can be used: threadIdx, blockIdx, blockDim, and gridDim. The following is an example of the function definition:

```
__simt_vf__ __launch_bounds__(MAX_THREAD_COUNT) inline void function_name(
    __gm__ type* gm_param,
    __ubuf__ type* ubuf_param,
    type scalar_param, ...);
```

The key modifiers in the SIMT VF function definition are described as follows:

<a name="zh-cn_topic_0000002571578013_table7661145014492"></a>

| Modifier | Description |
| --- | --- |
| `__simt_vf__` | Function identifier that identifies a SIMT VF function |
| `__launch_bounds__(N)` | Specifies the maximum number of threads (optional, 1024 by default) |
| `inline` | Suggests inlining; whether inlining is actually performed is determined by the compiler |
| `__gm__` | Memory space modifier that identifies the memory space as GM |
| `__ubuf__` | Memory space modifier that identifies the memory space as UB |

It is called in the kernel function or the \_\_aicore\_\_ function through the SIMT [asc\_vf\_call](../../../../../api/SIMT-API/SIMD_SIMT_hybrid_programming_intro/extended_syntax/kernel_function_config.md#asc-vf-call) interface. The call example is as follows:

```cpp
uint32_t thread_num = 1024;
asc_vf_call<function_name>(dim3(thread_num), arg1, arg2, ...);
```

> [!NOTE] Note 
> Unlike the SIMT programming scenario where the number of threads is configured in the kernel function call operator, in the mixed programming scenario the SIMT thread configuration is performed through the first parameter of the asc_vf_call interface.

A SIMT VF function has the following constraints:

-   The input parameters support only Ascend C [built-in data types](../../../language_extension/simd_simt_hybrid_builtin_keywords.md#zh-cn_topic_0000002571575581_section1880403364916) (int32\_t, uint32\_t, float, half, and so on) and the pointer, array, and struct types composed of them. Pointer types must point to Global Memory (GM) or Unified Buffer (UB) memory.
-   Passing the address or reference of a local variable in the kernel function to a VF function is not supported.
-   The function return type must be void.
-   Only \_\_simt\_callee\_\_ functions or \_\_callee\_\_ functions can be called within a SIMT VF function.

## \_\_simt\_callee\_\_ Subfunction<a name="zh-cn_topic_0000002571578013_section4680126514"></a>

The \_\_simt\_callee\_\_ subfunction is a helper function called inside a SIMT VF function. SIMT built-in variables can be used inside the function. The following is an example of its definition:

```
__simt_callee__ return_type function_name(__gm__ type* gm_param, __ubuf__ type* ubuf_param, type scalar_param, ...);
```

The following is a call example:

```
return_type result = function_name(arg1, arg2, ...);
```

This function has the following constraints:

-   The input parameters support only Ascend C [built-in data types](../../../language_extension/simd_simt_hybrid_builtin_keywords.md#zh-cn_topic_0000002571575581_section1880403364916) (int32\_t, uint32\_t, float, half, and so on) and pointer types.
-   The function return value can only be an Ascend C [built-in data type](../../../language_extension/simd_simt_hybrid_builtin_keywords.md#zh-cn_topic_0000002571575581_section1880403364916) (int32\_t, uint32\_t, float, half, and so on) or the corresponding pointer type.
-   Only \_\_simt\_callee\_\_ functions or \_\_callee\_\_ functions can be called inside the function.
