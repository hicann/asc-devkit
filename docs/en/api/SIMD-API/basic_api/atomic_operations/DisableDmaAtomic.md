# DisableDmaAtomic<a name="ZH-CN_TOPIC_0000001839732381"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T10:23:49.594Z pushedAt=2026-10-26T07:41:43.952Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id8 -->
- Ascend 950PR&950DT products: supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 products: supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 products: supported
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: supported
<!-- end id14 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/atomic_operations/DisableDmaAtomic_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_set_atomic_intf.h"`.

Disables the atomic operation attached to data movement. When data movement is subsequently performed, the original data in GM is completely overwritten by the newly moved data.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void DisableDmaAtomic()
```

## Parameters<a name="section622mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

After [SetAtomicAdd](SetAtomicAdd.md), [SetAtomicMax](SetAtomicMax_ISASI.md), and [SetAtomicMin](SetAtomicMin_ISASI.md) are used, clear the atomic operation state through DisableDmaAtomic to avoid affecting the functions of subsequent related instructions.

## Example<a name="section177231425115410"></a>

```cpp
AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
AscendC::LocalTensor<T> srcLocal = ubAllocator.Alloc<T, SIZE>();

AscendC::DisableDmaAtomic();
AscendC::DataCopy(srcLocal, srcGlobal, SIZE);
AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
AscendC::SyncAll();

// Enable atomic add to atomically add Unified Buffer (UB) data to GM.
AscendC::SetAtomicAdd<T>();
AscendC::DataCopy(dstGlobal, srcLocal, SIZE);

// Disable atomic add.
AscendC::DisableDmaAtomic();
```

For the complete sample, see the [DataMovementWithAtomicOperations sample](../../../../../../examples/01_simd_cpp_api/03_basic_api/06_atomic/data_movement_with_atomic_operations/data_movement_with_atomic_operations.asc).
