# MrgSort4<a name="ZH-CN_TOPIC_0000001493137873"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T15:16:31.074Z -->

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

Sorts and merges up to four sorted Region Proposals queues into one queue, with the results sorted by the score field in descending order.

## Prototype<a name="section620mcpsimp"></a>

```
template <typename T>
__aicore__ inline void MrgSort4(const LocalTensor<T>& dst, const MrgSortSrcList<T>& src, const MrgSort4Info& params)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="13.52%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="86.48%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1835857145817"><td class="cellrowborder" valign="top" width="13.52%" headers="mcps1.2.3.1.1 "><p id="p5835457165816"><a name="p5835457165816"></a><a name="p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="86.48%" headers="mcps1.2.3.1.2 "><p id="p168351657155818"><a name="p168351657155818"></a><a name="p168351657155818"></a>Operand data type.</p>
<p id="p332994321818"><a name="p332994321818"></a><a name="p332994321818"></a><span id="ph732984311185"><a name="ph732984311185"></a><a name="ph732984311185"></a><term id="zh-cn_topic_0000001312391781_term71949488213_1"><a name="zh-cn_topic_0000001312391781_term71949488213_1"></a><a name="zh-cn_topic_0000001312391781_term71949488213_1"></a>Atlas training products</term></span>, supported data type: half</p>
<p id="p13291943201812"><a name="p13291943201812"></a><a name="p13291943201812"></a><span id="ph4329104311814"><a name="ph4329104311814"></a><a name="ph4329104311814"></a><term id="zh-cn_topic_0000001312391781_term1964153212227_1"><a name="zh-cn_topic_0000001312391781_term1964153212227_1"></a><a name="zh-cn_topic_0000001312391781_term1964153212227_1"></a>Atlas inference products</term>AI Core</span>, supported data types: half/float</p></td>
</tr>
</tbody>
</table>

**Table 2** API parameters

<a name="table8955841508"></a>
<table><thead align="left"><tr id="row15956194105014"><th class="cellrowborder" valign="top" width="13.661366136613662%" id="mcps1.2.4.1.1"><p id="p7956144195014"><a name="p7956144195014"></a><a name="p7956144195014"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.58125812581258%" id="mcps1.2.4.1.2"><p id="p1295624145013"><a name="p1295624145013"></a><a name="p1295624145013"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="73.75737573757377%" id="mcps1.2.4.1.3"><p id="p16956144145011"><a name="p16956144145011"></a><a name="p16956144145011"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row5956546509"><td class="cellrowborder" valign="top" width="13.661366136613662%" headers="mcps1.2.4.1.1 "><p id="p19576531173410"><a name="p19576531173410"></a><a name="p19576531173410"></a>dst</p></td>
<td class="cellrowborder" valign="top" width="12.58125812581258%" headers="mcps1.2.4.1.2 "><p id="p16576163119347"><a name="p16576163119347"></a><a name="p16576163119347"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="73.75737573757377%" headers="mcps1.2.4.1.3 "><p id="p88116136127"><a name="p88116136127"></a><a name="p88116136127"></a>Destination operand, which stores the sorted Region Proposals.</p>
<p id="p16703131355116"><a name="p16703131355116"></a><a name="p16703131355116"></a><span id="zh-cn_topic_0000001530181537_ph173308471594"><a name="zh-cn_topic_0000001530181537_ph173308471594"></a><a name="zh-cn_topic_0000001530181537_ph173308471594"></a><span id="zh-cn_topic_0000001530181537_ph9902231466"><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><span id="zh-cn_topic_0000001530181537_ph1782115034816"><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a>Type is <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a>, and the supported TPosition is VECIN/VECCALC/VECOUT.</span></span></span></p>
<p id="p1871313861718"><a name="p1871313861718"></a><a name="p1871313861718"></a>The start address of LocalTensor must be 16-byte aligned (for the half data type) and 32-byte aligned (for the float data type).</p></td>
</tr>
<tr id="row4956154125018"><td class="cellrowborder" valign="top" width="13.661366136613662%" headers="mcps1.2.4.1.1 "><p id="p165761231123417"><a name="p165761231123417"></a><a name="p165761231123417"></a>src</p></td>
<td class="cellrowborder" valign="top" width="12.58125812581258%" headers="mcps1.2.4.1.2 "><p id="p757693163410"><a name="p757693163410"></a><a name="p757693163410"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.75737573757377%" headers="mcps1.2.4.1.3 "><p id="p527920812248"><a name="p527920812248"></a><a name="p527920812248"></a>Source operand, which consists of four Region Proposals queues, each of which is already sorted. The type is the MrgSortSrcList structure, which is defined as follows:</p>
<a name="screen17521147102017"></a><a name="screen17521147102017"></a><pre class="screen" codetype="Cpp" id="screen17521147102017">template &lt;typename T&gt; struct MrgSortSrcList {
    __aicore__ MrgSortSrcList() {}
    __aicore__ MrgSortSrcList(const LocalTensor&lt;T&gt;& src1In, const LocalTensor&lt;T&gt;& src2In, const LocalTensor&lt;T&gt;& src3In,
        const LocalTensor&lt;T&gt;& src4In)
    {
        src1 = src1In[0];
        src2 = src2In[0];
        src3 = src3In[0];
        src4 = src4In[0];
    }
    LocalTensor&lt;T&gt; src1; // First sorted Region Proposals queue
    LocalTensor&lt;T&gt; src2; // Second sorted Region Proposals queue
    LocalTensor&lt;T&gt; src3; // Third sorted Region Proposals queue
    LocalTensor&lt;T&gt; src4; // Fourth sorted Region Proposals queue
};</pre>
<p id="p8114998103"><a name="p8114998103"></a><a name="p8114998103"></a>The data type of the Region Proposals queue is consistent with that of the destination operand. The types of src1, src2, src3, and src4 are <a href="../../data_structures/LocalTensor/LocalTensor.md">LocalTensor</a>, and the supported TPosition is VECIN/VECCALC/VECOUT.</p>
<p id="p1938181518223"><a name="p1938181518223"></a><a name="p1938181518223"></a>The start address of LocalTensor must be 16-byte aligned (for the half data type) and 32-byte aligned (for the float data type).</p></td>
</tr>
<tr id="row6301859135119"><td class="cellrowborder" valign="top" width="13.661366136613662%" headers="mcps1.2.4.1.1 "><p id="p229173384114"><a name="p229173384114"></a><a name="p229173384114"></a>params</p></td>
<td class="cellrowborder" valign="top" width="12.58125812581258%" headers="mcps1.2.4.1.2 "><p id="p32933310418"><a name="p32933310418"></a><a name="p32933310418"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.75737573757377%" headers="mcps1.2.4.1.3 "><p id="p1465085251516"><a name="p1465085251516"></a><a name="p1465085251516"></a>Parameters required for sorting. The type is the MrgSort4Info structure.</p>
<p id="p395104375712"><a name="p395104375712"></a><a name="p395104375712"></a>For the specific definition, please refer to <span id="ph10562197165916"><a name="ph10562197165916"></a><a name="ph10562197165916"></a>${INSTALL_DIR}</span>/include/ascendc/basic_api/interface/kernel_struct_proposal.h. Replace <span id="ph14322531015"><a name="ph14322531015"></a><a name="ph14322531015"></a>${INSTALL_DIR}</span> with the path where the CANN software is installed.</p>
<p id="p102993315413"><a name="p102993315413"></a><a name="p102993315413"></a>For the parameter description, please refer to <a href="#table7515358184615">Table 3</a>.</p></td>
</tr>
</tbody>
</table>

**Table 3**  MrgSort4Info parameter description

<a name="table7515358184615"></a>
<table><thead align="left"><tr id="row1951513585460"><th class="cellrowborder" valign="top" width="13.611361136113612%" id="mcps1.2.4.1.1"><p id="p195151858154612"><a name="p195151858154612"></a><a name="p195151858154612"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.48124812481248%" id="mcps1.2.4.1.2"><p id="p185151585466"><a name="p185151585466"></a><a name="p185151585466"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="73.9073907390739%" id="mcps1.2.4.1.3"><p id="p1151555820466"><a name="p1151555820466"></a><a name="p1151555820466"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row25157580465"><td class="cellrowborder" valign="top" width="13.611361136113612%" headers="mcps1.2.4.1.1 "><p id="p05156585462"><a name="p05156585462"></a><a name="p05156585462"></a>elementLengths</p></td>
<td class="cellrowborder" valign="top" width="12.48124812481248%" headers="mcps1.2.4.1.2 "><p id="p125156582464"><a name="p125156582464"></a><a name="p125156582464"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.9073907390739%" headers="mcps1.2.4.1.3 "><p id="p1751545874619"><a name="p1751545874619"></a><a name="p1751545874619"></a>Lengths of the four source Region Proposals queues (number of Region Proposals). The type is a uint16_t array of length 4. Theoretically, each element ranges from [0, 4095], but cannot exceed the UB storage space.</p></td>
</tr>
<tr id="row1515155817465"><td class="cellrowborder" valign="top" width="13.611361136113612%" headers="mcps1.2.4.1.1 "><p id="p55154583465"><a name="p55154583465"></a><a name="p55154583465"></a>ifExhaustedSuspension</p></td>
<td class="cellrowborder" valign="top" width="12.48124812481248%" headers="mcps1.2.4.1.2 "><p id="p6515185814615"><a name="p6515185814615"></a><a name="p6515185814615"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.9073907390739%" headers="mcps1.2.4.1.3 "><p id="p135151558194611"><a name="p135151558194611"></a><a name="p135151558194611"></a>Whether the instruction needs to stop after a queue is exhausted. The type is bool, and the default value is false.</p></td>
</tr>
<tr id="row1234542411541"><td class="cellrowborder" valign="top" width="13.611361136113612%" headers="mcps1.2.4.1.1 "><p id="p143461024185414"><a name="p143461024185414"></a><a name="p143461024185414"></a>validBit</p></td>
<td class="cellrowborder" valign="top" width="12.48124812481248%" headers="mcps1.2.4.1.2 "><p id="p16291844175420"><a name="p16291844175420"></a><a name="p16291844175420"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.9073907390739%" headers="mcps1.2.4.1.3 "><div class="p" id="p4633153935413"><a name="p4633153935413"></a><a name="p4633153935413"></a>Number of valid queues. The value is as follows:<a name="ul1925313221792"></a><a name="ul1925313221792"></a><ul id="ul1925313221792"><li>3: The first two queues are valid.</li><li>7: The first three queues are valid.</li><li>15: All four queues are valid.</li></ul>
</div></td>
</tr>
<tr id="row59828263546"><td class="cellrowborder" valign="top" width="13.611361136113612%" headers="mcps1.2.4.1.1 "><p id="p898232615410"><a name="p898232615410"></a><a name="p898232615410"></a>repeatTimes</p></td>
<td class="cellrowborder" valign="top" width="12.48124812481248%" headers="mcps1.2.4.1.2 "><p id="p169826260544"><a name="p169826260544"></a><a name="p169826260544"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.9073907390739%" headers="mcps1.2.4.1.3 "><p id="p330857175415"><a name="p330857175415"></a><a name="p330857175415"></a>Number of iterations. In each iteration, the source operand and destination operand skip the total length of the four queues. Value range: repeatTimes∈[1,255].</p>
<div class="p" id="p1301057145414"><a name="p1301057145414"></a><a name="p1301057145414"></a>The repeatTimes parameter takes effect only when all the following four conditions are met:<a name="ul19311157155420"></a><a name="ul19311157155420"></a><ul id="ul19311157155420"><li>The four source Region Proposals queues have the same length.</li><li>The four source Region Proposals queues are stored contiguously.</li><li>ifExhaustedSuspension = False</li><li>validBit=15</li></ul>
</div></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

-   When the score values of proposal\[i\] and proposal\[j\] are the same, if i\>j, proposal\[j\] is selected first and placed in front.
-   For the operand address alignment requirements, please refer to [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

-   Address overlap between the source operand and the destination operand is not supported.

## Example<a name="section642mcpsimp"></a>

For a detailed example, please refer to the [RegionProposalSort sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/region_proposal_sort).

API Usage Example

```
// Region Proposals are created as follows:
// AscendC::ProposalConcat(vconcatWorkLocal[0], srcLocal[0], repeat, mode);
// AscendC::RpSort16(vconcatWorkLocal[0], vconcatWorkLocal[0], repeat);

