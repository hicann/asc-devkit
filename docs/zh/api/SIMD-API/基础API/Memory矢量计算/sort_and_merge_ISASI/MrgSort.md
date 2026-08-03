# MrgSort<a name="ZH-CN_TOPIC_0000001538216637"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：支持
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

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_proposal_intf.h"`

将已经排好序的最多4条队列，合并排列成1条队列，结果按照score域由大到小排序。

MrgSort接口通常处理经过Sort32接口预处理的数据，队列结构参考[Sort32功能说明](Sort32.md)。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void MrgSort(const LocalTensor<T>& dst, const MrgSortSrcList<T>& src, const MrgSort4Info& params)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
|--------|------|
| T | 目的操作数数据类型。 |

**表2**  参数说明

| 参数名称 | 输入/输出 | 说明 |
|:---|:---|:---|
| dst | 输出 | 目的操作数。<br> 类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。<br> LocalTensor的起始地址需要32字节对齐。 |
| src | 输入 | 源操作数，4个已经排序完成的队列，类型为MrgSortSrcList结构体，定义如下：<br><pre>template \<typename T\> struct MrgSortSrcList {<br>    \_\_aicore\_\_ MrgSortSrcList() {}<br>    \_\_aicore\_\_ MrgSortSrcList(const LocalTensor&lt;T&gt; src1In, const LocalTensor&lt;T&gt; src2In, const LocalTensor&lt;T&gt; src3In, const LocalTensor&lt;T&gt; src4In)<br>    {<br>        src1 = src1In[0];<br>        src2 = src2In[0];<br>        src3 = src3In[0];<br>        src4 = src4In[0];<br>    }<br>    LocalTensor&lt;T&gt; src1;<br>    LocalTensor&lt;T&gt; src2;<br>    LocalTensor&lt;T&gt; src3;<br>    LocalTensor&lt;T&gt; src4;<br>};</pre>src1、src2、src3、src4类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要8字节对齐。|
| params | 输入 | 排序参数，类型为MrgSort4Info结构体。<br> MrgSort4Info参数说明请参考[表MrgSort4Info结构体参数定义](MrgSort.md#参数说明)。|

**表3**  MrgSort4Info结构体参数定义

| 参数名称 | 含义 |
|---|---|
| elementLengths | 长度为4的uint16\_t数组，用于存储4个源队列的长度，单位为8B。取值范围为[0, 4095]。 |
| ifExhaustedSuspension | 某个队列耗尽时，控制指令是否停止执行。类型为bool，默认值为false。<br>&bull; false：某个队列耗尽时，指令不停止。<br>&bull; true：某个队列耗尽时，指令停止。|
| validBit | 控制参与计算的队列，有效值为3、7、15。<br>&bull;取值为3（0'b0011），表示前2个队列有效。<br>&bull;取值为7（0'b0111），表示前3个队列有效。<br>&bull;取值为15（0'b1111），表示前4个队列有效。|
| repeatTimes | 重复迭代次数。每次迭代完成4个队列的归并排序，源操作数和目的操作数跳过4个队列总长度。取值范围：repeatTimes∈[0, 255]。<br> repeatTimes参数生效需要满足4个条件： <br>&bull; src包含4个队列并且长度一致。<br>&bull; 4个队列连续存储。<br>&bull; ifExhaustedSuspension = false。<br>&bull; validBit = 15。|

## 数据类型

操作数支持的数据类型为：half/float。

## 约束说明<a name="section633mcpsimp"></a>

- 不支持源操作数和目的操作数地址重叠。
- dst的起始地址需要32字节对齐，src中LocalTensor的起始地址需要8字节对齐。
- 如果存在score\[i\]==score\[j\]并且i\>j，将优先选取score\[j\]排在前面。
- 每次迭代内的数据会进行排序，不同迭代间的数据不会进行排序。
<!-- npu="A3,910b,950" id12 -->
- 当参数repeatTimes取值为0时，该接口的行为如下：
    <!-- npu="A3,910b" id10 -->
    - 针对如下型号，当参数repeatTimes取值为0时，不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。
       <!-- npu="A3" id8 -->
       - Atlas A3 训练系列产品/Atlas A3 推理系列产品
       <!-- end id8 -->
       <!-- npu="910b" id9 -->
       - Atlas A2 训练系列产品/Atlas A2 推理系列产品
       <!-- end id9 -->
    <!-- end id10 -->
    <!-- npu="950" id11 -->
    - 针对Ascend 950PR/Ascend 950DT，该接口通过VF调用[Reg矢量计算](../../Reg矢量计算/Reg矢量计算.md)API实现兼容，当参数repeatTimes取值为0时，不保证该接口被视为NOP（空操作）。
    <!-- end id11 -->
<!-- end id12 -->


## 调用示例<a name="section642mcpsimp"></a>

完整示例请参考[MrgSort样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/mrg_sort)。

```cpp
// 对8个已排好序的队列进行合并排序，repeatTimes = 2，数据连续存放
// 每个队列包含32个(score,index)的8Bytes结构
// 最后输出对score域的256个数完成排序后的结果
AscendC::MrgSort4Info params;
params.elementLengths[0] = 32;
params.elementLengths[1] = 32;
params.elementLengths[2] = 32;
params.elementLengths[3] = 32;
params.ifExhaustedSuspension = false;
params.validBit = 0b1111;
params.repeatTimes = 2;

AscendC::MrgSortSrcList<float> srcList;
srcList.src1 = workLocal[0];
srcList.src2 = workLocal[64]; // workLocal为float类型，每个队列占据256Bytes空间
srcList.src3 = workLocal[128];
srcList.src4 = workLocal[192];

AscendC::MrgSort<float>(dstLocal, srcList, params);
outQueueDst.EnQue<float>(dstLocal);
outQueueDst.FreeTensor(dstLocal);
```
