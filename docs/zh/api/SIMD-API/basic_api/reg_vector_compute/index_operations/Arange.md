# Arange<a name="ZH-CN_TOPIC_0000001960926493"></a>

## 产品支持情况<a name="section1550532418810"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/index_operations/Arange_res.md#id1 -->


## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_vec_arange_intf.h"`。

递增模式时，该函数以传入的scalarValue的值为起始值，生成递增的索引；递减模式时，该函数以传入的scalarValue的值为终止值，生成递减的索引，并将索引保存在dstReg中。

**图 1**  Arange递增示意图

![Arange示意图](../../../../figures/reg_arange_in.png)

**图 2**  Arange递减示意图

![Arange示意图](../../../../figures/reg_arange_de.png)

## 函数原型<a name="section520771712327"></a>

```cpp
template <typename T = DefaultType, IndexOrder order = IndexOrder::INCREASE_ORDER, typename U, typename S>
__simd_callee__ inline void Arange(S& dstReg, U scalarValue);
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名称 | 描述 |
| ------ | ------ |
| T | 目的操作数的数据类型。 |
| IndexOrder | 递增/递减模式。支持如下取值：<br>• INCREASE_ORDER：递增模式<br>• DECREASE_ORDER：递减模式 |
| U | 起始索引值的数据类型。 |
| S | 目的操作数的RegTensor类型，例如`RegTensor<half>`，由编译器自动推导，无需用户显式指定。 |

**表2**  函数参数说明

| 参数名称 | 输入/输出 | 描述 |
| ------ | ------ | ------ |
| dstReg | 输出 | 目的操作数。<br>类型为[RegTensor](../register_data_types/RegTensor.md)。 |
| scalarValue | 输入 | 源操作数。<br>类型为标量。 |

## 数据类型

支持的数据类型为：int8_t/int16_t/int32_t/half/float/int64_t。

## 约束说明<a name="section177921451558"></a>

对于int8_t/int16_t/int32_t/int64_t，当索引超过该数据类型的边界值时会翻转为负数，如对于int8_t，127之后为-128、-127，以此类推。

## 调用示例<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void ArangeVF(__ubuf__ T* dstAddr, T scalarValue, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    mask = AscendC::Reg::CreateMask<T>();
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::Arange(dstReg, scalarValue);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
