# Operator Basic Concepts<a name="ZH-CN_TOPIC_0000001704079692"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T12:35:18.383Z pushedAt=2026-09-10T12:19:25.171Z -->

An operator (OP) is the basic unit that performs a specific mathematical operation in a deep learning algorithm, such as an activation function (for example, ReLU), convolution (Conv), pooling, and normalization (for example, Softmax). A neural network model can be built by combining these operators.

This section describes the basic concepts commonly used in operators.

## Operator Name<a name="zh-cn_topic_0228422146_zh-cn_topic_0187054064_section6753939141311"></a>

The operator name is used to identify an operator in a network. Operator names must be unique within the same network. As shown in the following figure, Conv1, Pool1, and Conv2 are operator names in this network. Conv1 and Conv2 are of the Convolution type, indicating that a convolution operation is performed respectively.

**Figure 1**  Network topology example<a name="fig129541116161914"></a>  
![](../../../figures/network_topology_example.png "Network topology example")

## Operator Type<a name="zh-cn_topic_0228422146_zh-cn_topic_0187054064_section44191431191313"></a>

Each operator in a network is matched to its implementation based on its operator type, and operators of the same type share the same implementation logic. A network may contain multiple operators of the same type. For example, both the Conv1 and Conv2 operators in the preceding figure are of the Convolution type.

## Tensor<a name="zh-cn_topic_0228422146_zh-cn_topic_0187054064_section31275509454"></a>

A tensor is a container for operator computation data and contains the following attributes.

**Table 1** Tensor attributes

<a name="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_zh-cn_topic_0160787607_table196765568157"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_zh-cn_topic_0160787607_ra39c1f8353b1438b9aeba38c06570057"><th class="cellrowborder" valign="top" width="28.199999999999996%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_zh-cn_topic_0160787607_zh-cn_topic_0116955499_p6480165326"><a name="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_zh-cn_topic_0160787607_zh-cn_topic_0116955499_p6480165326"></a><a name="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_zh-cn_topic_0160787607_zh-cn_topic_0116955499_p6480165326"></a>Attribute</p></th>
<th class="cellrowborder" valign="top" width="71.8%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_zh-cn_topic_0160787607_ab4f79bcaae2544bfad2388951768e902"><a name="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_zh-cn_topic_0160787607_ab4f79bcaae2544bfad2388951768e902"></a><a name="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_zh-cn_topic_0160787607_ab4f79bcaae2544bfad2388951768e902"></a>Definition</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_zh-cn_topic_0160787607_rdecd2ad5c69c483dab9157c02f68a7d7"><td class="cellrowborder" valign="top" width="28.199999999999996%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_zh-cn_topic_0160787607_zh-cn_topic_0116955499_p5400422325"><a name="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_zh-cn_topic_0160787607_zh-cn_topic_0116955499_p5400422325"></a><a name="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_zh-cn_topic_0160787607_zh-cn_topic_0116955499_p5400422325"></a>Shape</p></td>
<td class="cellrowborder" valign="top" width="71.8%" headers="mcps1.2.3.1.2 "><p id="p41638258576"><a name="p41638258576"></a><a name="p41638258576"></a>Shape of the tensor, for example, (10, ), (1024, 1024), or (2, 3, 4). A shape of (3, 4) indicates that the first dimension has 3 elements and the second dimension has 4 elements. (3, 4) represents a matrix array with 3 rows and 4 columns.</p>
<p id="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_p114681417191116"><a name="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_p114681417191116"></a><a name="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_p114681417191116"></a>Format: (i1, i2, ..., in), where i1 to in are positive integers.</p></td>
</tr>
<tr id="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_zh-cn_topic_0160787607_r1318f6b9bdfc45e58964cdf1bce95537"><td class="cellrowborder" valign="top" width="28.199999999999996%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_zh-cn_topic_0160787607_zh-cn_topic_0116955499_p240018213215"><a name="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_zh-cn_topic_0160787607_zh-cn_topic_0116955499_p240018213215"></a><a name="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_zh-cn_topic_0160787607_zh-cn_topic_0116955499_p240018213215"></a>Data type</p></td>
<td class="cellrowborder" valign="top" width="71.8%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_p12467181714116"><a name="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_p12467181714116"></a><a name="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_p12467181714116"></a>Specifies the data type of the tensor object.</p>
<p id="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_p146721701116"><a name="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_p146721701116"></a><a name="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_p146721701116"></a>Value range: float16, float32, int8, int16, int32, uint8, uint16, bfloat16, bool, etc.</p></td>
</tr>
<tr id="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_row1721553016508"><td class="cellrowborder" valign="top" width="28.199999999999996%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_p2021623017509"><a name="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_p2021623017509"></a><a name="zh-cn_topic_0000001265237154_zh-cn_topic_0228422146_zh-cn_topic_0187054064_p2021623017509"></a>Data layout format</p></td>
<td class="cellrowborder" valign="top" width="71.8%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000001265237154_p1445413288586"><a name="zh-cn_topic_0000001265237154_p1445413288586"></a><a name="zh-cn_topic_0000001265237154_p1445413288586"></a>Physical data layout format. For details, see <a href="data_layout_format.md">Data Layout Format</a>.</p></td>
</tr>
</tbody>
</table>

