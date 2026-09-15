# SetFlag/WaitFlag<a name="ZH-CN_TOPIC_0000001944599897"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T13:56:44.978Z -->

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

This is a synchronization instruction between different pipelines within the same core. It must be inserted between different pipeline instructions that have data dependencies.

- **SetFlag**: After all read and write operations of the preceding instructions are complete, the current instruction starts execution and sets the corresponding flag in the hardware to 1.
- **WaitFlag**: When this instruction is executed, if the corresponding flag is found to be 0, the subsequent instructions in this queue remain blocked. If the corresponding flag is found to be 1, it is set to 0, and the subsequent instructions start execution.

> [!NOTE]
> This API and the [SetFlag/WaitFlag(ISASI)](SetFlag_WaitFlag_ISASI.md) API can achieve the same functionality. The difference between them is that the **TQueSync** class API guarantees cross-hardware-version compatibility, whereas the APIs in [SetFlag/WaitFlag(ISASI)](SetFlag_WaitFlag_ISASI.md) are marked as ISASI category and do not guarantee cross-hardware-version compatibility.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetFlag(TEventID id)
__aicore__ inline void WaitFlag(TEventID id)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| id | Input | Event ID, specified by the user. It is recommended to obtain it through [AllocEventID](../../resource_management/TPipe/AllocEventID.md) or [FetchEventID](../../resource_management/TPipe/FetchEventID.md). It is defined as follows:<br>`using TEventID = int8_t;`<br>For the value range of **id** on different products, see [id value range](#id-value-range). |

### id Value Range

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT: value range is 0-7.
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 training products/Atlas A3 inference products: value range is 0-7.
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 training products/Atlas A2 inference products: value range is 0-7.
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: value range is 0-7.
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: value range is 0-7.
<!-- end id12 -->
<!-- npu="910" id13 -->
- Atlas training products: value range is 0-3.
<!-- end id13 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

**SetFlag**/**WaitFlag** must be used in pairs.

## Example<a name="section837496171220"></a>

If **DataCopy** needs to wait until **SetValue** completes before it can be executed, synchronization from **PIPE_S** to **PIPE_MTE3** must be inserted.

```cpp
AscendC::GlobalTensor<half> dstGlobal;
AscendC::LocalTensor<half> dstLocal;
dstLocal.SetValue(0, 0);
AscendC::TQueSync<PIPE_S, PIPE_MTE3> sync;
sync.SetFlag(0);
sync.WaitFlag(0);
AscendC::DataCopy(dstGlobal, dstLocal, dataSize);
```
