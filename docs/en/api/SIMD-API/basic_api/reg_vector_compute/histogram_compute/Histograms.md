# Histograms<a name="ZH-CN_TOPIC_0000002079418145"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T22:23:56.023Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

Performs statistics on histogram data by adding the statistical results of the source operand **srcReg** data to the base data of the destination operand **dstReg**, including frequency statistics and cumulative statistics of the data.

-   Frequency statistics

    As shown in the following figure, in low-bit mode, **dstReg** (that is, **dst0**) is used to count the occurrence frequency of each value in the range [0-127] (the first half) in **srcReg**; in high-bit mode, **dstReg** (that is, **dst1**) counts the frequency in the range [128-255] (the second half). The n-th bit in **dst0** and **dst1** indicates the occurrence count of value n in **srcReg**, and the result is accumulated on top of the original **dstReg** data.

    **Figure 1**  Frequency statistics<a name="fig1436621813216"></a>  
    ![](../../../../figures/frequency_statistics.png "Frequency statistics")

-   Cumulative statistics

    As shown in the following figure, in low-bit mode, the destination register **dstReg** (that is, **dst0**) counts the distribution of data in **srcReg** whose values fall within the low range [0-127]; in high-bit mode, the destination register **dstReg** (that is, **dst1**) counts the distribution of data in **srcReg** whose values fall within the high range [128-255]. In **dst0** and **dst1**, the data at the n-th bit indicates the total frequency of all values from 0 to n in **srcReg** within the corresponding range. Finally, the statistical result is accumulated on top of the original data of the destination register.

    **Figure 2**  Cumulative statistics<a name="fig117001116340"></a>  
    ![](../../../../figures/cumulative_statistics.png "Cumulative statistics")

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T = DefaultType, typename U = DefaultType, HistogramsBinType mode, HistogramsType type, typename S, typename V>
__simd_callee__ inline void Histograms(V& dstReg, S& srcReg, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ------ | ------ |
| T | Data type of the source operand. |
| U | Data type of the destination operation. |
| mode | **HistogramsBinType** enumeration type, used to select whether to count the low-bit range or the high-bit range of **srcReg**. **Histograms** divides the [0, 255] value range of the **uint8_t** type **srcReg** into a low-bit range and a high-bit range. Each range contains 128 values, corresponding to the 128 statistics results that the **uint16_t** type **dstReg** can hold within one VL (256 bytes). A single call counts only one of the ranges and writes the statistics results of that range into the passed-in **dstReg**. To obtain the complete statistics results over the [0, 255] range, call the API twice with **BIN0** and **BIN1** respectively and use two destination registers to hold the results, for example, **dst0** holds the low-bit range results and **dst1** holds the high-bit range results.<br>• **BIN0**: Counts the data in the [0, 127] range of **srcReg**, and writes the results into the passed-in **dstReg**.<br>• **BIN1**: Counts the data in the [128, 255] range of **srcReg**, and writes the results into the passed-in **dstReg**. |
| type | **HistogramsType** enumeration type, indicating the statistics mode.<br>• **FREQUENCY**: Frequency statistics mode, which counts the quantity of each number in [0, 255] of **srcReg**. Each dst has 128 elements, where in low-bit mode each element of **dstReg** corresponds to the cumulative count of each element in [0, 127] of src, and in high-bit mode each element of **dstReg** corresponds to the cumulative count of each element in [128, 255] of src.<br>• **ACCUMULATE**: Cumulative statistics mode, which counts the total quantity of each number in [0, 255] of **srcReg** and the numbers that appear before it. Each dst has 128 elements, where in low-bit mode each element of **dstReg** corresponds to the cumulative count within the range of each element in [0, 127] of src, and in high-bit mode each element of **dstReg** corresponds to the cumulative count within the range of each element in [128, 255] of src. |
| S | **RegTensor** type of the source operand, automatically deduced by the compiler and does not need to be specified by the user. |
| V | **RegTensor** type of the destination operation, automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="18.54%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="10.05%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="71.41%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p284425844311"><a name="p284425844311"></a><a name="p284425844311"></a>dstReg</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p6389114616514"><a name="p6389114616514"></a><a name="p6389114616514"></a>Destination operand.</p>
<p id="p66093533169"><a name="p66093533169"></a><a name="p66093533169"></a><span id="ph134278176129"><a name="ph134278176129"></a><a name="ph134278176129"></a>Type: <a href="../register_data_types/RegTensor.md">RegTensor</a>.</span></p></td>
</tr>
<tr id="row2137145181815"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p6844125874315"><a name="p6844125874315"></a><a name="p6844125874315"></a>srcReg</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p128442058144312"><a name="p128442058144312"></a><a name="p128442058144312"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p172083541517"><a name="p172083541517"></a><a name="p172083541517"></a>Source operand.</p>
<p id="p157411581277"><a name="p157411581277"></a><a name="p157411581277"></a><span id="ph890017117407"><a name="ph890017117407"></a><a name="ph890017117407"></a>Type: <a href="../register_data_types/RegTensor.md">RegTensor</a>.</span></p></td>
</tr>
<tr id="row19615183817191"><td class="cellrowborder" valign="top" width="18.54%" headers="mcps1.2.4.1.1 "><p id="p1484519586432"><a name="p1484519586432"></a><a name="p1484519586432"></a>mask</p></td>
<td class="cellrowborder" valign="top" width="10.05%" headers="mcps1.2.4.1.2 "><p id="p484514581433"><a name="p484514581433"></a><a name="p484514581433"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.41%" headers="mcps1.2.4.1.3 "><p id="p11541143920"><a name="p11541143920"></a><a name="p11541143920"></a><span id="ph15776181222"><a name="ph15776181222"></a><a name="ph15776181222"></a>Valid indication of the operation on source operand elements. For details, see <a href="../register_data_types/MaskReg.md">MaskReg</a>.</span></p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- When the mask bit count is 0, the value of the **src** source operand at the corresponding position is ignored, and the value at the corresponding position of **dst** is the value calculated after ignoring the **src** at that position.
- The data type of **dst** is `uint16_t`, with a maximum value of 65535. Pay attention to accumulation overflow during use.

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
