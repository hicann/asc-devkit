# Avoiding Bank Conflicts (NPU Architecture Version 3510)<a name="ZH-CN_TOPIC_0000002531242067"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-08-26T14:24:43.002Z -->

To improve the efficiency and throughput of data access, the Unified Buffer adopts a bank (memory modules of equal size) structure design. The total size of the Unified Buffer is 256K, divided into 16 banks. Each bank consists of 512 rows, and each row is 32B in length. These 16 banks are further organized into 8 bank groups, with each bank group containing 2 banks. For example, bank7 and bank15 form a bank group.

**Figure 1**  Bank structural diagram (the arrow in the figure indicates the order of the memory layout)<a name="fig873359165316"></a>  
![](../../../../figures/bank_structure_diagram_with_arrow_indicating_memory_layout_order_71.png "bank structural diagram (the arrow in the figure indicates the order of the memory layout)-71")

Each bank can independently perform data read and write operations, allowing multiple data requests to proceed simultaneously. However, when multiple read and write operations attempt to access the same bank at the same time, these operations must wait in a queue due to hardware resource limitations, which causes bank conflicts and degrades performance.

Specifically, the Vector computing unit can read or write one row of data from each bank group per cycle (one instruction cycle). When multiple read and write operations attempt to access the same bank at the same time, the Vector computing unit cannot process all requests within the same cycle, causing these requests to wait in a queue. This queuing increases the latency of data access and reduces the overall performance of the system.

## Typical Scenarios of Bank Conflicts<a name="section9689957379"></a>

Bank conflicts can be classified into the following three scenarios:

-   **Read-write conflict**: A read operation and a write operation attempt to access the same bank at the same time.
-   **Write-write conflict**: Multiple write operations attempt to access the same bank group at the same time.
-   **Read-read conflict**: Two read operations attempt to access the same bank at the same time, or more than two read operations attempt to access the same bank group at the same time.

The following provides some specific examples. Assume that address 0x10000 is on bank0 and 0x10020 is on bank1, as shown in the following figure:

**Figure 2**  Address allocation schematic diagram<a name="fig1750123073213"></a>  
![](../../../../figures/address_allocation_diagram_72.png "address-allocation-schematic-diagram-72")

