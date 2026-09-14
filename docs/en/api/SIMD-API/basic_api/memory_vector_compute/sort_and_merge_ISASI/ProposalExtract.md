# ProposalExtract<a name="ZH-CN_TOPIC_0000001492897985"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T15:27:25.280Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
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
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

This API performs the reverse operation of ProposalConcat. It extracts single elements at the corresponding positions from Region Proposals and rearranges them. In each iteration, 16 Region Proposals are processed, and 16 extracted elements are arranged consecutively.

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T>
__aicore__ inline void ProposalExtract(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime, const int32_t modeNumber)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001429830437_row118356578583"><th class="cellrowborder" valign="top" width="13.58%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000001429830437_p48354572582"><a name="zh-cn_topic_0000001429830437_p48354572582"></a><a name="zh-cn_topic_0000001429830437_p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="86.42%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000001429830437_p583535795817"><a name="zh-cn_topic_0000001429830437_p583535795817"></a><a name="zh-cn_topic_0000001429830437_p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001429830437_row1835857145817"><td class="cellrowborder" valign="top" width="13.58%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000001429830437_p5835457165816"><a name="zh-cn_topic_0000001429830437_p5835457165816"></a><a name="zh-cn_topic_0000001429830437_p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="86.42%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000001429830437_p168351657155818"><a name="zh-cn_topic_0000001429830437_p168351657155818"></a><a name="zh-cn_topic_0000001429830437_p168351657155818"></a>Data type of the operand.</p>
<p id="p489919112302"><a name="p489919112302"></a><a name="p489919112302"></a><span id="ph389915183019"><a name="ph389915183019"></a><a name="ph389915183019"></a><term id="zh-cn_topic_0000001312391781_term71949488213_1"><a name="zh-cn_topic_0000001312391781_term71949488213_1"></a><a name="zh-cn_topic_0000001312391781_term71949488213_1"></a>Atlas training products</term></span>: supported data type is half</p>
<p id="p690016111305"><a name="p690016111305"></a><a name="p690016111305"></a><span id="ph199001173011"><a name="ph199001173011"></a><a name="ph199001173011"></a><term id="zh-cn_topic_0000001312391781_term1964153212227_1"><a name="zh-cn_topic_0000001312391781_term1964153212227_1"></a><a name="zh-cn_topic_0000001312391781_term1964153212227_1"></a>Atlas inference products</term>AI Core</span>: supported data types are half/float</p></td>
</tr>
</tbody>
</table>

**Table 2** Parameters

