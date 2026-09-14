# Overview<a name="ZH-CN_TOPIC_0000001644105018"></a>

<!-- md-trans-meta sourceCommit=e4a5905334db2bef02e668063a0b99d64a2e7709 translatedAt=2026-08-26T10:55:57.161Z pushedAt=2026-08-31T07:18:31.367Z -->

The overall solution for debugging Ascend C operators is as follows: developers write the kernel-side source code of Ascend C operators by calling the Ascend C library. The kernel-side source code is compiled by the general-purpose GCC compiler to generate a general-purpose CPU-domain binary, which can be debugged through debugging methods such as the gdb general-purpose debugging tool. The kernel-side source code is compiled by the BiSheng Compiler to generate an NPU-domain binary file, which can be debugged by printing data through APIs such as **printf**/**assert**, or by collecting on-board data through the simulation dot graph or the Profiling tool.

![](../../figures/twin_debugging_introduction.png)

The specific debugging and tuning methods and the tools used are listed as follows:

**Table 1** Debugging and tuning methods and used tools

<a name="table3879116815"></a>
<table><thead align="left"><tr id="row128713114816"><th class="cellrowborder" valign="top" width="10.41104110411041%" id="mcps1.2.4.1.1"><p id="p98701186"><a name="p98701186"></a><a name="p98701186"></a>Category</p></th>
<th class="cellrowborder" valign="top" width="15.541554155415543%" id="mcps1.2.4.1.2"><p id="p16871313811"><a name="p16871313811"></a><a name="p16871313811"></a>Subcategory</p></th>
<th class="cellrowborder" valign="top" width="74.04740474047404%" id="mcps1.2.4.1.3"><p id="p58718116817"><a name="p58718116817"></a><a name="p58718116817"></a>Method</p></th>
</tr>
</thead>
<tbody><tr id="row138718119810"><td class="cellrowborder" rowspan="5" valign="top" width="10.41104110411041%" headers="mcps1.2.4.1.1 "><p id="p1287011382"><a name="p1287011382"></a><a name="p1287011382"></a>Function debugging</p></td>
<td class="cellrowborder" valign="top" width="15.541554155415543%" headers="mcps1.2.4.1.2 "><p id="p13871711885"><a name="p13871711885"></a><a name="p13871711885"></a>CPU domain twin debugging</p></td>
<td class="cellrowborder" valign="top" width="74.04740474047404%" headers="mcps1.2.4.1.3 "><p id="p23711516144012"><a name="p23711516144012"></a><a name="p23711516144012"></a><strong id="b811813562418"><a name="b811813562418"></a><a name="b811813562418"></a>Twin debugging</strong>: The same operator code can be used to debug precision in the CPU domain and performance in the NPU domain. In the CPU domain, you can perform gdb debugging and use the printf command to print information.</p></td>
</tr>
<tr id="row18871813811"><td class="cellrowborder" rowspan="4" valign="top" headers="mcps1.2.4.1.1 "><p id="p58791484"><a name="p58791484"></a><a name="p58791484"></a>NPU domain on-board debugging</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p14289333164011"><a name="p14289333164011"></a><a name="p14289333164011"></a><strong id="b3677614417"><a name="b3677614417"></a><a name="b3677614417"></a>printf/assert:</strong> printf is mainly used to print scalar and string information. assert is mainly used to set checkpoints in the code. When a condition is not met, the program terminates immediately and reports an error.</p></td>
</tr>
<tr id="row1187912816"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p13192834104014"><a name="p13192834104014"></a><a name="p13192834104014"></a><strong id="b17916121054718"><a name="b17916121054718"></a><a name="b17916121054718"></a>DumpTensor:</strong> Uses the DumpTensor API to print the data of a specified tensor. Only the SIMD programming scenario is supported.</p></td>
</tr>
<tr id="row158820118819"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p16881511783"><a name="p16881511783"></a><a name="p16881511783"></a><strong id="b95903509441"><a name="b95903509441"></a><a name="b95903509441"></a>On-board debugging tool:</strong> Uses the msDebug tool to debug operator programs running on the NPU. In a real hardware environment, it tests the inputs and outputs of an operator to verify whether the operator functions correctly. Specific functions include breakpoint setting, variable and memory printing, single-step debugging, and interrupt execution. The SIMT programming scenario is not supported currently.</p></td>
</tr>
<tr id="row488111481"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1444213362401"><a name="p1444213362401"></a><a name="p1444213362401"></a><strong id="b15632154718442"><a name="b15632154718442"></a><a name="b15632154718442"></a>Memory detection tool:</strong> Uses the msSanitizer tool for memory detection. It can detect and report memory access exceptions such as out-of-bounds and misaligned access to external memory (Global Memory) and internal memory (Local Memory) during operator execution.</p></td>
</tr>
<tr id="row788672010105"><td class="cellrowborder" valign="top" width="10.41104110411041%" headers="mcps1.2.4.1.1 "><p id="p2886162041013"><a name="p2886162041013"></a><a name="p2886162041013"></a>Performance tuning</p></td>
<td class="cellrowborder" valign="top" width="15.541554155415543%" headers="mcps1.2.4.1.2 "><p id="p14886720191015"><a name="p14886720191015"></a><a name="p14886720191015"></a>-</p></td>
<td class="cellrowborder" valign="top" width="74.04740474047404%" headers="mcps1.2.4.1.3 "><p id="p78861320181012"><a name="p78861320181012"></a><a name="p78861320181012"></a><strong id="b146353213493"><a name="b146353213493"></a><a name="b146353213493"></a>msOpProf tool:</strong></p>
<p id="zh-cn_topic_0000001740005733_p1488261312578"><a name="zh-cn_topic_0000001740005733_p1488261312578"></a><a name="zh-cn_topic_0000001740005733_p1488261312578"></a>The msOpProf tool is used to collect and analyze key performance metrics of operators running on the <span id="zh-cn_topic_0000001740005733_ph09541259133611"><a name="zh-cn_topic_0000001740005733_ph09541259133611"></a><a name="zh-cn_topic_0000001740005733_ph09541259133611"></a>AI processor</span>. Based on the output performance data, users can quickly locate the software and hardware performance bottlenecks of operators and improve the efficiency of operator performance analysis.</p>
<p id="zh-cn_topic_0000001740005733_p8060118"><a name="zh-cn_topic_0000001740005733_p8060118"></a><a name="zh-cn_topic_0000001740005733_p8060118"></a>Currently, it supports the collection and automatic parsing of performance data based on different running modes (on-board or simulation) and different file forms (executable files or operator binary <strong id="b8915828144915"><a name="b8915828144915"></a><a name="b8915828144915"></a>.o</strong> files).</p></td>
</tr>
</tbody>
</table>
