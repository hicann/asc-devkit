# GetBlockNum

<!-- md-trans-meta sourceCommit=53d6dd141dd5063557396f6918eca149e02f93ce translatedAt=2026-08-27T12:04:27.037Z -->

## Applicable Products

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: Supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: Not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: Supported
<!-- end id14 -->

## Description

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

Obtains the number of logical AI Cores configured for the current task, which is used for multi-core logic control within the code.

## Prototype

```cpp
__aicore__ inline int64_t GetBlockNum()
```

## Parameters

None

## Return Value

Returns the number of logical AI Cores configured for the current task.

- The return value is consistent with the **numBlocks** configured when <<<\>\>\> is called, and represents the number of logical AI Cores participating in the computation.
- When only **Cube Core (AIC)** or **Vector Core (AIV)** is started, the number of AICs or AIVs actually started is consistent with **numBlocks**.
- When both **Cube Core (AIC)** and **Vector Core (AIV)** are started, the number of AICs or AIVs actually started can be calculated as [GetTaskRatio\(\)](GetTaskRatio.md) \* **numBlocks**. Under the currently supported configurations:
    - When the AIC-to-AIV ratio is 1:2, the number of AICs started is **numBlocks**, and the number of AIVs started is 2 \* **numBlocks**;
    - When the AIC-to-AIV ratio is 1:1, the number of AICs started is **numBlocks**, and the number of AIVs started is **numBlocks**.

## Constraints

None

## Example

```cpp
// srcGm and dstGm are the GM spaces provided by external input.
AscendC::GlobalTensor<float> srcGlobal;
AscendC::GlobalTensor<float> dstGlobal;
int64_t blockNum = AscendC::GetBlockNum(); // Obtain the number of logical AI Cores configured for the current task.
int64_t perBlockSize = srcDataSize / blockNum; // Each core processes an equal number of elements.
int64_t blockIdx = AscendC::GetBlockIdx(); // Obtain the ID of the currently working core.
srcGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ float*>(srcGm + blockIdx * perBlockSize * sizeof(float)), perBlockSize);    // Allocate the memory address of srcGlobal on each core.
dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ float*>(dstGm + blockIdx * perBlockSize * sizeof(float)), perBlockSize);    // Allocate the memory address of dstGlobal on each core.
```
