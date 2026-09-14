# SetCommEngine

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T02:31:28.771Z -->

## Description

Sets the communication engine used by communication tasks.

## Function Prototype

```
uint32_t SetCommEngine(uint8_t commEngine)
```

## Parameter Description

**Table 1**  Parameter Description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **commEngine** | Input | Communication engine. The value is of the **uint8_t** type. For the value range of this parameter, see the description of the **hcclOpExpansionMode** parameter in [HcclCommConfig](https://gitcode.com/cann/hcomm/blob/9.1.0/docs/en/api_ref/comm_mgr_c/data_type_definition/HcclCommConfig.md#parameters). |

## Return Value

-   0 indicates that the setting is successful.
-   A non-zero value indicates that the setting failed.

## Constraints

None

## Examples

```
static ge::graphStatus AllToAllVCustomTilingFunc(gert::TilingContext *context)
{
    AllToAllVCustomV3TilingData *tiling = context->GetTilingData<AllToAllVCustomV3TilingData>();
    const std::string groupName = "testGroup";
    const std::string algConfig = "AlltoAll=level0:fullmesh;level1:pairwise";
    AscendC::Mc2CcTilingConfig mc2CcTilingConfig(groupName, HCCL_CMD_ALLTOALLV, algConfig, 0);
    mc2CcTilingConfig.SetCommEngine(1U);
    mc2CcTilingConfig.GetTiling(tiling->mc2InitTiling);
    mc2CcTilingConfig.GetTiling(tiling->mc2CcTiling);
    return ge::GRAPH_SUCCESS;
}
```
