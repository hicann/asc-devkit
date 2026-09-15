# Template Parameter<a name="ZH-CN_TOPIC_0000001973155037"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T13:57:38.231Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_block_sync_intf.h"`.

The template parameters of the **TQueSync** class specify the source pipeline and the target pipeline, where the destination pipeline waits for the source pipeline.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template<pipe_t src, pipe_t dst>
class TQueSync {
public:
    __aicore__ inline void SetFlag(TEventID id);
    __aicore__ inline void WaitFlag(TEventID id);
};
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src | Input | Source pipeline. For supported pipelines, see [hardware pipeline types](intra_core_synchronization_capability_overview.md#hardware-pipeline-types). |
| dst | Input | Destination pipeline. For supported pipelines, see [hardware pipeline types](intra_core_synchronization_capability_overview.md#hardware-pipeline-types). |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example

See the example in [SetFlag-WaitFlag](./SetFlag-WaitFlag.md).