-   Read-write conflict example

    When the source operand src and destination operand dst of a Vector instruction read from and write to the same bank at the same time, a read-write conflict occurs. The detailed analysis is as follows:

    **Table 1**  Read-write conflict example

    <a name="table178973521409"></a>
    <table><thead align="left"><tr id="row20897752164015"><th class="cellrowborder" valign="top" width="5.9988002399520095%" id="mcps1.2.7.1.1"><p id="p13897195244019"><a name="p13897195244019"></a><a name="p13897195244019"></a>Sequence Number</p>
    </th>
    <th class="cellrowborder" valign="top" width="9.948010397920417%" id="mcps1.2.7.1.2"><p id="p3897155224010"><a name="p3897155224010"></a><a name="p3897155224010"></a>src Address</p>
    </th>
    <th class="cellrowborder" valign="top" width="10.507898420315936%" id="mcps1.2.7.1.3"><p id="p13897252164011"><a name="p13897252164011"></a><a name="p13897252164011"></a>dst Address</p>
    </th>
    <th class="cellrowborder" valign="top" width="25.51489702059588%" id="mcps1.2.7.1.4"><p id="p138976528405"><a name="p138976528405"></a><a name="p138976528405"></a>bank</p>
    </th>
    <th class="cellrowborder" valign="top" width="31.36372725454909%" id="mcps1.2.7.1.5"><p id="p689795214408"><a name="p689795214408"></a><a name="p689795214408"></a>bank group</p>
    </th>
    <th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.2.7.1.6"><p id="p98976528403"><a name="p98976528403"></a><a name="p98976528403"></a>Conclusion</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row10897145224010"><td class="cellrowborder" valign="top" width="5.9988002399520095%" headers="mcps1.2.7.1.1 "><p id="p8897185254012"><a name="p8897185254012"></a><a name="p8897185254012"></a>Example 1</p>
    </td>
    <td class="cellrowborder" valign="top" width="9.948010397920417%" headers="mcps1.2.7.1.2 "><p id="p12897155215406"><a name="p12897155215406"></a><a name="p12897155215406"></a>0x10020</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.507898420315936%" headers="mcps1.2.7.1.3 "><p id="p19897165213403"><a name="p19897165213403"></a><a name="p19897165213403"></a>0x10000</p>
    </td>
    <td class="cellrowborder" valign="top" width="25.51489702059588%" headers="mcps1.2.7.1.4 "><p id="p1489705220408"><a name="p1489705220408"></a><a name="p1489705220408"></a><span>bank_id0 != </span><span>bank_id</span><span>1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="31.36372725454909%" headers="mcps1.2.7.1.5 "><p id="p4897155264016"><a name="p4897155264016"></a><a name="p4897155264016"></a><span>bank_group_id0 != bank_group_id1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.6 "><p id="p1589719525406"><a name="p1589719525406"></a><a name="p1589719525406"></a>The src address and dst address belong to bank0 and bank1 respectively, so there is no conflict.</p>
    </td>
    </tr>
    <tr id="row1489775264016"><td class="cellrowborder" valign="top" width="5.9988002399520095%" headers="mcps1.2.7.1.1 "><p id="p12897152184019"><a name="p12897152184019"></a><a name="p12897152184019"></a>Example 2</p>
    </td>
    <td class="cellrowborder" valign="top" width="9.948010397920417%" headers="mcps1.2.7.1.2 "><p id="p108981152174015"><a name="p108981152174015"></a><a name="p108981152174015"></a>0x10020</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.507898420315936%" headers="mcps1.2.7.1.3 "><p id="p1389811520408"><a name="p1389811520408"></a><a name="p1389811520408"></a>0x10120</p>
    </td>
    <td class="cellrowborder" valign="top" width="25.51489702059588%" headers="mcps1.2.7.1.4 "><p id="p389885274014"><a name="p389885274014"></a><a name="p389885274014"></a><span>bank_id0 </span><span>== </span><span>bank_id1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="31.36372725454909%" headers="mcps1.2.7.1.5 "><p id="p7898155214011"><a name="p7898155214011"></a><a name="p7898155214011"></a><span>bank_group_id0 =</span><span>= </span><span>bank_group_id1</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.6 "><p id="p989815214018"><a name="p989815214018"></a><a name="p989815214018"></a>The src address and dst address are both in bank0, so there is a conflict.</p>
    </td>
    </tr>
    </tbody>
    </table>

