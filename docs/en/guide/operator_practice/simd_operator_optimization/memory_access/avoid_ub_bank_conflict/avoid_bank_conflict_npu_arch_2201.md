# Avoiding Bank Conflicts (NPU Architecture Version 2201)<a name="ZH-CN_TOPIC_0000002499562140"></a>

<!-- md-trans-meta sourceCommit=f83e857605e3844db8b9de0442007cc115278a47 translatedAt=2026-10-23T12:45:13.307Z pushedAt=2026-11-03T07:05:13.341Z nl=2 -->

[Priority] High

>[!NOTE] Note 
>This performance optimization suggestion applies to the following product models:
><!-- npu="A3" id1 -->
>- Atlas A3 products
><!-- end id1 -->
><!-- npu="910b" id2 -->
>- Atlas A2 products
><!-- end id2 -->

[Description] To improve the efficiency and throughput of data access, the Unified Buffer (UB) adopts a bank (memory modules of equal size) structure design. The total size of the UB is 192K, divided into 48 banks. Each bank consists of 128 rows, and each row is 32B in length. These 48 banks are further organized into 16 bank groups, each containing 3 banks. For example, bank15, bank31, and bank47 form one bank group.

**Figure 1**  Schematic diagram of the bank structure (the arrow direction in the figure indicates the order of memory layout)<a name="fig1132542915196"></a>  
![](../../../../figures/bank_layout.png "Schematic diagram of the bank structure (the arrow direction in the figure indicates the order of memory layout)")

Each bank can independently perform data read and write operations, allowing multiple data requests to proceed simultaneously. However, when multiple read/write operations attempt to access the same bank or bank group at the same time, these operations must be queued due to hardware resource limitations, which causes bank conflicts and degrades performance.

Specifically, the Vector computing unit can read or write one row of data from each bank group per beat (one instruction cycle). If multiple operations in the same API attempt to access the same bank or bank group at the same time, the Vector computing unit cannot process all requests within the same cycle, causing these requests to be queued. This queuing increases the latency of data access and reduces the overall performance of the system.

## Typical Scenarios of Bank Conflicts<a name="section12644115352"></a>

Bank conflicts can be classified into the following three scenarios:

-   **Read-write conflict**: A read operation and a write operation attempt to access the same bank at the same time.
-   **Write-write conflict**: Multiple write operations attempt to access the same bank group at the same time.
-   **Read-read conflict**: Multiple read operations attempt to access the same bank group at the same time.

The following provides some specific examples. Assume that address 0x10000 is on bank16, 0x10020 is on bank17, and 0x20020 is on bank33, as shown in the following figure:

**Figure 2**  Address allocation schematic diagram<a name="fig129245311375"></a>  
![](../../../../figures/addr_alloc.png "Address allocation schematic diagram")