## Shape<a name="section1546519496196"></a>

The shape of a tensor is expressed in the form of \(D0, D1, … ,Dn-1\), where D0 to Dn are arbitrary positive integers.

For example, the shape \(3,4\) indicates that the first dimension has 3 elements and the second dimension has 4 elements. \(3,4\) represents a matrix array with 3 rows and 4 columns.

The first element of the shape corresponds to the number of elements in the outermost brackets of the tensor, the second element corresponds to the number of elements in the second brackets counted from the left, and so on. For example:

**Table 2** Examples of tensor shapes

<a name="table08211915203"></a>
<table><thead align="left"><tr id="row482209142014"><th class="cellrowborder" valign="top" width="27.339999999999996%" id="mcps1.2.4.1.1"><p id="p28223992018"><a name="p28223992018"></a><a name="p28223992018"></a>Tensor</p></th>
<th class="cellrowborder" valign="top" width="34.4%" id="mcps1.2.4.1.2"><p id="p1082213919205"><a name="p1082213919205"></a><a name="p1082213919205"></a>Shape</p></th>
<th class="cellrowborder" valign="top" width="38.26%" id="mcps1.2.4.1.3"><p id="p4259133131813"><a name="p4259133131813"></a><a name="p4259133131813"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1482216942017"><td class="cellrowborder" valign="top" width="27.339999999999996%" headers="mcps1.2.4.1.1 "><p id="p1182211962011"><a name="p1182211962011"></a><a name="p1182211962011"></a>1</p></td>
<td class="cellrowborder" valign="top" width="34.4%" headers="mcps1.2.4.1.2 "><p id="p158221962016"><a name="p158221962016"></a><a name="p158221962016"></a>(0,)</p></td>
<td class="cellrowborder" valign="top" width="38.26%" headers="mcps1.2.4.1.3 "><p id="p22591314182"><a name="p22591314182"></a><a name="p22591314182"></a>0-dimensional tensor, also a scalar</p></td>
</tr>
<tr id="row1382213917205"><td class="cellrowborder" valign="top" width="27.339999999999996%" headers="mcps1.2.4.1.1 "><p id="p782279192017"><a name="p782279192017"></a><a name="p782279192017"></a>[1,2,3]</p></td>
<td class="cellrowborder" valign="top" width="34.4%" headers="mcps1.2.4.1.2 "><p id="p1682209192010"><a name="p1682209192010"></a><a name="p1682209192010"></a>(3,)</p></td>
<td class="cellrowborder" valign="top" width="38.26%" headers="mcps1.2.4.1.3 "><p id="p925912341815"><a name="p925912341815"></a><a name="p925912341815"></a>1-dimensional tensor</p></td>
</tr>
<tr id="row1582269142013"><td class="cellrowborder" valign="top" width="27.339999999999996%" headers="mcps1.2.4.1.1 "><p id="p13822499207"><a name="p13822499207"></a><a name="p13822499207"></a>[[1,2],[3,4]]</p></td>
<td class="cellrowborder" valign="top" width="34.4%" headers="mcps1.2.4.1.2 "><p id="p168224972017"><a name="p168224972017"></a><a name="p168224972017"></a>(2, 2)</p></td>
<td class="cellrowborder" valign="top" width="38.26%" headers="mcps1.2.4.1.3 "><p id="p12259538182"><a name="p12259538182"></a><a name="p12259538182"></a>2-dimensional tensor</p></td>
</tr>
<tr id="row13822109132010"><td class="cellrowborder" valign="top" width="27.339999999999996%" headers="mcps1.2.4.1.1 "><p id="p3822591201"><a name="p3822591201"></a><a name="p3822591201"></a>[[[1,2],[3,4]], [[5,6],[7,8]]]</p></td>
<td class="cellrowborder" valign="top" width="34.4%" headers="mcps1.2.4.1.2 "><p id="p1282229142013"><a name="p1282229142013"></a><a name="p1282229142013"></a>(2, 2, 2)</p></td>
<td class="cellrowborder" valign="top" width="38.26%" headers="mcps1.2.4.1.3 "><p id="p152591631182"><a name="p152591631182"></a><a name="p152591631182"></a>3-dimensional tensor</p></td>
</tr>
</tbody>
</table>

