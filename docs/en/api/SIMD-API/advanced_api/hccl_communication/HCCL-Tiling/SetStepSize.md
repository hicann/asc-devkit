# SetStepSize

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:34:58.342Z -->

## Description

Sets the step size of the communication algorithm during [fine-grained communication](../HCCL-Kernel/Iterate-99.md), that is, the number of steps of the communication algorithm executed or prepared for execution by one sub-communication task during fine-grained communication. For example, in [Figure 1 Schematic diagram of AlltoAllV communication steps using the pairwise algorithm](../HCCL-Kernel/Iterate-99.md), the communication step size of the AlltoAllV communication task is 1 in this fine-grained communication scenario.

## Function Prototype

```
uint32_t SetStepSize(uint8_t stepSize)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| stepSize | Input | Step size for each communication during fine-grained communication. 0 indicates that the current communication is not fine-grained. |

## Return Value

-   **0** indicates success.
-   A non-zero value indicates failure.

## Constraints

Currently, only Atlas A3 training products/Atlas A3 inference products are supported.

## Examples

```
static ge::graphStatus AllToAllVCustomTilingFunc(gert::TilingContext *context)
{
    AllToAllVCustomV3TilingData *tiling = context->GetTilingData<AllToAllVCustomV3TilingData>();
    const std::string groupName = "testGroup";
    const std::string algConfig = "AlltoAll=level0:fullmesh;level1:pairwise";
    AscendC::Mc2CcTilingConfig mc2CcTilingConfig(groupName, HCCL_CMD_ALLTOALLV, algConfig, 0);
    mc2CcTilingConfig.SetStepSize(1U);
    mc2CcTilingConfig.GetTiling(tiling->mc2InitTiling);
    mc2CcTilingConfig.GetTiling(tiling->mc2CcTiling);
    return ge::GRAPH_SUCCESS;
}
```