-   Read-write conflict example

    When the source operand src and destination operand dst of a Vector instruction read from and write to the same bank at the same time, a read-write conflict occurs. The detailed analysis is as follows:

    **Table 1**  Read-write conflict example

    <a name="table06741342154717"></a>
    <table><thead align="left"><tr id="row1767434244710"><th class="cellrowborder" valign="top" width="5.9988002399520095%" id="mcps1.2.7.1.1"><p id="p17674144211470"><a name="p17674144211470"></a><a name="p17674144211470"></a>Sequence Number</p>
    </th>
    <th class="cellrowborder" valign="top" width="9.948010397920417%" id="mcps1.2.7.1.2"><p id="p9674114213473"><a name="p9674114213473"></a><a name="p9674114213473"></a>src Address</p>
    </th>
    <th class="cellrowborder" valign="top" width="10.507898420315936%" id="mcps1.2.7.1.3"><p id="p8674114294712"><a name="p8674114294712"></a><a name="p8674114294712"></a>dst Address</p>
    </th>
    <th class="cellrowborder" valign="top" width="25.51489702059588%" id="mcps1.2.7.1.4"><p id="p17674114244718"><a name="p17674114244718"></a><a name="p17674114244718"></a>Bank</p>
    </th>
    <th class="cellrowborder" valign="top" width="31.36372725454909%" id="mcps1.2.7.1.5"><p id="p1567444234713"><a name="p1567444234713"></a><a name="p1567444234713"></a>Bank group</p>
    </th>
    <th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.2.7.1.6"><p id="p1767410425479"><a name="p1767410425479"></a><a name="p1767410425479"></a>Conclusion</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row126741742144713"><td class="cellrowborder" valign="top" width="5.9988002399520095%" headers="mcps1.2.7.1.1 "><p id="p1367415427473"><a name="p1367415427473"></a><a name="p1367415427473"></a>Example 1</p>
    </td>
    <td class="cellrowborder" valign="top" width="9.948010397920417%" headers="mcps1.2.7.1.2 "><p id="p7674134216478"><a name="p7674134216478"></a><a name="p7674134216478"></a>0x10020</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.507898420315936%" headers="mcps1.2.7.1.3 "><p id="p1567416428472"><a name="p1567416428472"></a><a name="p1567416428472"></a>0x10000</p>
    </td>
    <td class="cellrowborder" valign="top" width="25.51489702059588%" headers="mcps1.2.7.1.4 "><p id="p16674184212479"><a name="p16674184212479"></a><a name="p16674184212479"></a><span>bank_id0 != </span><span>bank_id</span><span>1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="31.36372725454909%" headers="mcps1.2.7.1.5 "><p id="p7674042124711"><a name="p7674042124711"></a><a name="p7674042124711"></a><span>bank_group_id0 != bank_group_id1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.6 "><p id="p167515425479"><a name="p167515425479"></a><a name="p167515425479"></a>The src address and dst address belong to bank16 and bank17 respectively, so no conflict exists.</p>
    </td>
    </tr>
    <tr id="row3675154284710"><td class="cellrowborder" valign="top" width="5.9988002399520095%" headers="mcps1.2.7.1.1 "><p id="p967514254716"><a name="p967514254716"></a><a name="p967514254716"></a>Example 2</p>
    </td>
    <td class="cellrowborder" valign="top" width="9.948010397920417%" headers="mcps1.2.7.1.2 "><p id="p1467514214471"><a name="p1467514214471"></a><a name="p1467514214471"></a>0x10020</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.507898420315936%" headers="mcps1.2.7.1.3 "><p id="p156751042114711"><a name="p156751042114711"></a><a name="p156751042114711"></a>0x10E20</p>
    </td>
    <td class="cellrowborder" valign="top" width="25.51489702059588%" headers="mcps1.2.7.1.4 "><p id="p1367534210471"><a name="p1367534210471"></a><a name="p1367534210471"></a><span>bank_id0 </span><span>== </span><span>bank_id1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="31.36372725454909%" headers="mcps1.2.7.1.5 "><p id="p86756421472"><a name="p86756421472"></a><a name="p86756421472"></a><span>bank_group_id0 =</span><span>= </span><span>bank_group_id1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.6 "><p id="p1667544264720"><a name="p1667544264720"></a><a name="p1667544264720"></a>The src address and dst address are both in bank17, so a conflict exists.</p>
    </td>
    </tr>
    </tbody>
    </table>

