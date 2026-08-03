# 避免bank冲突（NPU架构版本3510）<a name="ZH-CN_TOPIC_0000002531242067"></a>

【优先级】中

>[!NOTE]说明 
>该性能优化建议适用于如下产品型号：
><!-- npu="950" id1 -->
>- Ascend 950PR/Ascend 950DT
><!-- end id1 -->

Unified Buffer缓存总大小为256KB，包含8个bank group，每个bank group包含2个bank。每个bank大小为16KB，由512行组成，每行长度为32B，采用低位地址交织。

UB的地址编码格式：
```
UB_ADDR[18:0] = {BANK_DEPTH[8:0], BANK[0], BG[2:0], BANK_WIDTH[4:0]}
```

**图1**  bank结构示意图（图中箭头方向表示内存排布的顺序）<a name="fig873359165316"></a>  

![](../../../../figures/950_UB内存结构图.png "950_UB内存结构图")

3510架构使用低位编址，地址排布情况如图所示：

**图2** 低位地址交织排布

![](../../../../figures/低位交织地址排布.png "低位交织地址排布")

每个bank可分为4个subbank，每个subbank宽度为8B，subbank仅在gather/scatter场景被感知，连续读写只感知到bank。

**图3** subbank结构

![](../../../../figures/subbank结构.png "subbank结构")

## bank冲突的典型场景<a name="section9689957379"></a>

每个bank一拍只能完成一读或者一写，每个bank group最多只允许2读0写或者1读1写。根据内存结构，bank冲突主要可以分为以下三种类型：

-   **读写冲突**：读操作和写操作同时尝试访问同一个bank，或者两个读操作和一个写操作同时尝试访问同一个bank group。
-   **写写冲突**：多个写操作同时尝试访问同一个bank group。
-   **读读冲突**：两个读操作同时尝试访问同一个bank，或者两个以上读操作同时尝试访问同一个bank group。

**注：bank冲突的场景与Unified Buffer的规格密切相关，规格的变化通常会导致bank冲突场景的变化。**

### 基于连续访存的bank冲突分析
以下对连续访存的冲突类型进行解读：

