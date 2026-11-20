# UnalignRegForLoad & UnalignRegForStore<a name="ZH-CN_TOPIC_0000001956862301"></a>

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-05T03:45:11.587Z pushedAt=2026-11-18T01:25:30.954Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/register_data_types/UnalignRegForLoad-UnalignRegForStore_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

To improve the handling of irregular memory addresses, **Reg** vector computation supports accessing addresses that are not 32-byte aligned during data transfer. This applies to scenarios of unaligned transfer from **Unified Buffer (UB)** to **RegTensor**, or from **RegTensor** to UB. **RegBase** introduces an unaligned register caching mechanism to reduce the performance overhead caused by unaligned access. This mechanism uses the unaligned registers (32B) **UnalignRegForLoad** and **UnalignRegForStore** as temporary buffers to hold data that is not 32B-aligned, thereby enabling efficient continuous unaligned data transfer.

- **UnalignRegForLoad** is the unaligned load-in register. When reading an unaligned address, first initialize it through **LoadUnAlignPre** (temporarily storing the unaligned address data into **UnalignRegForLoad**), and then call **LoadUnAlign** to perform the transfer.

- **UnalignRegForStore** is the unaligned store-out register. When writing an unaligned address, first call **StoreUnAlign**, and then use **StoreUnAlignPost** for post-processing (writing the data in **UnalignRegForStore** to the unaligned UB address).

For details about how to use **UnalignRegForLoad** and **UnalignRegForStore**, see [Continuous unaligned load (LoadUnAlign)](../reg_data_load/LoadUnAlign_continuous.md) and [Continuous unaligned store (StoreUnAlign)](../reg_data_store/StoreUnAlign_continuous.md).

## Constraints<a name="section11585101304320"></a>

- The register count limit of **UnalignRegForLoad** is 4. If the limit is exceeded, the compiler reports an error.

- The register count limit of **UnalignRegForStore** is 4. If the limit is exceeded, the compiler reports an error.