-   Write-write conflict example

    When the eight DataBlocks (block0-block7) corresponding to the dst operand of a Vector instruction are written to the same bank group at the same time, a write-write conflict occurs. The detailed analysis is as follows:

    **Table 2**  Write-write conflict example

    <a name="table11121346153814"></a>
    <table><thead align="left"><tr id="row212044623812"><th class="cellrowborder" valign="top" width="5.789726356216995%" id="mcps1.2.9.1.1"><p id="p13120184619386"><a name="p13120184619386"></a><a name="p13120184619386"></a>Sequence Number</p>
    </th>
    <th class="cellrowborder" valign="top" width="7.959673547767644%" id="mcps1.2.9.1.2"><p id="p2012004612384"><a name="p2012004612384"></a><a name="p2012004612384"></a>dst Address</p>
    </th>
    <th class="cellrowborder" valign="top" width="9.505520883341337%" id="mcps1.2.9.1.3"><p id="p112014610383"><a name="p112014610383"></a><a name="p112014610383"></a>blk_stride</p>
    </th>
    <th class="cellrowborder" valign="top" width="8.660585693710996%" id="mcps1.2.9.1.4"><p id="p212024619386"><a name="p212024619386"></a><a name="p212024619386"></a><span>block0_addr </span></p>
    </th>
    <th class="cellrowborder" valign="top" width="8.775804128660585%" id="mcps1.2.9.1.5"><p id="p1120146103816"><a name="p1120146103816"></a><a name="p1120146103816"></a><span>block1_addr </span></p>
    </th>
    <th class="cellrowborder" valign="top" width="8.967834853576573%" id="mcps1.2.9.1.6"><p id="p141208461384"><a name="p141208461384"></a><a name="p141208461384"></a><span>block2_addr </span></p>
    </th>
    <th class="cellrowborder" valign="top" width="6.721075372059531%" id="mcps1.2.9.1.7"><p id="p202701742154610"><a name="p202701742154610"></a><a name="p202701742154610"></a>...</p>
    </th>
    <th class="cellrowborder" valign="top" width="43.61977916466635%" id="mcps1.2.9.1.8"><p id="p171201746183813"><a name="p171201746183813"></a><a name="p171201746183813"></a>Conclusion</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row161211146173810"><td class="cellrowborder" valign="top" width="5.789726356216995%" headers="mcps1.2.9.1.1 "><p id="p111208465382"><a name="p111208465382"></a><a name="p111208465382"></a>Example 1</p>
    </td>
    <td class="cellrowborder" valign="top" width="7.959673547767644%" headers="mcps1.2.9.1.2 "><p id="p212044615383"><a name="p212044615383"></a><a name="p212044615383"></a>0x1FE00</p>
    </td>
    <td class="cellrowborder" valign="top" width="9.505520883341337%" headers="mcps1.2.9.1.3 "><p id="p14120046103813"><a name="p14120046103813"></a><a name="p14120046103813"></a>16</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.660585693710996%" headers="mcps1.2.9.1.4 "><p id="p1012016469382"><a name="p1012016469382"></a><a name="p1012016469382"></a>0x1FE00</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.775804128660585%" headers="mcps1.2.9.1.5 "><p id="p112019463387"><a name="p112019463387"></a><a name="p112019463387"></a>0x20000</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.967834853576573%" headers="mcps1.2.9.1.6 "><p id="p71213469382"><a name="p71213469382"></a><a name="p71213469382"></a>0x20200</p>
    </td>
    <td class="cellrowborder" valign="top" width="6.721075372059531%" headers="mcps1.2.9.1.7 "><p id="p127119427464"><a name="p127119427464"></a><a name="p127119427464"></a>...</p>
    </td>
    <td class="cellrowborder" valign="top" width="43.61977916466635%" headers="mcps1.2.9.1.8 "><p id="p81217462386"><a name="p81217462386"></a><a name="p81217462386"></a>All 8 DataBlocks are in the same bank group, so all conflict, and 8 beats complete the write of one Repeat.</p>
    </td>
    </tr>
    <tr id="row1112120465382"><td class="cellrowborder" valign="top" width="5.789726356216995%" headers="mcps1.2.9.1.1 "><p id="p1012184610384"><a name="p1012184610384"></a><a name="p1012184610384"></a>Example 2</p>
    </td>
    <td class="cellrowborder" valign="top" width="7.959673547767644%" headers="mcps1.2.9.1.2 "><p id="p41211746143815"><a name="p41211746143815"></a><a name="p41211746143815"></a>0x1FE00</p>
    </td>
    <td class="cellrowborder" valign="top" width="9.505520883341337%" headers="mcps1.2.9.1.3 "><p id="p71211446173816"><a name="p71211446173816"></a><a name="p71211446173816"></a>8</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.660585693710996%" headers="mcps1.2.9.1.4 "><p id="p8121154673814"><a name="p8121154673814"></a><a name="p8121154673814"></a>0x1FE00</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.775804128660585%" headers="mcps1.2.9.1.5 "><p id="p512119464387"><a name="p512119464387"></a><a name="p512119464387"></a>0x1FF00</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.967834853576573%" headers="mcps1.2.9.1.6 "><p id="p912194611387"><a name="p912194611387"></a><a name="p912194611387"></a>0x20000</p>
    </td>
    <td class="cellrowborder" valign="top" width="6.721075372059531%" headers="mcps1.2.9.1.7 "><p id="p20271174214619"><a name="p20271174214619"></a><a name="p20271174214619"></a>...</p>
    </td>
    <td class="cellrowborder" valign="top" width="43.61977916466635%" headers="mcps1.2.9.1.8 "><p id="p1449153217134"><a name="p1449153217134"></a><a name="p1449153217134"></a>block0 and block2 are in the same bank group, so a conflict exists. Writing one Repeat takes 4 beats.</p>
    </td>
    </tr>
    </tbody>
    </table>