-   Write-write conflict example

    When the eight DataBlocks (block0 to block7) corresponding to the destination operand dst of a Vector instruction are written to the same bank group simultaneously, a write-write conflict occurs. The detailed analysis is as follows:

    **Table 2**  Write-write conflict example

    <a name="table15913191235615"></a>
    <table><thead align="left"><tr id="row891371215561"><th class="cellrowborder" valign="top" width="5.789726356216995%" id="mcps1.2.9.1.1"><p id="p1913412195619"><a name="p1913412195619"></a><a name="p1913412195619"></a>Sequence number</p>
    </th>
    <th class="cellrowborder" valign="top" width="7.959673547767644%" id="mcps1.2.9.1.2"><p id="p19133129560"><a name="p19133129560"></a><a name="p19133129560"></a>dst address</p>
    </th>
    <th class="cellrowborder" valign="top" width="9.505520883341337%" id="mcps1.2.9.1.3"><p id="p991371215560"><a name="p991371215560"></a><a name="p991371215560"></a>blk_stride</p>
    </th>
    <th class="cellrowborder" valign="top" width="8.660585693710996%" id="mcps1.2.9.1.4"><p id="p10913171219568"><a name="p10913171219568"></a><a name="p10913171219568"></a><span>block0_addr </span></p>
    </th>
    <th class="cellrowborder" valign="top" width="8.775804128660585%" id="mcps1.2.9.1.5"><p id="p29131712145616"><a name="p29131712145616"></a><a name="p29131712145616"></a><span>block1_addr </span></p>
    </th>
    <th class="cellrowborder" valign="top" width="8.967834853576573%" id="mcps1.2.9.1.6"><p id="p4913151218567"><a name="p4913151218567"></a><a name="p4913151218567"></a><span>block2_addr </span></p>
    </th>
    <th class="cellrowborder" valign="top" width="6.721075372059531%" id="mcps1.2.9.1.7"><p id="p109131112115617"><a name="p109131112115617"></a><a name="p109131112115617"></a>...</p>
    </th>
    <th class="cellrowborder" valign="top" width="43.61977916466635%" id="mcps1.2.9.1.8"><p id="p491301211565"><a name="p491301211565"></a><a name="p491301211565"></a>Conclusion</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row2091314128569"><td class="cellrowborder" valign="top" width="5.789726356216995%" headers="mcps1.2.9.1.1 "><p id="p8913151214562"><a name="p8913151214562"></a><a name="p8913151214562"></a>Example 1</p>
    </td>
    <td class="cellrowborder" valign="top" width="7.959673547767644%" headers="mcps1.2.9.1.2 "><p id="p129131512125612"><a name="p129131512125612"></a><a name="p129131512125612"></a>0x10000</p>
    </td>
    <td class="cellrowborder" valign="top" width="9.505520883341337%" headers="mcps1.2.9.1.3 "><p id="p3913191218564"><a name="p3913191218564"></a><a name="p3913191218564"></a>8</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.660585693710996%" headers="mcps1.2.9.1.4 "><p id="p1191321220560"><a name="p1191321220560"></a><a name="p1191321220560"></a>0x10000</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.775804128660585%" headers="mcps1.2.9.1.5 "><p id="p109132012115614"><a name="p109132012115614"></a><a name="p109132012115614"></a>0x10100</p>
    </td>
    <td class="cellrowborder" valign="top" width="8.967834853576573%" headers="mcps1.2.9.1.6 "><p id="p1891371218569"><a name="p1891371218569"></a><a name="p1891371218569"></a>0x10200</p>
    </td>
    <td class="cellrowborder" valign="top" width="6.721075372059531%" headers="mcps1.2.9.1.7 "><p id="p1791371217565"><a name="p1791371217565"></a><a name="p1791371217565"></a>...</p>
    </td>
    <td class="cellrowborder" valign="top" width="43.61977916466635%" headers="mcps1.2.9.1.8 "><p id="p1691311124564"><a name="p1691311124564"></a><a name="p1691311124564"></a>All eight DataBlocks are in the same bank group, so they all conflict. It takes eight cycles to complete the write of one Repeat.</p>
    </td>
    </tr>
    </tbody>
    </table>

