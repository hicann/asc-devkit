# RpSort16<a name="ZH-CN_TOPIC_0000001443137898"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T15:30:23.461Z -->

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

Sorts the Region Proposals by their score field (larger scores come first), processing 16 Region Proposals at a time.

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T>
__aicore__ inline void RpSort16(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime)
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
<p id="p489919112302"><a name="p489919112302"></a><a name="p489919112302"></a><span id="ph389915183019"><a name="ph389915183019"></a><a name="ph389915183019"></a><term id="zh-cn_topic_0000001312391781_term71949488213_1"><a name="zh-cn_topic_0000001312391781_term71949488213_1"></a><a name="zh-cn_topic_0000001312391781_term71949488213_1"></a>Atlas training products</term></span>, supported data type: half</p>
<p id="p690016111305"><a name="p690016111305"></a><a name="p690016111305"></a><span id="ph199001173011"><a name="ph199001173011"></a><a name="ph199001173011"></a><term id="zh-cn_topic_0000001312391781_term1964153212227_1"><a name="zh-cn_topic_0000001312391781_term1964153212227_1"></a><a name="zh-cn_topic_0000001312391781_term1964153212227_1"></a>Atlas inference products</term>AI Core</span>, supported data types: half/float</p></td>
</tr>
</tbody>
</table>

**Table 2** Parameters

<a name="table16397194910519"></a>
<table><thead align="left"><tr id="row14397114985110"><th class="cellrowborder" valign="top" width="13.661366136613662%" id="mcps1.2.4.1.1"><p id="p10397449125111"><a name="p10397449125111"></a><a name="p10397449125111"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.591259125912593%" id="mcps1.2.4.1.2"><p id="p83971498517"><a name="p83971498517"></a><a name="p83971498517"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="73.74737473747375%" id="mcps1.2.4.1.3"><p id="p339754919510"><a name="p339754919510"></a><a name="p339754919510"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row19397104975114"><td class="cellrowborder" valign="top" width="13.661366136613662%" headers="mcps1.2.4.1.1 "><p id="p15397104905113"><a name="p15397104905113"></a><a name="p15397104905113"></a>dst</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p639784985113"><a name="p639784985113"></a><a name="p639784985113"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="73.74737473747375%" headers="mcps1.2.4.1.3 "><p id="p23620781116"><a name="p23620781116"></a><a name="p23620781116"></a>Destination operand, which stores the sorted Region Proposals.</p>
<p id="p16703131355116"><a name="p16703131355116"></a><a name="p16703131355116"></a><span id="zh-cn_topic_0000001530181537_ph173308471594"><a name="zh-cn_topic_0000001530181537_ph173308471594"></a><a name="zh-cn_topic_0000001530181537_ph173308471594"></a><span id="zh-cn_topic_0000001530181537_ph9902231466"><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><span id="zh-cn_topic_0000001530181537_ph1782115034816"><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a>The type is <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a>, and the supported TPosition values are VECIN/VECCALC/VECOUT.</span></span></span></p>
<p id="p5170152413011"><a name="p5170152413011"></a><a name="p5170152413011"></a><span id="ph1479701815419"><a name="ph1479701815419"></a><a name="ph1479701815419"></a>The start address of LocalTensor must be 32-byte aligned.</span></p></td>
</tr>
<tr id="row9397134918514"><td class="cellrowborder" valign="top" width="13.661366136613662%" headers="mcps1.2.4.1.1 "><p id="p8397849185111"><a name="p8397849185111"></a><a name="p8397849185111"></a>src</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p153971249195113"><a name="p153971249195113"></a><a name="p153971249195113"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.74737473747375%" headers="mcps1.2.4.1.3 "><p id="p2042185715113"><a name="p2042185715113"></a><a name="p2042185715113"></a>Source operand, which stores the unsorted Region Proposals.</p>
<p id="p557485981119"><a name="p557485981119"></a><a name="p557485981119"></a><span id="zh-cn_topic_0000001530181537_ph173308471594_1"><a name="zh-cn_topic_0000001530181537_ph173308471594_1"></a><a name="zh-cn_topic_0000001530181537_ph173308471594_1"></a><span id="zh-cn_topic_0000001530181537_ph9902231466_1"><a name="zh-cn_topic_0000001530181537_ph9902231466_1"></a><a name="zh-cn_topic_0000001530181537_ph9902231466_1"></a><span id="zh-cn_topic_0000001530181537_ph1782115034816_1"><a name="zh-cn_topic_0000001530181537_ph1782115034816_1"></a><a name="zh-cn_topic_0000001530181537_ph1782115034816_1"></a>The type is <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a>, and the supported TPosition values are VECIN/VECCALC/VECOUT.</span></span></span></p>
<p id="p131511267302"><a name="p131511267302"></a><a name="p131511267302"></a><span id="ph3760152613303"><a name="ph3760152613303"></a><a name="ph3760152613303"></a>The start address of LocalTensor must be 32-byte aligned.</span></p></td>
</tr>
<tr id="row139734965111"><td class="cellrowborder" valign="top" width="13.661366136613662%" headers="mcps1.2.4.1.1 "><p id="p1839754914514"><a name="p1839754914514"></a><a name="p1839754914514"></a>repeatTime</p></td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p103974492515"><a name="p103974492515"></a><a name="p103974492515"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.74737473747375%" headers="mcps1.2.4.1.3 "><p id="p539774915112"><a name="p539774915112"></a><a name="p539774915112"></a>Number of repeated iterations, of the int32_t type. 16 Region Proposals are sorted in each iteration. Value range: repeatTime ∈ [0, 255].</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