<a name="table8955841508"></a>
<table><thead align="left"><tr id="row15956194105014"><th class="cellrowborder" valign="top" width="13.661366136613662%" id="mcps1.2.4.1.1"><p id="p7956144195014"><a name="p7956144195014"></a><a name="p7956144195014"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.591259125912593%" id="mcps1.2.4.1.2"><p id="p1295624145013"><a name="p1295624145013"></a><a name="p1295624145013"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="73.74737473747375%" id="mcps1.2.4.1.3"><p id="p16956144145011"><a name="p16956144145011"></a><a name="p16956144145011"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row5956546509"><td class="cellrowborder" valign="top" width="13.661366136613662%" headers="mcps1.2.4.1.1 "><p id="p62165318282"><a name="p62165318282"></a><a name="p62165318282"></a>dst</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p102161931162814"><a name="p102161931162814"></a><a name="p102161931162814"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="73.74737473747375%" headers="mcps1.2.4.1.3 "><p id="p920444891017"><a name="p920444891017"></a><a name="p920444891017"></a>Destination operand.</p>
<p id="p16703131355116"><a name="p16703131355116"></a><a name="p16703131355116"></a><span id="zh-cn_topic_0000001530181537_ph173308471594"><a name="zh-cn_topic_0000001530181537_ph173308471594"></a><a name="zh-cn_topic_0000001530181537_ph173308471594"></a><span id="zh-cn_topic_0000001530181537_ph9902231466"><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><span id="zh-cn_topic_0000001530181537_ph1782115034816"><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a>The type is <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a>, and the supported TPosition values are VECIN/VECCALC/VECOUT.</span></span></span></p>
<p id="p14735141222810"><a name="p14735141222810"></a><a name="p14735141222810"></a><span id="ph1479701815419"><a name="ph1479701815419"></a><a name="ph1479701815419"></a>The start address of LocalTensor must be 32-byte aligned.</span></p></td>
</tr>
<tr id="row4956154125018"><td class="cellrowborder" valign="top" width="13.661366136613662%" headers="mcps1.2.4.1.1 "><p id="p13216193192813"><a name="p13216193192813"></a><a name="p13216193192813"></a>src</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p7217031182818"><a name="p7217031182818"></a><a name="p7217031182818"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.74737473747375%" headers="mcps1.2.4.1.3 "><p id="p1169815471018"><a name="p1169815471018"></a><a name="p1169815471018"></a>Source operand.</p>
<p id="p1735485614104"><a name="p1735485614104"></a><a name="p1735485614104"></a><span id="zh-cn_topic_0000001530181537_ph173308471594_1"><a name="zh-cn_topic_0000001530181537_ph173308471594_1"></a><a name="zh-cn_topic_0000001530181537_ph173308471594_1"></a><span id="zh-cn_topic_0000001530181537_ph9902231466_1"><a name="zh-cn_topic_0000001530181537_ph9902231466_1"></a><a name="zh-cn_topic_0000001530181537_ph9902231466_1"></a><span id="zh-cn_topic_0000001530181537_ph1782115034816_1"><a name="zh-cn_topic_0000001530181537_ph1782115034816_1"></a><a name="zh-cn_topic_0000001530181537_ph1782115034816_1"></a>The type is <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a>, and the supported TPosition values are VECIN/VECCALC/VECOUT.</span></span></span></p>
<p id="p159291714102816"><a name="p159291714102816"></a><a name="p159291714102816"></a><span id="ph4159191519282"><a name="ph4159191519282"></a><a name="ph4159191519282"></a>The start address of LocalTensor must be 32-byte aligned.</span></p>
<p id="p1521763119281"><a name="p1521763119281"></a><a name="p1521763119281"></a>The data type of the source operand must be the same as that of the destination operand.</p></td>
</tr>
<tr id="row6301859135119"><td class="cellrowborder" valign="top" width="13.661366136613662%" headers="mcps1.2.4.1.1 "><p id="p229173384114"><a name="p229173384114"></a><a name="p229173384114"></a>repeatTime</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p32933310418"><a name="p32933310418"></a><a name="p32933310418"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.74737473747375%" headers="mcps1.2.4.1.3 "><p id="p102993315413"><a name="p102993315413"></a><a name="p102993315413"></a>Number of repeated iterations, of the int32_t type. In each iteration, elements are extracted from 16 Region Proposals and arranged into 16 elements. In the next iteration, the operation moves to the next adjacent group of 16 Region Proposals and the next group of 16 elements. Value range: repeatTime∈[0,255].</p></td>
</tr>
<tr id="row0863135810539"><td class="cellrowborder" valign="top" width="13.661366136613662%" headers="mcps1.2.4.1.1 "><p id="p112076141454"><a name="p112076141454"></a><a name="p112076141454"></a>modeNumber</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p195761631163416"><a name="p195761631163416"></a><a name="p195761631163416"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.74737473747375%" headers="mcps1.2.4.1.3 "><div class="p" id="p10306152395018"><a name="p10306152395018"></a><a name="p10306152395018"></a>Extraction position parameter, of the int32_t type. Value range: modeNumber∈[0, 5]. Only the following configurations are supported:<a name="ul5307182311506"></a><a name="ul5307182311506"></a><ul id="ul5307182311506"><li>0 – Extract from x1</li><li>1 – Extract from y1</li><li>2 – Extract from x2</li><li>3 – Extract from y2</li><li>4 – Extract from score</li><li>5 – Extract from label</li></ul>
</div></td>
</tr>
</tbody>
</table>

