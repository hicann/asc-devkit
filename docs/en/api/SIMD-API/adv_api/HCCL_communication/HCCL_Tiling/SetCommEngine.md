# SetCommEngine

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-02T05:52:46.257Z pushedAt=2026-11-18T04:57:58.058Z nl=2 -->

## Description

Sets the communication engine used for communication tasks.

## Prototype

```
uint32_t SetCommEngine(uint8_t commEngine)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| commEngine | Input | Communication engine. The value is of the uint8_t type. For the value range, see the description of the hcclOpExpansionMode parameter in [HcclCommConfig](https://gitcode.com/cann/hcomm/blob/master/docs/en/api_ref/comm_mgr_c/data_type_definition/HcclCommConfig.md#parameters). |

## Return Value

-   0 indicates success.
-   A non-zero value indicates failure.

## Constraints

None

## Examples

```
static ge::graphStatus AllToAllVCustomTilingFunc(gert::TilingContext* context)
{
    AllToAllVCustomV3TilingData* tiling = context->GetTilingData<AllToAllVCustomV3TilingData>();
    const std::string groupName = "testGroup";
    const std::string algConfig = "AlltoAll=level0:fullmesh;level1:pairwise";
    AscendC::Mc2CcTilingConfig mc2CcTilingConfig(groupName, HCCL_CMD_ALLTOALLV, algConfig, 0);
    mc2CcTilingConfig.SetCommEngine(1U);
    mc2CcTilingConfig.GetTiling(tiling->mc2InitTiling);
    mc2CcTilingConfig.GetTiling(tiling->mc2CcTiling);
    return ge::GRAPH_SUCCESS;
}
```
