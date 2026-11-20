# SK\_BIND<a name="ZH-CN_TOPIC_0000002565043461"></a>

<!-- md-trans-meta sourceCommit=8dd4bac8c8fac6ab089fd9bc7e54551a308539d5 translatedAt=2026-11-13T10:07:47.003Z pushedAt=2026-11-16T07:07:41.706Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id3 -->
- Ascend 950PR&950DT product: Supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 products: Supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 products: Supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products AI Core: Not supported
<!-- end id7 -->
<!-- npu="310p" id8 -->
- Atlas inference products Vector Core: Not supported
<!-- end id8 -->
<!-- npu="910" id9 -->
- Atlas training products: Not supported
<!-- end id9 -->
<!-- npu="x90" id1 -->
- KirinX90 processor products: not supported
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin9030 processor products: not supported
<!-- end id2 -->


<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/SuperKernel/SK_BIND_res.md#id1 -->

## Description<a name="section259105813316"></a>

This API provides the capability to bind the original kernel function (Kernel) and SK subfunctions in the SuperKernel operator scenario. For the complete adaptation method for directly calling operators via the kernel function (Kernel), see [Additional Adaptation Notes for Kernel Function Direct Call Operators](../../../guide/programming_guide/advanced_programming/super_kernel/kernel_direct_call_adaptation.md).

## Prototype<a name="section2067518173415"></a>

```cpp
#define SK_BIND(GF, cap, SK0,...)
```

## Parameters<a name="section158061867342"></a>

| Parameter | Input/Output | Description |
| --- | --- | --- |
| GF | Input | kernel function signature. |
| cap | Input | `uint64_t` type, used to mark the SuperKernel feature of the current operator. The SuperKernel framework adjusts the corresponding synchronization and fusion strategies based on this information. The bits can be combined using bitwise OR:<br>&bull; 0: no feature bit is set.<br>&bull; 1: early start wait flag, indicates that the operator uses the [WaitPreTaskEnd](../../SIMD-API/basic_api/sync_control/inter_task_sync/WaitPreTaskEnd.md) API.<br>&bull; 2: early start set flag, indicates that the operator uses the [SetNextTaskStart](../../SIMD-API/basic_api/sync_control/inter_task_sync/SetNextTaskStart.md) API.<br>&bull; 4: disable_dcci, indicates that the current operator requires the SuperKernel framework to disable the DCCI ALL instructions before and after the call. The developer must ensure the cache consistency of the sub-operators; after confirming that these instructions are not relied upon, this bit can be set to reduce cache maintenance overhead and further improve performance. For details, see [Adaptation Description](../../../guide/programming_guide/advanced_programming/super_kernel/operator_adaptation.md).<br>&bull; 8: disable_batchmode_check, indicates that the `__schedmode__` check of the current operator is skipped, so that the operator can continue to participate in SuperKernel fusion.<br>&bull; 16: blockdim_scale_up, indicates that the operator already supports upward scaling of the number of cores. After fusion, SuperKernel launches the operator with its own number of cores; the operator must ensure that it can still execute correctly when the actual number of launched cores is greater than its original number of cores.<br><br>For example, when both the early start wait flag and the early start set flag are configured, the value of cap is 3. |
| SK0 | Input | SK sub-function signature. |
| ... | Input | SK1 to SK3. Optional. Additional SK sub-function signatures can be provided. SuperKernel can assign different SK sub-functions to different cores to mitigate the overhead of accessing the same address.|

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section43265506459"></a>

-   A kernel function can bind up to four SK subfunctions.
-   The kernel function direct call operator currently supports entering SuperKernel only on the npugraph_ex backend, and does not support GE graph mode.

## Header Files to Include<a name="section10354115115916"></a>

To use this API, include the "kernel\_operator.h" header file.

```cpp
#include "kernel_operator.h"
```

## Examples<a name="section990974612242"></a>

The following example shows the basic usage of the SK_BIND API. For the complete rules on SK sub-function signatures, parameter structure definitions, and more, see [Additional Adaptation Description for Direct-Call Operators](../../../guide/programming_guide/advanced_programming/super_kernel/kernel_direct_call_adaptation.md).

```cpp
#include "kernel_operator.h"

// Keep the original normal kernel function (Kernel) (for non-SuperKernel scenarios).
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
// Define an SK sub-function with a template parameter.
// The template parameter is only used to instantiate different symbols and does not affect the function logic.
template<uint32_t splitNum>
__sk__ __vector__ void add_custom_sk(const AddCustomArgs *args, sk::SkSystemArgs *sysArgs/* Optionally add the sysArgs parameter. */)
{
    // Obtain the parameters from the structure.
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
// Instantiate four different symbols by specifying the template parameter.
// A cap value of 0 indicates that no feature bit is set.
SK_BIND(add_custom, 0, add_custom_sk<0>, add_custom_sk<1>, add_custom_sk<2>, add_custom_sk<3>);
```
