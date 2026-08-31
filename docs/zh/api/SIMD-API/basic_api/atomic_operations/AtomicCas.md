# AtomicCas<a name="ZH-CN_TOPIC_0000002568264812"></a>

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/atomic_operations/AtomicCas_res.md#id1 -->

## 功能说明<a name="section163431416121118"></a>

头文件路径为：`"basic_api/kernel_operator_atomic_intf.h"`。

调用该接口后，可在指定GM地址上进行原子比较操作，读取**address**指向的GM地址上的旧值（**old_value**）与输入标量值**value1**进行比较：

- 如果相等，则将输入标量值**value2**写入GM地址；
- 如果不相等，则GM地址上的值保持不变。
返回该地址修改前的值（**old_value**）。

计算公式如下：

$$
new\_value = (old\_value == value1\ ?\ value2\ :\ old\_value)
$$

## 函数原型<a name="section1171112415119"></a>

```cpp
template <typename T>
__aicore__ inline T AtomicCas(__gm__ T *address, T value1, T value2)
```

## 参数说明<a name="section086962912112"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| address | 输入 | 输入GM的地址。 |
| value1/value2 | 输入 | 标量值，支持数据类型和address指向的数据类型保持一致。 |

## 数据类型<a name="section832119018128"></a>

address、value1/value2、返回值的数据类型相同，支持的数据类型为uint32_t、uint64_t。

## 返回值说明<a name="section97313820116"></a>

GM地址上做原子操作前的数据。

## 约束说明<a name="section24727811123"></a>

- 毕昇编译器能够在一部分属于PIPE_S的接口与属于PIPE_V、PIPE_MTE2、PIPE_MTE3的接口之间自动插入同步（详见[使用约束](../sync_control/intra_core_sync/key_features.md#使用约束-1)）。但标量原子操作接口均不在该范围内，因此AtomicCas与搬运接口（属于PIPE_MTE2、PIPE_MTE3）在读写GM时存在数据依赖时，毕昇编译器无法自动插入同步，开发者需根据实际情况手动插入同步。
- Scalar原子操作会绕过DCache，需要调用[DataCacheCleanAndInvalid](../cache_control/DataCacheCleanAndInvalid.md)接口确保GM与DCache的一致性。

## 调用示例<a name="section191505489122"></a>

```cpp
AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
AscendC::LocalTensor<uint32_t> yLocal = ubAllocator.Alloc<uint32_t>(DATA_SIZE);

AscendC::GlobalTensor<uint32_t> yGlobal;
yGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ uint32_t*>(y), DATA_SIZE);

// AtomicCas：三个核分别对GM首个元素原子比较交换。
uint32_t expected = 1;
uint32_t newValue = 2;
AscendC::AtomicCas(reinterpret_cast<__gm__ uint32_t*>(y), expected, newValue);

// 原子操作后插入同步。
AscendC::SetFlag<AscendC::HardEvent::S_MTE2>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::S_MTE2>(EVENT_ID0);
AscendC::DataCopy(yLocal, yGlobal, DATA_SIZE);
```

完整样例请参考[scalar_atomic_operations样例](../../../../../../examples/01_simd_cpp_api/03_basic_api/06_atomic/scalar_atomic_operations)。
