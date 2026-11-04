# Histograms<a name="ZH-CN_TOPIC_0000002079418145"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:16:16.771Z pushedAt=2026-10-26T07:41:44.523Z nl=2 -->

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
- Atlas 200I/500 A2 inference product: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/histogram_compute/Histograms_res.md#id1 -->


## Description<a name="section618mcpsimp"></a>

The header file path is: `"basic_api/reg_compute/kernel_reg_compute_histograms_intf.h"`.

Performs statistics on histogram data by adding the statistical result of the source operand **srcReg** data to the base data of the destination operand **dstReg**, including frequency statistics and cumulative statistics of the data.

- Frequency statistics
    As shown in the following figure, the n-th element in **dstReg** represents the count of occurrences of the value n in **srcReg**, and it is accumulated on top of the original **dstReg** data. In low bin mode, **dstReg** is used to count the frequency of each value in the range [0, 127] (the first half) of **srcReg**; for example, dst[i] represents the count of occurrences of the value i in [0, 127] of src. In high bin mode, **dstReg** counts the frequency in the range [128, 255] (the second half); for example, dst[i] represents the count of occurrences of the value i in [128, 255] of src.

    **Figure 1**  Frequency statistics  
    ![Frequency statistics](../../../../figures/reg_histograms_1.png)

- Cumulative statistics  
    As shown in the following figure, the n-th element in **dstReg** represents the total frequency of all values from 0 to n in **srcReg** within the corresponding range. Finally, the statistical result is accumulated on top of the original data of the destination register. In low bin mode, the destination register **dstReg** counts the data distribution of values in the source register **srcReg** that fall within the low range [0, 127]; for example, dst[i] represents the count of occurrences of the values 0 to i in [0, 127] of src. In high bin mode, the destination register **dstReg** counts the data distribution of values in **srcReg** that fall within the high range [128, 255]; for example, dst[i] represents the count of occurrences of the values 0 to i in [128, 255] of src.

    **Figure 2**  Cumulative statistics  
    ![Cumulative statistics](../../../../figures/reg_histograms_2.png)

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U = DefaultType, HistogramsBinType mode, HistogramsType type, typename S, typename V>
__simd_callee__ inline void Histograms(V& dstReg, S& srcReg, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ------ | ------ |
| T | Data type of the source operand. |
| U | Data type of the destination operand. |
| mode | Enumeration of the **HistogramsBinType** type, used to select the low bin or high bin of **srcReg** for statistics. **Histograms** divides the [0, 255] value range of the **uint8_t** type **srcReg** into a low bin and a high bin. Each bin contains 128 values, corresponding to the 128 statistical results that the **uint16_t** type **dstReg** can hold within one VL (256 bytes). A single call counts only one bin and writes the statistical results of that bin to the passed-in **dstReg**. To obtain the complete statistical results over the [0, 255] range, call the API twice with **BIN0** and **BIN1** respectively and use two destination registers to hold the results, for example, **dst0** holds the low bin results and **dst1** holds the high bin results.<br>• **BIN0**: Counts the data in the [0, 127] range of **srcReg** and writes the results to the passed-in **dstReg**.<br>• **BIN1**: Counts the data in the [128, 255] range of **srcReg** and writes the results to the passed-in **dstReg**. |
| type | Enumeration of the **HistogramsType** type, representing the statistics mode.<br>• **FREQUENCY**: Frequency statistics mode, which counts the number of occurrences of each value in the [0, 255] range of **srcReg**. Each dst has 128 elements. In low bin mode, each element of **dstReg** corresponds to the accumulated count of each element in the [0, 127] range of srcReg. In high bin mode, each element of **dstReg** corresponds to the accumulated count of each element in the [128, 255] range of src.<br>• **ACCUMULATE**: Cumulative statistics mode, which counts the cumulative total of each value in the [0, 255] range of **srcReg** and all values before it. Each dst has 128 elements. In low bin mode, each element of **dstReg** corresponds to the accumulated count for each element interval in the [0, 127] range of src. In high bin mode, each element of **dstReg** corresponds to the accumulated count for each element interval in the [128, 255] range of src. |
| S | **RegTensor** type of the source operand, automatically deduced by the compiler. The user does not need to specify it. |
| V | **RegTensor** type of the destination operand, automatically deduced by the compiler. The user does not need to specify it. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| dstReg | Input/Output | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| mask | Input | Valid indicator for the operation on the source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Return Value<a name="section640mcpsimp"></a>

None

## Data Types

**Table 3**  Data type mapping between source operands and destination operands<a id="table3"></a>  

| T Data Type | U Data Type |
| ------ | ------ |
| uint8_t | uint16_t |

## Constraints<a name="section633mcpsimp"></a>

- When the number of mask bits is 0, the value of the **src** source operand at the corresponding position is ignored, and the value at the corresponding position of **dst** is the value calculated after ignoring the **src** at that position.
- The data type of **dst** is **uint16_t**, with a maximum value of 65535. Pay attention to accumulation overflow during use.

## Example<a name="section642mcpsimp"></a>

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
