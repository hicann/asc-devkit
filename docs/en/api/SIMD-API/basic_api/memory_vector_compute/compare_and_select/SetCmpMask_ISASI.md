# SetCmpMask \(ISASI\)<a name="ZH-CN_TOPIC_0000001836926169"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T08:26:34.657Z pushedAt=2026-09-10T03:14:26.423Z -->

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
- Atlas training products: Not supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_cmpsel_intf.h"`

Sets the value of the comparison register. It is used together with the [Select](Select.md) API that does not pass the mask parameter, and different data is passed in based on different selMode values.

- Mode 0 (SELMODE::VSEL\_CMPMASK\_SPR)

    Pass the selMask LocalTensor to SetCmpMask.

- Mode 1 (SELMODE::VSEL\_TENSOR\_SCALAR\_MODE)

    Pass the src1 LocalTensor to SetCmpMask.

- Mode 2 (SELMODE::VSEL\_TENSOR\_TENSOR\_MODE)

    Pass a LocalTensor to SetCmpMask. The LocalTensor stores the address of selMask.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void SetCmpMask(const LocalTensor<T>& src)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src | Input | The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>The start address of the LocalTensor must be 16-byte aligned. |

## Data Type

Supported data types: b8, b16, b32, b64.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section837496171220"></a>

- When selMode is mode 0 or mode 2:

    ```cpp
    uint32_t dataSize = 256;
    uint32_t selDataSize = 8;
    TPipe pipe;
    TQue<TPosition::VECIN, 1> inQueueX;
    TQue<TPosition::VECIN, 1> inQueueY;
    TQue<TPosition::VECIN, 1> inQueueSel;
    TQue<TPosition::VECOUT, 1> outQueue;
    pipe.InitBuffer(inQueueX, 1, dataSize * sizeof(float));
    pipe.InitBuffer(inQueueY, 1, dataSize * sizeof(float));
    pipe.InitBuffer(inQueueSel, 1, selDataSize * sizeof(uint8_t));
    pipe.InitBuffer(outQueue, 1, dataSize * sizeof(float));
    AscendC::LocalTensor<float> dst = outQueue.AllocTensor<float>();
    AscendC::LocalTensor<uint8_t> sel = inQueueSel.AllocTensor<uint8_t>();
    AscendC::LocalTensor<float> src0 = inQueueX.AllocTensor<float>();
    AscendC::LocalTensor<float> src1 = inQueueY.AllocTensor<float>();
    uint8_t repeat = 4;
    uint32_t mask = 64;
    AscendC::BinaryRepeatParams repeatParams = { 1, 1, 1, 8, 8, 8 };

    // selMode is mode 0 (SELMODE::VSEL_CMPMASK_SPR)
    AscendC::SetCmpMask(sel);
    AscendC::PipeBarrier<PIPE_V>();
    AscendC::SetVectorMask<float>(mask);
    AscendC::Select<float, AscendC::SELMODE::VSEL_CMPMASK_SPR>(dst, src0, src1, repeat, repeatParams);

    // selMode is mode 2 (SELMODE::VSEL_TENSOR_TENSOR_MODE)
    AscendC::LocalTensor<int32_t> tempBuf;
    #if defined(ASCENDC_CPU_DEBUG) && (ASCENDC_CPU_DEBUG == 1)  // CPU debugging
    tempBuf.ReinterpretCast<int64_t>().SetValue(0, reinterpret_cast<int64_t>(reinterpret_cast<__ubuf__ int64_t*>(sel.GetPhyAddr())));
    event_t eventIdSToV = static_cast<event_t>(AscendC::GetTPipePtr()->FetchEventID(AscendC::HardEvent::S_V));
    AscendC::SetFlag<AscendC::HardEvent::S_V>(eventIdSToV);
    AscendC::WaitFlag<AscendC::HardEvent::S_V>(eventIdSToV);
    #else // NPU debugging
    uint32_t selAddr = static_cast<uint32_t>(reinterpret_cast<int64_t>(reinterpret_cast<__ubuf__ int64_t*>(sel.GetPhyAddr())));
    AscendC::SetVectorMask<uint32_t>(32);
    AscendC::Duplicate<uint32_t, false>(tempBuf.ReinterpretCast<uint32_t>(), selAddr, AscendC::MASK_PLACEHOLDER, 1, 1, 8);
    AscendC::PipeBarrier<PIPE_V>();
    #endif
    AscendC::SetCmpMask<int64_t>(tempBuf.ReinterpretCast<int64_t>());
    AscendC::PipeBarrier<PIPE_V>();
    AscendC::SetVectorMask<float>(mask);
    AscendC::Select<float, AscendC::SELMODE::VSEL_TENSOR_TENSOR_MODE>(dst, src0, src1, repeat, repeatParams);
    ```

- When selMode is mode 1:

    ```cpp
    uint32_t dataSize = 256;
    uint32_t selDataSize = 8;
    TPipe pipe;
    TQue<TPosition::VECIN, 1> inQueueX;
    TQue<TPosition::VECIN, 1> inQueueY;
    TQue<TPosition::VECIN, 1> inQueueSel;
    TQue<TPosition::VECOUT, 1> outQueue;
    pipe.InitBuffer(inQueueX, 1, dataSize * sizeof(float));
    pipe.InitBuffer(inQueueY, 1, dataSize * sizeof(float));
    pipe.InitBuffer(inQueueSel, 1, selDataSize * sizeof(uint8_t));
    pipe.InitBuffer(outQueue, 1, dataSize * sizeof(float));
    AscendC::LocalTensor<float> dst = outQueue.AllocTensor<float>();
    AscendC::LocalTensor<uint8_t> sel = inQueueSel.AllocTensor<uint8_t>();
    AscendC::LocalTensor<float> src0 = inQueueX.AllocTensor<float>();
    AscendC::LocalTensor<float> tmpScalar = inQueueY.AllocTensor<float>();

    uint8_t repeat = 4;
    uint32_t mask = 64;
    AscendC::BinaryRepeatParams repeatParams = { 1, 1, 1, 8, 8, 8 };

    // selMode is mode 1 (SELMODE::VSEL_TENSOR_SCALAR_MODE)
    AscendC::SetVectorMask<uint32_t>(32);
    AscendC::Duplicate<float, false>(tmpScalar, static_cast<float>(1.0), AscendC::MASK_PLACEHOLDER, 1, 1, 8);
    AscendC::PipeBarrier<PIPE_V>();
    AscendC::SetCmpMask(tmpScalar);
    AscendC::PipeBarrier<PIPE_V>();
    AscendC::SetVectorMask<float>(mask);
    AscendC::Select(dst, sel, src0, repeat, repeatParams);
    ```
