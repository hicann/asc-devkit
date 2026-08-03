# ProposalConcat<a name="ZH-CN_TOPIC_0000001442977962"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：不支持
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
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：支持
<!-- end id7 -->


## 功能说明<a name="section618mcpsimp"></a>

将连续元素合入Region Proposal内对应位置，每次迭代会将16个连续元素合入到16个Region Proposals的对应位置里。

**Region Proposal说明：**

目前仅支持两种数据类型：half、float。

每个Region Proposal占用连续8个half/float类型的元素，约定其格式：

```
[x1, y1, x2, y2, score, label, reserved_0, reserved_1]
```

对于数据类型half，每一个Region Proposal占16Bytes，Byte\[15:12\]是无效数据，Byte\[11:0\]包含6个half类型的元素，其中Byte\[11:10\]定义为label，Byte\[9:8\]定义为score，Byte\[7:6\]定义为y2，Byte\[5:4\]定义为x2，Byte\[3:2\]定义为y1，Byte\[1:0\]定义为x1。

如下图所示，总共包含16个Region Proposals。

![](../../../../figures/zh-cn_image_0000001820284977.png)

对于数据类型float，每一个Region Proposal占32Bytes，Byte\[31:24\]是无效数据，Byte\[23:0\]包含6个float类型的元素，其中Byte\[23:20\]定义为label，Byte\[19:16\]定义为score，Byte\[15:12\]定义为y2，Byte\[11:8\]定义为x2，Byte\[7:4\]定义为y1，Byte\[3:0\]定义为x1。

如下图所示，总共包含16个Region Proposals。

![](../../../../figures/zh-cn_image_0000001820324937.png)

## 函数原型<a name="section620mcpsimp"></a>

