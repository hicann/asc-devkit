# DisableDmaAtomic<a name="ZH-CN_TOPIC_0000001839732381"></a>

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T02:12:50.301Z pushedAt=2026-11-18T01:25:06.583Z nl=2 -->

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

Disables the on-the-fly atomic operation function of data movement. When data movement is subsequently executed, the original data in GM will be completely overwritten by the newly moved data.

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

// Enable atomic accumulation to atomically accumulate Unified Buffer (UB) data to GM.
AscendC::SetAtomicAdd<T>();
AscendC::DataCopy(dstGlobal, srcLocal, SIZE);

// Disable atomic accumulation.
AscendC::DisableDmaAtomic();
```

For the complete sample, see [DataMovementWithAtomicOperations sample](../../../../../../examples/01_simd_cpp_api/03_basic_api/06_atomic/data_movement_with_atomic_operations/data_movement_with_atomic_operations.asc).
