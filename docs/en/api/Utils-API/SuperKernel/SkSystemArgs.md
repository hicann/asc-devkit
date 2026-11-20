# SkSystemArgs

<!-- md-trans-meta sourceCommit=2c4c7ba2cf913729d4bb0932feba9566179f48fe translatedAt=2026-11-13T10:07:50.348Z pushedAt=2026-11-16T07:07:41.714Z nl=2 -->

## Applicable Products

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
- Atlas inference product AI Core: Not supported
<!-- end id7 -->
<!-- npu="310p" id8 -->
- Atlas inference product Vector Core: Not supported
<!-- end id8 -->
<!-- npu="910" id9 -->
- Atlas training products: Not supported
<!-- end id9 -->

## Description

`sk::SkSystemArgs` is used to pass system parameters to an SK subfunction, including the subfunction's block index, block quantity, and early start synchronization configuration. When an SK subfunction needs to use these system parameters or initialize system variables, `sk::SkSystemArgs *sysArgs` can be added to the entry parameters and passed in by the SuperKernel framework; it can be omitted when not needed. For the complete adaptation rules, see [Additional Adaptation Notes for Kernel Direct Call Operators](../../../guide/programming_guide/advanced_programming/super_kernel/kernel_direct_call_adaptation.md).

## Structure Definition

This structure is provided by the header file and does not need to be defined by the user. The main members are shown below:

```cpp
namespace sk {
struct SkSystemArgs {
    uint16_t skBlockIdx;
    uint16_t skNumBlocks;
    uint16_t skTaskSyncCfg;
    /*Other reserved information*/

    inline __aicore__ uint16_t SkGetBlockIdx();
    inline __aicore__ uint16_t SkGetNumBlocks();
    inline __aicore__ uint16_t SkGetTaskSyncCfg();
    inline __aicore__ void SkInit() const;
};
}
```

## Member Description

| Member | Type | Description |
| --- | --- | --- |
| skBlockIdx | uint16_t | Block index of the SK subfunction. |
| skNumBlocks | uint16_t | Block quantity of the SK subfunction. |
| skTaskSyncCfg | uint16_t | Early start synchronization configuration. |

## Member Function Description

| Function | Return Value | Description |
| --- | --- | --- |
| SkGetBlockIdx() | uint16_t | Returns `skBlockIdx`. |
| SkGetNumBlocks() | uint16_t | Returns `skNumBlocks`. |
| SkGetTaskSyncCfg() | uint16_t | Returns `skTaskSyncCfg`. |
| SkInit() | void | Initializes the system variables in the SuperKernel scenario. |

## Constraints

- When adapting a hand-written SK subfunction to the Early-Start feature, add the `sk::SkSystemArgs *sysArgs` parameter and call `sysArgs->SkInit()` before calling [WaitPreTaskEnd](../../SIMD-API/basic_api/sync_control/inter_task_sync/WaitPreTaskEnd.md) or [SetNextTaskStart](../../SIMD-API/basic_api/sync_control/inter_task_sync/SetNextTaskStart.md) to explicitly initialize the system variables.
- When using the inter-task synchronization APIs above, also configure the corresponding Early-Start flag in the `cap` parameter of [SK_BIND](SK_BIND.md).
- In the SK scenario, when the original operator uses `AscendC::GetBlockNum()`, the block quantity of the SK subfunction can be obtained through `sysArgs->skNumBlocks` or `sysArgs->SkGetNumBlocks()`.
- In the MIX 1:2 scenario, the `skNumBlocks` of the vector core is twice the block num of the original operator. Pay attention to the difference from `AscendC::GetBlockNum()` when using it.

## Required Header File

```cpp
#include "kernel_operator.h"
```

## Examples

The following code shows how to initialize system variables and read system parameters in an SK subfunction. `ArgsStruct` must be defined according to the parameters of the original kernel function. For the binding method, see [SK_BIND](SK_BIND.md).

```cpp
__sk__ __vector__ void func_sk(const ArgsStruct *args, sk::SkSystemArgs *sysArgs)
{
    sysArgs->SkInit();
    uint16_t blockIdx = sysArgs->SkGetBlockIdx();
    uint16_t blockNum = sysArgs->SkGetNumBlocks();
    // Use args, blockIdx, and blockNum to complete the operator computation.
}
```
