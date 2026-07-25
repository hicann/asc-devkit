# Get<a name="ZH-CN_TOPIC_0000001470540244"></a>

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
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->

## 功能说明<a name="section954881431816"></a>

头文件路径为：`"basic_api/kernel_tpipe.h"`。

从TBuf上获取指定长度的Tensor，或者获取全部长度的Tensor。

## 函数原型<a name="section1449617323189"></a>

-   获取全部长度的Tensor

    ```cpp
    template <typename T>
    __aicore__ inline LocalTensor<T> Get()
    ```

-   获取指定长度的Tensor

    ```cpp
    template <typename T>
    __aicore__ inline LocalTensor<T> Get(uint32_t len)
    ```

## 参数说明<a name="section85121215142514"></a>

**表1**  模板参数说明

<a name="table012895562310"></a>
<table><thead align="left"><tr id="row2128195532318"><th class="cellrowborder" valign="top" width="12.65%" id="mcps1.2.3.1.1"><p id="p1212885512232"><a name="p1212885512232"></a><a name="p1212885512232"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="87.35000000000001%" id="mcps1.2.3.1.2"><p id="p16129155552315"><a name="p16129155552315"></a><a name="p16129155552315"></a>含义</p>
</th>
</tr>
</thead>
<tbody><tr id="row6129355182310"><td class="cellrowborder" valign="top" width="12.65%" headers="mcps1.2.3.1.1 "><p id="p81291855102318"><a name="p81291855102318"></a><a name="p81291855102318"></a>T</p>
</td>
<td class="cellrowborder" valign="top" width="87.35000000000001%" headers="mcps1.2.3.1.2 "><p id="p14983161812418"><a name="p14983161812418"></a><a name="p14983161812418"></a>待获取Tensor的数据类型，支持的类型请见<a href="../../数据结构/LocalTensor/LocalTensor.md">LocalTensor</a>相关描述。</p>
</td>
</tr>
</tbody>
</table>

**表2**  参数说明

<a name="table1794522316251"></a>
<table><thead align="left"><tr id="row19456238252"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.2.4.1.1"><p id="p119458239258"><a name="p119458239258"></a><a name="p119458239258"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="12.379999999999999%" id="mcps1.2.4.1.2"><p id="p9945152332514"><a name="p9945152332514"></a><a name="p9945152332514"></a>输入/输出</p>
</th>
<th class="cellrowborder" valign="top" width="75.26%" id="mcps1.2.4.1.3"><p id="p1594552312513"><a name="p1594552312513"></a><a name="p1594552312513"></a>含义</p>
</th>
</tr>
</thead>
<tbody><tr id="row1694552372511"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p1094516239250"><a name="p1094516239250"></a><a name="p1094516239250"></a>len</p>
</td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p8945112312514"><a name="p8945112312514"></a><a name="p8945112312514"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p169454239253"><a name="p169454239253"></a><a name="p169454239253"></a>需要获取的Tensor元素个数。</p>
</td>
</tr>
</tbody>
</table>

## 约束说明<a name="section633mcpsimp"></a>

len的数值是Tensor中元素的个数，len\*sizeof\(T\)不能超过TBuf初始化时的长度。

## 返回值说明<a name="section640mcpsimp"></a>

获取到的[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)。

## 调用示例<a name="section5725818154718"></a>

```cpp
// 为TBuf初始化分配内存，分配内存长度为1024字节
AscendC::TPipe pipe;
AscendC::TBuf<AscendC::TPosition::VECCALC> calcBuf; // 模板参数为TPosition中的VECCALC类型
uint32_t byteLen = 1024;
pipe.InitBuffer(calcBuf, byteLen);
// 从calcBuf获取Tensor,Tensor为pipe分配的所有内存大小，为1024字节
AscendC::LocalTensor<int32_t> tempTensor1 = calcBuf.Get<int32_t>();
// 从calcBuf获取Tensor,Tensor为128个int32_t类型元素的内存大小，为512字节
AscendC::LocalTensor<int32_t> tempTensor2 = calcBuf.Get<int32_t>(128);
```

对同一个TBuf对象连续调用Get接口，获取到的Tensor首地址是相同的，不会依次向后偏移。如果需要获取偏移之后的Tensor，可以使用如下方法：

```
AscendC::TPipe pipe;
// 模板参数为TPosition中的VECCALC类型
AscendC::TBuf<AscendC::TPosition::VECCALC> calcBuf; 
// 分配一个2048字节的连续空间
uint32_t byteLen = 2048;
pipe.InitBuffer(calcBuf, byteLen);
// 从calcBuf获取tensor1，tensor1为pipe分配的所有内存大小，为2048字节
AscendC::LocalTensor<int32_t> tensor1 = calcBuf.Get<int32_t>();
// 用户指定tensor1的第256个int32_t的偏移位置为tensor2的首地址，实际上tensor2可以使用的内存大小为1024字节，两个tensor首地址相差256 * sizeof(int32_t)字节
auto tensor2 = tensor1[256];
```