-   读写冲突

    同时读写到同一个bank时造成读写冲突，具体分析如下：

    **表1**  一读一写冲突示例

    <a name="table1789735214010"></a>
    <table><thead align="left"><tr id="row20897752164015"><th class="cellrowborder" valign="top" width="5.9988002399520095%" id="mcps1.2.7.1.1"><p id="p13897195244019"><a name="p13897195244019"></a><a name="p13897195244019"></a>序号</p>
    </th>
    <th class="cellrowborder" valign="top" width="9.948010397920417%" id="mcps1.2.7.1.2"><p id="p3897155224010"><a name="p3897155224010"></a><a name="p3897155224010"></a>src地址</p>
    </th>
    <th class="cellrowborder" valign="top" width="10.507898420315936%" id="mcps1.2.7.1.3"><p id="p13897252164011"><a name="p13897252164011"></a><a name="p13897252164011"></a>dst地址</p>
    </th>
    <th class="cellrowborder" valign="top" width="25.51489702059588%" id="mcps1.2.7.1.4"><p id="p138976528405"><a name="p138976528405"></a><a name="p138976528405"></a>bank</p>
    </th>
    <th class="cellrowborder" valign="top" width="31.36372725454909%" id="mcps1.2.7.1.5"><p id="p689795214408"><a name="p689795214408"></a><a name="p689795214408"></a>bank group</p>
    </th>
    <th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.2.7.1.6"><p id="p98976528403"><a name="p98976528403"></a><a name="p98976528403"></a>结论</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row10897145224010"><td class="cellrowborder" valign="top" width="5.9988002399520095%" headers="mcps1.2.7.1.1 "><p id="p8897185254012"><a name="p8897185254012"></a><a name="p8897185254012"></a>示例1</p>
    </td>
    <td class="cellrowborder" valign="top" width="9.948010397920417%" headers="mcps1.2.7.1.2 "><p id="p12897155215406"><a name="p12897155215406"></a><a name="p12897155215406"></a>0x0000</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.507898420315936%" headers="mcps1.2.7.1.3 "><p id="p19897165213403"><a name="p19897165213403"></a><a name="p19897165213403"></a>0x4000</p>
    </td>
    <td class="cellrowborder" valign="top" width="25.51489702059588%" headers="mcps1.2.7.1.4 "><p id="p1489705220408"><a name="p1489705220408"></a><a name="p1489705220408"></a><span>bank_id0 == </span><span>bank_id</span><span>1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="31.36372725454909%" headers="mcps1.2.7.1.5 "><p id="p4897155264016"><a name="p4897155264016"></a><a name="p4897155264016"></a><span>bank_group_id0 == bank_group_id1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.6 "><p id="p1589719525406"><a name="p1589719525406"></a><a name="p1589719525406"></a>src地址和dst地址属于同一bank，存在读写冲突。</p>
    </td>
    </tr>
    <tr id="row1489775264016"><td class="cellrowborder" valign="top" width="5.9988002399520095%" headers="mcps1.2.7.1.1 "><p id="p12897152184019"><a name="p12897152184019"></a><a name="p12897152184019"></a>示例2</p>
    </td>
    <td class="cellrowborder" valign="top" width="9.948010397920417%" headers="mcps1.2.7.1.2 "><p id="p108981152174015"><a name="p108981152174015"></a><a name="p108981152174015"></a>0x0000</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.507898420315936%" headers="mcps1.2.7.1.3 "><p id="p1389811520408"><a name="p1389811520408"></a><a name="p1389811520408"></a>0x4100</p>
    </td>
    <td class="cellrowborder" valign="top" width="25.51489702059588%" headers="mcps1.2.7.1.4 "><p id="p389885274014"><a name="p389885274014"></a><a name="p389885274014"></a><span>bank_id0 </span><span>!= </span><span>bank_id1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="31.36372725454909%" headers="mcps1.2.7.1.5 "><p id="p7898155214011"><a name="p7898155214011"></a><a name="p7898155214011"></a><span>bank_group_id0 =</span><span>= </span><span>bank_group_id1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.6 "><p id="p989815214018"><a name="p989815214018"></a><a name="p989815214018"></a>src地址和dst地址属于同一bank group下的不同bank，无冲突。</p>
    </td>
    </tr>
    </tbody>
    </table>

    **表2** 两读一写冲突示例

    <table><thead align="left"><tr id="row1767434244710"><th class="cellrowborder" valign="top" width="5.55%" id="mcps1.2.8.1.1"><p id="p17674144211470"><a name="p17674144211470"></a><a name="p17674144211470"></a>序号</p>
    </th>
    <th class="cellrowborder" valign="top" width="7.53%" id="mcps1.2.8.1.2"><p id="p9674114213473"><a name="p9674114213473"></a><a name="p9674114213473"></a>src0地址</p>
    </th>
    <th class="cellrowborder" valign="top" width="7.5200000000000005%" id="mcps1.2.8.1.3"><p id="p1888613260115"><a name="p1888613260115"></a><a name="p1888613260115"></a>src1地址</p>
    </th>
    <th class="cellrowborder" valign="top" width="7.4399999999999995%" id="mcps1.2.8.1.4"><p id="p8674114294712"><a name="p8674114294712"></a><a name="p8674114294712"></a>dst地址</p>
    </th>
    <th class="cellrowborder" valign="top" width="15.409999999999998%" id="mcps1.2.8.1.5"><p id="p17674114244718"><a name="p17674114244718"></a><a name="p17674114244718"></a>bank</p>
    </th>
    <th class="cellrowborder" valign="top" width="23.09%" id="mcps1.2.8.1.6"><p id="p1567444234713"><a name="p1567444234713"></a><a name="p1567444234713"></a>bank group</p>
    </th>
    <th class="cellrowborder" valign="top" width="33.46%" id="mcps1.2.8.1.7"><p id="p1767410425479"><a name="p1767410425479"></a><a name="p1767410425479"></a>结论</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row126741742144713"><td class="cellrowborder" valign="top" width="5.55%" headers="mcps1.2.8.1.1 "><p id="p1367415427473"><a name="p1367415427473"></a><a name="p1367415427473"></a>示例1</p>
    </td>
    <td class="cellrowborder" valign="top" width="7.53%" headers="mcps1.2.8.1.2 "><p id="p7674134216478"><a name="p7674134216478"></a><a name="p7674134216478"></a>0x0000</p>
    </td>
    <td class="cellrowborder" valign="top" width="7.5200000000000005%" headers="mcps1.2.8.1.3 "><p id="p2036641181111"><a name="p2036641181111"></a><a name="p2036641181111"></a>0x4000</p>
    </td>
    <td class="cellrowborder" valign="top" width="7.4399999999999995%" headers="mcps1.2.8.1.4 "><p id="p1567416428472"><a name="p1567416428472"></a><a name="p1567416428472"></a>0x8000</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.409999999999998%" headers="mcps1.2.8.1.5 "><p id="p16674184212479"><a name="p16674184212479"></a><a name="p16674184212479"></a><span>bank_id0 == </span><span>bank_id</span><span>1</span></p>
    <p id="p85745331218"><a name="p85745331218"></a><a name="p85745331218"></a>bank_id0 == bank_id2</p>
    </td>
    <td class="cellrowborder" valign="top" width="23.09%" headers="mcps1.2.8.1.6 "><p id="p7674042124711"><a name="p7674042124711"></a><a name="p7674042124711"></a><span>bank_group_id0 == bank_group_id1</span></p>
    <p id="p1288482941216"><a name="p1288482941216"></a><a name="p1288482941216"></a>bank_group_id0 == bank_group_id2</p>
    </td>
    <td class="cellrowborder" valign="top" width="33.46%" headers="mcps1.2.8.1.7 "><p id="p167515425479"><a name="p167515425479"></a><a name="p167515425479"></a>src0地址与src1地址属于同一bank，存在读读冲突。</p>
    <p id="p1227485214177"><a name="p1227485214177"></a><a name="p1227485214177"></a>src0地址与dst地址属于同一bank，存在读写冲突。</p>
    <p id="p15494155717182"><a name="p15494155717182"></a><a name="p15494155717182"></a>同一bank group最多支持两读或者一读一写，当前存在冲突。</p>
    </td>
    </tr>
    <tr id="row3675154284710"><td class="cellrowborder" valign="top" width="5.55%" headers="mcps1.2.8.1.1 "><p id="p967514254716"><a name="p967514254716"></a><a name="p967514254716"></a>示例2</p>
    </td>
    <td class="cellrowborder" valign="top" width="7.53%" headers="mcps1.2.8.1.2 "><p id="p11927551151718"><a name="p11927551151718"></a><a name="p11927551151718"></a>0x0000</p>
    </td>
    <td class="cellrowborder" valign="top" width="7.5200000000000005%" headers="mcps1.2.8.1.3 "><p id="p1088632681119"><a name="p1088632681119"></a><a name="p1088632681119"></a>0x0000</p>
    </td>
    <td class="cellrowborder" valign="top" width="7.4399999999999995%" headers="mcps1.2.8.1.4 "><p id="p692705115176"><a name="p692705115176"></a><a name="p692705115176"></a>0x4100</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.409999999999998%" headers="mcps1.2.8.1.5 "><p id="p1367534210471"><a name="p1367534210471"></a><a name="p1367534210471"></a><span>bank_id0 </span><span>!= </span><span>bank_id2</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="23.09%" headers="mcps1.2.8.1.6 "><p id="p86756421472"><a name="p86756421472"></a><a name="p86756421472"></a><span>bank_group_id0 =</span><span>= </span><span>bank_group_id2</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="33.46%" headers="mcps1.2.8.1.7 "><p id="p1667544264720"><a name="p1667544264720"></a><a name="p1667544264720"></a>src0和src1起始地址相同，两读被硬件优化为一读。</p>
    <p id="p27720182416"><a name="p27720182416"></a><a name="p27720182416"></a>src0地址与dst地址不属于同一bank，无读写冲突。</p>
    </td>
    </tr>
    <tr id="row15280195115245"><td class="cellrowborder" valign="top" width="5.55%" headers="mcps1.2.8.1.1 "><p id="p10280175113242"><a name="p10280175113242"></a><a name="p10280175113242"></a>示例3</p>
    </td>
    <td class="cellrowborder" valign="top" width="7.53%" headers="mcps1.2.8.1.2 "><p id="p122801451122413"><a name="p122801451122413"></a><a name="p122801451122413"></a>0x0000</p>
    </td>
    <td class="cellrowborder" valign="top" width="7.5200000000000005%" headers="mcps1.2.8.1.3 "><p id="p18861626181115"><a name="p18861626181115"></a><a name="p18861626181115"></a>0x4100</p>
    </td>
    <td class="cellrowborder" valign="top" width="7.4399999999999995%" headers="mcps1.2.8.1.4 "><p id="p12280195113249"><a name="p12280195113249"></a><a name="p12280195113249"></a>0x8900</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.409999999999998%" headers="mcps1.2.8.1.5 "><p id="p1028035152414"><a name="p1028035152414"></a><a name="p1028035152414"></a>bank_id0 != bank_id1</p>
    <p id="p14474173217257"><a name="p14474173217257"></a><a name="p14474173217257"></a>bank_id0 != bank_id2</p>
    <p id="p10927153182716"><a name="p10927153182716"></a><a name="p10927153182716"></a>bank_id1 == bank_id2</p>
    </td>
    <td class="cellrowborder" valign="top" width="23.09%" headers="mcps1.2.8.1.6 "><p id="p9280051162413"><a name="p9280051162413"></a><a name="p9280051162413"></a><span>bank_group_id0 =</span><span>= </span><span>bank_group_id1</span></p>
    <p id="p299119242283"><a name="p299119242283"></a><a name="p299119242283"></a>bank_group_id0 == bank_group_id2</p>
    </td>
    <td class="cellrowborder" valign="top" width="33.46%" headers="mcps1.2.8.1.7 "><p id="p828065182410"><a name="p828065182410"></a><a name="p828065182410"></a>src0地址和src1地址属于同一bank group的不同bank，不产生读读冲突。</p>
    <p id="p7843650172915"><a name="p7843650172915"></a><a name="p7843650172915"></a>src1地址和dst地址属于同一bank，无冲突。</p>
    </td>
    </tr>
    </tbody>
    </table>

