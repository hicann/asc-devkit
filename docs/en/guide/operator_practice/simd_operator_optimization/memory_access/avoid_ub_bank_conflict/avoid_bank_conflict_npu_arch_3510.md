# Avoiding Bank Conflicts (NPU Architecture Version 3510)<a name="ZH-CN_TOPIC_0000002531242067"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T12:45:41.755Z pushedAt=2026-11-03T07:05:13.340Z nl=2 -->

[Priority] Medium

>[!NOTE] Note 
>This performance optimization suggestion applies to the following product models:
><!-- npu="950" id1 -->
>- Ascend 950PR & 950DT products
><!-- end id1 -->

The Unified Buffer (UB) cache has a total size of 256KB and contains 8 bank groups, each of which contains 2 banks. Each bank is 16KB in size and consists of 512 rows, with each row being 32B in length, using low-order address interleaving.

The UB address encoding format:
```
UB_ADDR[18:0] = {BANK_DEPTH[8:0], BANK[0], BG[2:0], BANK_WIDTH[4:0]}
```

**Figure 1**  Bank structure diagram (the arrow direction in the figure indicates the order of memory layout)<a name="fig873359165316"></a>  

![](../../../../figures/ub_950.png "950_UB memory structure diagram")

The 3510 architecture uses low-order addressing, and the address layout is shown in the figure:

**Figure 2**  Low-order address interleaving layout

![](../../../../figures/low_interleave.png "Low-order interleaved address layout")

Each bank can be divided into four subbanks, each with a width of 8B. Subbanks are perceived only in gather/scatter scenarios, while continuous read/write operations perceive only the bank.

**Figure 3** subbank structure

![](../../../../figures/subbank.png "subbank structure")

## Typical Scenarios of Bank Conflicts<a name="section9689957379"></a>

A single bank group supports two read operations, or one read operation and one write operation, in a single cycle. It does not support two write operations in a single cycle within the same bank group. Based on the memory structure, bank conflicts can be classified into the following three types:

-   **Read-write conflict**: A read operation and a write operation simultaneously attempt to access the same bank, or two read operations and one write operation simultaneously attempt to access the same bank group.
-   **Write-write conflict**: Multiple write operations attempt to access the same bank group at the same time.
-   **Read-read conflict**: Two read operations attempt to access the same bank at the same time, or more than two read operations attempt to access the same bank group at the same time.

**Note: Bank conflict scenarios are closely related to the UB specifications. Changes in the specifications usually lead to changes in bank conflict scenarios.**

### Bank Conflict Analysis Based on Continuous Memory Access
The following describes the conflict types in continuous memory access:

