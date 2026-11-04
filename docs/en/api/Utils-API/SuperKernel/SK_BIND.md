# SK\_BIND<a name="ZH-CN_TOPIC_0000002565043461"></a>

<!-- md-trans-meta sourceCommit=ffba65a285b93c584314c43929ffaa58f62204b1 translatedAt=2026-10-23T07:16:57.859Z pushedAt=2026-10-26T11:12:09.478Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id3 -->
- Ascend 950PR&950DT products: Supported
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

## Description<a name="section259105813316"></a>

This API provides the capability to bind the original kernel function and SK subfunctions in the operator SuperKernel scenario. For the complete adaptation method for the kernel function (Kernel) direct call operator, see [Additional Adaptation Notes for Kernel Function Direct Call Operators](../../../guide/programming_guide/advanced_programming/super_kernel/kernel_direct_call_adaptation.md).

## Prototype<a name="section2067518173415"></a>

```cpp
#define SK_BIND(GF, cap, SK0,...)
```

## Parameters<a name="section158061867342"></a>

| Parameter | Input/Output | Description |
| --- | --- | --- |
| GF | Input | Kernel function signature. |
| cap | Input | Type `uint64_t`, used to mark the SuperKernel features of the current operator. The SuperKernel framework adjusts the corresponding synchronization and fusion strategies based on this information. Bits can be combined with bitwise OR:<br>&bull; 0: No feature bit is set.<br>&bull; 1: early start wait flag, indicating that the operator uses the [WaitPreTaskEnd](../../SIMD-API/basic_api/sync_control/inter_task_sync/WaitPreTaskEnd.md) API.<br>&bull; 2: early start set flag, indicating that the operator uses the [SetNextTaskStart](../../SIMD-API/basic_api/sync_control/inter_task_sync/SetNextTaskStart.md) API.<br>&bull; 4: disable_dcci, indicating that the current operator requires the SuperKernel framework to disable the DCCI ALL instructions before and after the operator invocation. You must ensure cache coherence of the sub-operators. After confirming that these instructions are not required, you can set this bit to reduce cache maintenance overhead and further improve performance. For details, see [Operator Adaptation Notes](../../../guide/programming_guide/advanced_programming/super_kernel/operator_adaptation.md).<br>&bull; 8: disable_batchmode_check, indicating that the `__schedmode__` check of the current operator is skipped, so that the operator can continue to participate in SuperKernel fusion.<br>For example, when both early start wait flag and early start set flag are configured, the value of cap is 3. <br>&bull; 16: blockdim_scale_up, indicating that the operator supports upward scaling of the number of cores on its own. After fusion, SuperKernel launches the operator with SuperKernel's own number of cores. The operator must ensure correct execution when the actual number of launched cores is greater than its original number of cores.|
| SK0 | Input | SK subfunction signature. |
| ... | Input | SK1 to SK3. Optional. Additional SK subfunction signatures can be provided. SuperKernel can assign different SK subfunctions to different cores to mitigate the overhead caused by accessing the same address.|

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

The following example shows the basic usage of the SK_BIND API. For the complete rules on SK subfunction signatures, parameter structure definitions, and more, see [Additional Adaptation Notes for Kernel Function Direct Call Operators](../../../guide/programming_guide/advanced_programming/super_kernel/kernel_direct_call_adaptation.md).

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
__sk__ __vector__ void add_custom_sk(const AddCustomArgs *args, sk::SkSystemArgs *sysArgs/* Optional: add the sysArgs parameter. */)
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
// A cap value of 0 means no feature bits are set.
SK_BIND(add_custom, 0, add_custom_sk<0>, add_custom_sk<1>, add_custom_sk<2>, add_custom_sk<3>);
```