-   Read-read conflict
    -   A read-read conflict occurs when multiple source operands of a Vector instruction read from the same bank group at the same time. The detailed analysis is as follows:

        **Table 3** Read-read conflict example in dual-src scenario

        <a name="table1318881512255"></a>
        <table><thead align="left"><tr id="row8188715102517"><th class="cellrowborder" valign="top" width="6.5786842631473705%" id="mcps1.2.7.1.1"><p id="p111881154254"><a name="p111881154254"></a><a name="p111881154254"></a>Sequence Number</p>
        </th>
        <th class="cellrowborder" valign="top" width="12.787442511497702%" id="mcps1.2.7.1.2"><p id="p14188915172513"><a name="p14188915172513"></a><a name="p14188915172513"></a>src0 Address</p>
        </th>
        <th class="cellrowborder" valign="top" width="14.077184563087384%" id="mcps1.2.7.1.3"><p id="p4188181542510"><a name="p4188181542510"></a><a name="p4188181542510"></a>src1 Address</p>
        </th>
        <th class="cellrowborder" valign="top" width="18.52629474105179%" id="mcps1.2.7.1.4"><p id="p1618871572514"><a name="p1618871572514"></a><a name="p1618871572514"></a>Bank</p>
        </th>
        <th class="cellrowborder" valign="top" width="31.36372725454909%" id="mcps1.2.7.1.5"><p id="p31886156253"><a name="p31886156253"></a><a name="p31886156253"></a>Bank Group</p>
        </th>
        <th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.2.7.1.6"><p id="p61881115112512"><a name="p61881115112512"></a><a name="p61881115112512"></a>Conclusion</p>
        </th>
        </tr>
        </thead>
        <tbody><tr id="row6188915102519"><td class="cellrowborder" valign="top" width="6.5786842631473705%" headers="mcps1.2.7.1.1 "><p id="p10159543259"><a name="p10159543259"></a><a name="p10159543259"></a>Example 1</p>
        </td>
        <td class="cellrowborder" valign="top" width="12.787442511497702%" headers="mcps1.2.7.1.2 "><p id="p17188131518251"><a name="p17188131518251"></a><a name="p17188131518251"></a>0x10020</p>
        </td>
        <td class="cellrowborder" valign="top" width="14.077184563087384%" headers="mcps1.2.7.1.3 "><p id="p1818811582515"><a name="p1818811582515"></a><a name="p1818811582515"></a>0x20020</p>
        </td>
        <td class="cellrowborder" valign="top" width="18.52629474105179%" headers="mcps1.2.7.1.4 "><p id="p131881015122512"><a name="p131881015122512"></a><a name="p131881015122512"></a><span>bank_id0 != </span><span>bank_id</span><span>1</span></p>
        </td>
        <td class="cellrowborder" valign="top" width="31.36372725454909%" headers="mcps1.2.7.1.5 "><p id="p518881513257"><a name="p518881513257"></a><a name="p518881513257"></a><span>bank_group_id0 == bank_group_id1</span></p>
        </td>
        <td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.6 "><p id="p3188215122513"><a name="p3188215122513"></a><a name="p3188215122513"></a>Conflict exists.</p>
        </td>
        </tr>
        <tr id="row14188615142520"><td class="cellrowborder" valign="top" width="6.5786842631473705%" headers="mcps1.2.7.1.1 "><p id="p318815156259"><a name="p318815156259"></a><a name="p318815156259"></a>Example 2</p>
        </td>
        <td class="cellrowborder" valign="top" width="12.787442511497702%" headers="mcps1.2.7.1.2 "><p id="p151881115192511"><a name="p151881115192511"></a><a name="p151881115192511"></a>0x10020</p>
        </td>
        <td class="cellrowborder" valign="top" width="14.077184563087384%" headers="mcps1.2.7.1.3 "><p id="p12188201519257"><a name="p12188201519257"></a><a name="p12188201519257"></a>0x10000</p>
        </td>
        <td class="cellrowborder" valign="top" width="18.52629474105179%" headers="mcps1.2.7.1.4 "><p id="p71881515182513"><a name="p71881515182513"></a><a name="p71881515182513"></a><span>bank_id0 </span><span>!= </span><span>bank_id1</span></p>
        </td>
        <td class="cellrowborder" valign="top" width="31.36372725454909%" headers="mcps1.2.7.1.5 "><p id="p1318817159254"><a name="p1318817159254"></a><a name="p1318817159254"></a><span>bank_group_id0 !</span><span>= </span><span>bank_group_id1</span></p>
        </td>
        <td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.6 "><p id="p11881815182511"><a name="p11881815182511"></a><a name="p11881815182511"></a>No conflict.</p>
        </td>
        </tr>
        </tbody>
        </table>

    -   When the eight DataBlocks (block0-block7) corresponding to a source operand of a Vector instruction are read into the same bank group, a read-read conflict occurs. The detailed analysis is as follows:

        **Table 4**  Read-read conflict example in a single-src scenario

        <a name="table332972534717"></a>
        <table><thead align="left"><tr id="row832822516476"><th class="cellrowborder" valign="top" width="6.117919521374119%" id="mcps1.2.9.1.1"><p id="p157111614175011"><a name="p157111614175011"></a><a name="p157111614175011"></a>Sequence Number</p>
        </th>
        <th class="cellrowborder" valign="top" width="11.560358969410403%" id="mcps1.2.9.1.2"><p id="p33287255478"><a name="p33287255478"></a><a name="p33287255478"></a>src Address</p>
        </th>
        <th class="cellrowborder" valign="top" width="9.910257647399403%" id="mcps1.2.9.1.3"><p id="p73282255477"><a name="p73282255477"></a><a name="p73282255477"></a>blk_stride</p>
        </th>
        <th class="cellrowborder" valign="top" width="10.798031458071987%" id="mcps1.2.9.1.4"><p id="p1332819254473"><a name="p1332819254473"></a><a name="p1332819254473"></a><span>block0_addr </span></p>
        </th>
        <th class="cellrowborder" valign="top" width="10.633986297404228%" id="mcps1.2.9.1.5"><p id="p33281125164710"><a name="p33281125164710"></a><a name="p33281125164710"></a><span>block1_addr </span></p>
        </th>
        <th class="cellrowborder" valign="top" width="9.234777574061564%" id="mcps1.2.9.1.6"><p id="p632818253478"><a name="p632818253478"></a><a name="p632818253478"></a><span>block2_addr </span></p>
        </th>
        <th class="cellrowborder" valign="top" width="6.754800733378366%" id="mcps1.2.9.1.7"><p id="p20451145754719"><a name="p20451145754719"></a><a name="p20451145754719"></a>...</p>
        </th>
        <th class="cellrowborder" valign="top" width="34.98986779889993%" id="mcps1.2.9.1.8"><p id="p11328625144713"><a name="p11328625144713"></a><a name="p11328625144713"></a>Conclusion</p>
        </th>
        </tr>
        </thead>
        <tbody><tr id="row7329725144718"><td class="cellrowborder" valign="top" width="6.117919521374119%" headers="mcps1.2.9.1.1 "><p id="p33281425114710"><a name="p33281425114710"></a><a name="p33281425114710"></a>Example 1</p>
        </td>
        <td class="cellrowborder" valign="top" width="11.560358969410403%" headers="mcps1.2.9.1.2 "><p id="p1932817255472"><a name="p1932817255472"></a><a name="p1932817255472"></a>0x1FE00</p>
        </td>
        <td class="cellrowborder" valign="top" width="9.910257647399403%" headers="mcps1.2.9.1.3 "><p id="p113284254479"><a name="p113284254479"></a><a name="p113284254479"></a>16</p>
        </td>
        <td class="cellrowborder" valign="top" width="10.798031458071987%" headers="mcps1.2.9.1.4 "><p id="p183285257479"><a name="p183285257479"></a><a name="p183285257479"></a>0x1FE00</p>
        </td>
        <td class="cellrowborder" valign="top" width="10.633986297404228%" headers="mcps1.2.9.1.5 "><p id="p12329925194719"><a name="p12329925194719"></a><a name="p12329925194719"></a>0x20000</p>
        </td>
        <td class="cellrowborder" valign="top" width="9.234777574061564%" headers="mcps1.2.9.1.6 "><p id="p45321853204715"><a name="p45321853204715"></a><a name="p45321853204715"></a>0x20200</p>
        </td>
        <td class="cellrowborder" valign="top" width="6.754800733378366%" headers="mcps1.2.9.1.7 "><p id="p3451457164714"><a name="p3451457164714"></a><a name="p3451457164714"></a>...</p>
        </td>
        <td class="cellrowborder" valign="top" width="34.98986779889993%" headers="mcps1.2.9.1.8 "><p id="p3329152520475"><a name="p3329152520475"></a><a name="p3329152520475"></a>All 8 <span>DataBlock</span>s are in the same bank group, so all of them conflict, and the read operation of one Repeat takes 8 beats.</p>
        </td>
        </tr>
        <tr id="row432932554719"><td class="cellrowborder" valign="top" width="6.117919521374119%" headers="mcps1.2.9.1.1 "><p id="p18329112524713"><a name="p18329112524713"></a><a name="p18329112524713"></a>Example 2</p>
        </td>
        <td class="cellrowborder" valign="top" width="11.560358969410403%" headers="mcps1.2.9.1.2 "><p id="p163291925114715"><a name="p163291925114715"></a><a name="p163291925114715"></a>0x1FE00</p>
        </td>
        <td class="cellrowborder" valign="top" width="9.910257647399403%" headers="mcps1.2.9.1.3 "><p id="p1032918252477"><a name="p1032918252477"></a><a name="p1032918252477"></a>8</p>
        </td>
        <td class="cellrowborder" valign="top" width="10.798031458071987%" headers="mcps1.2.9.1.4 "><p id="p1732932594710"><a name="p1732932594710"></a><a name="p1732932594710"></a>0x1FE00</p>
        </td>
        <td class="cellrowborder" valign="top" width="10.633986297404228%" headers="mcps1.2.9.1.5 "><p id="p103291125134710"><a name="p103291125134710"></a><a name="p103291125134710"></a>0x1FF00</p>
        </td>
        <td class="cellrowborder" valign="top" width="9.234777574061564%" headers="mcps1.2.9.1.6 "><p id="p7329182515474"><a name="p7329182515474"></a><a name="p7329182515474"></a>0x20000</p>
        </td>
        <td class="cellrowborder" valign="top" width="6.754800733378366%" headers="mcps1.2.9.1.7 "><p id="p1945115714477"><a name="p1945115714477"></a><a name="p1945115714477"></a>...</p>
        </td>
        <td class="cellrowborder" valign="top" width="34.98986779889993%" headers="mcps1.2.9.1.8 "><p id="p9329192544712"><a name="p9329192544712"></a><a name="p9329192544712"></a>block0 and block2 are in the same bank group, so a conflict exists, and a Repeat takes 4 beats to complete.</p>
        </td>
        </tr>
        </tbody>
        </table>