-   Read-write conflict

    A read-write conflict occurs when a read operation and a write operation simultaneously attempt to access the same bank. The detailed analysis is as follows:

    **Table 1**  Example of one-read-one-write conflict

    <a name="table1789735214010"></a>
    <table><thead align="left"><tr id="row20897752164015"><th class="cellrowborder" valign="top" width="5.9988002399520095%" id="mcps1.2.7.1.1"><p id="p13897195244019"><a name="p13897195244019"></a><a name="p13897195244019"></a>Sequence Number</p>
    </th>
    <th class="cellrowborder" valign="top" width="9.948010397920417%" id="mcps1.2.7.1.2"><p id="p3897155224010"><a name="p3897155224010"></a><a name="p3897155224010"></a>src Address</p>
    </th>
    <th class="cellrowborder" valign="top" width="10.507898420315936%" id="mcps1.2.7.1.3"><p id="p13897252164011"><a name="p13897252164011"></a><a name="p13897252164011"></a>dst Address</p>
    </th>
    <th class="cellrowborder" valign="top" width="25.51489702059588%" id="mcps1.2.7.1.4"><p id="p138976528405"><a name="p138976528405"></a><a name="p138976528405"></a>Bank</p>
    </th>
    <th class="cellrowborder" valign="top" width="31.36372725454909%" id="mcps1.2.7.1.5"><p id="p689795214408"><a name="p689795214408"></a><a name="p689795214408"></a>Bank Group</p>
    </th>
    <th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.2.7.1.6"><p id="p98976528403"><a name="p98976528403"></a><a name="p98976528403"></a>Conclusion</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row10897145224010"><td class="cellrowborder" valign="top" width="5.9988002399520095%" headers="mcps1.2.7.1.1 "><p id="p8897185254012"><a name="p8897185254012"></a><a name="p8897185254012"></a>Example 1</p>
    </td>
    <td class="cellrowborder" valign="top" width="9.948010397920417%" headers="mcps1.2.7.1.2 "><p id="p12897155215406"><a name="p12897155215406"></a><a name="p12897155215406"></a>0x0000</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.507898420315936%" headers="mcps1.2.7.1.3 "><p id="p19897165213403"><a name="p19897165213403"></a><a name="p19897165213403"></a>0x4000</p>
    </td>
    <td class="cellrowborder" valign="top" width="25.51489702059588%" headers="mcps1.2.7.1.4 "><p id="p1489705220408"><a name="p1489705220408"></a><a name="p1489705220408"></a><span>bank_id0 == </span><span>bank_id</span><span>1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="31.36372725454909%" headers="mcps1.2.7.1.5 "><p id="p4897155264016"><a name="p4897155264016"></a><a name="p4897155264016"></a><span>bank_group_id0 == bank_group_id1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.6 "><p id="p1589719525406"><a name="p1589719525406"></a><a name="p1589719525406"></a>The src address and dst address belong to the same bank, causing a read-write conflict.</p>
    </td>
    </tr>
    <tr id="row1489775264016"><td class="cellrowborder" valign="top" width="5.9988002399520095%" headers="mcps1.2.7.1.1 "><p id="p12897152184019"><a name="p12897152184019"></a><a name="p12897152184019"></a>Example 2</p>
    </td>
    <td class="cellrowborder" valign="top" width="9.948010397920417%" headers="mcps1.2.7.1.2 "><p id="p108981152174015"><a name="p108981152174015"></a><a name="p108981152174015"></a>0x0000</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.507898420315936%" headers="mcps1.2.7.1.3 "><p id="p1389811520408"><a name="p1389811520408"></a><a name="p1389811520408"></a>0x4100</p>
    </td>
    <td class="cellrowborder" valign="top" width="25.51489702059588%" headers="mcps1.2.7.1.4 "><p id="p389885274014"><a name="p389885274014"></a><a name="p389885274014"></a><span>bank_id0 </span><span>!= </span><span>bank_id1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="31.36372725454909%" headers="mcps1.2.7.1.5 "><p id="p7898155214011"><a name="p7898155214011"></a><a name="p7898155214011"></a><span>bank_group_id0 =</span><span>= </span><span>bank_group_id1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.6 "><p id="p989815214018"><a name="p989815214018"></a><a name="p989815214018"></a>The src address and dst address belong to different banks in the same bank group, so there is no conflict.</p>
    </td>
    </tr>
    </tbody>
    </table>

    **Table 2** Example of two-read-one-write conflict

    <table><thead align="left"><tr id="row1767434244710"><th class="cellrowborder" valign="top" width="5.55%" id="mcps1.2.8.1.1"><p id="p17674144211470"><a name="p17674144211470"></a><a name="p17674144211470"></a>Sequence Number</p>
    </th>
    <th class="cellrowborder" valign="top" width="7.53%" id="mcps1.2.8.1.2"><p id="p9674114213473"><a name="p9674114213473"></a><a name="p9674114213473"></a>src0 Address</p>
    </th>
    <th class="cellrowborder" valign="top" width="7.5200000000000005%" id="mcps1.2.8.1.3"><p id="p1888613260115"><a name="p1888613260115"></a><a name="p1888613260115"></a>src1 Address</p>
    </th>
    <th class="cellrowborder" valign="top" width="7.4399999999999995%" id="mcps1.2.8.1.4"><p id="p8674114294712"><a name="p8674114294712"></a><a name="p8674114294712"></a>dst Address</p>
    </th>
    <th class="cellrowborder" valign="top" width="15.409999999999998%" id="mcps1.2.8.1.5"><p id="p17674114244718"><a name="p17674114244718"></a><a name="p17674114244718"></a>Bank</p>
    </th>
    <th class="cellrowborder" valign="top" width="23.09%" id="mcps1.2.8.1.6"><p id="p1567444234713"><a name="p1567444234713"></a><a name="p1567444234713"></a>Bank Group</p>
    </th>
    <th class="cellrowborder" valign="top" width="33.46%" id="mcps1.2.8.1.7"><p id="p1767410425479"><a name="p1767410425479"></a><a name="p1767410425479"></a>Conclusion</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row126741742144713"><td class="cellrowborder" valign="top" width="5.55%" headers="mcps1.2.8.1.1 "><p id="p1367415427473"><a name="p1367415427473"></a><a name="p1367415427473"></a>Example 1</p>
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
    <td class="cellrowborder" valign="top" width="33.46%" headers="mcps1.2.8.1.7 "><p id="p167515425479"><a name="p167515425479"></a><a name="p167515425479"></a>The src0 address and the src1 address belong to the same bank, resulting in a read-read conflict.</p>
    <p id="p1227485214177"><a name="p1227485214177"></a><a name="p1227485214177"></a>The src0 address and the dst address belong to the same bank, resulting in a read-write conflict.</p>
    <p id="p15494155717182"><a name="p15494155717182"></a><a name="p15494155717182"></a>The same bank group supports at most two reads or one read and one write, and a conflict currently exists.</p>
    </td>
    </tr>
    <tr id="row3675154284710"><td class="cellrowborder" valign="top" width="5.55%" headers="mcps1.2.8.1.1 "><p id="p967514254716"><a name="p967514254716"></a><a name="p967514254716"></a>Example 2</p>
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
    <td class="cellrowborder" valign="top" width="33.46%" headers="mcps1.2.8.1.7 "><p id="p1667544264720"><a name="p1667544264720"></a><a name="p1667544264720"></a>src0 and src1 have the same start address, so the two reads are optimized by hardware into one read.</p>
    <p id="p27720182416"><a name="p27720182416"></a><a name="p27720182416"></a>The src0 address and the dst address do not belong to the same bank, so there is no read-write conflict.</p>
    </td>
    </tr>
    <tr id="row15280195115245"><td class="cellrowborder" valign="top" width="5.55%" headers="mcps1.2.8.1.1 "><p id="p10280175113242"><a name="p10280175113242"></a><a name="p10280175113242"></a>Example 3</p>
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
    <td class="cellrowborder" valign="top" width="33.46%" headers="mcps1.2.8.1.7 "><p id="p828065182410"><a name="p828065182410"></a><a name="p828065182410"></a>The src0 address and src1 address belong to different banks of the same bank group, so no read-read conflict occurs.</p>
    <p id="p7843650172915"><a name="p7843650172915"></a><a name="p7843650172915"></a>The src1 address and dst address belong to the same bank, so no conflict occurs.</p>
    </td>
    </tr>
    </tbody>
    </table>