-   Read-read conflict
    -   When two source operands of a Vector instruction are read from the same bank at the same time, a read-read conflict occurs. The detailed analysis is as follows:

        **Table 3**  Example of read-read conflict in dual-src scenario

        <a name="table983101761813"></a>
        <table><thead align="left"><tr id="row7834178187"><th class="cellrowborder" valign="top" width="6.5786842631473705%" id="mcps1.2.7.1.1"><p id="p1283121713188"><a name="p1283121713188"></a><a name="p1283121713188"></a>Sequence Number</p>
        </th>
        <th class="cellrowborder" valign="top" width="12.76744651069786%" id="mcps1.2.7.1.2"><p id="p583117141810"><a name="p583117141810"></a><a name="p583117141810"></a>src0 Address</p>
        </th>
        <th class="cellrowborder" valign="top" width="14.097180563887221%" id="mcps1.2.7.1.3"><p id="p198312171181"><a name="p198312171181"></a><a name="p198312171181"></a>src1 Address</p>
        </th>
        <th class="cellrowborder" valign="top" width="18.52629474105179%" id="mcps1.2.7.1.4"><p id="p683917111815"><a name="p683917111815"></a><a name="p683917111815"></a>bank</p>
        </th>
        <th class="cellrowborder" valign="top" width="31.36372725454909%" id="mcps1.2.7.1.5"><p id="p1583151719187"><a name="p1583151719187"></a><a name="p1583151719187"></a>bank group</p>
        </th>
        <th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.2.7.1.6"><p id="p883171712184"><a name="p883171712184"></a><a name="p883171712184"></a>Conclusion</p>
        </th>
        </tr>
        </thead>
        <tbody><tr id="row1683121715185"><td class="cellrowborder" valign="top" width="6.5786842631473705%" headers="mcps1.2.7.1.1 "><p id="p13839177184"><a name="p13839177184"></a><a name="p13839177184"></a>Example 1</p>
        </td>
        <td class="cellrowborder" valign="top" width="12.76744651069786%" headers="mcps1.2.7.1.2 "><p id="p583121718188"><a name="p583121718188"></a><a name="p583121718188"></a>0x10000</p>
        </td>
        <td class="cellrowborder" valign="top" width="14.097180563887221%" headers="mcps1.2.7.1.3 "><p id="p198321717189"><a name="p198321717189"></a><a name="p198321717189"></a>0x10100</p>
        </td>
        <td class="cellrowborder" valign="top" width="18.52629474105179%" headers="mcps1.2.7.1.4 "><p id="p2836174181"><a name="p2836174181"></a><a name="p2836174181"></a><span>bank_id0 == </span><span>bank_id</span><span>1</span></p>
        </td>
        <td class="cellrowborder" valign="top" width="31.36372725454909%" headers="mcps1.2.7.1.5 "><p id="p8831178183"><a name="p8831178183"></a><a name="p8831178183"></a><span>bank_group_id0 == bank_group_id1</span></p>
        </td>
        <td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.6 "><p id="p1841179186"><a name="p1841179186"></a><a name="p1841179186"></a>A conflict exists.</p>
        </td>
        </tr>
        <tr id="row1484151719181"><td class="cellrowborder" valign="top" width="6.5786842631473705%" headers="mcps1.2.7.1.1 "><p id="p1084417181817"><a name="p1084417181817"></a><a name="p1084417181817"></a>Example 2</p>
        </td>
        <td class="cellrowborder" valign="top" width="12.76744651069786%" headers="mcps1.2.7.1.2 "><p id="p10849172181"><a name="p10849172181"></a><a name="p10849172181"></a>0x10000</p>
        </td>
        <td class="cellrowborder" valign="top" width="14.097180563887221%" headers="mcps1.2.7.1.3 "><p id="p1284131701810"><a name="p1284131701810"></a><a name="p1284131701810"></a>0x10020</p>
        </td>
        <td class="cellrowborder" valign="top" width="18.52629474105179%" headers="mcps1.2.7.1.4 "><p id="p12849176181"><a name="p12849176181"></a><a name="p12849176181"></a><span>bank_id0 </span><span>!= </span><span>bank_id1</span></p>
        </td>
        <td class="cellrowborder" valign="top" width="31.36372725454909%" headers="mcps1.2.7.1.5 "><p id="p784181761810"><a name="p784181761810"></a><a name="p784181761810"></a><span>bank_group_id0 !</span><span>= </span><span>bank_group_id1</span></p>
        </td>
        <td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.2.7.1.6 "><p id="p1684121791810"><a name="p1684121791810"></a><a name="p1684121791810"></a>No conflict.</p>
        </td>
        </tr>
        </tbody>
        </table>

    -   When the eight DataBlocks (block0-block7) corresponding to a source operand of a Vector instruction are read from the same bank, a read-read conflict occurs. The detailed analysis is as follows:

        **Table 4**  Example of read-read conflict in a single-src scenario

        <a name="table1055918277182"></a>
        <table><thead align="left"><tr id="row05601327161818"><th class="cellrowborder" valign="top" width="6.117919521374119%" id="mcps1.2.9.1.1"><p id="p856062714185"><a name="p856062714185"></a><a name="p856062714185"></a>Sequence Number</p>
        </th>
        <th class="cellrowborder" valign="top" width="11.560358969410403%" id="mcps1.2.9.1.2"><p id="p556016272186"><a name="p556016272186"></a><a name="p556016272186"></a>src Address</p>
        </th>
        <th class="cellrowborder" valign="top" width="9.910257647399403%" id="mcps1.2.9.1.3"><p id="p1560127181817"><a name="p1560127181817"></a><a name="p1560127181817"></a>blk_stride</p>
        </th>
        <th class="cellrowborder" valign="top" width="10.798031458071987%" id="mcps1.2.9.1.4"><p id="p165607278181"><a name="p165607278181"></a><a name="p165607278181"></a><span>block0_addr </span></p>
        </th>
        <th class="cellrowborder" valign="top" width="10.633986297404228%" id="mcps1.2.9.1.5"><p id="p856022717184"><a name="p856022717184"></a><a name="p856022717184"></a><span>block1_addr </span></p>
        </th>
        <th class="cellrowborder" valign="top" width="9.234777574061564%" id="mcps1.2.9.1.6"><p id="p11560327181814"><a name="p11560327181814"></a><a name="p11560327181814"></a><span>block2_addr </span></p>
        </th>
        <th class="cellrowborder" valign="top" width="6.754800733378366%" id="mcps1.2.9.1.7"><p id="p175601827191811"><a name="p175601827191811"></a><a name="p175601827191811"></a>...</p>
        </th>
        <th class="cellrowborder" valign="top" width="34.98986779889993%" id="mcps1.2.9.1.8"><p id="p9560327191810"><a name="p9560327191810"></a><a name="p9560327191810"></a>Conclusion</p>
        </th>
        </tr>
        </thead>
        <tbody><tr id="row1456042701815"><td class="cellrowborder" valign="top" width="6.117919521374119%" headers="mcps1.2.9.1.1 "><p id="p656052771820"><a name="p656052771820"></a><a name="p656052771820"></a>Example 1</p>
        </td>
        <td class="cellrowborder" valign="top" width="11.560358969410403%" headers="mcps1.2.9.1.2 "><p id="p1256016274187"><a name="p1256016274187"></a><a name="p1256016274187"></a>0x10000</p>
        </td>
        <td class="cellrowborder" valign="top" width="9.910257647399403%" headers="mcps1.2.9.1.3 "><p id="p1456022741810"><a name="p1456022741810"></a><a name="p1456022741810"></a>8</p>
        </td>
        <td class="cellrowborder" valign="top" width="10.798031458071987%" headers="mcps1.2.9.1.4 "><p id="p1956032761819"><a name="p1956032761819"></a><a name="p1956032761819"></a>0x10000</p>
        </td>
        <td class="cellrowborder" valign="top" width="10.633986297404228%" headers="mcps1.2.9.1.5 "><p id="p125607279187"><a name="p125607279187"></a><a name="p125607279187"></a>0x10100</p>
        </td>
        <td class="cellrowborder" valign="top" width="9.234777574061564%" headers="mcps1.2.9.1.6 "><p id="p1560152716187"><a name="p1560152716187"></a><a name="p1560152716187"></a>0x10200</p>
        </td>
        <td class="cellrowborder" valign="top" width="6.754800733378366%" headers="mcps1.2.9.1.7 "><p id="p11560202719180"><a name="p11560202719180"></a><a name="p11560202719180"></a>...</p>
        </td>
        <td class="cellrowborder" valign="top" width="34.98986779889993%" headers="mcps1.2.9.1.8 "><p id="p156052714183"><a name="p156052714183"></a><a name="p156052714183"></a>All 8 <span>DataBlock</span>s are in the same bank, so they all conflict, and the read operation of one Repeat takes 8 cycles.</p>
        </td>
        </tr>
        </tbody>
        </table>

