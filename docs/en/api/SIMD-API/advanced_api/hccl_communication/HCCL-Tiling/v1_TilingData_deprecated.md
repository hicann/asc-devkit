# v1 TilingData (Deprecated)

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-09-10T02:36:08.668Z -->

> [!NOTE]Note
> This structure is deprecated and will be removed in a later version. Do not use this structure. You do not need to directly set the members of this structure; instead, use the APIs provided by HCCL Tiling.
> For the TilingData structure described in this section, when building a communication-computation fusion operator, the compute Tiling structure in the TilingData structure of the communication-computation fusion operator must be appended after the communication Tiling structure described in this section.
> For the v1 and v2 versions of TilingData, the first uint32\_t field of the Tiling structure is used to distinguish the two versions, namely the **preparePosition** field of [v1](#table4835205712588) and the **version** field of [v2](v2_TilingData_deprecated.md#table4835205712588). If you use the v2 Tiling structure, you must set **version**=2; if you use the v1 Tiling structure, set **preparePosition**=1. When using either version of TilingData, you must strictly follow the Tiling structure of the corresponding version and use it as a component of the operator's TilingData structure.

## Description

Before the AI CPU starts and delivers a communication task, it must obtain the fixed communication configuration [Mc2Msg](#table4835205712588). In the operator implementation, Tiling assembles the communication configuration items and passes the communication configuration information to the AI CPU when calling the AI CPU communication API, by configuring the Tiling Data with fixed parameters and a fixed parameter order.

## Parameters

**Table 1**  Mc2Msg parameters

<a name="table4835205712588"></a>
| Parameter | Description |
| --- | --- |
| **preparePosition** | Sets how the server assembles tasks. You need to explicitly assign a value in **Tiling**. Type: **uint32_t**. Supported values:<br><br>1: The AI CPU and AI Core implement message passing and task delivery through the communication task mechanism. Set this to 1 when the AI Core side notifies through messages, that is, set it to 1 when [HCCL](../HCCL-Kernel/hccl_kernel_api.md) is used in the operator. |
| **sendOff** | Reserved parameter, non-configurable. |
| **recvOff** | Reserved parameter, non-configurable. |
| **tailSendOff** | Reserved parameter, non-configurable. |
| **tailRecvOff** | Reserved parameter, non-configurable. |
| **sendCnt** | Reserved parameter, non-configurable. |
| **recvCnt** | Reserved parameter, non-configurable. |
| **tailSendCnt** | Reserved parameter, non-configurable. |
| **tailRecvCnt** | Reserved parameter, non-configurable. |
| **totalCnt** | Reserved parameter, non-configurable. |
| **turnNum** | Reserved parameter, non-configurable. |
| **tailNum** | Reserved parameter, non-configurable. |
| **stride** | Reserved parameter, non-configurable. |
| **workspaceOff** | Reserved parameter, non-configurable. |
| **notifyOff** | Reserved parameter, non-configurable. |
| **notifyBeginCnt** | Reserved parameter, non-configurable. |
| **notifyEndCnt** | Reserved parameter, non-configurable. |
| **useBufferType** | Sets where the communication algorithm obtains input data. Type: **uint8_t**. Supported values:<br>0: Default value. By default, communication input is not placed in windows, where windows is a shared buffer accessible to other cards.<br>1: Communication input is not placed in windows. Currently, the function of value 1 is the same as that of value 0.<br>2: Communication input is placed in windows. This applies only to the AllReduce algorithm. |
| **funID** | Reserved parameter, non-configurable. |
| **dataType** | Reserved parameter, non-configurable. |
| **groupNum** | Reserved parameter, non-configurable. |
| **reuseMode** | Reserved parameter, non-configurable. |
| **commType** | Reserved parameter, non-configurable. |
| **reduceOp** | Reserved parameter, non-configurable. |
| **commOrder** | Reserved parameter, non-configurable. |
| **waitPolicy** | Reserved parameter, non-configurable. |
| **rspPolicy** | Reserved parameter, non-configurable. |
| **exitPolicy** | Reserved parameter, non-configurable. |
| **commAlg** | Sets the specific communication algorithm. You need to explicitly assign a value in **Tiling**. Type: **uint8_t**. Supported values:<br><br>1: FullMesh algorithm, that is, full connection between NPUs, where any two NPUs can directly send and receive data. |
| **taskType** | Reserved parameter, non-configurable. |
| **debugMode** | Reserved parameter, non-configurable. |
| **stepSize** | Reserved parameter, non-configurable. |
| **sendArgIndex** | Reserved parameter, non-configurable. |
| **recvArgIndex** | Reserved parameter, non-configurable. |
| **commOutArgIndex** | Reserved parameter, non-configurable. |
| **hasCommOut** | Whether the computation result of the communication algorithm on the current card is output to **recvBuf** (the destination data buffer address). Only the AllGather and AlltoAll algorithms support configuring this parameter. Type: **uint8_t**. Supported values:<br>0: The computation result of the communication algorithm on the current card is not output. When the communication result does not need to be output, set this parameter to 0. In this case, the communication result data of the current card is not copied, which improves operator performance. For example, in an 8-card scenario, if the current card only takes part of the data from other cards, set this parameter to 0.<br>1: The computation result of the communication algorithm on the current card is output. |
| **reserve** | Reserved field. |
| **reserve2** | Reserved field. |

## Constraints

-   The Tiling Data structure of the operator must completely contain the [Mc2Msg parameters](#table4835205712588) in order.
-   The AI CPU must obtain the communication configuration of a fixed data structure. Keep the structure consistent when registering Tiling Data for the operator.
-   Atlas A3 training products/Atlas A3 inference products do not support this version of TilingData.

## Examples

Take the custom operator **AllGatherMatmulCustom** as an example. The following shows the operator prototype, where "gather\_out" is the output of the communication task **AllGather**.

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

The Tiling Data structure of the operator must contain the **Mc2Msg** parameters in sequence and in full. The following shows an example of the operator Tiling Data code.

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
