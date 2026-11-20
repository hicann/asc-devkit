# v1 TilingData (Deprecated)

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T05:58:47.113Z pushedAt=2026-11-18T04:58:22.761Z nl=2 -->

> [!NOTE]
>This structure is deprecated and will be removed in a later version. Do not use this structure. You do not need to directly set the members of this structure; use the APIs provided by HCCL Tiling for configuration.
>For the TilingData structure described in this section, when building a communication-computing fusion operator, the computing Tiling structure must be appended after the communication Tiling structure in the TilingData structure of the fusion operator.
>For the v1 and v2 versions of TilingData, the first uint32\_t field of the Tiling structure is used to distinguish the two versions, that is, the **preparePosition** field of the [v1 version](#table4835205712588) and the **version** field of the [v2 version](v2_TilingData_deprecated.md#table4835205712588). If the v2 Tiling structure is used, **version** must be set to 2; if the v1 Tiling structure is used, **preparePosition** must be set to 1. When using any version of TilingData, you must strictly follow the corresponding version of the Tiling structure and use it as a component of the operator's TilingData structure.

## Description

Before the AI CPU starts delivering communication tasks, it needs to obtain the fixed communication configuration [Mc2Msg](#table4835205712588). In the operator implementation, Tiling assembles the communication configuration items. By configuring Tiling Data with fixed parameters and a fixed parameter order, the communication configuration is passed to the AI CPU when calling the AI CPU communication API.

## Parameters

**Table 1** Mc2Msg parameters

<a name="table4835205712588"></a>

| Parameter | Description |
| --- | --- |
| preparePosition | Sets the way the server assembles tasks. The user needs to explicitly assign a value in Tiling. **uint32_t** type. Currently supported values are as follows:<br><br>**1**: The AI CPU and AI Core implement message passing and task delivery through the communication task mechanism. Set to **1** when the AI Core side notifies via messages, that is, set to **1** when [HCCL](../HCCL_Kernel/HCCL_Kernel.md) is used in the operator. |
| sendOff | Reserved parameter, non-configurable. |
| recvOff | Reserved parameter, non-configurable. |
| tailSendOff | Reserved parameter, non-configurable. |
| tailRecvOff | Reserved parameter, non-configurable. |
| sendCnt | Reserved parameter, non-configurable. |
| recvCnt | Reserved parameter, non-configurable. |
| tailSendCnt | Reserved parameter, non-configurable. |
| tailRecvCnt | Reserved parameter, non-configurable. |
| totalCnt | Reserved parameter, non-configurable. |
| turnNum | Reserved parameter, non-configurable. |
| tailNum | Reserved parameter, non-configurable. |
| stride | Reserved parameter, non-configurable. |
| workspaceOff | Reserved parameter, non-configurable. |
| notifyOff | Reserved parameter, non-configurable. |
| notifyBeginCnt | Reserved parameter, non-configurable. |
| notifyEndCnt | Reserved parameter, non-configurable. |
| useBufferType | Sets the location where the communication algorithm obtains input data. **uint8_t** type. Parameter values are as follows:<br>**0**: Default value. By default, the communication input is not placed in windows, where windows is a shared buffer accessible to other cards.<br>**1**: The communication input is not placed in windows. Currently, the function of value **1** is the same as that of value **0**.<br>**2**: The communication input is placed in windows. Applicable only to the AllReduce algorithm. |
| funID | Reserved parameter, non-configurable. |
| dataType | Reserved parameter, non-configurable. |
| groupNum | Reserved parameter, non-configurable. |
| reuseMode | Reserved parameter, non-configurable. |
| commType | Reserved parameter, non-configurable. |
| reduceOp | Reserved parameter, non-configurable. |
| commOrder | Reserved parameter, non-configurable. |
| waitPolicy | Reserved parameter, non-configurable. |
| rspPolicy | Reserved parameter, non-configurable. |
| exitPolicy | Reserved parameter, non-configurable. |
| commAlg | Sets the specific communication algorithm. The user needs to explicitly assign a value in Tiling. **uint8_t** type. Currently supported values are as follows:<br><br>**1**: FullMesh algorithm, that is, full connection between NPUs, where any two NPUs can directly send and receive data. |
| taskType | Reserved parameter, non-configurable. |
| debugMode | Reserved parameter, non-configurable. |
| stepSize | Reserved parameter, non-configurable. |
| sendArgIndex | Reserved parameter, non-configurable. |
| recvArgIndex | Reserved parameter, non-configurable. |
| commOutArgIndex | Reserved parameter, non-configurable. |
| hasCommOut | Whether the computation result of the local card's communication algorithm is output to recvBuf (destination data buffer address). Only the AllGather and AlltoAll algorithms support configuring this parameter. **uint8_t** type. Parameter values are as follows:<br>**0**: Does not output the computation result of the local card's communication algorithm. When the communication result does not need to be output, set this parameter to **0**. In this case, the local card's communication result data is not copied, which improves operator performance. For example, in an 8-card scenario, if the local card only takes part of the data from other cards, this parameter can be set to **0**.<br>**1**: Outputs the computation result of the local card's communication algorithm. |
| reserve | Reserved field. |
| reserve2 | Reserved field. |

## Constraints

-   The operator's Tiling Data structure must completely include the [Mc2Msg parameters](#table4835205712588) in order.
-   The AI CPU needs to obtain the communication configuration of a fixed data structure, and the operator must maintain the consistency of this structure when registering Tiling Data.
<!-- npu="A3" id1 -->
-   Atlas A3 products do not currently support this version of TilingData.
<!-- end id1 -->

## Examples

Taking the custom operator AllGatherMatmulCustom as an example, the following is the operator prototype of this operator, where "gather\_out" is the output of the AllGather communication task.

```
[
    {
        "op": "AllGatherMatmulCustom",
        "input_desc": [
            {
                "name": "x1",
                "param_type": "required",
                "format": [
                    "ND",
		    "ND"
                ],
                "type": [
                    "float16",
                    "bfloat16"
                ]
            },
            {
                "name": "x2",
                "param_type": "required",
                "format": [
                    "ND",
		    "ND"
                ],
                "type": [
                    "float16",
                    "bfloat16"
                ]
            },
            {
                "name": "bias",
                "param_type": "optional",
                "format": [
                    "ND",
		    "ND"
                ],
                "type": [
                    "float16",
                    "bfloat16"
                ]
            }
        ],
        "output_desc":[
            {
                "name": "y",
                "param_type": "required",
                "format": [
                    "ND",
		    "ND"
                ],
                "type": [
                    "float16",
                    "bfloat16"
                ]
            },
            {
                "name": "gather_out",
                "param_type": "required",
                "format": [
                    "ND",
		    "ND"
                ],
                "type": [
                    "float16",
                    "bfloat16"
                ]
            }
        ],
        "attr": [
            {
                "name": "group",
                "type": "string",
                "default_value":"",
                "param_type":"required"
            },
            {
                "name": "rank_size",
                "type": "int",
                "default_value":0,
                "param_type":"optional"
            },
            {
                "name": "is_gather_out",
                "type": "bool",
                "default_value":true,
                "param_type":"optional"
            }
        ]
    }
]
```

The operator's Tiling Data structure must contain the Mc2Msg parameters in order and in full. The following is an example of the operator's Tiling Data code.

```
// Declare the Mc2Msg structure.
BEGIN_TILING_DATA_DEF(Mc2Msg)
TILING_DATA_FIELD_DEF(uint32_t, preparePosition);
TILING_DATA_FIELD_DEF(uint32_t, sendOff);
TILING_DATA_FIELD_DEF(uint32_t, recvOff);
TILING_DATA_FIELD_DEF(uint32_t, tailSendOff);
TILING_DATA_FIELD_DEF(uint32_t, tailRecvOff);
TILING_DATA_FIELD_DEF(uint64_t, sendCnt);
TILING_DATA_FIELD_DEF(uint32_t, recvCnt);
TILING_DATA_FIELD_DEF(uint32_t, tailSendCnt);
TILING_DATA_FIELD_DEF(uint32_t, tailRecvCnt);
TILING_DATA_FIELD_DEF(uint32_t, totalCnt);
TILING_DATA_FIELD_DEF(uint32_t, turnNum);
TILING_DATA_FIELD_DEF(uint32_t, tailNum);
TILING_DATA_FIELD_DEF(uint32_t, stride);
TILING_DATA_FIELD_DEF(uint32_t, workspaceOff);
TILING_DATA_FIELD_DEF(uint32_t, notifyOff);
TILING_DATA_FIELD_DEF(uint16_t, notifyBeginCnt);
TILING_DATA_FIELD_DEF(uint16_t, notifyEndCnt);
TILING_DATA_FIELD_DEF(uint8_t, useBufferType);
TILING_DATA_FIELD_DEF(uint8_t, funID);
TILING_DATA_FIELD_DEF(uint8_t, dataType);
TILING_DATA_FIELD_DEF(uint8_t, groupNum);
TILING_DATA_FIELD_DEF(uint8_t, reuseMode);
TILING_DATA_FIELD_DEF(uint8_t, commType);
TILING_DATA_FIELD_DEF(uint8_t, reduceOp);
TILING_DATA_FIELD_DEF(uint8_t, commOrder);
TILING_DATA_FIELD_DEF(uint8_t, waitPolicy);
TILING_DATA_FIELD_DEF(uint8_t, rspPolicy);
TILING_DATA_FIELD_DEF(uint8_t, exitPolicy);
TILING_DATA_FIELD_DEF(uint8_t, commAlg);
TILING_DATA_FIELD_DEF(uint8_t, taskType);
TILING_DATA_FIELD_DEF(uint8_t, debugMode);
TILING_DATA_FIELD_DEF(uint8_t, stepSize);
TILING_DATA_FIELD_DEF(uint8_t, sendArgIndex);
TILING_DATA_FIELD_DEF(uint8_t, recvArgIndex);
TILING_DATA_FIELD_DEF(uint8_t, commOutArgIndex);
TILING_DATA_FIELD_DEF(uint8_t, hasCommOut);
TILING_DATA_FIELD_DEF(uint8_t, reserve);
TILING_DATA_FIELD_DEF(uint32_t, reserve2);
END_TILING_DATA_DEF;
REGISTER_TILING_DATA_CLASS(Mc2MsgOp, Mc2Msg)

BEGIN_TILING_DATA_DEF(AllGatherMatmulCustomTilingData)
TILING_DATA_FIELD_DEF_STRUCT(Mc2Msg, msg);
END_TILING_DATA_DEF;
```

```
// Configure Mc2Msg.
AllGatherMatmulCustomTilingData tiling;
tiling.msg.set_preparePosition(1);
tiling.msg.set_commAlg(1);
tiling.msg.set_useBufferType(1);
tiling.msg.set_hasCommOut(1);
```
