# Histograms<a name="ZH-CN_TOPIC_0000002079418145"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/histogram_compute/Histograms_res.md#id1 -->


## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_histograms_intf.h"`。

对直方图数据进行统计，在目的操作数dstReg的基础数据上加上源操作数srcReg数据的统计结果，包括数据的频率统计和累计统计。

- 频率统计
    如下图所示，dstReg中的第n个元素表示srcReg中数值n的出现次数，并在原始dstReg数据的基础上进行累加。在低位模式下，dstReg用于统计srcReg中index为[0, 127]范围内（前半部分）各个值的出现频率，例如dst[i]表示数值i在src的[0, 127]中出现的次数；而在高位模式下，dstReg则统计[128, 255]范围内（后半部分）的频率，例如dst[i]表示数值i在src的[128, 255]中出现的次数。

    **图 1**  频率统计  
    ![频率统计](../../../../figures/reg_histograms_1.png)

- 累计统计  
    如下图所示，dstReg中的第n个元素表示srcReg中从0到n的所有数值在对应区间中出现的总频率。最终，统计结果会在目的寄存器原始数据的基础上进行累加。在低位模式下，目的寄存器dstReg会统计源寄存器srcReg中值落在低位区间[0, 127]的数据分布情况，例如dst[i]表示数值0~i在src的[0, 127]中出现的次数；在高位模式下，目的寄存器dstReg则会统计srcReg中值落在高位区间[128, 255]的数据分布情况，例如dst[i]表示数值0~i在src的[128, 255]中出现的次数。

    **图 2**  累计统计  
    ![累计统计](../../../../figures/reg_histograms_2.png)

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U = DefaultType, HistogramsBinType mode, HistogramsType type, typename S, typename V>
__simd_callee__ inline void Histograms(V& dstReg, S& srcReg, MaskReg& mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名称 | 描述 |
| ------ | ------ |
| T | 源操作数的数据类型。 |
| U | 目的操作数的数据类型。 |
| mode | HistogramsBinType枚举类型，用于选择统计srcReg的低位区间或高位区间。Histograms将uint8_t类型srcReg的[0, 255]取值范围划分为低位、高位两个区间；每个区间包含128个取值，对应uint16_t类型dstReg在一个VL（256Byte）内可保存的128个统计结果。单次调用只统计其中一个区间，并将该区间的统计结果写入传入的dstReg。若需要获得完整[0, 255]范围的统计结果，需要分别以BIN0和BIN1调用两次接口，并使用两个目的寄存器保存结果，例如dst0保存低位区间结果，dst1保存高位区间结果。<br>• BIN0：统计srcReg中[0, 127]范围内的数据，结果写入传入的dstReg。<br>• BIN1：统计srcReg中[128, 255]范围内的数据，结果写入传入的dstReg。 |
| type | HistogramsType 枚举类型，表示统计模式。<br>• FREQUENCY：频率统计模式，统计srcReg中[0, 255]每个数的数量。每个dst有128个元素，其中低位模式dstReg中每个元素对应src中[0, 127]每个元素的累加个数，高位模式dstReg中每个元素对应src中[128,255]每个元素的累加个数。<br>• ACCUMULATE：累计统计模式，统计srcReg中[0, 255]每个数及其之前出现数的数量总和。每个dst有128个元素，其中低位模式dstReg中每个元素对应src中[0, 127]每个元素区间累加个数，高位模式dstReg中每个元素对应src中[128,255]每个元素区间累加个数。 |
| S | 源操作数RegTensor类型，由编译器自动推导，用户不需要填写。 |
| V | 目的操作数RegTensor类型，由编译器自动推导，用户不需要填写。 |

**表2**  参数说明

| 参数名称 | 输入/输出 | 描述 |
| ------ | ------ | ------ |
| dstReg | 输入/输出 | 目的操作数。<br>类型为[RegTensor](../register_data_types/RegTensor.md)。 |
| srcReg | 输入 | 源操作数。<br>类型为[RegTensor](../register_data_types/RegTensor.md)。 |
| mask | 输入 | 源操作数元素操作的有效指示，详细说明请参考[MaskReg](../register_data_types/MaskReg.md)。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 数据类型

**表 3**  源操作数和目的操作数的数据类型对应表<a id="table3"></a>  

| T数据类型 | U数据类型 |
| ------ | ------ |
| uint8_t | uint16_t |

## 约束说明<a name="section633mcpsimp"></a>

- 当mask位数为0时，对应位置的src源操作数的数值被忽略，dst对应位置数值为忽略该位置src后计算得到的值。
- dst的数据类型为uint16_t，最大值为65535，使用时需注意累加溢出问题。

## 调用示例<a name="section642mcpsimp"></a>

```cpp
template <typename T, typename U, AscendC::Reg::HistogramsBinType mode, AscendC::Reg::HistogramsType type>
__simd_vf__ inline void HistogramsVF(__ubuf__ U* dstAddr, __ubuf__ T* srcAddr, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<U> dstReg;
    AscendC::Reg::MaskReg mask0 = AscendC::Reg::CreateMask<T>();
    AscendC::Reg::MaskReg mask1 = AscendC::Reg::CreateMask<T>();
    AscendC::Reg::Duplicate<U>(dstReg, 0);
    for (uint16_t i = 0; i < repeatTimes; ++i){
        AscendC::Reg::LoadAlign(srcReg, srcAddr + oneRepeatSize * i);
        AscendC::Reg::Histograms<T, U, mode, type>(dstReg, srcReg, mask0);
    }
    AscendC::Reg::StoreAlign(dstAddr, dstReg, mask1);
}
```