How should we understand its physical meaning? Suppose we have a shape of \(4, 20, 20, 3\).

Suppose there are some photos, and each pixel consists of three colors: red, green, and blue. This is the meaning of the 3 in the shape. The width and height of each photo are both 20, that is, 20*20 = 400 pixels. There are four photos in total. This is the physical meaning of shape = \(4, 20, 20, 3\).

**Figure 2**  Schematic diagram<a name="fig2036333061811"></a>  
![](../../../figures/schematic_diagram.png "schematic diagram")

In terms of programming, the shape can be simply understood as the loops for operating each layer of the tensor. For example, to operate on tensor A with shape = \(4, 20, 20, 3\), the loop statements are as follows:

```
produce A {
  for (i, 0, 4) {
    for (j, 0, 20) {
      for (p, 0, 20) {
        for (q, 0, 3) {
          A[((((((i*20) + j)*20) + p)*3) + q)] = a_tensor[((((((i*20) + j)*20) + p)*3) + q)]
        }
      }
    }
  }
}
```

## Axis<a name="zh-cn_topic_0228422146_section31342202085"></a>

An axis is defined relative to the shape. It represents the subscript of a dimension in the shape of a tensor. For example, tensor a is a two-dimensional array with 5 rows and 6 columns, that is, its shape is \(5,6\). In this case, axis=0 indicates the first dimension of the tensor, that is, the row, and axis=1 indicates the second dimension, that is, the column.

For example, for tensor data \[\[\[1,2\],\[3,4\]\], \[\[5,6\],\[7,8\]\]\] whose shape is \(2,2,2\), axis 0 represents the data of the first dimension, that is, the two matrices \[\[1,2\],\[3,4\]\] and \[\[5,6\],\[7,8\]\]. Axis 1 represents the data of the second dimension, that is, the four arrays \[1,2\], \[3,4\], \[5,6\], and \[7,8\]. Axis 2 represents the data of the third dimension, that is, the eight numbers 1, 2, 3, 4, 5, 6, 7, and 8.

An axis can be a negative number, in which case it indicates the axis-th dimension from the end.

The axes of an N-dimensional tensor are 0, 1, 2, ..., N-1.

**Figure 3** Axis schematic diagram<a name="zh-cn_topic_0228422146_fig82601017112413"></a>  
![](../../../figures/axis_schematic_diagram.png "Axis schematic diagram")

