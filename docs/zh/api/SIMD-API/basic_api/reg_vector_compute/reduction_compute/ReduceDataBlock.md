# ReduceDataBlock<a name="ZH-CN_TOPIC_0000001938755198"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reduction_compute/ReduceDataBlock_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

归约指令根据ReduceType，将数据集合在每个DataBlock（32B）内简化为单一值。

当ReduceType为SUM，将每个DataBlock（32B）中参与计算的元素相加，并将最终的计算结果依次保存在dstReg的最低位。

当ReduceType为MAX，将每个DataBlock（32B）中的最大值依次保存在dstReg中的最低位。

当ReduceType为MIN，将每个DataBlock（32B）中的最小值依次保存在dstReg中的最低位。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <ReduceType type = ReduceType::SUM, typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void ReduceDataBlock(U& dstReg, U srcReg, MaskReg mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| type | ReduceType类型，支持SUM、MAX、MIN。<br>enum class ReduceType {<br>    SUM = 0,<br>    MAX,<br>    MIN,<br>}; |
| T | 目的操作数和源操作数的数据类型。 |
| mode | [MaskMergeMode](../aux_data_types/MaskMergeMode.md)，选择MERGING模式或ZEROING模式。当前仅支持ZEROING模式。<br>&bull; ZEROING，mask未筛选的元素在dst中置零。<br>&bull; MERGING，当前不支持。 |
| U | 目的操作数和源操作数的RegTensor类型，例如RegTensor&lt;int32_t&gt;，由编译器自动推导，用户不需要填写。 |

**表2** 函数参数说明

| 参数名 | 描述 |
| --- | --- |
| dstReg | 目的操作数。<br>类型为[RegTensor](../register_data_types/RegTensor.md)。 |
| srcReg | 源操作数。<br>类型为[RegTensor](../register_data_types/RegTensor.md)。 |
| mask | 源操作数元素操作的有效指示，详细说明请参考[MaskReg](../register_data_types/MaskReg.md)。 |

## 数据类型

**表3** ReduceType::SUM数据类型支持情况

| dstReg | srcReg |
| --- | --- |
| half | half |
| int32_t | int32_t |
| int32_t | int16_t |
| uint32_t | uint32_t |
| uint32_t | uint16_t |
| float | float |

**表4** ReduceType::MAX或ReduceType::MIN数据类型支持情况

| dstReg | srcReg |
| --- | --- |
| half | half |
| int16_t | int16_t |
| uint16_t | uint16_t |
| int32_t | int32_t |
| uint32_t | uint32_t |
| float | float |

## 返回值说明

无

## 约束说明<a name="section177921451558"></a>

- 对于归约求最大值，max\(-0, +0\) = +0。
- 对于归约求最小值，min\(-0, +0\) = -0。
- 对于归约求数据总和，mask指定源操作数是否参与计算，不参与计算的元素，被当作0进行处理。
- 对于归约求最大值，mask指定源操作数是否参与计算，不参与计算的元素，被当作数据类型的最小值进行处理。
- 对于归约求最小值，mask指定源操作数是否参与计算，不参与计算的元素，被当作数据类型的最大值进行处理。
- mask为空场景及异常值处理参考[mask为空场景及异常值处理](#mask为空场景及异常值处理)。
- 指令内累加顺序采用二叉树累加方式，规则参考[ReduceType::SUM累加顺序](#ReduceType::SUM累加顺序)。

## 关键特性<a name="zh-cn_topic_0000002568852247_section18972943153217"></a>

### ReduceType::SUM累加顺序<a id="ReduceType::SUM累加顺序"></a>

以二叉树累加的方式计算每个DataBlock内的和。

以half类型的数据求和为例，在每个DataBlock内有16个数，通过二叉树的方式，两两相加，计算过程如下图所示：

1. data1和data2相加得到data01，data3和data4相加得到data02，……，data13和data14相加得到data07，data15和data16相加得到data08；
2. data01和data02相加得到data001，data03和data04相加得到data002，……，data07和data08相加得到data004；
3. 以此类推，得到目的操作数为1个half类型的数据sum。

**图1** ReduceDataBlock示意图<a name="zh-cn_topic_0000002568852247_fig10524085414"></a>  
![ReduceDataBlock示意图](../../../../figures/reg_reduce_datablock_index.png "ReduceDataBlock示意图")

### mask为空场景及异常值处理

- 对于归约求数据总和，当DataBlock中的元素均不参与计算时，将目的操作数数据类型的0写入dstReg。
- 对于归约求最大值，当DataBlock中的元素均不参与计算时，将该数据类型的最小值写入dstReg。
- 对于归约求最小值，当DataBlock中的元素均不参与计算时，将该数据类型的最大值写入dstReg。
- 对于归约求最大值/最小值，如果DataBlock中存在nan，将该数据类型的nan写入dstReg。

## 调用示例<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void ReduceDataBlockVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, 
 uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        mask = AscendC::Reg::UpdateMask<T>(count);
        // type = ReduceType::SUM
        AscendC::Reg::ReduceDataBlock<AscendC::Reg::ReduceType::SUM>(dstReg, srcReg, mask);
        // type = ReduceType::MAX
        // AscendC::Reg::ReduceDataBlock<AscendC::Reg::ReduceType::MAX>(dstReg, srcReg, mask);
        // type = ReduceType::MIN
        // AscendC::Reg::ReduceDataBlock<AscendC::Reg::ReduceType::MIN>(dstReg, srcReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask); 
    }
}
```
