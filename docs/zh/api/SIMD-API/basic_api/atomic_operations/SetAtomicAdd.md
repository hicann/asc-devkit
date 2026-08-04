# SetAtomicAdd<a name="ZH-CN_TOPIC_0000001487536650"></a>

## 产品支持情况

<!-- npu="950" id13 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id13 -->
<!-- npu="A3" id14 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id14 -->
<!-- npu="910b" id15 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id15 -->
<!-- npu="310b" id16 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id16 -->
<!-- npu="310p" id17 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id17 -->
<!-- npu="310p" id18 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id18 -->
<!-- npu="910" id19 -->
- Atlas 训练系列产品：支持
<!-- end id19 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_set_atomic_intf.h"`。

对后续目的地址为GM的数据搬运开启原子累加。原子累加过程：将待拷贝的内容和GM已有内容进行求和，然后将求和结果写入GM。SetAtomicAdd接口可通过模板参数设定不同的累加数据类型。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void SetAtomicAdd()
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
|--------|------|
| T | 设定不同的原子累加操作数据类型。 |

## 数据类型

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT，支持的数据类型为int8_t、int16_t、half、bfloat16_t、int32_t、float。
<!-- end id1 -->

<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为int8_t、int16_t、half、bfloat16_t、int32_t、float。
<!-- end id2 -->

<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为int8_t、int16_t、half、bfloat16_t、int32_t、float。
<!-- end id3 -->

<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品，支持的数据类型为int16_t、half、int32_t、float。
<!-- end id4 -->

<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core，支持的数据类型为int16_t、half、float。
<!-- end id5 -->

<!-- npu="910" id6 -->
- Atlas 训练系列产品，支持的数据类型为float。
<!-- end id6 -->

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

<!-- npu="950" id7 -->
- Ascend 950PR/Ascend 950DT，支持的数据通路为UB/L0C Buffer-\>GM。
<!-- end id7 -->

<!-- npu="A3" id8 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据通路为UB/L0C Buffer/L1 Buffer-\>GM。
<!-- end id8 -->

<!-- npu="910b" id9 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据通路为UB/L0C Buffer/L1 Buffer-\>GM。
<!-- end id9 -->

<!-- npu="310b" id10 -->
- Atlas 200I/500 A2 推理产品，支持的数据通路为UB/L0C Buffer/L1 Buffer-\>GM。
<!-- end id10 -->

<!-- npu="310p" id11 -->
- Atlas 推理系列产品AI Core，支持的数据通路为UB-\>GM。
<!-- end id11 -->

<!-- npu="910" id12 -->
- Atlas 训练系列产品，支持的数据通路为UB-\>GM。
<!-- end id12 -->

- 累加操作完成后，建议通过[DisableDmaAtomic](DisableDmaAtomic.md)关闭原子累加，以免影响后续相关指令功能。
- 该接口执行前不会自动清零GM数据。开发者需根据算子逻辑判断是否清零，如需清零，请在执行前手动完成。
- SetAtomicAdd内部已集成与[SetAtomicType](SetAtomicType.md)相同的功能。建议调用上述接口时，通过设置模板参数显式指定原子操作的数据类型，无需额外调用SetAtomicType接口。

## 调用示例<a name="section177231425115410"></a>

```cpp
AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
AscendC::LocalTensor<T> srcLocal = ubAllocator.Alloc<T, SIZE>();

AscendC::DisableDmaAtomic();
AscendC::DataCopy(srcLocal, srcGlobal, SIZE);
AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
AscendC::SyncAll();

// 开启原子累加，将UB数据原子累加到GM
AscendC::SetAtomicAdd<T>();
AscendC::DataCopy(dstGlobal, srcLocal, SIZE);

// 关闭原子累加
AscendC::DisableDmaAtomic();
```

完整样例请参考[DataMovementWithAtomicOperations样例](../../../../../../examples/01_simd_cpp_api/03_basic_api/06_atomic/data_movement_with_atomic_operations/data_movement_with_atomic_operations.asc)。