-   Write-write conflict

    Simultaneously writing to a bank group causes a write-write conflict. The detailed analysis is as follows:

    **Table 3**  Write-write conflict example

    <table><thead align="left"><tr id="row17580229172710"><th class="cellrowborder" valign="top" width="5.9988002399520095%" id="mcps1.2.7.1.1"><p id="p115801529192712"><a name="p115801529192712"></a><a name="p115801529192712"></a>Sequence Number</p>
    </th>
    <th class="cellrowborder" valign="top" width="8.138372325534892%" id="mcps1.2.7.1.2"><p id="p1258022919273"><a name="p1258022919273"></a><a name="p1258022919273"></a>dst0 Address</p>
    </th>
    <th class="cellrowborder" valign="top" width="8.048390321935612%" id="mcps1.2.7.1.3"><p id="p18580172932720"><a name="p18580172932720"></a><a name="p18580172932720"></a>dst1 Address</p>
    </th>
    <th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.2.7.1.4"><p id="p8580729102716"><a name="p8580729102716"></a><a name="p8580729102716"></a>Bank</p>
    </th>
    <th class="cellrowborder" valign="top" width="24.96500699860028%" id="mcps1.2.7.1.5"><p id="p18580152942710"><a name="p18580152942710"></a><a name="p18580152942710"></a>Bank Group</p>
    </th>
    <th class="cellrowborder" valign="top" width="36.18276344731054%" id="mcps1.2.7.1.6"><p id="p11580192916275"><a name="p11580192916275"></a><a name="p11580192916275"></a>Conclusion</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row1058082919273"><td class="cellrowborder" valign="top" width="5.9988002399520095%" headers="mcps1.2.7.1.1 "><p id="p8580429102713"><a name="p8580429102713"></a><a name="p8580429102713"></a>Example 1</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.138372325534892%" headers="mcps1.2.7.1.2 "><p id="p1158092992713"><a name="p1158092992713"></a><a name="p1158092992713"></a>0x0000</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.048390321935612%" headers="mcps1.2.7.1.3 "><p id="p3580132919279"><a name="p3580132919279"></a><a name="p3580132919279"></a>0x4100</p>
    </td>
    <td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.4 "><p id="p1858018294272"><a name="p1858018294272"></a><a name="p1858018294272"></a><span>bank_id0 </span><span>!= </span><span>bank_id1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="24.96500699860028%" headers="mcps1.2.7.1.5 "><p id="p165801829132711"><a name="p165801829132711"></a><a name="p165801829132711"></a><span>bank_group_id0 =</span><span>= </span><span>bank_group_id1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="36.18276344731054%" headers="mcps1.2.7.1.6 "><p id="p11580729142715"><a name="p11580729142715"></a><a name="p11580729142715"></a>The dst0 address and dst1 address belong to the same bank group, so a conflict exists.</p>
    </td>
    </tr>
    </tbody>
    </table>

