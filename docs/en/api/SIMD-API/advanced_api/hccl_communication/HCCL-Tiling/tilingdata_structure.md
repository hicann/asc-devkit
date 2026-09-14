# TilingData Structure

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T02:34:39.633Z -->

## Description

In operator implementation, Tiling assembles the communication configuration items. During kernel function implementation, the Tiling configuration items are passed to the kernel-side communication API through input parameters for communication computation. The TilingData structures in this section include [Mc2InitTiling](#table4835205712588) and [Mc2CcTiling](#table678914014562). Both structures are returned by calling the [GetTiling](GetTiling-100.md) API. Among them, [Mc2CcTiling](#table678914014562) contains the parameter configuration for each specific communication task. When an operator has multiple communication tasks, multiple [Mc2CcTiling](#table678914014562) parameters can be defined (up to 8).

## Parameters

**Table 1**  Mc2InitTiling parameter description

<a name="table4835205712588"></a>
| Parameter | Description |
| --- | --- |
| reserved | Configuration for initializing communication tasks. Type **uint8_t \***, with a maximum length of 64 bytes. This structure can only be obtained through the [GetTiling](GetTiling-100.md) API. |

**Table 2**  Mc2CcTiling parameter description

<a name="table678914014562"></a>
| Parameter | Description |
| --- | --- |
| reserved | Parameter configuration for each communication task in each communicator. Type **uint8_t \***, with a maximum length of 280 bytes. This structure can only be obtained through the [GetTiling](GetTiling-100.md) API. Note that a maximum of 8 communication tasks can be configured. |

