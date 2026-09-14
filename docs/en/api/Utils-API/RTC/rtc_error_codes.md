# RTC Error Codes<a name="ZH-CN_TOPIC_0000002487335325"></a>

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T15:34:59.200Z pushedAt=2026-09-11T07:39:47.288Z -->

**Table 1** Description of aclError error codes returned by the aclrtc API

<a name="table1098718523239"></a>
<table><thead align="left"><tr id="row298895218235"><th class="cellrowborder" valign="top" width="43.65%" id="mcps1.2.4.1.1"><p id="p14988852132312"><a name="p14988852132312"></a><a name="p14988852132312"></a>Error Code Name</p></th>
<th class="cellrowborder" valign="top" width="11.600000000000001%" id="mcps1.2.4.1.2"><p id="p883419364128"><a name="p883419364128"></a><a name="p883419364128"></a>Error Code Value</p></th>
<th class="cellrowborder" valign="top" width="44.75%" id="mcps1.2.4.1.3"><p id="p3988155212316"><a name="p3988155212316"></a><a name="p3988155212316"></a>Meaning</p></th>
</tr>
</thead>
<tbody><tr id="row598810528235"><td class="cellrowborder" valign="top" width="43.65%" headers="mcps1.2.4.1.1 "><p id="p15624127152713"><a name="p15624127152713"></a><a name="p15624127152713"></a>ACL_SUCCESS</p></td>
<td class="cellrowborder" valign="top" width="11.600000000000001%" headers="mcps1.2.4.1.2 "><p id="p13834123610129"><a name="p13834123610129"></a><a name="p13834123610129"></a>0</p></td>
<td class="cellrowborder" valign="top" width="44.75%" headers="mcps1.2.4.1.3 "><p id="p89881652142315"><a name="p89881652142315"></a><a name="p89881652142315"></a>Execution succeeded.</p></td>
</tr>
<tr id="row109881852132319"><td class="cellrowborder" valign="top" width="43.65%" headers="mcps1.2.4.1.1 "><p id="p18988155262319"><a name="p18988155262319"></a><a name="p18988155262319"></a>ACL_ERROR_RTC_INVALID_PROG</p></td>
<td class="cellrowborder" valign="top" width="11.600000000000001%" headers="mcps1.2.4.1.2 "><p id="p683433615127"><a name="p683433615127"></a><a name="p683433615127"></a>176000</p></td>
<td class="cellrowborder" valign="top" width="44.75%" headers="mcps1.2.4.1.3 "><p id="p119881952132315"><a name="p119881952132315"></a><a name="p119881952132315"></a>Invalid aclrtcProg (handle).</p></td>
</tr>
<tr id="row2247122304"><td class="cellrowborder" valign="top" width="43.65%" headers="mcps1.2.4.1.1 "><p id="p19951198146"><a name="p19951198146"></a><a name="p19951198146"></a>ACL_ERROR_RTC_INVALID_INPUT</p></td>
<td class="cellrowborder" valign="top" width="11.600000000000001%" headers="mcps1.2.4.1.2 "><p id="p1683415363128"><a name="p1683415363128"></a><a name="p1683415363128"></a>176001</p></td>
<td class="cellrowborder" valign="top" width="44.75%" headers="mcps1.2.4.1.3 "><p id="p0123155517210"><a name="p0123155517210"></a><a name="p0123155517210"></a>Input parameter error other than the prog input parameter.</p></td>
</tr>
<tr id="row19359422193413"><td class="cellrowborder" valign="top" width="43.65%" headers="mcps1.2.4.1.1 "><p id="p840617304149"><a name="p840617304149"></a><a name="p840617304149"></a>ACL_ERROR_RTC_INVALID_OPTION</p></td>
<td class="cellrowborder" valign="top" width="11.600000000000001%" headers="mcps1.2.4.1.2 "><p id="p188343363128"><a name="p188343363128"></a><a name="p188343363128"></a>176002</p></td>
<td class="cellrowborder" valign="top" width="44.75%" headers="mcps1.2.4.1.3 "><p id="p11247132113013"><a name="p11247132113013"></a><a name="p11247132113013"></a>Compilation option error.</p></td>
</tr>
<tr id="row12359192223413"><td class="cellrowborder" valign="top" width="43.65%" headers="mcps1.2.4.1.1 "><p id="p8157183273020"><a name="p8157183273020"></a><a name="p8157183273020"></a>ACL_ERROR_RTC_COMPILATION</p></td>
<td class="cellrowborder" valign="top" width="11.600000000000001%" headers="mcps1.2.4.1.2 "><p id="p78341936101218"><a name="p78341936101218"></a><a name="p78341936101218"></a>176003</p></td>
<td class="cellrowborder" valign="top" width="44.75%" headers="mcps1.2.4.1.3 "><p id="p7359162210340"><a name="p7359162210340"></a><a name="p7359162210340"></a>Compilation error.</p></td>
</tr>
<tr id="row14359122218343"><td class="cellrowborder" valign="top" width="43.65%" headers="mcps1.2.4.1.1 "><p id="p5148741181413"><a name="p5148741181413"></a><a name="p5148741181413"></a>ACL_ERROR_RTC_LINKING</p></td>
<td class="cellrowborder" valign="top" width="11.600000000000001%" headers="mcps1.2.4.1.2 "><p id="p16834173612127"><a name="p16834173612127"></a><a name="p16834173612127"></a>176004</p></td>
<td class="cellrowborder" valign="top" width="44.75%" headers="mcps1.2.4.1.3 "><p id="p142535312218"><a name="p142535312218"></a><a name="p142535312218"></a>Linking error.</p></td>
</tr>
<tr id="row524702143010"><td class="cellrowborder" valign="top" width="43.65%" headers="mcps1.2.4.1.1 "><p id="p1748814473146"><a name="p1748814473146"></a><a name="p1748814473146"></a>ACL_ERROR_RTC_NO_NAME_EXPR_AFTER_COMPILATION</p></td>
<td class="cellrowborder" valign="top" width="11.600000000000001%" headers="mcps1.2.4.1.2 "><p id="p58344366129"><a name="p58344366129"></a><a name="p58344366129"></a>176005</p></td>
<td class="cellrowborder" valign="top" width="44.75%" headers="mcps1.2.4.1.3 "><p id="p1701202820349"><a name="p1701202820349"></a><a name="p1701202820349"></a>No function name after compilation.</p></td>
</tr>
<tr id="row1160551273012"><td class="cellrowborder" valign="top" width="43.65%" headers="mcps1.2.4.1.1 "><p id="p139391732344"><a name="p139391732344"></a><a name="p139391732344"></a>ACL_ERROR_RTC_NO_LOWERED_NAMES_BEFORE_COMPILATION</p></td>
<td class="cellrowborder" valign="top" width="11.600000000000001%" headers="mcps1.2.4.1.2 "><p id="p783493610121"><a name="p783493610121"></a><a name="p783493610121"></a>176006</p></td>
<td class="cellrowborder" valign="top" width="44.75%" headers="mcps1.2.4.1.3 "><p id="p1390413153316"><a name="p1390413153316"></a><a name="p1390413153316"></a>The kernel function name cannot be converted to a mangled name after compilation.</p></td>
</tr>
<tr id="row1460513124307"><td class="cellrowborder" valign="top" width="43.65%" headers="mcps1.2.4.1.1 "><p id="p17605512153019"><a name="p17605512153019"></a><a name="p17605512153019"></a>ACL_ERROR_RTC_NAME_EXPR_NOT_VALID</p></td>
<td class="cellrowborder" valign="top" width="11.600000000000001%" headers="mcps1.2.4.1.2 "><p id="p1783411368128"><a name="p1783411368128"></a><a name="p1783411368128"></a>176007</p></td>
<td class="cellrowborder" valign="top" width="44.75%" headers="mcps1.2.4.1.3 "><p id="p166051712143013"><a name="p166051712143013"></a><a name="p166051712143013"></a>An invalid kernel function name is passed in.</p></td>
</tr>
<tr id="row260514124302"><td class="cellrowborder" valign="top" width="43.65%" headers="mcps1.2.4.1.1 "><p id="p156791225203411"><a name="p156791225203411"></a><a name="p156791225203411"></a>ACL_ERROR_RTC_NAME_EXPR_NOT_VALID</p></td>
<td class="cellrowborder" valign="top" width="11.600000000000001%" headers="mcps1.2.4.1.2 "><p id="p08341936131220"><a name="p08341936131220"></a><a name="p08341936131220"></a>276000</p></td>
<td class="cellrowborder" valign="top" width="44.75%" headers="mcps1.2.4.1.3 "><p id="p6605141210301"><a name="p6605141210301"></a><a name="p6605141210301"></a>Failed to create aclrtcProg (handle).</p></td>
</tr>
<tr id="row76051712123013"><td class="cellrowborder" valign="top" width="43.65%" headers="mcps1.2.4.1.1 "><p id="p1467912252343"><a name="p1467912252343"></a><a name="p1467912252343"></a>ACL_ERROR_RTC_OUT_OF_MEMORY</p></td>
<td class="cellrowborder" valign="top" width="11.600000000000001%" headers="mcps1.2.4.1.2 "><p id="p178357365128"><a name="p178357365128"></a><a name="p178357365128"></a>276001</p></td>
<td class="cellrowborder" valign="top" width="44.75%" headers="mcps1.2.4.1.3 "><p id="p760511210304"><a name="p760511210304"></a><a name="p760511210304"></a>Insufficient memory.</p></td>
</tr>
<tr id="row16605171223011"><td class="cellrowborder" valign="top" width="43.65%" headers="mcps1.2.4.1.1 "><p id="p5202156153215"><a name="p5202156153215"></a><a name="p5202156153215"></a>ACL_ERROR_RTC_FAILURE</p></td>
<td class="cellrowborder" valign="top" width="11.600000000000001%" headers="mcps1.2.4.1.2 "><p id="p183517368121"><a name="p183517368121"></a><a name="p183517368121"></a>576000</p></td>
<td class="cellrowborder" valign="top" width="44.75%" headers="mcps1.2.4.1.3 "><p id="p19605191273010"><a name="p19605191273010"></a><a name="p19605191273010"></a>RTC internal error.</p></td>
</tr>
</tbody>
</table>

