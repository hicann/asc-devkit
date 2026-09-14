# SetReduceType

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:34:42.630Z -->

## Description

Sets the reduce operation type, which takes effect only for communication tasks with reduction operations.

## Prototype

```
uint32_t SetReduceType(uint32_t reduceType, uint8_t dstDataType = 0, uint8_t srcDataType = 0)
```

## Parameters

**Table 1** Parameter Description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| reduceType | Input | Reduction operation type, effective only for communication tasks with reduction operations. Type: uint32_t. For values, see [Table 2](../HCCL-Kernel/hccl_usage.md#hcclreduceop) Parameter Description. |
| dstDataType | Input | Data type of the output data in the communication task. Type: uint8_t. For the value range of this parameter, see [Table 1](../HCCL-Kernel/hccl_usage.md#table116710585514).<br><br>On Ascend 950PR/Ascend 950DT, when the CCU acts as the server, different communication tasks support different output data types, as follows:<br>For the AllReduce, AllGather, AllToAll, AllToAllV, and AllToAllVWrite communication tasks: the output data type must be the same as the input data type. For the input data types supported by each communication task, see srcDataType.<br>For the ReduceScatter communication task, when the input data type is int16_t, int32_t, half, float, or bfloat16_t, the output data type must be the same as the input data type; when the input data type is int8_t, hifloat8_t, fp8_e5m2_t, or fp8_e4m3fn_t, the output data type must be one of half, bfloat16_t, and float. |
| srcDataType | Input | Data type of the input data in the communication task. Type: uint8_t. For the value range of this parameter, see [Table 1](../HCCL-Kernel/hccl_usage.md#table116710585514).<br><br>On Ascend 950PR/Ascend 950DT, when the CCU acts as the server, the input data types supported by different communication tasks are as follows:<br>AllReduce communication task: supported input types are int16_t, half, bfloat16_t, int32_t, and float.<br>AllGather, AllToAll, AllToAllV, and AllToAllVWrite communication tasks: supported input types are int8_t, uint8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, and double.<br>ReduceScatter communication task: supported input types are int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, half, bfloat16_t, int32_t, and float. |

## Return Value

-   0 indicates that the setting is successful.
-   A non-zero value indicates that the setting failed.

## Constraints
For Ascend 950PR/Ascend 950DT, when the AI CPU acts as a server, this API is not supported yet.
For Atlas A3 training products/Atlas A3 inference products, this API is not supported yet.
For Atlas A2 training products/Atlas A2 inference products, this API is not supported yet.

## Examples

For an example of calling this API, see [Examples](SetOpType.md#examples).