```
template <typename T>
__aicore__ inline void ProposalConcat(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime, const int32_t modeNumber)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001429830437_row118356578583"><th class="cellrowborder" valign="top" width="13.58%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000001429830437_p48354572582"><a name="zh-cn_topic_0000001429830437_p48354572582"></a><a name="zh-cn_topic_0000001429830437_p48354572582"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="86.42%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000001429830437_p583535795817"><a name="zh-cn_topic_0000001429830437_p583535795817"></a><a name="zh-cn_topic_0000001429830437_p583535795817"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001429830437_row1835857145817"><td class="cellrowborder" valign="top" width="13.58%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000001429830437_p5835457165816"><a name="zh-cn_topic_0000001429830437_p5835457165816"></a><a name="zh-cn_topic_0000001429830437_p5835457165816"></a>T</p>
</td>
<td class="cellrowborder" valign="top" width="86.42%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000001429830437_p168351657155818"><a name="zh-cn_topic_0000001429830437_p168351657155818"></a><a name="zh-cn_topic_0000001429830437_p168351657155818"></a>操作数数据类型。</p>
<p id="p7265124521915"><a name="p7265124521915"></a><a name="p7265124521915"></a><span id="ph194756336178"><a name="ph194756336178"></a><a name="ph194756336178"></a><term id="zh-cn_topic_0000001312391781_term71949488213_1"><a name="zh-cn_topic_0000001312391781_term71949488213_1"></a><a name="zh-cn_topic_0000001312391781_term71949488213_1"></a>Atlas 训练系列产品</term></span>，支持的数据类型为：half</p>
<p id="p12659457192"><a name="p12659457192"></a><a name="p12659457192"></a><span id="ph750620211814"><a name="ph750620211814"></a><a name="ph750620211814"></a><term id="zh-cn_topic_0000001312391781_term1964153212227_1"><a name="zh-cn_topic_0000001312391781_term1964153212227_1"></a><a name="zh-cn_topic_0000001312391781_term1964153212227_1"></a>Atlas 推理系列产品</term>AI Core</span>，支持的数据类型为：half/float</p>
</td>
</tr>
</tbody>
</table>

**表2**  参数说明

<a name="table62161631132810"></a>
<table><thead align="left"><tr id="row12216103118284"><th class="cellrowborder" valign="top" width="13.661366136613662%" id="mcps1.2.4.1.1"><p id="p1421643114288"><a name="p1421643114288"></a><a name="p1421643114288"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="12.591259125912593%" id="mcps1.2.4.1.2"><p id="p82165310285"><a name="p82165310285"></a><a name="p82165310285"></a>输入/输出</p>
</th>
<th class="cellrowborder" valign="top" width="73.74737473747375%" id="mcps1.2.4.1.3"><p id="p1121663111288"><a name="p1121663111288"></a><a name="p1121663111288"></a>含义</p>
</th>
</tr>
</thead>
<tbody><tr id="row82161131182810"><td class="cellrowborder" valign="top" width="13.661366136613662%" headers="mcps1.2.4.1.1 "><p id="p62165318282"><a name="p62165318282"></a><a name="p62165318282"></a>dst</p>
</td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p102161931162814"><a name="p102161931162814"></a><a name="p102161931162814"></a>输出</p>
</td>
<td class="cellrowborder" valign="top" width="73.74737473747375%" headers="mcps1.2.4.1.3 "><p id="p541935917913"><a name="p541935917913"></a><a name="p541935917913"></a>目的操作数。</p>
<p id="p16703131355116"><a name="p16703131355116"></a><a name="p16703131355116"></a><span id="zh-cn_topic_0000001530181537_ph173308471594"><a name="zh-cn_topic_0000001530181537_ph173308471594"></a><a name="zh-cn_topic_0000001530181537_ph173308471594"></a><span id="zh-cn_topic_0000001530181537_ph9902231466"><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><a name="zh-cn_topic_0000001530181537_ph9902231466"></a><span id="zh-cn_topic_0000001530181537_ph1782115034816"><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a><a name="zh-cn_topic_0000001530181537_ph1782115034816"></a>类型为<a href="../../数据结构/LocalTensor/LocalTensor.md">LocalTensor</a>，支持的TPosition为VECIN/VECCALC/VECOUT。</span></span></span></p>
<p id="p2735747102716"><a name="p2735747102716"></a><a name="p2735747102716"></a><span id="ph1479701815419"><a name="ph1479701815419"></a><a name="ph1479701815419"></a>LocalTensor的起始地址需要32字节对齐。</span></p>
</td>
</tr>
<tr id="row5216163192815"><td class="cellrowborder" valign="top" width="13.661366136613662%" headers="mcps1.2.4.1.1 "><p id="p13216193192813"><a name="p13216193192813"></a><a name="p13216193192813"></a>src</p>
</td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p7217031182818"><a name="p7217031182818"></a><a name="p7217031182818"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="73.74737473747375%" headers="mcps1.2.4.1.3 "><p id="p12791103312108"><a name="p12791103312108"></a><a name="p12791103312108"></a>源操作数。</p>
<p id="p86181935151016"><a name="p86181935151016"></a><a name="p86181935151016"></a><span id="zh-cn_topic_0000001530181537_ph173308471594_1"><a name="zh-cn_topic_0000001530181537_ph173308471594_1"></a><a name="zh-cn_topic_0000001530181537_ph173308471594_1"></a><span id="zh-cn_topic_0000001530181537_ph9902231466_1"><a name="zh-cn_topic_0000001530181537_ph9902231466_1"></a><a name="zh-cn_topic_0000001530181537_ph9902231466_1"></a><span id="zh-cn_topic_0000001530181537_ph1782115034816_1"><a name="zh-cn_topic_0000001530181537_ph1782115034816_1"></a><a name="zh-cn_topic_0000001530181537_ph1782115034816_1"></a>类型为<a href="../../数据结构/LocalTensor/LocalTensor.md">LocalTensor</a>，支持的TPosition为VECIN/VECCALC/VECOUT。</span></span></span></p>
<p id="p149990310282"><a name="p149990310282"></a><a name="p149990310282"></a><span id="ph133625442815"><a name="ph133625442815"></a><a name="ph133625442815"></a>LocalTensor的起始地址需要32字节对齐。</span></p>
<p id="p1521763119281"><a name="p1521763119281"></a><a name="p1521763119281"></a>源操作数的数据类型需要与目的操作数保持一致。</p>
</td>
</tr>
<tr id="row221753112285"><td class="cellrowborder" valign="top" width="13.661366136613662%" headers="mcps1.2.4.1.1 "><p id="p221713318283"><a name="p221713318283"></a><a name="p221713318283"></a>repeatTime</p>
</td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p02174318282"><a name="p02174318282"></a><a name="p02174318282"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="73.74737473747375%" headers="mcps1.2.4.1.3 "><p id="p12217153111284"><a name="p12217153111284"></a><a name="p12217153111284"></a>重复迭代次数，int32_t类型，每次迭代完成16个元素合入到16个Region Proposals里，下次迭代跳至相邻的下一组16个Region Proposals和下一组16个元素。取值范围：repeatTime∈[0,255]。</p>
</td>
</tr>
<tr id="row521753120287"><td class="cellrowborder" valign="top" width="13.661366136613662%" headers="mcps1.2.4.1.1 "><p id="p3217133122812"><a name="p3217133122812"></a><a name="p3217133122812"></a>modeNumber</p>
</td>
<td class="cellrowborder" valign="top" width="12.591259125912593%" headers="mcps1.2.4.1.2 "><p id="p021763116285"><a name="p021763116285"></a><a name="p021763116285"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="73.74737473747375%" headers="mcps1.2.4.1.3 "><div class="p" id="p132171431112815"><a name="p132171431112815"></a><a name="p132171431112815"></a>合入位置参数，取值范围：modeNumber∈[0, 5]，int32_t类型，仅限于以下配置：<a name="ul14217183111284"></a><a name="ul14217183111284"></a><ul id="ul14217183111284"><li>0 – 合入x1</li><li>1 – 合入y1</li><li>2 – 合入x2</li><li>3 – 合入y2</li><li>4 – 合入score</li><li>5 – 合入label</li></ul>
</div>
</td>
</tr>
</tbody>
</table>

## 返回值说明<a name="section1719311422244"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

-   用户需保证dst中存储的proposal数目大于等于实际所需数目，否则会存在tensor越界错误。
-   用户需保证src中存储的元素大于等于实际所需数目，否则会存在tensor越界错误。
-   操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。

## 调用示例<a name="section642mcpsimp"></a>

详细示例请参考[RegionProposalSort样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/region_proposal_sort)。

-   接口使用样例

    ```
    // repeatTime = 2, modeNumber = 4,把32个数合入到32个Region Proposal中的score域中
    AscendC::ProposalConcat(dstLocal, srcLocal, 2, 4);
    ```

    ```
    示例结果 
    输入数据(src_gm):
    [ 33.3    67.56   68.5   -11.914  25.19  -72.8    11.79  -49.47   49.44
      84.4   -14.36   45.97   52.47   -5.387 -13.12  -88.9    54.    -51.62
     -20.67   59.56   35.72   -6.12  -39.4   -11.46   -7.066  30.23  -11.18
     -35.84  -40.88   60.9   -73.3    38.47 ]
    输出数据(dst_gm):
    因为moodel=4，第一个元素33.3的起始位置是4。每个Region Proposal占用连续8个half/float类型的元素。这里使用的类型是half。后续被插入的每个元素间隔8个元素。repeat为2，每次迭代完成16个元素，共计32个元素
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
    ```
