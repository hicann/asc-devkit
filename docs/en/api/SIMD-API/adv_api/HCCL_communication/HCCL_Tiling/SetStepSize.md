# SetStepSize

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T05:57:52.818Z pushedAt=2026-11-18T04:58:17.813Z nl=2 -->

## Description

Sets the step size of the communication algorithm for [fine-grained communication](../HCCL_Kernel/Iterate.md), that is, the number of steps of the communication algorithm executed or prepared for execution in one sub-communication task during fine-grained communication. For example, in [Figure 1 Schematic diagram of AlltoAllV communication steps using the pairwise algorithm](../HCCL_Kernel/Iterate.md), the communication step size of the AlltoAllV communication task in this fine-grained communication scenario is 1.

## Prototype

```
uint32_t SetStepSize(uint8_t stepSize)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| stepSize | Input | Step size of each communication during fine-grained communication. 0 indicates that the current communication is not fine-grained communication. |

## Return Value

-   0 indicates success.
-   A non-zero value indicates failure.

## Constraints

<!-- npu="A3" id1 -->
Currently, only Atlas A3 products are supported.
<!-- end id1 -->

## Examples

```
static ge::graphStatus AllToAllVCustomTilingFunc(gert::TilingContext* context)
{
    AllToAllVCustomV3TilingData* tiling = context->GetTilingData<AllToAllVCustomV3TilingData>();
    const std::string groupName = "testGroup";
    const std::string algConfig = "AlltoAll=level0:fullmesh;level1:pairwise";
    AscendC::Mc2CcTilingConfig mc2CcTilingConfig(groupName, HCCL_CMD_ALLTOALLV, algConfig, 0);
    mc2CcTilingConfig.SetStepSize(1U);
    mc2CcTilingConfig.GetTiling(tiling->mc2InitTiling);
    mc2CcTilingConfig.GetTiling(tiling->mc2CcTiling);
    return ge::GRAPH_SUCCESS;
}
```