// vconcatWorkLocal contains four Region Proposals that have been created and sorted, with 16 elements in each Region Proposal.
// The input data first goes through ProposalConcat to merge consecutive elements into the corresponding positions within the Region Proposals, and then RpSort16 sorts the score field of the Region Proposals to obtain each vconcatWorkLocal.

struct MrgSortSrcList<half> srcList(vconcatWorkLocal[0], vconcatWorkLocal[1], vconcatWorkLocal[2], vconcatWorkLocal[3]);
// The length of each of the four source Region Proposal queues is 16.
uint16_t elementLengths[4] = {16, 16, 16, 16};
// false indicates that the instruction does not need to stop after a queue is exhausted. 15 indicates that all four queues are valid. repeat=1, one iteration.
struct MrgSort4Info srcInfo(elementLengths, false, 15, 1);
AscendC::MrgSort4(dstLocal, srcList, srcInfo);
```

```
Example Result
Input data (src_gm):
[-38.1    82.7   -40.75  -54.62   21.67  -58.53   25.94  -79.5   -61.44
  26.7   -27.45   48.78   86.75  -18.1   -58.8    62.38   46.38  -78.94
 -87.7   -13.81  -13.25   46.94  -47.8   -50.44   34.16   20.3    80.1
 -94.1    52.4   -42.75   83.4    80.44  -66.8   -82.7   -91.44  -95.6
  66.2   -30.97  -36.53   61.66   24.92  -45.1    38.97  -34.62  -69.8
  59.1    34.22   11.695 -33.47   52.1    -4.832  46.88   56.78   71.4
  13.29  -35.78   52.44  -46.03   83.8    83.56   71.3    -9.086 -65.06
  46.25 ]

