# OpAttrDef<a name="ZH-CN_TOPIC_0000002078492716"></a>

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T10:38:59.476Z pushedAt=2026-09-11T02:55:07.305Z -->

## Description<a name="zh-cn_topic_0000001656780128_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_section36583473819"></a>

Defines operator attributes.

## Prototype<a name="zh-cn_topic_0000001656780128_section3992421457"></a>

```
class OpAttrDef {
public:
  explicit OpAttrDef(const char *name);
  OpAttrDef(const OpAttrDef &attr_def);
  ~OpAttrDef();
  OpAttrDef &operator=(const OpAttrDef &attr_def);
  OpAttrDef &AttrType(Option attr_type);
  OpAttrDef &Bool(void);
  OpAttrDef &Bool(bool value);
  OpAttrDef &Float(void);
  OpAttrDef &Float(float value);
  OpAttrDef &Int(void);
  OpAttrDef &Int(int64_t value);
  OpAttrDef &String(void);
  OpAttrDef &String(const char *value);
  OpAttrDef &ListBool(void);
  OpAttrDef &ListBool(std::vector<bool> value);
  OpAttrDef &ListFloat(void);
  OpAttrDef &ListFloat(std::vector<float> value);
  OpAttrDef &ListInt(void);
  OpAttrDef &ListInt(std::vector<int64_t> value);
  OpAttrDef &ListListInt(void);
  OpAttrDef &ListListInt(std::vector<std::vector<int64_t>> value);
  OpAttrDef &Version(uint32_t version);
  ge::AscendString &GetName(void) const;
  bool IsRequired(void);
private:
  ...
};
```

## Function Description<a name="zh-cn_topic_0000001656780128_section1340317244469"></a>

**Table 1**  Description of OpAttrDef class member functions

