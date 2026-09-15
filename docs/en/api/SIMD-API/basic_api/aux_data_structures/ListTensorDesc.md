# ListTensorDesc<a name="ZH-CN_TOPIC_0000001714160421"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T11:59:41.581Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
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
- Atlas training products: Not supported
<!-- end id7 -->


## Description<a name="section195171847105215"></a>

**ListTensorDesc** is used to parse data that conforms to the following memory layout format, and to obtain the address and shape information of the corresponding stored data by index on the kernel side.

![](../../../figures/tiling_copy.png)

## Header Files to Include<a name="section12341115212912"></a>

```cpp
#include "kernel_operator_list_tensor_intf.h"
```

## Prototype<a name="zh-cn_topic_0000001441184464_section620mcpsimp"></a>

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

## Description<a name="section396516531098"></a>

**Table 1**  Template parameter description

<a name="table13588175515344"></a>
<table><thead align="left"><tr id="row1160915519346"><th class="cellrowborder" valign="top" width="21.8%" id="mcps1.2.3.1.1"><p id="p9609105553412"><a name="p9609105553412"></a><a name="p9609105553412"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="78.2%" id="mcps1.2.3.1.2"><p id="p156091955143419"><a name="p156091955143419"></a><a name="p156091955143419"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1545073919457"><td class="cellrowborder" valign="top" width="21.8%" headers="mcps1.2.3.1.1 "><p id="p1745103924512"><a name="p1745103924512"></a><a name="p1745103924512"></a>T</p></td>
<td class="cellrowborder" valign="top" width="78.2%" headers="mcps1.2.3.1.2 "><p id="p11700113714195"><a name="p11700113714195"></a><a name="p11700113714195"></a>Data type of the elements in the tensor.</p></td>
</tr>
</tbody>
</table>

**Table 2**  Function and parameter description

