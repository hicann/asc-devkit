# GetSystemCycle(ISASI)

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-05T05:44:59.471Z pushedAt=2026-11-18T01:25:31.220Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetSystemCycle_ISASI_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

Obtains the current cycle value of the system counter. By calculating the difference between the cycle values before and after a code segment, the number of system cycles consumed by the code segment can be obtained, and the execution time of the code segment can then be derived through conversion.

<!-- npu="950" id8 -->
For the Ascend 950PR&950DT products, to convert to time, use a frequency of 1 GHz with the time unit of us. The conversion formula is: time = (number of cycles/1000) us.
<!-- end id8 -->

<!-- npu="A3" id9 -->
For the Atlas A3 products, to convert to time, use a frequency of 50 MHz with the time unit of us. The conversion formula is: time = (number of cycles/50) us.
<!-- end id9 -->

<!-- npu="910b" id10 -->
For the Atlas A2 products, to convert to time, use a frequency of 50 MHz with the time unit of us. The conversion formula is: time = (number of cycles/50) us.
<!-- end id10 --> 

## Function Prototype

```cpp
__aicore__ inline int64_t GetSystemCycle()
```

## Parameters

None

## Return Value

Returns the current cycle value of the system counter.

## Constraints

This API belongs to the PIPE\_S pipeline. To measure the instruction time of other pipelines, insert synchronization for the corresponding pipeline by calling [PipeBarrier](../../sync_control/intra_core_sync/PipeBarrier_ISASI.md) before calling this API. For details, see [Example](#example2).

## Example

- The following example obtains the system cycle count through `GetSystemCycle` and converts it to time (unit: us).

    ```cpp
    #include "kernel_operator.h"

    __aicore__ inline void InitTilingParam(int32_t& totalSize, int32_t& loopSize)
    {
        int64_t systemCycleBefore = AscendC::GetSystemCycle(); // Number of cycles before the GetBlockNum instruction is called.
        loopSize = totalSize / AscendC::GetBlockNum();
        int64_t systemCycleAfter = AscendC::GetSystemCycle(); // Number of cycles after the GetBlockNum instruction is called.
        int64_t GetBlockNumCycle = systemCycleAfter - systemCycleBefore; // Number of cycles consumed by executing the GetBlockNum instruction.
        int64_t CycleToTimeBase = 50; // Base unit for converting the number of cycles to time.
        /* 1000 on Ascend 950PR&950DT products.
        50 on Atlas A3 products.
        50 on Atlas A2 products. */
        int64_t GetBlockNumTime = GetBlockNumCycle / CycleToTimeBase; // Time consumed by executing the GetBlockNum instruction, in us.
    };
    ```

- <a id="example2"></a>The following example shows the key code snippet for obtaining the time of the vector Add instruction. Before calling `GetSystemCycle`, `PIPE_ALL` synchronization is inserted to ensure that the cycle count is obtained only after the related instructions finish execution.

    ```cpp
    PipeBarrier<PIPE_ALL>();
    int64_t systemCycleBefore = AscendC::GetSystemCycle(); // Number of cycles before the Add instruction is called.
    AscendC::Add(dstLocal, src0Local, src1Local, 512);
    PipeBarrier<PIPE_ALL>();
    int64_t systemCycleAfter = AscendC::GetSystemCycle(); // Number of cycles after the Add instruction is called.
    int64_t GetBlockNumCycle = systemCycleAfter - systemCycleBefore; // Number of cycles used to execute the Add instruction.
    int64_t CycleToTimeBase = 50; // Base unit for converting the number of cycles to time.
    /* 1000 on Ascend 950PR&950DT products,
    50 on Atlas A3 products,
    and 50 on Atlas A2 products. */
    int64_t GetBlockNumTime = GetBlockNumCycle / CycleToTimeBase; // Time used to execute the Add instruction, in us.
    ```
