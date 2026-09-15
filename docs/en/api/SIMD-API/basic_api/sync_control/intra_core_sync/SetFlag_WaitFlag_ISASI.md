# SetFlag/WaitFlag\(ISASI\)<a name="ZH-CN_TOPIC_0000001837467889"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T13:56:22.363Z -->

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
- Atlas 200I/500 A2 inference products: Not supported
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

As shown in Figure 1, the **SetFlag**/**WaitFlag** APIs are used for synchronization between multiple pipelines within a core:

> [!NOTE]Note
> These APIs are marked as ISASI category and cannot guarantee compatibility across hardware versions. If you need synchronization control APIs that guarantee cross-hardware-version compatibility, see [TQueSync SetFlag/WaitFlag](SetFlag-WaitFlag.md). The difference between the two is that TQueSync-class APIs can guarantee cross-hardware-version compatibility.

- **SetFlag**: After all read and write operations of the preceding instructions in the source pipeline are completed, the current instruction starts execution and sets the corresponding flag in the hardware to 1. **SetFlag** only sets the corresponding flag in the hardware and does not block the next instruction in the source pipeline.
- **WaitFlag**: When the destination pipeline executes this instruction, if the corresponding flag in the hardware is found to be 0, the subsequent instructions of the destination pipeline remain blocked; if the corresponding flag in the hardware is found to be 1, the corresponding flag in the hardware is set to 0, and the subsequent instructions of the destination pipeline start execution.

**Figure 1**  Functional diagram of the SetFlag/WaitFlag APIs<a name="zh-cn_topic_0000002511125384_fig58242463299"></a>  
![](../../../../figures/setflag_waitflag_multi_pipeline_sync.png "SetFlag_WaitFlag_multi-pipeline synchronization diagram")

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <HardEvent event>
__aicore__ inline void SetFlag(int32_t eventID)
template <HardEvent event>
__aicore__ inline void WaitFlag(int32_t eventID)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| event | Input | Template parameter.<br>Synchronization event, with the data type **HardEvent**. When data access dependencies exist between different pipelines within the same core, the corresponding synchronization events must be inserted according to the order of data access. **HardEvent** represents the corresponding synchronization event. The naming rule of **HardEvent** is <source pipeline\_target pipeline>, where the instructions in the source pipeline are executed first and the instructions in the target pipeline are executed later. For example, **MTE2\_V** indicates that **PIPE\_MTE2** is the source pipeline and **PIPE\_V** is the target pipeline, representing synchronization from **PIPE\_MTE2** to **PIPE\_V**, where **PIPE\_V** waits for **PIPE\_MTE2**. Due to differences between hardware architecture generations, events differ across hardware architectures. |
| eventID | Input | Event ID, with the data type **int32_t**. The value range of **eventID** depends on the product model. For details, see [Constraints](#section633mcpsimp). |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- **SetFlag** only sets the hardware flag and does not block the next instruction in the source pipeline.

- **SetFlag** and **WaitFlag** must be used in pairs, and their parameters must be exactly the same (including the template parameter **event** and the input parameter **eventID**). If they do not match, a timeout issue will occur. For example, `SetFlag<HardEvent::S_MTE3>(1)` and `WaitFlag<HardEvent::MTE3_MTE1>(1)` do not match because their template parameter **event** differs.

- When using the [TPipe-TQue framework programming paradigm](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_framework_programming_paradigm.md), **eventID** must be obtained through [AllocEventID](../../resource_management/TPipe/AllocEventID.md) or [FetchEventID](../../resource_management/TPipe/FetchEventID.md).

- When using the [static Tensor programming paradigm](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md), the event type and event ID are managed by the developer. It is recommended to use event IDs 0-5. Event ID 6 is reserved for internal system planning (currently unused), and event ID 7 is used for the **automatic synchronization** feature in TPipe programming. It is currently not recommended to directly use event IDs 6-7.

- The value range of **eventID** is as follows:
<!-- npu="950" id10 -->
    - Ascend 950PR/Ascend 950DT, data range: 0-7.
<!-- end id10 -->
<!-- npu="A3" id11 -->
    - Atlas A3 training products/Atlas A3 inference products, data range: 0-7.
<!-- end id11 -->
<!-- npu="910b" id12 -->
    - Atlas A2 training products/Atlas A2 inference products, data range: 0-7.
<!-- end id12 -->
<!-- npu="310p" id13 -->
    - Atlas inference products AI Core, data range: 0-7.
<!-- end id13 -->
<!-- npu="910" id14 -->
    - For Atlas training products, the data range is 0-3.
<!-- end id14 -->

- Under the same pipeline and the same eventID, using **SetFlag** consecutively causes undefined behavior. In this case, executing **PipeBarrier<PIPE_ALL>** subsequently results in a hang:

    ```cpp
    SetFlag<M_MTE1>(0);  // First SetFlag.
    SetFlag<M_MTE1>(0);  // Second SetFlag (consecutive use under the same pipeline and the same eventID causes undefined behavior).
    ...
    PipeBarrier<PIPE_ALL>();  // Trigger a hang.
    ...
    WaitFlag<M_MTE1>(0);
    WaitFlag<M_MTE1>(0);
    ```

## Example<a name="section837496171220"></a>

```cpp
    AscendC::DataCopy(src1Local, src1Global[i * tileLength], tileLength);
    AscendC::DataCopy(src0Local, src0Global[i * tileLength], tileLength);
    
    // Intra-loop dependency: "DataCopy (PIPE_MTE2)" writes src0Local first, and then "Maxs and Mins (PIPE_V)" reads src0Local.
    // Because PIPE_V needs to wait for PIPE_MTE2, insert the following synchronization.
    AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

    AscendC::Maxs(tmpTensor1, src0Local, inputVal, tileLength);
    AscendC::Mins(tmpTensor2, src0Local, inputVal, tileLength);
```
