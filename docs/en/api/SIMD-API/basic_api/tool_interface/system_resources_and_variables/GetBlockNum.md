# GetBlockNum

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T19:21:22.894Z pushedAt=2026-10-26T07:41:44.862Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetBlockNum_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

Obtains the quantity of logical AI Cores configured for the current task, for use in multi-core logic control within the code.

## Prototype

```cpp
__aicore__ inline int64_t GetBlockNum()
```

## Parameters

None

## Return Value

Returns the number of logical AI Cores configured for the current task.

- The return value is consistent with <<<\>\>\> **numBlocks** configured at the time of invocation, and is the number of logical AI Cores participating in computation.
- When only Cube Core (AIC) or Vector Core (AIV) is started, the number of AICs or AIVs actually started is consistent with **numBlocks**.
- When both Cube Core (AIC) and Vector Core (AIV) are started, the number of AICs or AIVs actually started can be calculated from [GetTaskRatio\(\)](GetTaskRatio.md) \* **numBlocks**. Under the currently supported configurations:
    - When the AIC-to-AIV ratio is 1:2, the number of AICs started is **numBlocks**, and the number of AIVs started is 2 \* **numBlocks**;
    - When the AIC-to-AIV ratio is 1:1, the number of AICs started is **numBlocks**, and the number of AIVs started is **numBlocks**.

## Constraints

None

## Example

```cpp
// srcGm and dstGm are externally input GM spaces.
AscendC::GlobalTensor<float> srcGlobal;
AscendC::GlobalTensor<float> dstGlobal;
int64_t blockNum = AscendC::GetBlockNum(); // Obtain the quantity of logical AI Cores configured for the current task.
int64_t perBlockSize = srcDataSize / blockNum; // Each core processes the same number of elements equally.
int64_t blockIdx = AscendC::GetBlockIdx(); // Obtain the ID of the currently working core.
srcGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ float*>(srcGm + blockIdx * perBlockSize * sizeof(float)), perBlockSize);    // Allocate the memory address of srcGlobal on each core.
dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ float*>(dstGm + blockIdx * perBlockSize * sizeof(float)), perBlockSize);    // Allocate the memory address of dstGlobal on each core.
```
