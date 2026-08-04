# ListTensorDesc<a name="ZH-CN_TOPIC_0000001714160421"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：不支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
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
- Atlas 训练系列产品：不支持
<!-- end id7 -->


## 功能说明<a name="section195171847105215"></a>

ListTensorDesc用来解析符合以下内存排布格式的数据，并在kernel侧根据索引获取储存对应数据的地址及shape信息。

![](../../../figures/Tiling---副本.png)

## 需要包含的头文件<a name="section12341115212912"></a>

```cpp
#include "kernel_operator_list_tensor_intf.h"
```

## 函数原型<a name="zh-cn_topic_0000001441184464_section620mcpsimp"></a>

```cpp
class ListTensorDesc {
    ListTensorDesc();
    ListTensorDesc(__gm__ void* data, uint32_t length = 0xffffffff, uint32_t shapeSize = 0xffffffff);
    void Init(__gm__ void* data, uint32_t length = 0xffffffff, uint32_t shapeSize = 0xffffffff);
    template<class T> void GetDesc(TensorDesc<T>& desc, uint32_t index);
    template<class T> T* GetDataPtr(uint32_t index);
    uint32_t GetSize();
}
```

## 函数说明<a name="section396516531098"></a>

**表1**  模板参数说明

<a name="table13588175515344"></a>
<table><thead align="left"><tr id="row1160915519346"><th class="cellrowborder" valign="top" width="21.8%" id="mcps1.2.3.1.1"><p id="p9609105553412"><a name="p9609105553412"></a><a name="p9609105553412"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="78.2%" id="mcps1.2.3.1.2"><p id="p156091955143419"><a name="p156091955143419"></a><a name="p156091955143419"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row1545073919457"><td class="cellrowborder" valign="top" width="21.8%" headers="mcps1.2.3.1.1 "><p id="p1745103924512"><a name="p1745103924512"></a><a name="p1745103924512"></a>T</p>
</td>
<td class="cellrowborder" valign="top" width="78.2%" headers="mcps1.2.3.1.2 "><p id="p11700113714195"><a name="p11700113714195"></a><a name="p11700113714195"></a>Tensor中元素的数据类型。</p>
</td>
</tr>
</tbody>
</table>

**表2**  函数及参数说明