// The input data first goes through ProposalConcat to merge consecutive elements into the corresponding positions within the Region Proposals, and then RpSort16 sorts them by the score field of the Region Proposals, resulting in 4 vconcatWorkLocal; merging these 4 yields the output.

Output data (dst_gm):
[
0.0       0.0       0.0       0.0       86.75     0.0       0.0       0.0
0.0       0.0       0.0       83.8      0.0       0.0       0.0       0.0
0.0       0.0       83.56     0.0       0.0       0.0       0.0       0.0
0.0       83.4      0.0       0.0       0.0       0.0       0.0       0.0
82.7      0.0       0.0       0.0       0.0       0.0       0.0       80.44
0.0       0.0       0.0       0.0       0.0       0.0       0.0       80.1
0.0       0.0       0.0       0.0       0.0       0.0       71.4      0.0
0.0       0.0       0.0       0.0       0.0       71.3      0.0       0.0
0.0       0.0       0.0       0.0       66.2      0.0       0.0       0.0
0.0       0.0       0.0       62.38     0.0       0.0       0.0       0.0
0.0       0.0       61.66     0.0       0.0       0.0       0.0       0.0
0.0       59.1      0.0       0.0       0.0       0.0       0.0       0.0
56.78     0.0       0.0       0.0       0.0       0.0       0.0       52.44
0.0       0.0       0.0       0.0       0.0       0.0       52.4      0.0
0.0       0.0       0.0       0.0       0.0       52.1      0.0       0.0
0.0       0.0       0.0       0.0       0.0       48.78     0.0       0.0
0.0       0.0       0.0       0.0       46.94     0.0       0.0       0.0
0.0       0.0       0.0       46.88     0.0       0.0       0.0       0.0
0.0       0.0       46.38     0.0       0.0       0.0       0.0       0.0
0.0       46.25     0.0       0.0       0.0       0.0       0.0       0.0
38.97     0.0       0.0       0.0       0.0       0.0       0.0       34.22
0.0       0.0       0.0       0.0       0.0       0.0       34.16     0.0
0.0       0.0       0.0       0.0       0.0       26.7      0.0       0.0
0.0       0.0       0.0       0.0       25.94     0.0       0.0       0.0
0.0       0.0       0.0       24.92     0.0       0.0       0.0       0.0
0.0       0.0       21.67     0.0       0.0       0.0       0.0       0.0
0.0       20.3      0.0       0.0       0.0       0.0       0.0       0.0
0.0       13.29     0.0       0.0       0.0       0.0       0.0       0.0
11.695    0.0       0.0       0.0       0.0       0.0       0.0       -4.832
0.0       0.0       0.0       0.0       0.0       0.0       -9.086    0.0
0.0       0.0       0.0       0.0       0.0       -13.25    0.0       0.0
0.0       0.0       0.0       0.0       -13.81    0.0       0.0       0.0
0.0       0.0       0.0       -18.1     0.0       0.0       0.0       0.0
0.0       0.0       0.0       -27.45    0.0       0.0       0.0       0.0
0.0       0.0       -30.97    0.0       0.0       0.0       0.0       0.0
0.0       -33.47    0.0       0.0       0.0       0.0       0.0       0.0
-34.62    0.0       0.0       0.0       0.0       0.0       0.0       -35.78
0.0       0.0       0.0       0.0       0.0       0.0       -36.53    0.0
0.0       0.0       0.0       0.0       0.0       -38.1     0.0       0.0
0.0       0.0       0.0       0.0       -40.75    0.0       0.0       0.0
0.0       0.0       0.0       -42.75    0.0       0.0       0.0       0.0
0.0       0.0       -45.1     0.0       0.0       0.0       0.0       0.0
0.0       -46.03    0.0       0.0       0.0       0.0       0.0       0.0
-47.8     0.0       0.0       0.0       0.0       0.0       0.0       0.0
-50.44    0.0       0.0       0.0       0.0       0.0       0.0       -54.62
0.0       0.0       0.0       0.0       0.0       0.0       -58.53    0.0
0.0       0.0       0.0       0.0       0.0       -58.8     0.0       0.0
0.0       0.0       0.0       0.0       -61.44    0.0       0.0       0.0
0.0       0.0       0.0       -65.06    0.0       0.0       0.0       0.0
0.0       0.0       -66.8     0.0       0.0       0.0       0.0       0.0
0.0       -69.8     0.0       0.0       0.0       0.0       0.0       0.0
-78.94    0.0       0.0       0.0       0.0       0.0       0.0       -79.5
0.0       0.0       0.0       0.0       0.0       0.0       -82.7     0.0
0.0       0.0       0.0       0.0       0.0       -87.7     0.0       0.0
0.0       0.0       0.0       0.0       -91.44    0.0       0.0       0.0
0.0       0.0       0.0       -94.1     0.0       0.0       0.0       0.0
0.0       0.0       0.0       -95.6     0.0       0.0       0.0       0.0
]
```