-   写写冲突

    同时写入一个bank group造成写写冲突，具体分析如下：

    **表3**  写写冲突示例

    <table><thead align="left"><tr id="row17580229172710"><th class="cellrowborder" valign="top" width="5.9988002399520095%" id="mcps1.2.7.1.1"><p id="p115801529192712"><a name="p115801529192712"></a><a name="p115801529192712"></a>序号</p>
    </th>
    <th class="cellrowborder" valign="top" width="8.138372325534892%" id="mcps1.2.7.1.2"><p id="p1258022919273"><a name="p1258022919273"></a><a name="p1258022919273"></a>dst0地址</p>
    </th>
    <th class="cellrowborder" valign="top" width="8.048390321935612%" id="mcps1.2.7.1.3"><p id="p18580172932720"><a name="p18580172932720"></a><a name="p18580172932720"></a>dst1地址</p>
    </th>
    <th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.2.7.1.4"><p id="p8580729102716"><a name="p8580729102716"></a><a name="p8580729102716"></a>bank</p>
    </th>
    <th class="cellrowborder" valign="top" width="24.96500699860028%" id="mcps1.2.7.1.5"><p id="p18580152942710"><a name="p18580152942710"></a><a name="p18580152942710"></a>bank group</p>
    </th>
    <th class="cellrowborder" valign="top" width="36.18276344731054%" id="mcps1.2.7.1.6"><p id="p11580192916275"><a name="p11580192916275"></a><a name="p11580192916275"></a>结论</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row1058082919273"><td class="cellrowborder" valign="top" width="5.9988002399520095%" headers="mcps1.2.7.1.1 "><p id="p8580429102713"><a name="p8580429102713"></a><a name="p8580429102713"></a>示例1</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.138372325534892%" headers="mcps1.2.7.1.2 "><p id="p1158092992713"><a name="p1158092992713"></a><a name="p1158092992713"></a>0x0000</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.048390321935612%" headers="mcps1.2.7.1.3 "><p id="p3580132919279"><a name="p3580132919279"></a><a name="p3580132919279"></a>0x4100</p>
    </td>
    <td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.4 "><p id="p1858018294272"><a name="p1858018294272"></a><a name="p1858018294272"></a><span>bank_id0 </span><span>!= </span><span>bank_id1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="24.96500699860028%" headers="mcps1.2.7.1.5 "><p id="p165801829132711"><a name="p165801829132711"></a><a name="p165801829132711"></a><span>bank_group_id0 =</span><span>= </span><span>bank_group_id1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="36.18276344731054%" headers="mcps1.2.7.1.6 "><p id="p11580729142715"><a name="p11580729142715"></a><a name="p11580729142715"></a>dst0地址和dst1地址属于同一bank group，故存在冲突。</p>
    </td>
    </tr>
    </tbody>
    </table>