## How to Avoid Bank Conflicts<a name="section12501642143515"></a>

There are two methods to avoid bank conflicts: **optimizing the computation logic** and **optimizing the address allocation**.

-   **Optimizing the computation logic**

    Add 1 to each element of an input whose data type is float and shape is \(16, 64\). By changing the computation logic from column-by-column computation to row-by-row computation, the conflict issue under the same Repeat can be avoided. The implementation schemes are compared as follows:

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
    <td class="cellrowborder" valign="top" width="42.6652389759564%" headers="mcps1.1.4.1.2 "><p id="p115401884217"><a name="p115401884217"></a><a name="p115401884217"></a>Column-by-column computation. The eight DataBlocks of the input within the same Repeat are all in the same bank, causing a read-read conflict.</p>
    </td>
    <td class="cellrowborder" valign="top" width="50.520782634089365%" headers="mcps1.1.4.1.3 "><p id="p6641340414"><a name="p6641340414"></a><a name="p6641340414"></a>Calculation is performed row by row. The eight input DataBlocks within the same Repeat are not in the same bank, which avoids read-read conflicts within the same Repeat.</p>
    </td>
    </tr>
    <tr id="row14922142214585"><td class="cellrowborder" valign="top" width="6.813978389954251%" headers="mcps1.1.4.1.1 "><p id="p6922182210587"><a name="p6922182210587"></a><a name="p6922182210587"></a>Schematic diagram</p>
    </td>
    <td class="cellrowborder" valign="top" width="42.6652389759564%" headers="mcps1.1.4.1.2 "><p id="p1273333434818"><a name="p1273333434818"></a><a name="p1273333434818"></a><a name="image47338343486"></a><a name="image47338343486"></a><span><img class="eddx" id="image47338343486" src="../../../../figures/matrix_programming_logical_position_diagram_73.png"></span></p>
    </td>
    <td class="cellrowborder" valign="top" width="50.520782634089365%" headers="mcps1.1.4.1.3 "><p id="p1074101154916"><a name="p1074101154916"></a><a name="p1074101154916"></a><a name="image147416115491"></a><a name="image147416115491"></a><span><img class="eddx" id="image147416115491" src="../../../../figures/matrix_programming_logical_position_diagram_74.png"></span></p>
    </td>
    </tr>
    <tr id="row3293124918559"><td class="cellrowborder" valign="top" width="6.813978389954251%" headers="mcps1.1.4.1.1 "><p id="p12812993573"><a name="p12812993573"></a><a name="p12812993573"></a>Example code</p>
    </td>
    <td class="cellrowborder" valign="top" width="42.6652389759564%" headers="mcps1.1.4.1.2 "><a name="screen924835613570"></a><a name="screen924835613570"></a><pre class="screen" codetype="Cpp" id="screen924835613570">uint64_t mask = 64;
    AscendC::UnaryRepeatParams params;
    params.dstBlkStride = 8;
    params.srcBlkStride = 8;
    for(uint16_t i = 0; i &lt; 8; ++i){
        AscendC::Adds(dst[i * 8], src[i * 8], 1, mask, 1, params);
    }</pre>
    </td>
    <td class="cellrowborder" valign="top" width="50.520782634089365%" headers="mcps1.1.4.1.3 "><a name="screen271414925813"></a><a name="screen271414925813"></a><pre class="screen" codetype="Cpp" id="screen271414925813">uint64_t mask = 64;
    AscendC::UnaryRepeatParams params;
    params.dstBlkStride = 1;
    params.srcBlkStride = 1;
    for(uint16_t i = 0; i &lt; 8; ++i){
        AscendC::Adds(dst[i * 64], src[i * 64], 1, mask, 1, params);
    }</pre>
    </td>
    </tr>
    </tbody>
    </table>