-   Read-read conflict
    
    **Table 4** Read-read conflict example
    <table><thead align="left"><tr id="row134631185514"><th class="cellrowborder" valign="top" width="5.9988002399520095%" id="mcps1.2.7.1.1"><p id="p16461312553"><a name="p16461312553"></a><a name="p16461312553"></a>Sequence Number</p>
    </th>
    <th class="cellrowborder" valign="top" width="8.138372325534892%" id="mcps1.2.7.1.2"><p id="p4466118554"><a name="p4466118554"></a><a name="p4466118554"></a>src0 Address</p>
    </th>
    <th class="cellrowborder" valign="top" width="8.048390321935612%" id="mcps1.2.7.1.3"><p id="p14691125519"><a name="p14691125519"></a><a name="p14691125519"></a>src1 Address</p>
    </th>
    <th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.2.7.1.4"><p id="p246181165514"><a name="p246181165514"></a><a name="p246181165514"></a>Bank</p>
    </th>
    <th class="cellrowborder" valign="top" width="24.96500699860028%" id="mcps1.2.7.1.5"><p id="p14463165511"><a name="p14463165511"></a><a name="p14463165511"></a>Bank Group</p>
    </th>
    <th class="cellrowborder" valign="top" width="36.18276344731054%" id="mcps1.2.7.1.6"><p id="p94601175513"><a name="p94601175513"></a><a name="p94601175513"></a>Conclusion</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row13467155518"><td class="cellrowborder" valign="top" width="5.9988002399520095%" headers="mcps1.2.7.1.1 "><p id="p34612175519"><a name="p34612175519"></a><a name="p34612175519"></a>Example 1</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.138372325534892%" headers="mcps1.2.7.1.2 "><p id="p154619117550"><a name="p154619117550"></a><a name="p154619117550"></a>0x0000</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.048390321935612%" headers="mcps1.2.7.1.3 "><p id="p144691175519"><a name="p144691175519"></a><a name="p144691175519"></a>0x4000</p>
    </td>
    <td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.4 "><p id="p124620185513"><a name="p124620185513"></a><a name="p124620185513"></a><span>bank_id0 </span><span>== </span><span>bank_id1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="24.96500699860028%" headers="mcps1.2.7.1.5 "><p id="p15474195517"><a name="p15474195517"></a><a name="p15474195517"></a><span>bank_group_id0 =</span><span>= </span><span>bank_group_id1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="36.18276344731054%" headers="mcps1.2.7.1.6 "><p id="p1471819555"><a name="p1471819555"></a><a name="p1471819555"></a>The src0 address and src1 address belong to the same bank, so a conflict exists.</p>
    </td>
    </tr>
    </tbody>
    </table>

### Bank Conflict Analysis Based on Scatter Memory Access
The following interprets conflicts under scatter memory access:

