# Reduce<a name="ZH-CN_TOPIC_0000001938595870"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reduction_compute/Reduce_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_vec_reduce_intf.h"`。

归约指令根据ReduceType，将数据集合简化为单一值或者更小的集合。

ReduceType::SUM：根据mask，计算源操作数srcReg内有效元素的数据总和，将结果写入目的操作数dstReg。

ReduceType::MAX：根据mask，计算源操作数srcReg内有效元素中的最大值和索引，将结果写入目的操作数dstReg，当存在多个最大值时，会将第一个最大值的索引保存在dstReg中。

ReduceType::MIN：根据mask，计算源操作数srcReg内有效元素中的最小值和索引，将结果写入目的操作数dstReg，当存在多个最小值时，会将第一个最小值的索引保存在dstReg中。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <ReduceType type = ReduceType::SUM, typename T = DefaultType, typename U = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S, typename V>
__simd_callee__ inline void Reduce(S& dstReg, V srcReg, MaskReg mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| type | ReduceType类型，支持SUM、MAX、MIN。<br>enum class ReduceType {<br>    SUM = 0,<br>    MAX,<br>    MIN,<br>}; |
| T | 目的操作数dstReg的数据类型。 |
| U | 源操作数srcReg的数据类型。 |
| mode | [MaskMergeMode](../aux_data_types/MaskMergeMode.md)，选择MERGING模式或ZEROING模式。<br>&bull; ZEROING，mask未选中的元素在dst中置零。<br>&bull; MERGING，当前不支持。 |
| S | 目的操作数的RegTensor类型，例如RegTensor&lt;half&gt;，由编译器自动推导，用户不需要填写。 |
| V | 源操作数的RegTensor类型，例如RegTensor&lt;half&gt;，由编译器自动推导，用户不需要填写。 |

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
| int32_t | int16_t |
| uint32_t | uint16_t |
| int32_t | int32_t |
| uint32_t | uint32_t |
| float | float |
| int64_t | int64_t |
| uint64_t | uint64_t |

**表4** ReduceType::MAX或ReduceType::MIN数据类型支持情况

| dstReg | srcReg |
| --- | --- |
| int16_t | int16_t |
| uint16_t | uint16_t |
| half | half |
| int32_t | int32_t |
| uint32_t | uint32_t |
| float | float |
| int64_t | int64_t |
| uint64_t | uint64_t |

## 返回值说明

无

## 约束说明<a name="section177921451558"></a>

- 对于归约求数据总和，mask指定源操作数是否参与计算，不参与计算的元素，被当作0进行处理。
- 对于归约求最大值，mask指定源操作数是否参与计算，不参与计算的元素，被当作数据类型的最小值进行处理。
- 对于归约求最小值，mask指定源操作数是否参与计算，不参与计算的元素，被当作数据类型的最大值进行处理。
- 对于归约求最大值，当所有元素均不参与计算时，将该数据类型的最小值写入dstReg，当存在多个最大值时，会将第一个最大值的索引保存在dstReg中，max\(-0, +0\) = +0。
- 对于归约求最小值，当所有元素均不参与计算时，将该数据类型的最大值写入dstReg，当存在多个最小值时，会将第一个最小值的索引保存在dstReg中，min\(-0, +0\) = -0。
- 当归约求最小值或者归约求最大值时，源操作数的数据类型和目的操作数相同。
- mask为空场景及异常值处理参考[mask为空场景及异常值处理](#mask为空场景及异常值处理)。
- 指令内累加顺序采用二叉树累加方式，规则参考[ReduceType::SUM累加顺序](#ReduceType::SUM累加顺序)。

## 关键特性<a name="zh-cn_topic_0000002537812548_section18972943153217"></a>

### 索引值需要强制类型转换

dstReg的最大值/最小值索引按照dstReg的数据类型存储，比如dstReg为half类型时，索引按照half类型存储，因此读取索引需要使用reinterpret\_cast方法转换到整数类型。若数据类型是half，需要使用reinterpret\_cast\<uint16_t\*\>；若数据类型是float，需要使用reinterpret\_cast\<uint32\_t\*\>。

例如，输入数据是half类型，计算结果为\[1.14, 6.8e-06\]，6.8e-06需要使用reinterpret\_cast<uint16\_t\*\>方法转换得到索引值114。转换伪代码示例如下：

```cpp
__simd_vf__ inline void ReduceVF(__ubuf__ half* dstAddr, __ubuf__ half* srcAddr, uint32_t count)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;    
    AscendC::Reg::LoadAlign(srcReg, srcAddr);
    mask = AscendC::Reg::UpdateMask<T>(count);
    AscendC::Reg::Reduce<AscendC::Reg::ReduceType::MAX>(dstReg, srcReg, mask); 
    AscendC::Reg::StoreAlign(dstAddr, dstReg, mask);
} // End of Vector Function
```

```cpp
// Index强制类型转换保存到Scalar，需要放Vector Function外处理
AscendC::PipeBarrier<PIPE_V>(); // 需要插入同步，等待归约计算完后才能处理索引
half maxIndex = *(dstAddr + 1);
uint16_t realIndex = *reinterpret_cast<uint16_t*>(&maxIndex);
```

![reg_reduce_sum](../../../../figures/reg_reduce_sum.png)

### ReduceType::SUM累加顺序<a id="ReduceType::SUM累加顺序"></a>

以二叉树累加的方式计算源操作数srcReg内有效元素的数据总和。

以half类型的数据求和为例，在srcReg内有128个数，通过二叉树的方式，两两相加，计算过程如下图所示：

1. data1和data2相加得到data01，data3和data4相加得到data02，……，data125和data126相加得到data63，data127和data128相加得到data64；
2. data01和data02相加得到data001，data03和data04相加得到data002，……，data63和data64相加得到data032；
3. 以此类推，得到目的操作数为1个half类型的数据sum。

**图1** reg_reduce_index<a name="zh-cn_topic_0000002537812548_fig1630121573314"></a>  
![reg_reduce_index](../../../../figures/reg_reduce_index.png "reg_reduce_index")

### mask为空场景及异常值处理

- 对于归约求数据总和，当所有元素均不参与计算时，将目的操作数数据类型的0写入dstReg。
- 对于归约求最大值，当所有元素均不参与计算时，将该数据类型的最小值写入dstReg。
- 对于归约求最小值，当所有元素均不参与计算时，将该数据类型的最大值写入dstReg。
- 对于归约求最大值/最小值，如果输入数据存在nan，将该数据类型的nan写入dstReg，将第一个nan的索引保存在dstReg中。

### 提取MAX/MIN结果（值+索引）

MAX/MIN归约产生两个有效结果，需要分别提取。以下示例中dstAddr为32B对齐地址，如果需要写出到非对齐地址，请参考[连续非对齐搬出\(StoreUnAlign\)](../reg_data_store/StoreUnAlign_continuous.md)。

```cpp
AscendC::Reg::Reduce<AscendC::Reg::ReduceType::MAX>(dstReg, srcReg, pregFull); 
```

方式一：整体存出后按偏移访问（最直接，适用于需要同时使用值和索引的场景）

```cpp
AscendC::Reg::MaskReg pregVL2 = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::VL2>();
// 后续直接访问// dstAddr[0] -> 最大值// dstAddr[1] -> 最大值索引
AscendC::Reg::StoreAlign(dstAddr, dstReg, pregVL2);  
```

方式二：用Mask分离到目标地址

```cpp
AscendC::Reg::MaskReg pregVL1 = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::VL1>();
// 只搬出最大值到目标地址
AscendC::Reg::StoreAlign(dstAddr, dstReg, pregVL1); 
```

## 调用示例<a name="section642mcpsimp"></a>

- 归约求和：

    ```cpp
    template<typename T, typename U>
    __simd_vf__ inline void ReduceVF(__ubuf__ T* dstAddr, __ubuf__ U* srcAddr, uint32_t count, 
     uint32_t srcRepeatSize, uint32_t dstRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<U> srcReg;
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            AscendC::Reg::LoadAlign(srcReg, srcAddr + i * srcRepeatSize);
            mask = AscendC::Reg::UpdateMask<U>(count);
            AscendC::Reg::Reduce<AscendC::Reg::ReduceType::SUM>(dstReg, srcReg, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * dstRepeatSize, dstReg, mask);
        }
    }
    ```

- 归约求最大值或者最小值：

    ```cpp
    template<typename T>
    __aicore__ inline void ReduceVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
            mask = AscendC::Reg::UpdateMask<T>(count);
            // type = ReduceType::MAX
            AscendC::Reg::Reduce<AscendC::Reg::ReduceType::MAX>(dstReg, srcReg, mask);
            // type = ReduceType::MIN
            // AscendC::Reg::Reduce<AscendC::Reg::ReduceType::MIN>(dstReg, srcReg, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```
