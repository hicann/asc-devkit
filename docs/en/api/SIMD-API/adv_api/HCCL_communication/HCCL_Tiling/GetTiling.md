# GetTiling

<!-- md-trans-meta sourceCommit=8d4511303e7599d86c4c8f565e5b0a328eb60b1e translatedAt=2026-11-02T05:50:01.966Z pushedAt=2026-11-18T04:57:45.664Z nl=2 -->

## Description

Obtains the [Mc2InitTiling](TilingData_struct.md#table4835205712588) parameters and [Mc2CcTiling](TilingData_struct.md#table678914014562) parameters.

## Prototype

```
uint32_t GetTiling(::Mc2InitTiling& tiling)
```

```
uint32_t GetTiling(::Mc2CcTiling& tiling)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tiling | Output | Tiling information stored in the Tiling structure. |

## Return Value

-   If the return value is 0, the tiling calculation succeeds, and the value of this Tiling structure can be used for subsequent computation.
-   If the return value is not 0, the tiling calculation fails, and the Tiling result cannot be used.

## Constraints

None

## Examples

```
const char* groupName = "testGroup";
uint32_t opType = HCCL_CMD_REDUCE_SCATTER;
std::string algConfig = "ReduceScatter=level0:fullmesh";
uint32_t reduceType = HCCL_REDUCE_SUM;
AscendC::Mc2CcTilingConfig mc2CcTilingConfig(groupName, opType, algConfig, reduceType);
mc2CcTilingConfig.GetTiling(tiling->mc2InitTiling); // tiling is the TilingData structure assembled by the operator. Obtain Mc2InitTiling.
mc2CcTilingConfig.GetTiling(tiling->reduceScatterTiling); // tiling is the TilingData structure assembled by the operator. Obtain Mc2CcTiling.
```
