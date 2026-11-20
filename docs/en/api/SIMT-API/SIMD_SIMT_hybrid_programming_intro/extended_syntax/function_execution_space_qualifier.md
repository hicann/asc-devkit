# Function Execution Space Qualifier

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-10-27T03:45:14.837Z pushedAt=2026-10-27T09:21:55.724Z nl=2 -->

SIMD and SIMT mixed programming involves multiple types of functions, which follow strict calling relationships and hierarchical constraints. This chapter introduces the overall function types and calling relationships, and elaborates on the definitions, calling syntax, and constraints of SIMT-related functions (\_\_simt\_vf\_\_ and \_\_simt\_callee\_\_). For other qualifiers, see the corresponding chapters.

The function types involved in SIMD and SIMT mixed programming are listed in the following table:

| Function Execution Space Qualifier | Function Purpose | Calling Convention |
| --- | --- | --- |
| \_\_global\_\_ \_\_aicore\_\_ | Operator entry that coordinates VF execution. If only the SIMD and SIMT mixed programming scenario executed within the AIV core is involved, __global__ __vector__ can be used to identify that only the AIV core is launched. | Called from the Host side via <<<>>> |
| \_\_aicore\_\_ | Device-side helper function | Called by a kernel function or a function at the same level |
| \_\_simt_vf\_\_ | Thread-level parallel computing task | Called via the asc_vf_call API provided by SIMT |
| \_\_simd_vf\_\_ | Vector-level parallel computing task | Called via the asc_vf_call API provided by SIMD |
| \_\_simt_callee\_\_ | Sub-function of a SIMT VF | Called from within the SIMT VF |
| \_\_simd_callee\_\_ | Sub-function of a SIMD VF | Called from within the SIMD VF |
| \_\_callee\_\_ | Device-side helper function executed at compile time | Can be called from within any Device-side function |

The function call relationships are shown in the following figure:

![](../../../figures/function_call_hierarchy_64.png)

## \_\_simt\_vf\_\_

\_\_simt\_vf\_\_ is used to mark a SIMT VF entry function, which has no return value. Use the asc\_vf\_call API to call the SIMT VF entry function and launch a VF subtask. The SIMT built-in variables threadIdx, blockIdx, blockDim, and gridDim can be used inside the function. The following is an example of the function definition:

```cpp
__simt_vf__ inline void function_name(
    __gm__ uint32_t* gm_param,
    __ubuf__ float* ubuf_param,
    uint64_t scalar_param, ...);
```

The key modifiers in the SIMT VF function definition are described as follows:

| Modifier | Purpose |
| --- | --- |
| \_\_simt_vf\_\_ | Function identifier that identifies a SIMT VF function. |
| inline | Suggests inlining; whether inlining actually occurs is determined by the compiler. |
| \_\_gm\_\_ | Memory space modifier that identifies the memory space as GM. |
| \_\_ubuf\_\_ | Memory space modifier that identifies the memory space as Unified Buffer (UB). |

Call the SIMT [asc\_vf\_call](kernel_function_config.md#asc_vf_call-call) API in a kernel function or an `__aicore__` function. The following is an example of the call:

```cpp
asc_vf_call<function_name>(dim3(blockDim), arg1, arg2, ...);
```

The SIMT VF function has the following constraints:

-   The input parameters support only Ascend C [built-in data types](builtin_data_types.md) (int32\_t, uint32\_t, float, half, etc.) and the pointer, array, and structure types composed of them, and pointer types must point to GM or UB memory.
-   The function return type must be void.
-   Only \_\_simt\_callee\_\_ functions or \_\_callee\_\_ functions can be called inside a SIMT VF.

## \_\_simt\_callee\_\_

\_\_simt\_callee\_\_ subfunctions are helper functions called inside a SIMT VF function. SIMT built-in variables can be used inside the function. The following is a definition example:

```cpp
__simt_callee__ uint32_t simt_helper(__gm__ uint32_t* gm_param, __ubuf__ float* ubuf_param, uint64_t scalar_param, ...);
```

The following is a call example:

```cpp
uint32_t result = simt_helper(arg1, arg2, ...);
```

This function has the following constraints:

-   Input parameters support only Ascend C [built-in data types](builtin_data_types.md) (int32\_t, uint32\_t, float, half, and so on) and their corresponding pointer types.
-   The function return value can only be an Ascend C [built-in data type](builtin_data_types.md) (int32\_t, uint32\_t, float, half, and so on) or its corresponding pointer type.
-   Only \_\_simt\_callee\_\_ functions or \_\_callee\_\_ functions can be called inside the function.