<a name="zh-cn_topic_0000001656780128_zh-cn_topic_0000001441184464_table18149577913"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001656780128_zh-cn_topic_0000001441184464_row61411571196"><th class="cellrowborder" valign="top" width="16.650000000000002%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000001656780128_zh-cn_topic_0000001441184464_p2093713281104"><a name="zh-cn_topic_0000001656780128_zh-cn_topic_0000001441184464_p2093713281104"></a><a name="zh-cn_topic_0000001656780128_zh-cn_topic_0000001441184464_p2093713281104"></a>Function Name</p></th>
<th class="cellrowborder" valign="top" width="20.1%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000001656780128_zh-cn_topic_0000001441184464_p1593811282101"><a name="zh-cn_topic_0000001656780128_zh-cn_topic_0000001441184464_p1593811282101"></a><a name="zh-cn_topic_0000001656780128_zh-cn_topic_0000001441184464_p1593811282101"></a>Input Parameter Description</p></th>
<th class="cellrowborder" valign="top" width="63.24999999999999%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000001656780128_zh-cn_topic_0000001441184464_p393813285106"><a name="zh-cn_topic_0000001656780128_zh-cn_topic_0000001441184464_p393813285106"></a><a name="zh-cn_topic_0000001656780128_zh-cn_topic_0000001441184464_p393813285106"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001656780128_zh-cn_topic_0000001441184464_row8906103284616"><td class="cellrowborder" valign="top" width="16.650000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001656780128_p1985231611167"><a name="zh-cn_topic_0000001656780128_p1985231611167"></a><a name="zh-cn_topic_0000001656780128_p1985231611167"></a>AttrType</p></td>
<td class="cellrowborder" valign="top" width="20.1%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001656780128_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001656780128_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001656780128_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_p318615392613"></a>attr_type: attribute type</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001656780128_zh-cn_topic_0000001526594958_p096733515614"><a name="zh-cn_topic_0000001656780128_zh-cn_topic_0000001526594958_p096733515614"></a><a name="zh-cn_topic_0000001656780128_zh-cn_topic_0000001526594958_p096733515614"></a>Sets the operator attribute type. The value can be OPTIONAL or REQUIRED.</p></td>
</tr>
<tr id="zh-cn_topic_0000001656780128_zh-cn_topic_0000001441184464_row433315311399"><td class="cellrowborder" valign="top" width="16.650000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001656780128_p414793501615"><a name="zh-cn_topic_0000001656780128_p414793501615"></a><a name="zh-cn_topic_0000001656780128_p414793501615"></a>Bool</p></td>
<td class="cellrowborder" valign="top" width="20.1%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001656780128_p05124438166"><a name="zh-cn_topic_0000001656780128_p05124438166"></a><a name="zh-cn_topic_0000001656780128_p05124438166"></a>None</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001656780128_zh-cn_topic_0000001441184464_p8444068494"><a name="zh-cn_topic_0000001656780128_zh-cn_topic_0000001441184464_p8444068494"></a><a name="zh-cn_topic_0000001656780128_zh-cn_topic_0000001441184464_p8444068494"></a>Sets the operator attribute data type to Bool.</p></td>
</tr>
<tr id="zh-cn_topic_0000001656780128_zh-cn_topic_0000001441184464_row833317313398"><td class="cellrowborder" valign="top" width="16.650000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001656780128_p2991838121618"><a name="zh-cn_topic_0000001656780128_p2991838121618"></a><a name="zh-cn_topic_0000001656780128_p2991838121618"></a>Bool</p></td>
<td class="cellrowborder" valign="top" width="20.1%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001656780128_p1135123414252"><a name="zh-cn_topic_0000001656780128_p1135123414252"></a><a name="zh-cn_topic_0000001656780128_p1135123414252"></a>value</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001656780128_p197371148163118"><a name="zh-cn_topic_0000001656780128_p197371148163118"></a><a name="zh-cn_topic_0000001656780128_p197371148163118"></a>Sets the operator attribute data type to Bool and sets the attribute default value to value. When the attribute type is set to OPTIONAL, this type of API must be called to set the default value.</p></td>
</tr>
<tr id="zh-cn_topic_0000001656780128_zh-cn_topic_0000001441184464_row16152579911"><td class="cellrowborder" valign="top" width="16.650000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001656780128_p045625381620"><a name="zh-cn_topic_0000001656780128_p045625381620"></a><a name="zh-cn_topic_0000001656780128_p045625381620"></a>Float</p></td>
<td class="cellrowborder" valign="top" width="20.1%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001656780128_p1254453017309"><a name="zh-cn_topic_0000001656780128_p1254453017309"></a><a name="zh-cn_topic_0000001656780128_p1254453017309"></a>None</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001656780128_p454413012305"><a name="zh-cn_topic_0000001656780128_p454413012305"></a><a name="zh-cn_topic_0000001656780128_p454413012305"></a>Sets the operator attribute data type to Float.</p></td>
</tr>
<tr id="zh-cn_topic_0000001656780128_row1559818378231"><td class="cellrowborder" valign="top" width="16.650000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001656780128_p334545512166"><a name="zh-cn_topic_0000001656780128_p334545512166"></a><a name="zh-cn_topic_0000001656780128_p334545512166"></a>Float</p></td>
<td class="cellrowborder" valign="top" width="20.1%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001656780128_p1934919344253"><a name="zh-cn_topic_0000001656780128_p1934919344253"></a><a name="zh-cn_topic_0000001656780128_p1934919344253"></a>value</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001656780128_p04071414839"><a name="zh-cn_topic_0000001656780128_p04071414839"></a><a name="zh-cn_topic_0000001656780128_p04071414839"></a>Sets the operator attribute data type to Float and sets the attribute default value to value. This type of API must be called to set the default value when the attribute type is set to OPTIONAL.</p></td>
</tr>
<tr id="zh-cn_topic_0000001656780128_row11161541192316"><td class="cellrowborder" valign="top" width="16.650000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001656780128_p124601136173"><a name="zh-cn_topic_0000001656780128_p124601136173"></a><a name="zh-cn_topic_0000001656780128_p124601136173"></a>Int</p></td>
<td class="cellrowborder" valign="top" width="20.1%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001656780128_p17205733133017"><a name="zh-cn_topic_0000001656780128_p17205733133017"></a><a name="zh-cn_topic_0000001656780128_p17205733133017"></a>None</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001656780128_p2020523363016"><a name="zh-cn_topic_0000001656780128_p2020523363016"></a><a name="zh-cn_topic_0000001656780128_p2020523363016"></a>Sets the operator attribute data type to Int.</p></td>
</tr>
<tr id="zh-cn_topic_0000001656780128_row61311051338"><td class="cellrowborder" valign="top" width="16.650000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001656780128_p271512531715"><a name="zh-cn_topic_0000001656780128_p271512531715"></a><a name="zh-cn_topic_0000001656780128_p271512531715"></a>Int</p></td>
<td class="cellrowborder" valign="top" width="20.1%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001656780128_p713214510311"><a name="zh-cn_topic_0000001656780128_p713214510311"></a><a name="zh-cn_topic_0000001656780128_p713214510311"></a>value</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001656780128_p132411030939"><a name="zh-cn_topic_0000001656780128_p132411030939"></a><a name="zh-cn_topic_0000001656780128_p132411030939"></a>Sets the operator attribute data type to Int and sets the attribute default value to value. This type of API must be called to set the default value when the attribute type is set to OPTIONAL.</p></td>
</tr>
<tr id="zh-cn_topic_0000001656780128_row855612286173"><td class="cellrowborder" valign="top" width="16.650000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001656780128_p1255662841719"><a name="zh-cn_topic_0000001656780128_p1255662841719"></a><a name="zh-cn_topic_0000001656780128_p1255662841719"></a>String</p></td>
<td class="cellrowborder" valign="top" width="20.1%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001656780128_p11878123519306"><a name="zh-cn_topic_0000001656780128_p11878123519306"></a><a name="zh-cn_topic_0000001656780128_p11878123519306"></a>None</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001656780128_p14878193593016"><a name="zh-cn_topic_0000001656780128_p14878193593016"></a><a name="zh-cn_topic_0000001656780128_p14878193593016"></a>Sets the operator attribute data type to String.</p></td>
</tr>
<tr id="zh-cn_topic_0000001656780128_row1147831122216"><td class="cellrowborder" valign="top" width="16.650000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001656780128_p2478511122211"><a name="zh-cn_topic_0000001656780128_p2478511122211"></a><a name="zh-cn_topic_0000001656780128_p2478511122211"></a>String</p></td>
<td class="cellrowborder" valign="top" width="20.1%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001656780128_p747881152213"><a name="zh-cn_topic_0000001656780128_p747881152213"></a><a name="zh-cn_topic_0000001656780128_p747881152213"></a>value</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001656780128_p547814111225"><a name="zh-cn_topic_0000001656780128_p547814111225"></a><a name="zh-cn_topic_0000001656780128_p547814111225"></a>Sets the operator attribute data type to String and sets the attribute default value to value. When the attribute type is set to OPTIONAL, this type of API must be called to set the default value.</p></td>
</tr>
<tr id="zh-cn_topic_0000001656780128_row1910382522211"><td class="cellrowborder" valign="top" width="16.650000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001656780128_p41491428112418"><a name="zh-cn_topic_0000001656780128_p41491428112418"></a><a name="zh-cn_topic_0000001656780128_p41491428112418"></a>ListBool</p></td>
<td class="cellrowborder" valign="top" width="20.1%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001656780128_p11480104373019"><a name="zh-cn_topic_0000001656780128_p11480104373019"></a><a name="zh-cn_topic_0000001656780128_p11480104373019"></a>None</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001656780128_p1448064317306"><a name="zh-cn_topic_0000001656780128_p1448064317306"></a><a name="zh-cn_topic_0000001656780128_p1448064317306"></a>Sets the operator attribute data type to ListBool.</p></td>
</tr>
<tr id="zh-cn_topic_0000001656780128_row730692212212"><td class="cellrowborder" valign="top" width="16.650000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001656780128_p348153019244"><a name="zh-cn_topic_0000001656780128_p348153019244"></a><a name="zh-cn_topic_0000001656780128_p348153019244"></a>ListBool</p></td>
<td class="cellrowborder" valign="top" width="20.1%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001656780128_p230720227229"><a name="zh-cn_topic_0000001656780128_p230720227229"></a><a name="zh-cn_topic_0000001656780128_p230720227229"></a>value</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001656780128_p1530792214229"><a name="zh-cn_topic_0000001656780128_p1530792214229"></a><a name="zh-cn_topic_0000001656780128_p1530792214229"></a>Sets the operator attribute data type to ListBool and sets the attribute default value to value. When the attribute type is set to OPTIONAL, this type of API must be called to set the default value.</p></td>
</tr>
<tr id="zh-cn_topic_0000001656780128_row79541415102215"><td class="cellrowborder" valign="top" width="16.650000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001656780128_p183013613248"><a name="zh-cn_topic_0000001656780128_p183013613248"></a><a name="zh-cn_topic_0000001656780128_p183013613248"></a>ListFloat</p></td>
<td class="cellrowborder" valign="top" width="20.1%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001656780128_p12930204511309"><a name="zh-cn_topic_0000001656780128_p12930204511309"></a><a name="zh-cn_topic_0000001656780128_p12930204511309"></a>None</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001656780128_p193154519302"><a name="zh-cn_topic_0000001656780128_p193154519302"></a><a name="zh-cn_topic_0000001656780128_p193154519302"></a>Sets the operator attribute data type to ListFloat.</p></td>
</tr>
<tr id="zh-cn_topic_0000001656780128_row228196227"><td class="cellrowborder" valign="top" width="16.650000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001656780128_p12364113852420"><a name="zh-cn_topic_0000001656780128_p12364113852420"></a><a name="zh-cn_topic_0000001656780128_p12364113852420"></a>ListFloat</p></td>
<td class="cellrowborder" valign="top" width="20.1%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001656780128_p82219112219"><a name="zh-cn_topic_0000001656780128_p82219112219"></a><a name="zh-cn_topic_0000001656780128_p82219112219"></a>value</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001656780128_p1455218391332"><a name="zh-cn_topic_0000001656780128_p1455218391332"></a><a name="zh-cn_topic_0000001656780128_p1455218391332"></a>Sets the operator attribute data type to ListFloat and sets the attribute default value to value. When the attribute type is set to OPTIONAL, this API must be called to set the default value.</p></td>
</tr>
<tr id="zh-cn_topic_0000001656780128_row8424456244"><td class="cellrowborder" valign="top" width="16.650000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001656780128_p136785561242"><a name="zh-cn_topic_0000001656780128_p136785561242"></a><a name="zh-cn_topic_0000001656780128_p136785561242"></a>ListInt</p></td>
<td class="cellrowborder" valign="top" width="20.1%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001656780128_p865744873011"><a name="zh-cn_topic_0000001656780128_p865744873011"></a><a name="zh-cn_topic_0000001656780128_p865744873011"></a>None</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001656780128_p16657114812304"><a name="zh-cn_topic_0000001656780128_p16657114812304"></a><a name="zh-cn_topic_0000001656780128_p16657114812304"></a>Sets the operator attribute data type to ListInt.</p></td>
</tr>
<tr id="zh-cn_topic_0000001656780128_row17577122518"><td class="cellrowborder" valign="top" width="16.650000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001656780128_p1875713142514"><a name="zh-cn_topic_0000001656780128_p1875713142514"></a><a name="zh-cn_topic_0000001656780128_p1875713142514"></a>ListInt</p></td>
<td class="cellrowborder" valign="top" width="20.1%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001656780128_p1175714182510"><a name="zh-cn_topic_0000001656780128_p1175714182510"></a><a name="zh-cn_topic_0000001656780128_p1175714182510"></a>value</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001656780128_p136389175357"><a name="zh-cn_topic_0000001656780128_p136389175357"></a><a name="zh-cn_topic_0000001656780128_p136389175357"></a>Sets the operator attribute data type to ListInt and sets the attribute default value to value. When the attribute type is set to OPTIONAL, this type of API must be called to set the default value.</p></td>
</tr>
<tr id="zh-cn_topic_0000001656780128_row10511155142519"><td class="cellrowborder" valign="top" width="16.650000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001656780128_p381219185254"><a name="zh-cn_topic_0000001656780128_p381219185254"></a><a name="zh-cn_topic_0000001656780128_p381219185254"></a>ListListInt</p></td>
<td class="cellrowborder" valign="top" width="20.1%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001656780128_p1366115143012"><a name="zh-cn_topic_0000001656780128_p1366115143012"></a><a name="zh-cn_topic_0000001656780128_p1366115143012"></a>None</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001656780128_p436685116306"><a name="zh-cn_topic_0000001656780128_p436685116306"></a><a name="zh-cn_topic_0000001656780128_p436685116306"></a>Sets the operator attribute data type to ListListInt.</p></td>
</tr>
<tr id="zh-cn_topic_0000001656780128_row756154892410"><td class="cellrowborder" valign="top" width="16.650000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001656780128_p193941710257"><a name="zh-cn_topic_0000001656780128_p193941710257"></a><a name="zh-cn_topic_0000001656780128_p193941710257"></a>ListListInt</p></td>
<td class="cellrowborder" valign="top" width="20.1%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001656780128_p16561104882413"><a name="zh-cn_topic_0000001656780128_p16561104882413"></a><a name="zh-cn_topic_0000001656780128_p16561104882413"></a>value</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001656780128_p1471518283520"><a name="zh-cn_topic_0000001656780128_p1471518283520"></a><a name="zh-cn_topic_0000001656780128_p1471518283520"></a>Sets the operator attribute data type to ListListInt and sets the attribute default value to value. When the attribute type is set to OPTIONAL, this type of API must be called to set the default value.</p></td>
</tr>
<tr id="zh-cn_topic_0000001656780128_row1652113814816"><td class="cellrowborder" valign="top" width="16.650000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001656780128_p152158154812"><a name="zh-cn_topic_0000001656780128_p152158154812"></a><a name="zh-cn_topic_0000001656780128_p152158154812"></a>Version</p></td>
<td class="cellrowborder" valign="top" width="20.1%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001656780128_p752117864811"><a name="zh-cn_topic_0000001656780128_p752117864811"></a><a name="zh-cn_topic_0000001656780128_p752117864811"></a>version: Configured version number.</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001656780128_p252168154812"><a name="zh-cn_topic_0000001656780128_p252168154812"></a><a name="zh-cn_topic_0000001656780128_p252168154812"></a>When adding an optional attribute, to maintain compatibility with the existing single-operator API (aclnnxxx), you can configure the version number of the aclnn API through the Version API. The version number must start from 1 and be configured consecutively (numbered uniformly with <a href="../OpParamDef/Version.md">optional inputs</a>). After configuration, the automatically generated aclnn API carries the version number. An API with a higher version number includes all parameters of APIs with lower version numbers.</p></td>
</tr>
<tr id="zh-cn_topic_0000001656780128_row430719366115"><td class="cellrowborder" valign="top" width="16.650000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001656780128_p5182152013136"><a name="zh-cn_topic_0000001656780128_p5182152013136"></a><a name="zh-cn_topic_0000001656780128_p5182152013136"></a>GetName</p></td>
<td class="cellrowborder" valign="top" width="20.1%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001656780128_p8667522131317"><a name="zh-cn_topic_0000001656780128_p8667522131317"></a><a name="zh-cn_topic_0000001656780128_p8667522131317"></a>None</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001656780128_p15307113621112"><a name="zh-cn_topic_0000001656780128_p15307113621112"></a><a name="zh-cn_topic_0000001656780128_p15307113621112"></a>Obtains the attribute name.</p></td>
</tr>
<tr id="zh-cn_topic_0000001656780128_row125751739111113"><td class="cellrowborder" valign="top" width="16.650000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001656780128_p2182020131317"><a name="zh-cn_topic_0000001656780128_p2182020131317"></a><a name="zh-cn_topic_0000001656780128_p2182020131317"></a>IsRequired</p></td>
<td class="cellrowborder" valign="top" width="20.1%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001656780128_p166762212139"><a name="zh-cn_topic_0000001656780128_p166762212139"></a><a name="zh-cn_topic_0000001656780128_p166762212139"></a>None</p></td>
<td class="cellrowborder" valign="top" width="63.24999999999999%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001656780128_p757503901110"><a name="zh-cn_topic_0000001656780128_p757503901110"></a><a name="zh-cn_topic_0000001656780128_p757503901110"></a>Checks whether the operator attribute is required. Returns true if required, and false if optional.</p></td>
</tr>
</tbody>
</table>

