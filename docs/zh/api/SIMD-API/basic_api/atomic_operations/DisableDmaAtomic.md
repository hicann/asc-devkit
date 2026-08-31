# DisableDmaAtomic<a name="ZH-CN_TOPIC_0000001839732381"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas 训练系列产品：支持
<!-- end id14 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/atomic_operations/DisableDmaAtomic_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_set_atomic_intf.h"`。

关闭数据搬运随路原子操作功能，后续执行数据搬运时，GM中原始数据将被新搬运数据完全覆盖。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void DisableDmaAtomic()
```

## 参数说明<a name="section622mcpsimp"></a>

无

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

[SetAtomicAdd](SetAtomicAdd.md)、[SetAtomicMax](SetAtomicMax_ISASI.md)、[SetAtomicMin](SetAtomicMin_ISASI.md)使用完成后，通过DisableDmaAtomic清空原子操作的状态，以免影响后续相关指令功能。

## 调用示例<a name="section177231425115410"></a>

```cpp
AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
AscendC::LocalTensor<T> srcLocal = ubAllocator.Alloc<T, SIZE>();

AscendC::DisableDmaAtomic();
AscendC::DataCopy(srcLocal, srcGlobal, SIZE);
AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
AscendC::SyncAll();

// 开启原子累加，将Unified Buffer（UB）数据原子累加到GM
AscendC::SetAtomicAdd<T>();
AscendC::DataCopy(dstGlobal, srcLocal, SIZE);

// 关闭原子累加
AscendC::DisableDmaAtomic();
```

完整样例请参考[DataMovementWithAtomicOperations样例](../../../../../../examples/01_simd_cpp_api/03_basic_api/06_atomic/data_movement_with_atomic_operations/data_movement_with_atomic_operations.asc)。
