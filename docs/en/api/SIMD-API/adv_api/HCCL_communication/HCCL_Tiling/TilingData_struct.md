# TilingData Structure

<!-- md-trans-meta sourceCommit=ca3b72f6b4f376702407e982cf2cc174087341f5 translatedAt=2026-11-02T05:58:38.675Z pushedAt=2026-11-18T04:58:20.873Z nl=2 -->

## Description

In operator implementation, Tiling assembles the communication configuration items. When the kernel is implemented, the Tiling configuration items are passed to the communication API on the kernel side through input parameters for communication computation. The TilingData structures in this section include [Mc2InitTiling](#table4835205712588) and [Mc2CcTiling](#table678914014562), both of which are returned by calling the [GetTiling](GetTiling.md) API. Among them, [Mc2CcTiling](#table678914014562) is the parameter configuration for each specific communication task. When an operator has multiple communication tasks, multiple [Mc2CcTiling](#table678914014562) parameters can be defined (up to 8 are supported).

## Parameters

**Table 1** Mc2InitTiling parameters

<a name="table4835205712588"></a>

| Parameter | Description |
| --- | --- |
| reserved | Initializes the communication task configuration. The type is uint8_t*, with a maximum supported length of 64 bytes. This structure can only be obtained through the [GetTiling](GetTiling.md) API. |

**Table 2** Mc2CcTiling parameters

<a name="table678914014562"></a>

| Parameter | Description |
| --- | --- |
| reserved | Parameter configuration for each communication task in each communicator. The type is uint8_t*, with a maximum supported length of 280 bytes. This structure can only be obtained through the [GetTiling](GetTiling.md) API. Note that a maximum of 8 communication tasks can be configured. |