-   **Optimizing address allocation**

    To implement the addition z = x + y of 4096 consecutive float elements, expand the memory appropriately during memory allocation to ensure that within a Repeat, x/y and z do not appear in the same bank at the same time.

    The implementation schemes are compared as follows:

    <a name="table82050585313"></a>
    <table><thead align="left"><tr id="row1320525823115"><th class="cellrowborder" valign="top" width="6.813978389954251%" id="mcps1.1.4.1.1"><p id="p6205125853117"><a name="p6205125853117"></a><a name="p6205125853117"></a>Implementation Scheme</p>
    </th>
    <th class="cellrowborder" valign="top" width="42.01304390148935%" id="mcps1.1.4.1.2"><p id="p920565815314"><a name="p920565815314"></a><a name="p920565815314"></a>Original Implementation</p>
    </th>
    <th class="cellrowborder" valign="top" width="51.17297770855641%" id="mcps1.1.4.1.3"><p id="p10205105819318"><a name="p10205105819318"></a><a name="p10205105819318"></a>Optimized Implementation</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row1220513586316"><td class="cellrowborder" valign="top" width="6.813978389954251%" headers="mcps1.1.4.1.1 "><p id="p320516586313"><a name="p320516586313"></a><a name="p320516586313"></a>Implementation Method</p>
    </td>
    <td class="cellrowborder" valign="top" width="42.01304390148935%" headers="mcps1.1.4.1.2 "><p id="p5205155873113"><a name="p5205155873113"></a><a name="p5205155873113"></a>No address optimization is performed. InitBuffer is directly used to allocate memory, and the addresses of the tensors are as follows:</p>
    <p id="p120535813115"><a name="p120535813115"></a><a name="p120535813115"></a>x: start address 0x00000, tensor length is 4096 * sizeof(float) bytes</p>
    <p id="p11205125814318"><a name="p11205125814318"></a><a name="p11205125814318"></a>y: start address 0x04000, tensor length is 4096 * sizeof(float) bytes</p>
    <p id="p420545893110"><a name="p420545893110"></a><a name="p420545893110"></a>z: start address 0x08000, tensor length is 4096 * sizeof(float) bytes</p>
    <p id="p10205175810318"><a name="p10205175810318"></a><a name="p10205175810318"></a>Within one Repeat, x and y read the same bank group simultaneously, and x/y and z read and write the same bank simultaneously.</p>
    </td>
    <td class="cellrowborder" valign="top" width="51.17297770855641%" headers="mcps1.1.4.1.3 "><p id="p1720555893117"><a name="p1720555893117"></a><a name="p1720555893117"></a>Optimize the addresses. When using InitBuffer to allocate memory, appropriately expand the memory request. The addresses of the tensors are as follows:</p>
    <p id="p1205105810316"><a name="p1205105810316"></a><a name="p1205105810316"></a>x: start address 0x00000, tensor length is 4096 * sizeof(float) bytes</p>
    <p id="p420575820318"><a name="p420575820318"></a><a name="p420575820318"></a>y: start address 0x04000, tensor length is (8 * 16 * 1024 - (4096 * sizeof(float)) bytes</p>
    <p id="p120525813116"><a name="p120525813116"></a><a name="p120525813116"></a>z: start address 0x20000, tensor length is 4096 * sizeof(float) bytes</p>
    <p id="p162053589314"><a name="p162053589314"></a><a name="p162053589314"></a>y requests additional space to ensure that z does not fall into the same bank as x/y.</p>
    </td>
    </tr>
    <tr id="row92051958113118"><td class="cellrowborder" valign="top" width="6.813978389954251%" headers="mcps1.1.4.1.1 "><p id="p820614589313"><a name="p820614589313"></a><a name="p820614589313"></a>Schematic Diagram</p>
    </td>
    <td class="cellrowborder" valign="top" width="42.01304390148935%" headers="mcps1.1.4.1.2 "><p id="p11206958143119"><a name="p11206958143119"></a><a name="p11206958143119"></a><a name="image17957162514335"></a><a name="image17957162514335"></a><span><img class="eddx" id="image17957162514335" src="../../../../figures/matrix_programming_logical_position_diagram_75.png"></span></p>
    </td>
    <td class="cellrowborder" valign="top" width="51.17297770855641%" headers="mcps1.1.4.1.3 "><p id="p1979513571429"><a name="p1979513571429"></a><a name="p1979513571429"></a><a name="image6795145717421"></a><a name="image6795145717421"></a><span><img class="eddx" id="image6795145717421" src="../../../../figures/matrix_programming_logical_position_diagram_76.png"></span></p>
    </td>
    </tr>
    <tr id="row1520625893111"><td class="cellrowborder" valign="top" width="6.813978389954251%" headers="mcps1.1.4.1.1 "><p id="p22061858183118"><a name="p22061858183118"></a><a name="p22061858183118"></a>Example code</p>
    </td>
    <td class="cellrowborder" valign="top" width="42.01304390148935%" headers="mcps1.1.4.1.2 "><a name="screen62061858143118"></a><a name="screen62061858143118"></a><pre class="screen" codetype="Cpp" id="screen62061858143118">pipe.InitBuffer(inQueueX, 1, 4096 * sizeof(float));
    pipe.InitBuffer(inQueueY, 1, 4096 * sizeof(float));
    pipe.InitBuffer(outQueueZ, 1, 4096 * sizeof(float));</pre>
    </td>
    <td class="cellrowborder" valign="top" width="51.17297770855641%" headers="mcps1.1.4.1.3 "><a name="screen7206858113117"></a><a name="screen7206858113117"></a><pre class="screen" codetype="Cpp" id="screen7206858113117">constexpr int32_t TOTAL_LENGTH = 1024 * 4;
    constexpr int32_t BUFFER_NUM = 1;
    constexpr int32_t BANKGROUP_SIZE  =  1024 * 128; 
    ...
    pipe.InitBuffer(inQueueX, BUFFER_NUM, TOTAL_LENGTH * sizeof(float));
    pipe.InitBuffer(inQueueY, BUFFER_NUM, BANKGROUP_SIZE - TOTAL_LENGTH * sizeof(float));
    pipe.InitBuffer(outQueueZ, BUFFER_NUM, TOTAL_LENGTH * sizeof(float));</pre>
    </td>
    </tr>
    </tbody>
    </table>