<a name="table153364918102"></a>
<table><thead align="left"><tr id="row7363209171013"><th class="cellrowborder" valign="top" width="22.58%" id="mcps1.2.4.1.1"><p id="p136399171010"><a name="p136399171010"></a><a name="p136399171010"></a><strong id="b137544519107"><a name="b137544519107"></a><a name="b137544519107"></a>函数名称</strong></p>
</th>
<th class="cellrowborder" valign="top" width="32.879999999999995%" id="mcps1.2.4.1.2"><p id="p7363209141012"><a name="p7363209141012"></a><a name="p7363209141012"></a><strong id="b1767135119100"><a name="b1767135119100"></a><a name="b1767135119100"></a>入参说明</strong></p>
</th>
<th class="cellrowborder" valign="top" width="44.54%" id="mcps1.2.4.1.3"><p id="p736369161015"><a name="p736369161015"></a><a name="p736369161015"></a><strong id="b3770145112107"><a name="b3770145112107"></a><a name="b3770145112107"></a>含义</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row8679452171114"><td class="cellrowborder" valign="top" width="22.58%" headers="mcps1.2.4.1.1 "><p id="p048812536114"><a name="p048812536114"></a><a name="p048812536114"></a>ListTensorDesc</p>
</td>
<td class="cellrowborder" valign="top" width="32.879999999999995%" headers="mcps1.2.4.1.2 "><p id="p20679185210116"><a name="p20679185210116"></a><a name="p20679185210116"></a>-</p>
</td>
<td class="cellrowborder" valign="top" width="44.54%" headers="mcps1.2.4.1.3 "><p id="p66799524117"><a name="p66799524117"></a><a name="p66799524117"></a>默认构造函数，需配合Init函数使用。</p>
</td>
</tr>
<tr id="row2036317913108"><td class="cellrowborder" valign="top" width="22.58%" headers="mcps1.2.4.1.1 "><p id="p836312981011"><a name="p836312981011"></a><a name="p836312981011"></a>ListTensorDesc</p>
</td>
<td class="cellrowborder" valign="top" width="32.879999999999995%" headers="mcps1.2.4.1.2 "><p id="p412513492010"><a name="p412513492010"></a><a name="p412513492010"></a>data：待解析数据的首地址</p>
<p id="p13363493109"><a name="p13363493109"></a><a name="p13363493109"></a>length：待解析内存的长度</p>
<p id="p133633981020"><a name="p133633981020"></a><a name="p133633981020"></a>shapeSize：数据指针的个数</p>
<p id="p5441241210"><a name="p5441241210"></a><a name="p5441241210"></a>length和shapeSize仅用于校验，不填写时不进行校验</p>
</td>
<td class="cellrowborder" valign="top" width="44.54%" headers="mcps1.2.4.1.3 "><p id="p736312951011"><a name="p736312951011"></a><a name="p736312951011"></a>ListTensorDesc类的构造函数，用于解析对应的内存排布。</p>
</td>
</tr>
<tr id="row18487145155715"><td class="cellrowborder" valign="top" width="22.58%" headers="mcps1.2.4.1.1 "><p id="p727005014572"><a name="p727005014572"></a><a name="p727005014572"></a>Init</p>
</td>
<td class="cellrowborder" valign="top" width="32.879999999999995%" headers="mcps1.2.4.1.2 "><p id="p337451113583"><a name="p337451113583"></a><a name="p337451113583"></a>data：待解析数据的首地址</p>
<p id="p637411135819"><a name="p637411135819"></a><a name="p637411135819"></a>length：待解析内存的长度</p>
<p id="p11374171114582"><a name="p11374171114582"></a><a name="p11374171114582"></a>shapeSize：数据指针的个数</p>
<p id="p10374131114583"><a name="p10374131114583"></a><a name="p10374131114583"></a>length和shapeSize仅用于校验，不填写时不进行校验</p>
</td>
<td class="cellrowborder" valign="top" width="44.54%" headers="mcps1.2.4.1.3 "><p id="p1848724515571"><a name="p1848724515571"></a><a name="p1848724515571"></a>初始化函数，用于解析对应的内存排布。</p>
</td>
</tr>
<tr id="row6363594108"><td class="cellrowborder" valign="top" width="22.58%" headers="mcps1.2.4.1.1 "><p id="p203647981019"><a name="p203647981019"></a><a name="p203647981019"></a>GetDesc</p>
</td>
<td class="cellrowborder" valign="top" width="32.879999999999995%" headers="mcps1.2.4.1.2 "><p id="p036429151011"><a name="p036429151011"></a><a name="p036429151011"></a>desc：出参，解析后的Tensor描述信息</p>
<p id="p1836410919102"><a name="p1836410919102"></a><a name="p1836410919102"></a>index：索引值</p>
</td>
<td class="cellrowborder" valign="top" width="44.54%" headers="mcps1.2.4.1.3 "><p id="p1364129101013"><a name="p1364129101013"></a><a name="p1364129101013"></a>根据index获得功能说明图中对应的TensorDesc信息。</p>
<p id="zh-cn_topic_0000001441184464_p4760716131514"><a name="zh-cn_topic_0000001441184464_p4760716131514"></a><a name="zh-cn_topic_0000001441184464_p4760716131514"></a>使用GetDesc前需要先调用TensorDesc.SetShapeAddr为desc指定用于储存shape信息的地址，调用GetDesc后会将shape信息写入该地址。</p>
<p id="p1163195320433"><a name="p1163195320433"></a><a name="p1163195320433"></a><span id="ph340755317588"><a name="ph340755317588"></a><a name="ph340755317588"></a><term id="zh-cn_topic_0000001312391781_term1964153212227_1"><a name="zh-cn_topic_0000001312391781_term1964153212227_1"></a><a name="zh-cn_topic_0000001312391781_term1964153212227_1"></a>Atlas 推理系列产品</term>AI Core</span>支持该功能</p>
<p id="p4601733194519"><a name="p4601733194519"></a><a name="p4601733194519"></a><span id="ph176033374518"><a name="ph176033374518"></a><a name="ph176033374518"></a><term id="zh-cn_topic_0000001312391781_term71949488213_1"><a name="zh-cn_topic_0000001312391781_term71949488213_1"></a><a name="zh-cn_topic_0000001312391781_term71949488213_1"></a>Atlas 训练系列产品</term></span>不支持该功能</p>
<p id="p173433415610"><a name="p173433415610"></a><a name="p173433415610"></a><span id="ph113414344611"><a name="ph113414344611"></a><a name="ph113414344611"></a><term id="zh-cn_topic_0000001312391781_term11962195213215_1"><a name="zh-cn_topic_0000001312391781_term11962195213215_1"></a><a name="zh-cn_topic_0000001312391781_term11962195213215_1"></a>Atlas A2 训练系列产品</term>/<term id="zh-cn_topic_0000001312391781_term184716139811_1"><a name="zh-cn_topic_0000001312391781_term184716139811_1"></a><a name="zh-cn_topic_0000001312391781_term184716139811_1"></a>Atlas A2 推理系列产品</term></span>支持该功能</p>
<p id="p286194811518"><a name="p286194811518"></a><a name="p286194811518"></a><span id="ph14862134820150"><a name="ph14862134820150"></a><a name="ph14862134820150"></a><term id="zh-cn_topic_0000001312391781_term1253731311225_1"><a name="zh-cn_topic_0000001312391781_term1253731311225_1"></a><a name="zh-cn_topic_0000001312391781_term1253731311225_1"></a>Atlas A3 训练系列产品</term>/<term id="zh-cn_topic_0000001312391781_term131434243115_1"><a name="zh-cn_topic_0000001312391781_term131434243115_1"></a><a name="zh-cn_topic_0000001312391781_term131434243115_1"></a>Atlas A3 推理系列产品</term></span>支持该功能</p>
<p id="p830124417119"><a name="p830124417119"></a><a name="p830124417119"></a><span id="ph15301744513"><a name="ph15301744513"></a><a name="ph15301744513"></a><term id="zh-cn_topic_0000001312391781_term354143892110_1"><a name="zh-cn_topic_0000001312391781_term354143892110_1"></a><a name="zh-cn_topic_0000001312391781_term354143892110_1"></a>Atlas 200I/500 A2 推理产品</term></span>不支持该功能</p>
</td>
</tr>
<tr id="row936499191010"><td class="cellrowborder" valign="top" width="22.58%" headers="mcps1.2.4.1.1 "><p id="p1636415981013"><a name="p1636415981013"></a><a name="p1636415981013"></a>GetDataPtr</p>
</td>
<td class="cellrowborder" valign="top" width="32.879999999999995%" headers="mcps1.2.4.1.2 "><p id="p93647921018"><a name="p93647921018"></a><a name="p93647921018"></a>index：索引值</p>
</td>
<td class="cellrowborder" valign="top" width="44.54%" headers="mcps1.2.4.1.3 "><p id="p4364996106"><a name="p4364996106"></a><a name="p4364996106"></a>根据index获取储存对应数据的地址。</p>
</td>
</tr>
<tr id="row1936417910103"><td class="cellrowborder" valign="top" width="22.58%" headers="mcps1.2.4.1.1 "><p id="p133641294106"><a name="p133641294106"></a><a name="p133641294106"></a>GetSize</p>
</td>
<td class="cellrowborder" valign="top" width="32.879999999999995%" headers="mcps1.2.4.1.2 "><p id="p636449101018"><a name="p636449101018"></a><a name="p636449101018"></a>-</p>
</td>
<td class="cellrowborder" valign="top" width="44.54%" headers="mcps1.2.4.1.3 "><p id="p2036420914106"><a name="p2036420914106"></a><a name="p2036420914106"></a>获取ListTensor中包含的数据指针的个数。</p>
</td>
</tr>
</tbody>
</table>