-   When gather/scatter memory access data is distributed across different subbanks, no conflict occurs.
-   When gather/scatter data exists at the same start address of the same subbank, no conflict occurs; therefore, accesses with the same index value do not conflict.
-   When gather/scatter data exists at different start addresses of the same subbank, a conflict occurs.
-   Gather/scatter conflicts with continuous access instructions/MTE transfer instructions.

## How to Avoid Bank Conflicts<a name="section12501642143515"></a>

The methods for avoiding bank conflicts are as follows:

- Reduce read instructions through address overlap.

  The same bank group supports only two read operations, or one read operation and one write operation, in a single cycle. Two reads and one write exceed the supported parallel access combination and cause a bank conflict. When the start addresses of two read instructions overlap, the hardware optimizes them into one.

- Distribute data to different subbanks in scatter memory access.

  When gather/scatter data exists at different start addresses of the same subbank, a conflict occurs. You can distribute the data to different subbanks to reduce subbank conflicts and avoid concentrating data with different start addresses in the same subbank.

- Optimize the computation logic.

  Add 1 to each element of an input whose data type is float and whose shape is \(8, 64\). By changing the computation logic from column-by-column to row-by-row, conflicts under the same Repeat can be avoided. The implementation schemes are compared as follows:

    <a name="table12921549195512"></a>
    <table><thead align="left"><tr id="row1229364945511"><th class="cellrowborder" valign="top" width="6.813978389954251%" id="mcps1.1.4.1.1"><p id="p2081249145715"><a name="p2081249145715"></a><a name="p2081249145715"></a>Implementation Scheme</p>
    </th>
    <th class="cellrowborder" valign="top" width="42.6652389759564%" id="mcps1.1.4.1.2"><p id="p2029374985519"><a name="p2029374985519"></a><a name="p2029374985519"></a>Original Implementation</p>
    </th>
    <th class="cellrowborder" valign="top" width="50.520782634089365%" id="mcps1.1.4.1.3"><p id="p152931149115516"><a name="p152931149115516"></a><a name="p152931149115516"></a>Optimized Implementation</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row1629374995517"><td class="cellrowborder" valign="top" width="6.813978389954251%" headers="mcps1.1.4.1.1 "><p id="p108124995710"><a name="p108124995710"></a><a name="p108124995710"></a>Implementation Method</p>
    </td>
    <td class="cellrowborder" valign="top" width="42.6652389759564%" headers="mcps1.1.4.1.2 "><p id="p115401884217"><a name="p115401884217"></a><a name="p115401884217"></a>Column-by-column computation. The eight input DataBlocks within the same Repeat are all in the same bank group, causing read-read conflicts.</p>
    </td>
    <td class="cellrowborder" valign="top" width="50.520782634089365%" headers="mcps1.1.4.1.3 "><p id="p6641340414"><a name="p6641340414"></a><a name="p6641340414"></a>Row-by-row computation. The eight input DataBlocks within the same Repeat are not in the same bank group, avoiding read-read conflicts within the same Repeat.</p>
    </td>
    </tr>
    <tr id="row14922142214585"><td class="cellrowborder" valign="top" width="6.813978389954251%" headers="mcps1.1.4.1.1 "><p id="p6922182210587"><a name="p6922182210587"></a><a name="p6922182210587"></a>Schematic Diagram</p>
    </td>
    <td class="cellrowborder" valign="top" width="42.6652389759564%" headers="mcps1.1.4.1.2 "><p id="p1273333434818"><a name="p1273333434818"></a><a name="p1273333434818"></a><a name="image47338343486"></a><a name="image47338343486"></a><span><img class="eddx" id="image47338343486" src="../../../../figures/mat_pos_73.png"></span></p>
    </td>
    <td class="cellrowborder" valign="top" width="50.520782634089365%" headers="mcps1.1.4.1.3 "><p id="p1074101154916"><a name="p1074101154916"></a><a name="p1074101154916"></a><a name="image147416115491"></a><a name="image147416115491"></a><span><img class="eddx" id="image147416115491" src="../../../../figures/mat_pos_74.png"></span></p>
    </td>
    </tr>
    </tbody>
    </table>