-   You need to ensure that the number of Region Proposals stored in src and dst is greater than the actually required number. Otherwise, a tensor out-of-bounds error occurs.
-   When the score values of proposal\[i\] and proposal\[j\] are the same, if i\>j, proposal\[j\] is selected first and placed in the front.
-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Example<a name="section642mcpsimp"></a>

For a detailed example, see [RegionProposalSort Sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/region_proposal_sort).

-   API usage example

    ```
    // ProposalConcat merges consecutive elements into the corresponding positions in the Region Proposal.
    // repeatTime = 2, sort two Region Proposals, model = 4, starting position is 4.
    AscendC::ProposalConcat(dstLocal, srcLocal, 2, 4);
    AscendC::RpSort16(dstLocal, dstLocal, 2);
    ```

    ```
    Example result
    Input data srcLocal:
    [ -1.624 -42.3   -54.12   91.25  -99.4    36.72   67.44  -66.3   -52.53
       3.377 -62.47  -15.85  -31.47    3.143  58.47  -83.75 21.58   63.47    
       7.234  35.16  -39.72   37.8    73.06  -98.7    44.1 -77.2    67.2    
       19.62  -87.9   -14.875  15.86  -77.75]
    dstLocal data after ProposalConcat, repeat=2 computes 32 elements, model=4 with the start position at 4
    [  
     0.        0.      0.      0.
    -1.624     0.      0.      0.      0.      0.      0.      0.
    -42.3      0.      0.      0.      0.      0.      0.      0.
    -54.12     0.      0.      0.      0.      0.      0.      0.
    91.25      0.      0.      0.      0.      0.      0.      0.
    -99.4      0.      0.      0.      0.      0.      0.      0.
    36.72      0.      0.      0.      0.      0.      0.      0.
    67.44      0.      0.      0.      0.      0.      0.      0.
    -66.3      0.      0.      0.      0.      0.      0.      0.
    -52.53     0.      0.      0.      0.      0.      0.      0.
    3.377      0.      0.      0.      0.      0.      0.      0.
    -62.47     0.      0.      0.      0.      0.      0.      0.
    -15.85     0.      0.      0.      0.      0.      0.      0.
    -31.47     0.      0.      0.      0.      0.      0.      0.
    3.143      0.      0.      0.      0.      0.      0.      0.
    58.47      0.      0.      0.      0.      0.      0.      0.
    -83.75     0.      0.      0.      0.      0.      0.      0.
    21.58      0.      0.      0.      0.      0.      0.      0.
    63.47      0.      0.      0.      0.      0.      0.      0.
    7.234      0.      0.      0.      0.      0.      0.      0.
    35.16      0.      0.      0.      0.      0.      0.      0.
    -39.72     0.      0.      0.      0.      0.      0.      0.
    37.8       0.      0.      0.      0.      0.      0.      0.
    73.06      0.      0.      0.      0.      0.      0.      0.
    -98.7      0.      0.      0.      0.      0.      0.      0.
    44.1       0.      0.      0.      0.      0.      0.      0.
    -77.2      0.      0.      0.      0.      0.      0.      0.
    67.2       0.      0.      0.      0.      0.      0.      0.
    19.62      0.      0.      0.      0.      0.      0.      0.
    -87.9      0.      0.      0.      0.      0.      0.      0.
    -14.875    0.      0.      0.      0.      0.      0.      0.
    15.86      0.      0.      0.      0.      0.      0.      0.
    -77.75     0.      0.      0.     
    ]
    Output data (dst_gm):
     [
     0.      0.      0.      0.
     91.25   0.      0.      0.      0.      0.      0.      0.
     67.44   0.      0.      0.      0.      0.      0.      0.
     58.47   0.      0.      0.      0.      0.      0.      0.
     36.72   0.      0.      0.      0.      0.      0.      0.
     3.377   0.      0.      0.      0.      0.      0.      0.
     3.143   0.      0.      0.      0.      0.      0.      0.
     -1.624  0.      0.      0.      0.      0.      0.      0.
     -15.85  0.      0.      0.      0.      0.      0.      0.
     -31.47  0.      0.      0.      0.      0.      0.      0.
     -42.3   0.      0.      0.      0.      0.      0.      0.
     -52.53  0.      0.      0.      0.      0.      0.      0.
     -54.12  0.      0.      0.      0.      0.      0.      0.
     -62.47  0.      0.      0.      0.      0.      0.      0.
     -66.3   0.      0.      0.      0.      0.      0.      0
     -83.75  0.      0.      0.      0.      0.      0.      0.
     -99.4   0.      0.      0.      0.      0.      0.      0.
     73.06   0.      0.      0.      0.      0.      0.      0.
     67.2    0.      0.      0.      0.      0.      0.      0.
     63.47   0.      0.      0.      0.      0.      0.      0.
     44.1    0.      0.      0.      0.      0.      0.      0.
     37.8    0.      0.      0.      0.      0.      0.      0.
     35.16   0.      0.      0.      0.      0.      0.      0.
     21.58   0.      0.      0.      0.      0.      0.      0.
     19.62   0.      0.      0.      0.      0.      0.      0.
     15.86   0.      0.      0.      0.      0.      0.      0.
     7.234   0.      0.      0.      0.      0.      0.      0.
     -14.875 0.      0.      0.      0.      0.      0.      0.
     -39.72  0.      0.      0.      0.      0.      0.      0.
     -77.2   0.      0.      0.      0.      0.      0.      0.
     -77.75  0.      0.      0.      0.      0.      0.      0.
     -87.9   0.      0.      0.      0.      0.      0.      0.
     -98.7   0.      0.      0.
     ]
    ```
