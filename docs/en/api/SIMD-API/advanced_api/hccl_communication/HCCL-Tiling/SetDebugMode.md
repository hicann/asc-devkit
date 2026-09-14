# SetDebugMode

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T02:31:03.197Z -->

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
| debugMode | Input | Debug mode to select, of the **uint8_t** type. Supported values are as follows:<br>1: Disable the HCCL high-level API communication function.<br>2: Print information such as the message queue and the number of Prepare message executions.<br>3: Print the data in the source data buffer and destination data buffer of the Prepare message.<br>4: Print the timestamps and elapsed time of each stage when the AI CPU server executes communication tasks, printing once every 30 operators executed. |

## Return Value

-   0 indicates success.
-   A non-zero value indicates failure.

## Constraints

None

## Examples

```
const char *groupName = "testGroup";
uint32_t opType = HCCL_CMD_REDUCE_SCATTER;
std::string algConfig = "ReduceScatter=level0:fullmesh";
uint32_t reduceType = HCCL_REDUCE_SUM;
AscendC::Mc2CcTilingConfig mc2CcTilingConfig(groupName, opType, algConfig, reduceType);
mc2CcTilingConfig.SetDebugMode(3); // Set the debug mode.
mc2CcTilingConfig.GetTiling(tiling->mc2InitTiling);
mc2CcTilingConfig.GetTiling(tiling->reduceScatterTiling);
```
