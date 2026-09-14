# SK\_BIND<a name="ZH-CN_TOPIC_0000002565043461"></a>

<!-- md-trans-meta sourceCommit=49bd8daaa119aaf40209df8164ba36af169c99bc translatedAt=2026-09-06T15:37:36.135Z pushedAt=2026-09-11T06:22:50.431Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id3 -->
- Ascend 950PR/Ascend 950DT: not supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 training products/Atlas A3 inference products: supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 training products/Atlas A2 inference products: supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference product: not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products AI Core: not supported
<!-- end id7 -->
<!-- npu="310p" id8 -->
- Atlas inference products Vector Core: not supported
<!-- end id8 -->
<!-- npu="910" id9 -->
- Atlas training products: not supported
<!-- end id9 -->
<!-- npu="x90" id1 -->
- Kirin X90: not supported
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030: not supported
<!-- end id2 -->

## Function Description<a name="section259105813316"></a>

This API provides the capability to bind the original kernel function and the SK subfunction in the operator SuperKernel scenario. For the complete adaptation method of kernel function direct call operators, see [Additional Adaptation Description for Kernel Launch Operator](../../../guide/programming_guide/advanced_programming/super_kernel/additional_adaptation_description_for_kernel_launch_operator.md).

## Prototype<a name="section2067518173415"></a>

```cpp
// GF, cap, SK0, ...
#define SK_BIND(...)
```

## Parameters<a name="section158061867342"></a>

| Parameter | Input/Output | Description |
| --- | --- | --- |
| GF | Input | Kernel function signature. |
| cap | Input | Type `uint64_t`, used to mark the SuperKernel features of the current operator. The SuperKernel framework adjusts the corresponding synchronization and fusion strategies based on this information. Each bit can be combined with bitwise OR:<br>&bull; 1: early start wait flag, indicating that the operator uses the [WaitPreTaskEnd](../../SIMD-API/basic_api/sync_control/inter_task_sync/WaitPreTaskEnd.md) API.<br>&bull; 2: early start set flag, indicating that the operator uses the [SetNextTaskStart](../../SIMD-API/basic_api/sync_control/inter_task_sync/SetNextTaskStart.md) API.<br>&bull; 4: disable_dcci, indicating that the current operator requires the SuperKernel framework to disable the DCCI ALL instructions before and after the call. For details, see [Operator Adaptation Description](../../../guide/programming_guide/advanced_programming/super_kernel/operator_adaptation_description.md).<br>&bull; 8: disable_batchmode_check, indicating that the `__schedmode__` check of the current operator is skipped, so that the operator can continue to participate in SuperKernel fusion.<br>For example, when both the early start wait flag and the early start set flag are configured, the value of cap is 3. |
| SK0 | Input | SK subfunction signature. |
| ... | Input | SK1 to SK3. Multiple SK subfunction signatures can be provided, with a maximum of four function signatures including SK0. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section43265506459"></a>

-   A kernel function can be bound to a maximum of four SK subfunctions.
-   The kernel function direct call operator currently supports entering SuperKernel only on the npugraph_ex backend, and does not support GE graph mode.

## Header Files to Include<a name="section10354115115916"></a>

To use this API, include the "kernel\_operator.h" header file.

```cpp
#include "kernel_operator.h"
```

## Example<a name="section990974612242"></a>

The following example shows the basic usage of **SK_BIND**. For complete rules such as the SK subfunction signature and parameter structure definition, see [Additional Adaptation Description for Kernel Launch Operator](../../../guide/programming_guide/advanced_programming/super_kernel/additional_adaptation_description_for_kernel_launch_operator.md).

```cpp
#include "kernel_operator.h"

// Keep the original normal kernel (for non-SuperKernel scenarios).
__global__ __vector__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, uint32_t totalLength) 
{ 
    KernelAdd op; 
    op.Init(x, y, z, totalLength); 
    op.Process(); 
} 

// Rule 3: Define the parameter structure (based on the actual parameters of the original global function).
struct AddCustomArgs {
   GM_ADDR x;                      // Corresponds to the first parameter of add_custom. 
   GM_ADDR y;                      // Corresponds to the second parameter of add_custom. 
   GM_ADDR z;                      // Corresponds to the third parameter of add_custom. 
   uint32_t totalLength;           // Corresponds to the fourth parameter of add_custom.
};  
// Define an SK subfunction with a template parameter.
// The template parameter is used only to instantiate different symbols and does not affect the function logic.
template<uint32_t splitNum>
__sk__ __vector__ void add_custom_sk(const AddCustomArgs *args, sk::SkSystemArgs *sysArgs/* Optionally add the sysArgs parameter. */)
{
    // Obtain parameters from the structure.
    GM_ADDR x = args->x;
    GM_ADDR y = args->y; 
    GM_ADDR z = args->z; 
    uint32_t totalLength = args->totalLength; 
    // Rule 5: The logic is consistent with the global function.
    KernelAdd op;
    op.Init(x, y, z, totalLength);
    op.Process();
}
// Rule 6: Use SK_BIND for binding.
// Instantiate four different symbols by specifying template parameters.
// A cap value of 4 indicates configuring disable_dcci.
SK_BIND(add_custom, 4, add_custom_sk<0>, add_custom_sk<1>, add_custom_sk<2>, add_custom_sk<3>);
```