-   读读冲突
    
    **表4** 读读冲突示例
    <table><thead align="left"><tr id="row134631185514"><th class="cellrowborder" valign="top" width="5.9988002399520095%" id="mcps1.2.7.1.1"><p id="p16461312553"><a name="p16461312553"></a><a name="p16461312553"></a>序号</p>
    </th>
    <th class="cellrowborder" valign="top" width="8.138372325534892%" id="mcps1.2.7.1.2"><p id="p4466118554"><a name="p4466118554"></a><a name="p4466118554"></a>src0地址</p>
    </th>
    <th class="cellrowborder" valign="top" width="8.048390321935612%" id="mcps1.2.7.1.3"><p id="p14691125519"><a name="p14691125519"></a><a name="p14691125519"></a>src1地址</p>
    </th>
    <th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.2.7.1.4"><p id="p246181165514"><a name="p246181165514"></a><a name="p246181165514"></a>bank</p>
    </th>
    <th class="cellrowborder" valign="top" width="24.96500699860028%" id="mcps1.2.7.1.5"><p id="p14463165511"><a name="p14463165511"></a><a name="p14463165511"></a>bank group</p>
    </th>
    <th class="cellrowborder" valign="top" width="36.18276344731054%" id="mcps1.2.7.1.6"><p id="p94601175513"><a name="p94601175513"></a><a name="p94601175513"></a>结论</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row13467155518"><td class="cellrowborder" valign="top" width="5.9988002399520095%" headers="mcps1.2.7.1.1 "><p id="p34612175519"><a name="p34612175519"></a><a name="p34612175519"></a>示例1</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.138372325534892%" headers="mcps1.2.7.1.2 "><p id="p154619117550"><a name="p154619117550"></a><a name="p154619117550"></a>0x0000</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.048390321935612%" headers="mcps1.2.7.1.3 "><p id="p144691175519"><a name="p144691175519"></a><a name="p144691175519"></a>0x4000</p>
    </td>
    <td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.4 "><p id="p124620185513"><a name="p124620185513"></a><a name="p124620185513"></a><span>bank_id0 </span><span>== </span><span>bank_id1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="24.96500699860028%" headers="mcps1.2.7.1.5 "><p id="p15474195517"><a name="p15474195517"></a><a name="p15474195517"></a><span>bank_group_id0 =</span><span>= </span><span>bank_group_id1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="36.18276344731054%" headers="mcps1.2.7.1.6 "><p id="p1471819555"><a name="p1471819555"></a><a name="p1471819555"></a>src0地址和src1地址属于同一bank，故存在冲突。</p>
    </td>
    </tr>
    </tbody>
    </table>

