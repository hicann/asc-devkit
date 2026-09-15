# complex32/complex64<a name="ZH-CN_TOPIC_0000002187413378"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T11:59:18.940Z -->

## Applicable Products<a name="section73648168211"></a>

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


Complex number types. **complex32** represents a complex number whose real part and imaginary part are both of the **half** type, with a bit width of 32 bits; **complex64** represents a complex number whose real part and imaginary part are both of the **float** type, with a bit width of 64 bits.

The specific definition is as follows:

```cpp
namespace AscendC {
template<class T>
struct Complex {
    T real;
    T imag;
};
} // namespace AscendC
using complex32 = AscendC::Complex<half>;
using complex64 = AscendC::Complex<float>;
```

**Table 1** Complex template parameter description

<a name="zh-cn_topic_0000001441184464_table18149577913"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001441184464_row61411571196"><th class="cellrowborder" valign="top" width="19.59%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000001441184464_p2093713281104"><a name="zh-cn_topic_0000001441184464_p2093713281104"></a><a name="zh-cn_topic_0000001441184464_p2093713281104"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="80.41%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000001441184464_p393813285106"><a name="zh-cn_topic_0000001441184464_p393813285106"></a><a name="zh-cn_topic_0000001441184464_p393813285106"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001441184464_row8906103284616"><td class="cellrowborder" valign="top" width="19.59%" headers="mcps1.2.3.1.1 "><p id="p71771922134011"><a name="p71771922134011"></a><a name="p71771922134011"></a>T</p></td>
<td class="cellrowborder" valign="top" width="80.41%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000001441184464_p14953134584410"><a name="zh-cn_topic_0000001441184464_p14953134584410"></a><a name="zh-cn_topic_0000001441184464_p14953134584410"></a>Data type of the real and imaginary parts, only supported for half/float.</p></td>
</tr>
</tbody>
</table>

**Table 2** Complex struct parameter description

<a name="table153364918102"></a>
<table><thead align="left"><tr id="row7363209171013"><th class="cellrowborder" valign="top" width="19.62%" id="mcps1.2.3.1.1"><p id="p136399171010"><a name="p136399171010"></a><a name="p136399171010"></a><strong id="b137544519107"><a name="b137544519107"></a><a name="b137544519107"></a>Function</strong></p></th>
<th class="cellrowborder" valign="top" width="80.38%" id="mcps1.2.3.1.2"><p id="p7363209141012"><a name="p7363209141012"></a><a name="p7363209141012"></a><strong id="b1767135119100"><a name="b1767135119100"></a><a name="b1767135119100"></a>Input Parameter Description</strong></p></th>
</tr>
</thead>
<tbody><tr id="row2036317913108"><td class="cellrowborder" valign="top" width="19.62%" headers="mcps1.2.3.1.1 "><p id="p57906662716"><a name="p57906662716"></a><a name="p57906662716"></a>**real**</p></td>
<td class="cellrowborder" valign="top" width="80.38%" headers="mcps1.2.3.1.2 "><p id="p5441241210"><a name="p5441241210"></a><a name="p5441241210"></a>Real part, of type **T**, only supported for half/float.</p></td>
</tr>
<tr id="row424115175561"><td class="cellrowborder" valign="top" width="19.62%" headers="mcps1.2.3.1.1 "><p id="p11241181710563"><a name="p11241181710563"></a><a name="p11241181710563"></a>**imag**</p></td>
<td class="cellrowborder" valign="top" width="80.38%" headers="mcps1.2.3.1.2 "><p id="p172416177566"><a name="p172416177566"></a><a name="p172416177566"></a>Imaginary part, of type **T**, only supported for half/float.</p></td>
</tr>
</tbody>
</table>

The following is an example:

```cpp
// value0 represents a complex number with a real part of 1 and an imaginary part of 2, that is, 1+2j.
complex32 value0(1, 2);
// value1 represents a complex number with a real part of 3 and an imaginary part of 0, that is, 3+0j.
complex32 value1(3);
// value2 represents a complex number with a real part of 4 and an imaginary part of 0, that is, 4+0j.
complex64 value2 = 4;
```

