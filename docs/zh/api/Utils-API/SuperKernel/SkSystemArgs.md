# SkSystemArgs

## 产品支持情况

<!-- npu="950" id3 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3系列产品：支持
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2系列产品：支持
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id7 -->
<!-- npu="310p" id8 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id8 -->
<!-- npu="910" id9 -->
- Atlas训练系列产品：不支持
<!-- end id9 -->

## 功能说明

`sk::SkSystemArgs`用于向SK子函数传递系统参数，包括子函数的block索引、block数量和early start同步配置。SK子函数需要使用这些系统参数或初始化系统变量时，可在入口参数中添加`sk::SkSystemArgs *sysArgs`，由SuperKernel框架传入；不需要时可以省略。完整适配规则请参见[核函数（Kernel）直调算子额外适配说明](../../../guide/programming_guide/advanced_programming/super_kernel/kernel_direct_call_adaptation.md)。

## 结构体定义

该结构体由头文件提供，无需自行定义。以下展示主要成员：

```cpp
namespace sk {
struct SkSystemArgs {
    uint16_t skBlockIdx;
    uint16_t skNumBlocks;
    uint16_t skTaskSyncCfg;
    /*其他预留信息*/

    inline __aicore__ uint16_t SkGetBlockIdx();
    inline __aicore__ uint16_t SkGetNumBlocks();
    inline __aicore__ uint16_t SkGetTaskSyncCfg();
    inline __aicore__ void SkInit() const;
};
}
```

## 成员说明

| 成员 | 类型 | 说明 |
| --- | --- | --- |
| skBlockIdx | uint16_t | SK子函数的block索引。 |
| skNumBlocks | uint16_t | SK子函数的block数量。 |
| skTaskSyncCfg | uint16_t | early start同步配置。 |

## 成员函数说明

| 函数 | 返回值 | 说明 |
| --- | --- | --- |
| SkGetBlockIdx() | uint16_t | 返回`skBlockIdx`。 |
| SkGetNumBlocks() | uint16_t | 返回`skNumBlocks`。 |
| SkGetTaskSyncCfg() | uint16_t | 返回`skTaskSyncCfg`。 |
| SkInit() | void | 初始化SuperKernel场景下的系统变量。 |

## 约束说明

- 手写SK子函数适配Early-Start特性时，需添加`sk::SkSystemArgs *sysArgs`参数，并在调用[WaitPreTaskEnd](../../SIMD-API/basic_api/sync_control/inter_task_sync/WaitPreTaskEnd.md)或[SetNextTaskStart](../../SIMD-API/basic_api/sync_control/inter_task_sync/SetNextTaskStart.md)前调用`sysArgs->SkInit()`，显式初始化系统变量。
- 使用上述任务间同步接口时，还需在[SK_BIND](SK_BIND.md)的`cap`参数中配置对应的Early-Start标记。
- 在SK场景下，原算子使用`AscendC::GetBlockNum()`时，可通过`sysArgs->skNumBlocks`或`sysArgs->SkGetNumBlocks()`获取SK子函数的block数量。
- MIX 1:2场景中，vector核的`skNumBlocks`为原算子block num的2倍，使用时应注意与`AscendC::GetBlockNum()`的差异。

## 需要包含的头文件

```cpp
#include "kernel_operator.h"
```

## 调用示例

以下代码展示如何在SK子函数中初始化系统变量并读取系统参数，`ArgsStruct`需按原核函数的参数定义，绑定方法参见[SK_BIND](SK_BIND.md)。

```cpp
__sk__ __vector__ void func_sk(const ArgsStruct *args, sk::SkSystemArgs *sysArgs)
{
    sysArgs->SkInit();
    uint16_t blockIdx = sysArgs->SkGetBlockIdx();
    uint16_t blockNum = sysArgs->SkGetNumBlocks();
    // 使用args、blockIdx和blockNum完成算子计算。
}
```