<a name="table153364918102"></a>
<table><thead align="left"><tr id="row7363209171013"><th class="cellrowborder" valign="top" width="22.58%" id="mcps1.2.4.1.1"><p id="p136399171010"><a name="p136399171010"></a><a name="p136399171010"></a><strong id="b137544519107"><a name="b137544519107"></a><a name="b137544519107"></a>Function Name</strong></p></th>
<th class="cellrowborder" valign="top" width="32.879999999999995%" id="mcps1.2.4.1.2"><p id="p7363209141012"><a name="p7363209141012"></a><a name="p7363209141012"></a><strong id="b1767135119100"><a name="b1767135119100"></a><a name="b1767135119100"></a>Parameter</strong></p></th>
<th class="cellrowborder" valign="top" width="44.54%" id="mcps1.2.4.1.3"><p id="p736369161015"><a name="p736369161015"></a><a name="p736369161015"></a><strong id="b3770145112107"><a name="b3770145112107"></a><a name="b3770145112107"></a>Description</strong></p></th>
</tr>
</thead>
<tbody><tr id="row8679452171114"><td class="cellrowborder" valign="top" width="22.58%" headers="mcps1.2.4.1.1 "><p id="p048812536114"><a name="p048812536114"></a><a name="p048812536114"></a>**ListTensorDesc**</p></td>
<td class="cellrowborder" valign="top" width="32.879999999999995%" headers="mcps1.2.4.1.2 "><p id="p20679185210116"><a name="p20679185210116"></a><a name="p20679185210116"></a>-</p></td>
<td class="cellrowborder" valign="top" width="44.54%" headers="mcps1.2.4.1.3 "><p id="p66799524117"><a name="p66799524117"></a><a name="p66799524117"></a>Default constructor, which must be used together with the **Init** function.</p></td>
</tr>
<tr id="row2036317913108"><td class="cellrowborder" valign="top" width="22.58%" headers="mcps1.2.4.1.1 "><p id="p836312981011"><a name="p836312981011"></a><a name="p836312981011"></a>**ListTensorDesc**</p></td>
<td class="cellrowborder" valign="top" width="32.879999999999995%" headers="mcps1.2.4.1.2 "><p id="p412513492010"><a name="p412513492010"></a><a name="p412513492010"></a>**data**: Start address of the data to be parsed.</p>
<p id="p13363493109"><a name="p13363493109"></a><a name="p13363493109"></a>**length**: Length of the memory to be parsed.</p>
<p id="p133633981020"><a name="p133633981020"></a><a name="p133633981020"></a>**shapeSize**: Number of data pointers.</p>
<p id="p5441241210"><a name="p5441241210"></a><a name="p5441241210"></a>**length** and **shapeSize** are used only for validation. If they are not specified, no validation is performed.</p></td>
<td class="cellrowborder" valign="top" width="44.54%" headers="mcps1.2.4.1.3 "><p id="p736312951011"><a name="p736312951011"></a><a name="p736312951011"></a>Constructor of the **ListTensorDesc** class, used to parse the corresponding memory layout.</p></td>
</tr>
<tr id="row18487145155715"><td class="cellrowborder" valign="top" width="22.58%" headers="mcps1.2.4.1.1 "><p id="p727005014572"><a name="p727005014572"></a><a name="p727005014572"></a>**Init**</p></td>
<td class="cellrowborder" valign="top" width="32.879999999999995%" headers="mcps1.2.4.1.2 "><p id="p337451113583"><a name="p337451113583"></a><a name="p337451113583"></a>**data**: Start address of the data to be parsed.</p>
<p id="p637411135819"><a name="p637411135819"></a><a name="p637411135819"></a>**length**: Length of the memory to be parsed.</p>
<p id="p11374171114582"><a name="p11374171114582"></a><a name="p11374171114582"></a>**shapeSize**: Number of data pointers.</p>
<p id="p10374131114583"><a name="p10374131114583"></a><a name="p10374131114583"></a>**length** and **shapeSize** are used only for validation. If they are not specified, no validation is performed.</p></td>
<td class="cellrowborder" valign="top" width="44.54%" headers="mcps1.2.4.1.3 "><p id="p1848724515571"><a name="p1848724515571"></a><a name="p1848724515571"></a>Initialization function used to parse the corresponding memory layout.</p></td>
</tr>
<tr id="row6363594108"><td class="cellrowborder" valign="top" width="22.58%" headers="mcps1.2.4.1.1 "><p id="p203647981019"><a name="p203647981019"></a><a name="p203647981019"></a>**GetDesc**</p></td>
<td class="cellrowborder" valign="top" width="32.879999999999995%" headers="mcps1.2.4.1.2 "><p id="p036429151011"><a name="p036429151011"></a><a name="p036429151011"></a>**desc**: Output parameter, the parsed tensor description information.</p>
<p id="p1836410919102"><a name="p1836410919102"></a><a name="p1836410919102"></a>**index**: Index value.</p></td>
<td class="cellrowborder" valign="top" width="44.54%" headers="mcps1.2.4.1.3 "><p id="p1364129101013"><a name="p1364129101013"></a><a name="p1364129101013"></a>Obtains the corresponding **TensorDesc** information in the functional description diagram based on **index**.</p>
<p id="zh-cn_topic_0000001441184464_p4760716131514"><a name="zh-cn_topic_0000001441184464_p4760716131514"></a><a name="zh-cn_topic_0000001441184464_p4760716131514"></a>Before calling **GetDesc**, call **TensorDesc.SetShapeAddr** to specify the address for storing shape information for **desc**. After **GetDesc** is called, the shape information is written to this address.</p>
<p id="p1163195320433"><a name="p1163195320433"></a><a name="p1163195320433"></a><span id="ph340755317588"><a name="ph340755317588"></a><a name="ph340755317588"></a><term id="zh-cn_topic_0000001312391781_term1964153212227_1"><a name="zh-cn_topic_0000001312391781_term1964153212227_1"></a><a name="zh-cn_topic_0000001312391781_term1964153212227_1"></a>Atlas inference products</term>AI Core</span> support this feature.</p>
<p id="p4601733194519"><a name="p4601733194519"></a><a name="p4601733194519"></a><span id="ph176033374518"><a name="ph176033374518"></a><a name="ph176033374518"></a><term id="zh-cn_topic_0000001312391781_term71949488213_1"><a name="zh-cn_topic_0000001312391781_term71949488213_1"></a><a name="zh-cn_topic_0000001312391781_term71949488213_1"></a>Atlas training products</term></span> does not support this feature.</p>
<p id="p173433415610"><a name="p173433415610"></a><a name="p173433415610"></a><span id="ph113414344611"><a name="ph113414344611"></a><a name="ph113414344611"></a><term id="zh-cn_topic_0000001312391781_term11962195213215_1"><a name="zh-cn_topic_0000001312391781_term11962195213215_1"></a><a name="zh-cn_topic_0000001312391781_term11962195213215_1"></a>Atlas A2 training products</term>/<term id="zh-cn_topic_0000001312391781_term184716139811_1"><a name="zh-cn_topic_0000001312391781_term184716139811_1"></a><a name="zh-cn_topic_0000001312391781_term184716139811_1"></a>Atlas A2 inference products</term></span> support this feature.</p>
<p id="p286194811518"><a name="p286194811518"></a><a name="p286194811518"></a><span id="ph14862134820150"><a name="ph14862134820150"></a><a name="ph14862134820150"></a><term id="zh-cn_topic_0000001312391781_term1253731311225_1"><a name="zh-cn_topic_0000001312391781_term1253731311225_1"></a><a name="zh-cn_topic_0000001312391781_term1253731311225_1"></a>Atlas A3 training products</term>/<term id="zh-cn_topic_0000001312391781_term131434243115_1"><a name="zh-cn_topic_0000001312391781_term131434243115_1"></a><a name="zh-cn_topic_0000001312391781_term131434243115_1"></a>Atlas A3 inference products</term></span> support this feature.</p>
<p id="p830124417119"><a name="p830124417119"></a><a name="p830124417119"></a><span id="ph15301744513"><a name="ph15301744513"></a><a name="ph15301744513"></a><term id="zh-cn_topic_0000001312391781_term354143892110_1"><a name="zh-cn_topic_0000001312391781_term354143892110_1"></a><a name="zh-cn_topic_0000001312391781_term354143892110_1"></a>Atlas 200I/500 A2 inference products</term></span> does not support this feature.</p></td>
</tr>
<tr id="row936499191010"><td class="cellrowborder" valign="top" width="22.58%" headers="mcps1.2.4.1.1 "><p id="p1636415981013"><a name="p1636415981013"></a><a name="p1636415981013"></a>**GetDataPtr**</p></td>
<td class="cellrowborder" valign="top" width="32.879999999999995%" headers="mcps1.2.4.1.2 "><p id="p93647921018"><a name="p93647921018"></a><a name="p93647921018"></a>**index**: Index value.</p></td>
<td class="cellrowborder" valign="top" width="44.54%" headers="mcps1.2.4.1.3 "><p id="p4364996106"><a name="p4364996106"></a><a name="p4364996106"></a>Obtains the address where the corresponding data is stored based on **index**.</p></td>
</tr>
<tr id="row1936417910103"><td class="cellrowborder" valign="top" width="22.58%" headers="mcps1.2.4.1.1 "><p id="p133641294106"><a name="p133641294106"></a><a name="p133641294106"></a>**GetSize**</p></td>
<td class="cellrowborder" valign="top" width="32.879999999999995%" headers="mcps1.2.4.1.2 "><p id="p636449101018"><a name="p636449101018"></a><a name="p636449101018"></a>-</p></td>
<td class="cellrowborder" valign="top" width="44.54%" headers="mcps1.2.4.1.3 "><p id="p2036420914106"><a name="p2036420914106"></a><a name="p2036420914106"></a>Obtains the number of data pointers contained in **ListTensor**.</p></td>
</tr>
</tbody>
</table>

