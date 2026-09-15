# GetSystemCycle(ISASI)

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T12:09:47.891Z -->

## Applicable Products

<!-- npu="950" id11 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id11 -->
<!-- npu="A3" id12 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id12 -->
<!-- npu="910b" id13 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id13 -->
<!-- npu="310b" id14 -->
- Atlas 200I/500 A2 inference product: Not supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products AI Core: Not supported
<!-- end id15 -->
<!-- npu="310p" id16 -->
- Atlas inference products Vector Core: Not supported
<!-- end id16 -->
<!-- npu="910" id17 -->
- Atlas training products: Not supported
<!-- end id17 -->

## Description

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

Obtains the current cycle value of the system counter. By calculating the difference between the cycle values before and after a code segment, you can obtain the number of system cycles consumed by the code segment, and then derive the execution time of the code segment through conversion.

<!-- npu="950" id1 -->
For Ascend 950PR/Ascend 950DT, to convert to time, use a frequency of 1 GHz with the time unit of us. The conversion formula is: time = (number of cycles/1000) us.
<!-- end id1 -->

<!-- npu="A3" id2 -->
For Atlas A3 training products/Atlas A3 inference products, to convert to time, use a frequency of 50 MHz with the time unit of us. The conversion formula is: time = (number of cycles/50) us.
<!-- end id2 -->

<!-- npu="910b" id3 -->
For Atlas A2 training products/Atlas A2 inference products, to convert to time, use a frequency of 50 MHz with the time unit of us. The conversion formula is: time = (number of cycles/50) us.
<!-- end id3 --> 

## Function Prototype

```cpp
__aicore__ inline int64_t GetSystemCycle()
```

## Parameters

None

## Return Value

Returns the current cycle value of the system counter.

## Constraints

This API belongs to the PIPE_S pipeline. To measure the instruction time of other pipelines, insert synchronization for the corresponding pipeline through [PipeBarrier](../../sync_control/intra_core_sync/PipeBarrier_ISASI.md) before calling this API. For details, see [Example](#example2).

## Example

- The following example obtains the system cycle count through `GetSystemCycle` and converts it to time (in us).

    ```cpp
    #include "kernel_operator.h"

    __aicore__ inline void InitTilingParam(int32_t& totalSize, int32_t& loopSize)
    {
        int64_t systemCycleBefore = AscendC::GetSystemCycle(); // Number of cycles before the GetBlockNum instruction is called.
        loopSize = totalSize / AscendC::GetBlockNum();
        int64_t systemCycleAfter = AscendC::GetSystemCycle(); // Number of cycles after the GetBlockNum instruction is called.
        int64_t GetBlockNumCycle = systemCycleAfter - systemCycleBefore; // Number of cycles consumed by the GetBlockNum instruction.
        int64_t CycleToTimeBase = 50; // Base unit for converting cycles to time.
        /* 1000 on Ascend 950PR/Ascend 950DT
        50 on Atlas A3 training products/Atlas A3 inference products
        50 on Atlas A2 training products/Atlas A2 inference products */
        int64_t GetBlockNumTime = GetBlockNumCycle / CycleToTimeBase; // Time consumed by the GetBlockNum instruction, in us.
    };
    ```

- <a id="example2"></a>The following example shows the key code snippet for obtaining the time of the vector Add instruction. Before calling `GetSystemCycle`, `PIPE_ALL` synchronization is inserted to ensure that the related instructions have completed execution before the cycle count is obtained.

    ```cpp
    PipeBarrier<PIPE_ALL>();
    int64_t systemCycleBefore = AscendC::GetSystemCycle(); // Number of cycles before the Add instruction is called.
    AscendC::Add(dstLocal, src0Local, src1Local, 512);
    PipeBarrier<PIPE_ALL>();
    int64_t systemCycleAfter = AscendC::GetSystemCycle(); // Number of cycles after the Add instruction is called.
    int64_t GetBlockNumCycle = systemCycleAfter - systemCycleBefore; // Number of cycles consumed by executing the Add instruction.
    int64_t CycleToTimeBase = 50; // Base unit for converting the number of cycles to time.
    /* 1000 on Ascend 950PR/Ascend 950DT
    50 on Atlas A3 training products/Atlas A3 inference products
    50 on Atlas A2 training products/Atlas A2 inference products */
    int64_t GetBlockNumTime = GetBlockNumCycle / CycleToTimeBase; // Time consumed by executing the Add instruction, in us.
    ```