## Return Value<a name="section1719311422244"></a>

None

## Constraints<a name="section633mcpsimp"></a>

-   The user must ensure that the number of proposals stored in src is greater than or equal to the number actually required. Otherwise, a tensor out-of-bounds error occurs.
-   The user must ensure that the number of elements stored in dst is greater than or equal to the number actually required. Otherwise, a tensor out-of-bounds error occurs.
-   For the address alignment requirements of the operands, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Example<a name="section642mcpsimp"></a>

For a detailed example, see [RegionProposalSort Sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/region_proposal_sort).

-   API usage example

    ```
    // repeatTime = 2, modeNumber = 4. Extract the score elements from 32 Region Proposals and arrange them into 32 consecutive elements.
    AscendC::ProposalExtract(dstLocal, srcLocal, 2, 4);
    ```

    ```
    Example result 
    Input data (src_gm):
    Since moodel = 4, the starting position of the first element, 33.3, is 4. Each Region Proposal occupies eight consecutive elements of the half or float type. The type used here is half. Subsequent elements are extracted with a stride of eight elements. repeat is 2, with 16 elements processed per iteration and 32 elements in total.
    [ 0.      0.      0.      0. 
      33.3    0.      0.      0.      0.      0.      0.      0.
      67.56   0.      0.      0.      0.      0.      0.      0.
      68.5    0.      0.      0.      0.      0.      0.      0.
      -11.914 0.      0.      0.      0.      0.      0.      0.
      25.19   0.      0.      0.      0.      0.      0.      0.
      -72.8   0.      0.      0.      0.      0.      0.      0.
      11.79   0.      0.      0.      0.      0.      0.      0.
      -49.47  0.      0.      0.      0.      0.      0.      0.
      49.44   0.      0.      0.      0.      0.      0.      0.
      84.4    0.      0.      0.      0.      0.      0.      0.
      -14.36  0.      0.      0.      0.      0.      0.      0.
      45.97   0.      0.      0.      0.      0.      0.      0.
      52.47   0.      0.      0.      0.      0.      0.      0.
      -5.387  0.      0.      0.      0.      0.      0.      0.
      -13.12  0.      0.      0.      0.      0.      0.      0.
      -88.9   0.      0.      0.      0.      0.      0.      0.
      54.     0.      0.      0.      0.      0.      0.      0.
      -51.62  0.      0.      0.      0.      0.      0.      0.
     -20.67   0.      0.      0.      0.      0.      0.      0.
     59.56    0.      0.      0.      0.      0.      0.      0.
     35.72    0.      0.      0.      0.      0.      0.      0.
     -6.12    0.      0.      0.      0.      0.      0.      0.
     -39.4    0.      0.      0.      0.      0.      0.      0.
     -11.46   0.      0.      0.      0.      0.      0.      0.
     -7.066   0.      0.      0.      0.      0.      0.      0.
     30.23    0.      0.      0.      0.      0.      0.      0.
     -11.18   0.      0.      0.      0.      0.      0.      0.
     -35.84   0.      0.      0.      0.      0.      0.      0.
     -40.88   0.      0.      0.      0.      0.      0.      0.
     60.9     0.      0.      0.      0.      0.      0.      0.
     -73.3    0.      0.      0.      0.      0.      0.      0.
     38.47    0.      0.      0. 
     ]
    Output data (dst_gm):
    [ 33.3    67.56   68.5   -11.914  25.19  -72.8    11.79  -49.47   49.44
      84.4   -14.36   45.97   52.47   -5.387 -13.12  -88.9    54.    -51.62
     -20.67   59.56   35.72   -6.12  -39.4   -11.46   -7.066  30.23  -11.18
     -35.84  -40.88   60.9   -73.3    38.47 ]
    ```