>[!NOTE] Note
>You can use the msOpProf tool to collect performance data related to the resource conflict ratio.
>For details about how to use the tool and the description of the resource conflict ratio performance data file, see [msOpProf User Guide](https://gitcode.com/Ascend/msopprof/blob/26.2.0/docs/en/user_guide/msopprof_user_guide.md).

## How to Avoid Bank Conflicts<a name="section12501642143515"></a>

There are two methods to avoid bank conflicts: **optimizing the computation logic** and **optimizing the address allocation**.

-   **Optimizing the computation logic**

    Perform a transpose operation of \(1, 0, 2\) on an input with a shape of \(8, 16, 16\), and the output shape is \(16, 8, 16\). By changing the computation logic from "skip read, continuous write" to "continuous read, skip write", the conflict can be avoided. The implementation schemes are compared as follows:

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
    <td class="cellrowborder" valign="top" width="42.6652389759564%" headers="mcps1.1.4.1.2 "><p id="p55722115019"><a name="p55722115019"></a><a name="p55722115019"></a>Skip read, continuous write</p>
    <p id="p144638315712"><a name="p144638315712"></a><a name="p144638315712"></a>Within the same Repeat, the eight input DataBlocks are all in the same bank group, causing a read-read conflict.</p>
    </td>
    <td class="cellrowborder" valign="top" width="50.520782634089365%" headers="mcps1.1.4.1.3 "><p id="p2293249135516"><a name="p2293249135516"></a><a name="p2293249135516"></a>Continuous read, skip write</p>
    <p id="p6641340414"><a name="p6641340414"></a><a name="p6641340414"></a>The eight input DataBlocks within the same Repeat are not in the same bank group, which avoids read-read conflicts.</p>
    </td>
    </tr>
    <tr id="row14922142214585"><td class="cellrowborder" valign="top" width="6.813978389954251%" headers="mcps1.1.4.1.1 "><p id="p6922182210587"><a name="p6922182210587"></a><a name="p6922182210587"></a>Schematic diagram</p>
    </td>
    <td class="cellrowborder" valign="top" width="42.6652389759564%" headers="mcps1.1.4.1.2 "><p id="p10922222205810"><a name="p10922222205810"></a><a name="p10922222205810"></a><a name="image1757423545813"></a><a name="image1757423545813"></a><span><img class="eddx" id="image1757423545813" src="../../../../figures/mat_pos_67.png" width="422.94" height="376.36672500000003"></span></p>
    </td>
    <td class="cellrowborder" valign="top" width="50.520782634089365%" headers="mcps1.1.4.1.3 "><p id="p1922622115813"><a name="p1922622115813"></a><a name="p1922622115813"></a><a name="image6621154316580"></a><a name="image6621154316580"></a><span><img class="eddx" id="image6621154316580" src="../../../../figures/mat_pos_68.png" width="489.77250000000004" height="363.174721"></span></p>
    </td>
    </tr>
    <tr id="row3293124918559"><td class="cellrowborder" valign="top" width="6.813978389954251%" headers="mcps1.1.4.1.1 "><p id="p12812993573"><a name="p12812993573"></a><a name="p12812993573"></a>Example code</p>
    </td>
    <td class="cellrowborder" valign="top" width="42.6652389759564%" headers="mcps1.1.4.1.2 "><a name="screen924835613570"></a><a name="screen924835613570"></a><pre class="screen" codetype="Cpp" id="screen924835613570">uint64_t mask = 128;
    UnaryRepeatParams params;
    params.dstBlkStride  = 1;
    params.srcBlkStride = 16;
    for(uint32_t i=0; i&lt;16; i++)   {
        AscendC::Adds(dstLocal[i * 128], srcLocal[i * 16], 0, mask, 1, params);
    }</pre>
    </td>
    <td class="cellrowborder" valign="top" width="50.520782634089365%" headers="mcps1.1.4.1.3 "><a name="screen271414925813"></a><a name="screen271414925813"></a><pre class="screen" codetype="Cpp" id="screen271414925813">uint64_t mask = 128;
    UnaryRepeatParams params;
    params.dstBlkStride  = 8;
    params.srcBlkStride = 1;
    for(uint32_t i=0; i&lt;8; i++)   {
        AscendC::Adds(dstLocal[i * 16], srcLocal[i * 256], 0, mask, 2, params);
    }</pre>
    </td>
    </tr>
    </tbody>
    </table>

-   **Optimized address allocation**

    To implement the conversion of an `8192 × 8192` half ND cube to the global compact NZ layout, after splitting, each Tile block completes the conversion of `144 × 128` half data. By appropriately expanding the memory during memory allocation and adjusting the address stride of adjacent DataBlocks within a single copy operation, it is ensured that within the same Copy operation, the write operations of different DataBlocks do not occur in the same bank group at the same time. For the complete example, see [Bank conflict avoidance example](../../../../../../../examples/01_simd_cpp_api/05_best_practices/04_memory_access/bank_conflict_nd2nz).
    The implementation schemes are compared as follows:

    - Original implementation: No address optimization or copy stride is applied. The Copy stride is 144, and the addresses of each Tensor are as follows:
        - ND cube: start address 0x0, tensor length of 144 * 128 * sizeof(half) bytes
        - NZ cube: start address 0x12000, tensor length of 144 * 128 * sizeof(half) bytes
        - In each Copy operation, eight Datablocks write to one bank group simultaneously, stretching a single Copy from one beat to eight beats.

    ![](../../../../figures/mat_pos_69.png)

    - Optimized implementation: optimize the address and copy stride by allocating 256 extra bytes for the NZ cube (that is, one extra row of UB memory space), and set the Copy stride to 145. The addresses of each tensor are as follows:
        - ND cube: start address 0x0, tensor length of 144 * 128 * sizeof(half) bytes
        - NZ cube: start address 0x12000, tensor length of 145 * 128 * sizeof(half) bytes
        - The NZ cube applies for 256 extra bytes, and the Copy stride is increased by 1 to prevent eight Datablocks from writing to one bank group simultaneously during Copy, so that a single Copy completes within one beat.

    ![](../../../../figures/mat_pos_70.png)

    - Example code of the original implementation:
        ```cpp
        AscendC::LocalTensor<half> nd(AscendC::TPosition::VECIN, 0, 144 * 128);
        AscendC::LocalTensor<half> nz(AscendC::TPosition::VECOUT, 2 * 144 * 128 * sizeof(half), 144 * 128);
        AscendC::CopyRepeatParams copyParams;
        copyParams.dstStride = 144;
        copyParams.srcStride = 1;
        copyParams.dstRepeatSize = 1;
        copyParams.srcRepeatSize = 128 / 16;

        AscendC::Copy(nz[0], nd[0],
            static_cast<uint64_t>(128), static_cast<uint8_t>(144), copyParams);
        ```
    - Optimized implementation example code:
        ```cpp
        AscendC::LocalTensor<half> nd(AscendC::TPosition::VECIN, 0, 144 * 128);
        AscendC::LocalTensor<half> nz(AscendC::TPosition::VECOUT, 2 * 144 * 128 * sizeof(half), 145 * 128); // Allocate 256 more bytes
        AscendC::CopyRepeatParams copyParams;
        copyParams.dstStride = 145;         // Increase the address stride by 1 during copy
        copyParams.srcStride = 1;
        copyParams.dstRepeatSize = 1;
        copyParams.srcRepeatSize = 128 / 16;

        AscendC::Copy(nz[0], nd[0],
            static_cast<uint64_t>(128), static_cast<uint8_t>(144), copyParams);
        ```

For more UB bank conflict scenario analysis, see [bank conflict analysis example](../../../../../../../examples/01_simd_cpp_api/05_best_practices/04_memory_access/bank_conflict_ub).
