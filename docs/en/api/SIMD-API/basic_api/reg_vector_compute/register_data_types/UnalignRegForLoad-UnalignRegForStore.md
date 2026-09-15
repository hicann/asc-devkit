# UnalignRegForLoad & UnalignRegForStore<a name="ZH-CN_TOPIC_0000001956862301"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T22:00:26.248Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description<a name="section618mcpsimp"></a>

To improve the handling of irregular memory addresses, Reg vector computation supports accessing addresses that are not 32-byte aligned during data transfer, which applies to scenarios of non-aligned transfer from UB to **RegTensor** or from **RegTensor** to UB. **RegBase** introduces a non-aligned register cache mechanism to reduce the performance overhead caused by non-aligned access. This mechanism uses the non-aligned registers (32B) **UnalignRegForLoad** and **UnalignRegForStore** as temporary cache buffers for temporarily storing data that is not 32B-aligned, thereby enabling efficient continuous non-aligned data transfer.

- **UnalignRegForLoad** is a non-aligned load-in register. When reading a non-aligned address, first initialize it through **LoadUnAlignPre** (temporarily store the non-aligned address data into **UnalignRegForLoad**), and then call **LoadUnAlign** to perform the transfer.

- **UnalignRegForStore** is a non-aligned store-out register. When writing a non-aligned address, first call **StoreUnAlign**, and then use **StoreUnAlignPost** for post-processing (write the data in **UnalignRegForStore** to the non-aligned UB address).

For details about how to use **UnalignRegForLoad** and **UnalignRegForStore**, see [Continuous Non-aligned Load-in (LoadUnAlign)](../Reg Data Load-in/LoadUnAlign_continuous.md) and [Continuous Non-aligned Store-out (StoreUnAlign)](../Reg Data Store-out/StoreUnAlign_continuous.md).

## Applicable Products<a name="section156721693504"></a>

Ascend 950PR/Ascend 950DT

## Constraints<a name="section11585101304320"></a>

- The maximum number of **UnalignRegForLoad** registers is 4.

- The maximum number of **UnalignRegForStore** registers is 4.
