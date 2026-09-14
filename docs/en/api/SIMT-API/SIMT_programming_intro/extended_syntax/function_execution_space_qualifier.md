# Function Execution Space Qualifier

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T02:27:47.132Z pushedAt=2026-09-09T10:00:56.669Z -->

A function execution space qualifier indicates whether a function is executed on the host side or the device side, as well as the space range from which it can be called.

**Table 1** Function execution space qualifiers

| Function Execution Space Qualifier | Execution Space | Description |
| --- | --- | --- |
| \_\_host\_\_ | host | Can be called only by host-side functions. A function defined without a function execution space qualifier is a host function by default. |
| \_\_global\_\_ | device | Qualifies a function as the kernel function entry. It can be called only by host-side functions. The function return type must be void, and it cannot be a member function of a class, struct, or union. Recursive calls are not supported, and calls to a __global__ function are asynchronous. |
| \_\_aicore\_\_ | device | Qualifies a function that can be executed only on the device side. It can be called only by a __global__ function or another __aicore__ function. |

The function call relationships are shown in the following figure:

![](../../../figures/function_call_hierarchy.png)