## Example<a name="section1742652412511"></a>

The memory layout of srcGm to be parsed in the example is shown in the following figure:

![](../../../figures/zh-cn_image_0000001866617737.png)

```cpp
AscendC::ListTensorDesc listTensorDesc(reinterpret_cast<__gm__ void *>(srcGm)); // srcGm is the GM address to be parsed.
uint32_t size = listTensorDesc.GetSize();                                       // size = 2
auto dataPtr0 = listTensorDesc.GetDataPtr<int32_t>(0);                          // Obtain ptr0.
auto dataPtr1 = listTensorDesc.GetDataPtr<int32_t>(1);                          // Obtain ptr1.

uint64_t buf[100] = {0}; // In the example, the dim of the Tensor is 3. Here, 100 indicates reserving sufficient space.
AscendC::TensorDesc<int32_t> desc;
desc.SetShapeAddr(buf);          // Specify the address for desc to store shape information.
listTensorDesc.GetDesc(desc, 0); // Obtain the shape information at index 0.

uint64_t dim = desc.GetDim();   // dim = 3
uint64_t idx = desc.GetIndex(); // idx = 0
uint64_t shape[3] = {0};
for (uint32_t i = 0; i < desc.GetDim(); i++)
{
    shape[i] = desc.GetShape(i); // GetShape(0) = 1, GetShape(1) = 2, GetShape(2) = 3
}
auto ptr = desc.GetDataPtr();
```