## 调用示例<a name="section1742652412511"></a>

示例中待解析的srcGm内存排布如下图所示：

![](../../../figures/zh-cn_image_0000001866617737.png)

```cpp
AscendC::ListTensorDesc listTensorDesc(reinterpret_cast<__gm__ void *>(srcGm)); // srcGm为待解析的gm地址
uint32_t size = listTensorDesc.GetSize();                                       // size = 2
auto dataPtr0 = listTensorDesc.GetDataPtr<int32_t>(0);                          // 获取ptr0
auto dataPtr1 = listTensorDesc.GetDataPtr<int32_t>(1);                          // 获取ptr1

uint64_t buf[100] = {0}; // 示例中Tensor的dim为3,此处的100表示预留足够大的空间
AscendC::TensorDesc<int32_t> desc;
desc.SetShapeAddr(buf);          // 为desc指定用于储存shape信息的地址
listTensorDesc.GetDesc(desc, 0); // 获取索引0的shape信息

uint64_t dim = desc.GetDim();   // dim = 3
uint64_t idx = desc.GetIndex(); // idx = 0
uint64_t shape[3] = {0};
for (uint32_t i = 0; i < desc.GetDim(); i++)
{
    shape[i] = desc.GetShape(i); // GetShape(0) = 1, GetShape(1) = 2, GetShape(2) = 3
}
auto ptr = desc.GetDataPtr();
```
