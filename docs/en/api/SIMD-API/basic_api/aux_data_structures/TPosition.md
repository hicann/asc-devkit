# TPosition<a name="ZH-CN_TOPIC_0000001712887453"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T12:01:28.448Z -->

Ascend C manages physical memory at different levels by using an abstract logical position (**TPosition**) to represent storage at each level, replacing the concept of on-chip physical storage to hide the hardware architecture. The main TPosition types include **VECIN**, **VECOUT**, **VECCALC**, **A1**, **A2**, **B1**, **B2**, **C1**, **C2**, **CO1**, and **CO2**. Among them, **VECIN**, **VECCALC**, and **VECOUT** are mainly used for vector programming, while **A1**, **A2**, **B1**, **B2**, **C1**, **C2**, **CO1**, and **CO2** are used for cube programming. You can refer to [Table 1](../../general_description_and_constraints.md#table07372185712) to understand the mapping between TPosition and physical storage.

TPosition is defined as follows:

```cpp
enum class TPosition : uint8_t {
    GM,
    A1,
    A2,
    B1,
    B2,
    C1,
    C2,
    CO1,
    CO2,
    VECIN,
    VECOUT,
    VECCALC,
    LCM = VECCALC,
    SPM,
    SHM = SPM,
    TSCM,
    C2PIPE2GM,
    C2PIPE2LOCAL,
    MAX,
};
```

The specific definitions of the TPosition enumeration values are as follows:

**Table 1**  Description of TPosition enumeration values

<a name="table5376122715308"></a>
<table><thead align="left"><tr id="row1337716275309"><th class="cellrowborder" valign="top" width="17.53%" id="mcps1.2.3.1.1"><p id="p1537762711305"><a name="p1537762711305"></a><a name="p1537762711305"></a>Enumeration Value</p></th>
<th class="cellrowborder" valign="top" width="82.47%" id="mcps1.2.3.1.2"><p id="p153771127123013"><a name="p153771127123013"></a><a name="p153771127123013"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row175152367479"><td class="cellrowborder" valign="top" width="17.53%" headers="mcps1.2.3.1.1 "><p id="p1351520369474"><a name="p1351520369474"></a><a name="p1351520369474"></a>GM</p></td>
<td class="cellrowborder" valign="top" width="82.47%" headers="mcps1.2.3.1.2 "><p id="p1251517360479"><a name="p1251517360479"></a><a name="p1251517360479"></a>Global Memory, which corresponds to the external storage of <span id="zh-cn_topic_0000001588832845_ph519622414417"><a name="zh-cn_topic_0000001588832845_ph519622414417"></a><a name="zh-cn_topic_0000001588832845_ph519622414417"></a>AI Core</span>.</p></td>
</tr>
<tr id="row19377627133012"><td class="cellrowborder" valign="top" width="17.53%" headers="mcps1.2.3.1.1 "><p id="p2714429161019"><a name="p2714429161019"></a><a name="p2714429161019"></a>VECIN</p></td>
<td class="cellrowborder" valign="top" width="82.47%" headers="mcps1.2.3.1.2 "><p id="p13377122733010"><a name="p13377122733010"></a><a name="p13377122733010"></a>For vector computation, the storage position for data moved in. This position is used when data is moved into the Vector computation unit.</p></td>
</tr>
<tr id="row13377162793019"><td class="cellrowborder" valign="top" width="17.53%" headers="mcps1.2.3.1.1 "><p id="p16117319129"><a name="p16117319129"></a><a name="p16117319129"></a>VECOUT</p></td>
<td class="cellrowborder" valign="top" width="82.47%" headers="mcps1.2.3.1.2 "><p id="p12377122712304"><a name="p12377122712304"></a><a name="p12377122712304"></a>For vector computation, the storage position for data moved out. This position is used when the Vector computation unit results are moved out.</p></td>
</tr>
<tr id="row14563209291"><td class="cellrowborder" valign="top" width="17.53%" headers="mcps1.2.3.1.1 "><p id="p95631897919"><a name="p95631897919"></a><a name="p95631897919"></a>VECCALC</p></td>
<td class="cellrowborder" valign="top" width="82.47%" headers="mcps1.2.3.1.2 "><p id="p1956359598"><a name="p1956359598"></a><a name="p1956359598"></a>For vector computation/cube computation. This position is used when temporary variables are needed during computation.</p></td>
</tr>
<tr id="row28295254916"><td class="cellrowborder" valign="top" width="17.53%" headers="mcps1.2.3.1.1 "><p id="p158298253916"><a name="p158298253916"></a><a name="p158298253916"></a>A1</p></td>
<td class="cellrowborder" valign="top" width="82.47%" headers="mcps1.2.3.1.2 "><p id="p68298253918"><a name="p68298253918"></a><a name="p68298253918"></a>For cube computation, stores whole blocks of cube A, which can be analogous to the second-level cache in a CPU multi-level cache.</p></td>
</tr>
<tr id="row696513258910"><td class="cellrowborder" valign="top" width="17.53%" headers="mcps1.2.3.1.1 "><p id="p896516251990"><a name="p896516251990"></a><a name="p896516251990"></a>B1</p></td>
<td class="cellrowborder" valign="top" width="82.47%" headers="mcps1.2.3.1.2 "><p id="p139658256916"><a name="p139658256916"></a><a name="p139658256916"></a>For cube computation, stores whole blocks of cube B, which can be analogous to the second-level cache in a CPU multi-level cache.</p></td>
</tr>
<tr id="row21718485391"><td class="cellrowborder" valign="top" width="17.53%" headers="mcps1.2.3.1.1 "><p id="p101720489392"><a name="p101720489392"></a><a name="p101720489392"></a>C1</p></td>
<td class="cellrowborder" valign="top" width="82.47%" headers="mcps1.2.3.1.2 "><p id="p1976331084020"><a name="p1976331084020"></a><a name="p1976331084020"></a>For cube computation, stores whole blocks of the Bias cube, which can be analogous to the second-level cache in a CPU multi-level cache.</p></td>
</tr>
<tr id="row201031261391"><td class="cellrowborder" valign="top" width="17.53%" headers="mcps1.2.3.1.1 "><p id="p31036261791"><a name="p31036261791"></a><a name="p31036261791"></a>A2</p></td>
<td class="cellrowborder" valign="top" width="82.47%" headers="mcps1.2.3.1.2 "><p id="p1610362610912"><a name="p1610362610912"></a><a name="p1610362610912"></a>For cube computation, stores split small blocks of the A cube, which can be analogous to the first-level cache in a CPU multi-level cache.</p></td>
</tr>
<tr id="row22337267913"><td class="cellrowborder" valign="top" width="17.53%" headers="mcps1.2.3.1.1 "><p id="p2023314263913"><a name="p2023314263913"></a><a name="p2023314263913"></a>B2</p></td>
<td class="cellrowborder" valign="top" width="82.47%" headers="mcps1.2.3.1.2 "><p id="p323312261998"><a name="p323312261998"></a><a name="p323312261998"></a>For cube computation, stores split small blocks of the B cube, which can be analogous to the first-level cache in a CPU multi-level cache.</p></td>
</tr>
<tr id="row2087225123915"><td class="cellrowborder" valign="top" width="17.53%" headers="mcps1.2.3.1.1 "><p id="p1687225193913"><a name="p1687225193913"></a><a name="p1687225193913"></a>C2</p></td>
<td class="cellrowborder" valign="top" width="82.47%" headers="mcps1.2.3.1.2 "><p id="p250583044018"><a name="p250583044018"></a><a name="p250583044018"></a>For cube computation, stores split small blocks of the Bias cube, which can be analogous to the first-level cache in a CPU multi-level cache.</p></td>
</tr>
<tr id="row14365326693"><td class="cellrowborder" valign="top" width="17.53%" headers="mcps1.2.3.1.1 "><p id="p7365162618910"><a name="p7365162618910"></a><a name="p7365162618910"></a>CO1</p></td>
<td class="cellrowborder" valign="top" width="82.47%" headers="mcps1.2.3.1.2 "><p id="p43656266915"><a name="p43656266915"></a><a name="p43656266915"></a>For cube computation, stores small blocks of the result C cube, which can be understood as Cube Out.</p></td>
</tr>
<tr id="row194957263919"><td class="cellrowborder" valign="top" width="17.53%" headers="mcps1.2.3.1.1 "><p id="p4495142613918"><a name="p4495142613918"></a><a name="p4495142613918"></a>CO2</p></td>
<td class="cellrowborder" valign="top" width="82.47%" headers="mcps1.2.3.1.2 "><p id="p949611261196"><a name="p949611261196"></a><a name="p949611261196"></a>For cube computation, stores the whole result C cube, which can be understood as Cube Out.</p></td>
</tr>
<tr id="row1682573410509"><td class="cellrowborder" valign="top" width="17.53%" headers="mcps1.2.3.1.1 "><p id="p1082533425015"><a name="p1082533425015"></a><a name="p1082533425015"></a>LCM</p></td>
<td class="cellrowborder" valign="top" width="82.47%" headers="mcps1.2.3.1.2 "><p id="p58251234135017"><a name="p58251234135017"></a><a name="p58251234135017"></a>Local Cache Memory, representing a temporary shared Unified Buffer space. It is an alias of VECCALC and implements the same functionality as VECCALC.</p></td>
</tr>
<tr id="row18899424276"><td class="cellrowborder" valign="top" width="17.53%" headers="mcps1.2.3.1.1 "><p id="p9889942112716"><a name="p9889942112716"></a><a name="p9889942112716"></a>SPM</p></td>
<td class="cellrowborder" valign="top" width="82.47%" headers="mcps1.2.3.1.2 "><p id="p18891442102718"><a name="p18891442102718"></a><a name="p18891442102718"></a>Used for temporary data storage of the Unified Buffer when there is a risk of overflow in the Unified Buffer memory.</p></td>
</tr>
<tr id="row121471975507"><td class="cellrowborder" valign="top" width="17.53%" headers="mcps1.2.3.1.1 "><p id="p181474775015"><a name="p181474775015"></a><a name="p181474775015"></a>SHM</p></td>
<td class="cellrowborder" valign="top" width="82.47%" headers="mcps1.2.3.1.2 "><p id="p1314714710505"><a name="p1314714710505"></a><a name="p1314714710505"></a>An alias of SPM.</p></td>
</tr>
<tr id="row12441532142714"><td class="cellrowborder" valign="top" width="17.53%" headers="mcps1.2.3.1.1 "><p id="p1845163262710"><a name="p1845163262710"></a><a name="p1845163262710"></a>TSCM</p></td>
<td class="cellrowborder" valign="top" width="82.47%" headers="mcps1.2.3.1.2 "><p id="p1245133252711"><a name="p1245133252711"></a><a name="p1245133252711"></a>Temp Swap Cache Memory, used to temporarily swap data to additional space for Matmul computation.</p></td>
</tr>
<tr id="row1854865064818"><td class="cellrowborder" valign="top" width="17.53%" headers="mcps1.2.3.1.1 "><p id="p1454925074819"><a name="p1454925074819"></a><a name="p1454925074819"></a>C2PIPE2GM</p></td>
<td class="cellrowborder" valign="top" width="82.47%" headers="mcps1.2.3.1.2 "><p id="p1159617463492"><a name="p1159617463492"></a><a name="p1159617463492"></a>Used to store FIXPIPE quantization parameters.</p></td>
</tr>
<tr id="row16016386275"><td class="cellrowborder" valign="top" width="17.53%" headers="mcps1.2.3.1.1 "><p id="p11073816276"><a name="p11073816276"></a><a name="p11073816276"></a>C2PIPE2LOCAL</p></td>
<td class="cellrowborder" valign="top" width="82.47%" headers="mcps1.2.3.1.2 "><p id="p1601381273"><a name="p1601381273"></a><a name="p1601381273"></a>Reserved parameter. It is reserved for future functionality, and developers do not need to pay attention to it for now.</p></td>
</tr>
</tbody>
</table>
