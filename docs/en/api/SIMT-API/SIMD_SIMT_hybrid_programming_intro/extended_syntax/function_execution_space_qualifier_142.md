# Function Execution Space Qualifier

<!-- md-trans-meta sourceCommit=2c97acf47d28b2c9fa27e8b745084f6e4f3eedea translatedAt=2026-09-07T10:05:23.162Z pushedAt=2026-09-09T01:49:40.948Z -->

SIMD and SIMT hybrid programming involves multiple types of functions, which follow strict calling relationships and hierarchical constraints. This chapter first introduces the overall function types and calling relationships, and then elaborates on the definition, calling syntax, and constraints of each type of function.

The function types involved in SIMD and SIMT hybrid programming are shown in the following table:

| Function Execution Space Qualifier | Function Purpose | Calling Convention |
| --- | --- | --- |
| \_\_global\_\_ \_\_aicore\_\_ | Operator entry that coordinates VF execution. If only the SIMD and SIMT hybrid programming scenario executed on the AIV core is involved, __global__ __vector__ can be used to identify that only the AIV core is launched. | Called from the Host side through <<<>>> |
| \_\_aicore\_\_ | Device-side helper function | Called by a kernel function or a function at the same level |
| \_\_simt_vf\_\_ | Thread-level parallel computing task | Called through the asc_vf_call interface provided by SIMT |
| \_\_simd_vf\_\_ | Vector-level parallel computing task | Called through the asc_vf_call interface provided by SIMD |
| \_\_simt_callee\_\_ | Sub-function of a SIMT VF | Called inside the SIMT VF |
| \_\_simd_callee\_\_ | Sub-function of a SIMD VF | Called inside the SIMD VF |
| \_\_callee\_\_ | Device-side helper function executed at compile time | Can be called inside any Device-side function |

The function calling relationships are shown in the following figure:

![](../../../figures/function_call_hierarchy_64.png)

## \_\_simt\_vf\_\_

\_\_simt\_vf\_\_ is used to mark the SIMT VF entry function, which has no return value. Use the asc\_vf\_call interface to call the SIMT VF entry function and start the VF subtask. The function can use the SIMT built-in variables: **threadIdx**, **blockIdx**, **blockDim**, and **gridDim**. The function definition example is as follows:

```
__simt_vf__ inline void function_name(
    __gm__ uint32_t* gm_param,
    __ubuf__ float* ubuf_param,
    uint64_t scalar_param, ...);
```

The key modifiers in the SIMT VF function definition are described as follows:

| Modifier | Description |
| --- | --- |
| \_\_simt_vf\_\_ | Function identifier that identifies the SIMT VF function. |
| inline | Suggests inlining; whether inlining actually occurs is determined by the compiler. |
| \_\_gm\_\_ | Memory space modifier that identifies the memory space as GM. |
| \_\_ubuf\_\_ | Memory space modifier that identifies the memory space as UB. |

Call the SIMT VF function through the [asc\_vf\_call](kernel_function_config_147.md#asc_vf_call) interface in a kernel function or a \_\_aicore\_\_ function. The calling example is as follows:

```
asc_vf_call<function_name>(dim3(blockDim), arg1, arg2, ...);
```

The SIMT VF function has the following constraints:

-   The input parameters support only Ascend C [built-in data types](built_in_data_type_144) (int32\_t, uint32\_t, **float**, **half**, etc.) and the pointer, array, and structure types composed of them, and pointer types must point to GM or UB memory.
-   The function return type must be **void**.
-   Within a SIMT VF, only \_\_simt\_callee\_\_ functions or \_\_callee\_\_ functions can be called.

## \_\_simt\_callee\_\_

\_\_simt\_callee\_\_ sub-functions are helper functions called inside SIMT VF functions, and SIMT built-in variables can be used inside them. The following is an example of the definition:

```
__simt_callee__ uint32_t simt_helper(__gm__ uint32_t* gm_param, __ubuf__ float* ubuf_param, uint64_t scalar_param, ...);
```

The following is an example of calling it:

```
uint32_t result = simt_helper(arg1, arg2, ...);
```

This function has the following constraints:

-   Input parameters support only Ascend C [built-in data types](built_in_data_type_144) (int32\_t, uint32\_t, float, half, and so on) and their corresponding pointer types.
-   The function return value can only be an Ascend C [built-in data type](built_in_data_type_144) (int32\_t, uint32\_t, float, half, and so on) or its corresponding pointer type.
-   Only \_\_simt\_callee\_\_ functions or \_\_callee\_\_ functions can be called inside the function.
