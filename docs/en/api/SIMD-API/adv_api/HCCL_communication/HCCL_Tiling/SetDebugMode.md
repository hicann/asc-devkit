# SetDebugMode

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-02T05:53:54.408Z pushedAt=2026-11-18T04:58:00.119Z nl=2 -->

## Description

Sets the debug mode.

## Prototype

```
uint32_t SetDebugMode(uint8_t debugMode)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| debugMode | Input | Indicates the selected debug mode. The type is uint8_t. The supported values are as follows:<br>1: Disables the HCCL high-level API communication function.<br>2: Prints information such as the message queue and the number of Prepare message executions.<br>3: Prints the data in the source data buffer and destination data buffer of the Prepare message.<br>4: Prints the timestamps and elapsed time of each stage when the AI CPU server executes communication tasks, once every 30 operators. |

## Return Value

-   0 indicates success.
-   A non-zero value indicates failure.

## Constraints

None

## Examples

```
const char* groupName = "testGroup";
uint32_t opType = HCCL_CMD_REDUCE_SCATTER;
std::string algConfig = "ReduceScatter=level0:fullmesh";
uint32_t reduceType = HCCL_REDUCE_SUM;
AscendC::Mc2CcTilingConfig mc2CcTilingConfig(groupName, opType, algConfig, reduceType);
mc2CcTilingConfig.SetDebugMode(3); // Set the debug mode.
mc2CcTilingConfig.GetTiling(tiling->mc2InitTiling);
mc2CcTilingConfig.GetTiling(tiling->reduceScatterTiling);
```