### 基于离散访存的bank冲突分析
以下对离散访存下的冲突进行解读：

-   当gather/scatter访存数据分布在不同的subbank中时，不发生冲突。
-   当gather/scatter数据存在同一subbank的同一起始地址时，不发生冲突，因此相同索引值的访存不发生冲突。
-   当gather/scatter数据存在同一subbank的不同起始地址时，发生冲突。
-   gather/scatter会与连续访存指令/MTE搬运指令冲突。

## 如何避免bank冲突<a name="section12501642143515"></a>

避免bank冲突的方法如下：

- 通过地址重叠减少读指令

  由于一个bank group最多支持两读或者一读一写，在两读一写时会因为读/写口不足而引发bank冲突，当读指令的起始地址重叠时，硬件会将指令优化为一条。

- 离散访存下，将数据分散到不同subbank

  当gather/scatter数据存在同一subbank的不同起始地址时，发生冲突，可通过将数据分散到不同subbank来减少subbank冲突，避免不同起始地址的数据集中在同一subbank下。

- 优化计算逻辑

  对一个数据类型为float，shape为\(8, 64\)的输入每个元素加1。通过将计算逻辑由逐列计算改为逐行计算可避免同一Repeat下的冲突问题，实现方案对比如下：

    <a name="table12921549195512"></a>
    <table><thead align="left"><tr id="row1229364945511"><th class="cellrowborder" valign="top" width="6.813978389954251%" id="mcps1.1.4.1.1"><p id="p2081249145715"><a name="p2081249145715"></a><a name="p2081249145715"></a>实现方案</p>
    </th>
    <th class="cellrowborder" valign="top" width="42.6652389759564%" id="mcps1.1.4.1.2"><p id="p2029374985519"><a name="p2029374985519"></a><a name="p2029374985519"></a>原始实现</p>
    </th>
    <th class="cellrowborder" valign="top" width="50.520782634089365%" id="mcps1.1.4.1.3"><p id="p152931149115516"><a name="p152931149115516"></a><a name="p152931149115516"></a>优化实现</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row1629374995517"><td class="cellrowborder" valign="top" width="6.813978389954251%" headers="mcps1.1.4.1.1 "><p id="p108124995710"><a name="p108124995710"></a><a name="p108124995710"></a>实现方法</p>
    </td>
    <td class="cellrowborder" valign="top" width="42.6652389759564%" headers="mcps1.1.4.1.2 "><p id="p115401884217"><a name="p115401884217"></a><a name="p115401884217"></a>逐列计算，同一Repeat内输入的8个DataBlock都在同一个bank group而发生读读冲突。</p>
    </td>
    <td class="cellrowborder" valign="top" width="50.520782634089365%" headers="mcps1.1.4.1.3 "><p id="p6641340414"><a name="p6641340414"></a><a name="p6641340414"></a>逐行计算，同一个Repeat内输入的8个DataBlock不在同一个bank group内，避免了同一Repeat内的读读冲突。</p>
    </td>
    </tr>
    <tr id="row14922142214585"><td class="cellrowborder" valign="top" width="6.813978389954251%" headers="mcps1.1.4.1.1 "><p id="p6922182210587"><a name="p6922182210587"></a><a name="p6922182210587"></a>示意图</p>
    </td>
    <td class="cellrowborder" valign="top" width="42.6652389759564%" headers="mcps1.1.4.1.2 "><p id="p1273333434818"><a name="p1273333434818"></a><a name="p1273333434818"></a><a name="image47338343486"></a><a name="image47338343486"></a><span><img class="eddx" id="image47338343486" src="../../../../figures/矩阵编程逻辑位置示意图-73.png"></span></p>
    </td>
    <td class="cellrowborder" valign="top" width="50.520782634089365%" headers="mcps1.1.4.1.3 "><p id="p1074101154916"><a name="p1074101154916"></a><a name="p1074101154916"></a><a name="image147416115491"></a><a name="image147416115491"></a><span><img class="eddx" id="image147416115491" src="../../../../figures/矩阵编程逻辑位置示意图-74.png"></span></p>
    </td>
    </tr>
    </tbody>
    </table>
