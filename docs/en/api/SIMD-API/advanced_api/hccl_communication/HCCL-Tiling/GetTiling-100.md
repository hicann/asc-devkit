# GetTiling

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T02:26:52.284Z -->

## Description

Obtains the [Mc2InitTiling](../HCCL-Tiling/tilingdata_structure.md#table4835205712588) parameters and [Mc2CcTiling](../HCCL-Tiling/tilingdata_structure.md#table678914014562) parameters.

## Prototype

```
uint32_t GetTiling(::Mc2InitTiling &tiling)
```

```
uint32_t GetTiling(::Mc2CcTiling &tiling)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tiling | Output | Tiling information stored in the **Tiling** structure. |

## Return Value

-   If the return value is 0, the tiling computation succeeds, and the value of this **Tiling** structure can be used for subsequent computation.
-   If the return value is non-zero, the tiling computation fails, and the **Tiling** result cannot be used.

## Constraints

None

## Examples

```
const char *groupName = "testGroup";
uint32_t opType = HCCL_CMD_REDUCE_SCATTER;
std::string algConfig = "ReduceScatter=level0:fullmesh";
uint32_t reduceType = HCCL_REDUCE_SUM;
AscendC::Mc2CcTilingConfig mc2CcTilingConfig(groupName, opType, algConfig, reduceType);
mc2CcTilingConfig.GetTiling(tiling->mc2InitTiling); // Obtain Mc2InitTiling from the TilingData structure assembled by the operator.
mc2CcTilingConfig.GetTiling(tiling->reduceScatterTiling); // Obtain Mc2CcTiling from the TilingData structure assembled by the operator.
```

