# Glossary<a name="ZH-CN_TOPIC_0000002191896252"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T12:33:14.819Z -->

**Table 1** Glossary

<a name="table276611752817"></a>
<table><thead align="left"><tr id="row117661277289"><th class="cellrowborder" valign="top" width="22.978822978822976%" id="mcps1.2.3.1.1"><p id="p17766117132820"><a name="p17766117132820"></a><a name="p17766117132820"></a>Term/Abbreviation</p></th>
<th class="cellrowborder" valign="top" width="77.02117702117702%" id="mcps1.2.3.1.2"><p id="p47664714282"><a name="p47664714282"></a><a name="p47664714282"></a>Meaning</p></th>
</tr>
</thead>
<tbody><tr id="row1076614772815"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p14388182105119"><a name="p14388182105119"></a><a name="p14388182105119"></a>A1</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p0363142875116"><a name="p0363142875116"></a><a name="p0363142875116"></a>AscendC::TPosition::A1 represents the logical memory on the device used for matrix computation, which stores the left matrix. Its physical storage corresponds to the L1 Buffer of the AI Core.</p></td>
</tr>
<tr id="row076714714285"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p3389132115116"><a name="p3389132115116"></a><a name="p3389132115116"></a>A2</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p7363152855111"><a name="p7363152855111"></a><a name="p7363152855111"></a>AscendC::TPosition::A2 represents the logical memory on the device used for matrix computation, which stores the small-block left matrix (for example, a block split and adapted to the L0A Buffer capacity). Its physical storage corresponds to the L0A Buffer of the AI Core.</p></td>
</tr>
<tr id="row476113137588"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p234721565817"><a name="p234721565817"></a><a name="p234721565817"></a>AddrReg</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p834731545816"><a name="p834731545816"></a><a name="p834731545816"></a>Address Register, a register used to store address offsets.</p></td>
</tr>
<tr id="row167671713280"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p16389425519"><a name="p16389425519"></a><a name="p16389425519"></a>AI Core</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p236318282514"><a name="p236318282514"></a><a name="p236318282514"></a><span id="ph19321164217255"><a name="ph19321164217255"></a><a name="ph19321164217255"></a>AI processor</span> compute core, responsible for executing matrix- and vector-computation-intensive tasks.</p></td>
</tr>
<tr id="row1878118319442"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p14781535441"><a name="p14781535441"></a><a name="p14781535441"></a>AIC</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p14781123184410"><a name="p14781123184410"></a><a name="p14781123184410"></a>In AI Core separation mode, the Cube Core in a Cube Core and Vector Core combination.</p></td>
</tr>
<tr id="row16828135184419"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p2829165164416"><a name="p2829165164416"></a><a name="p2829165164416"></a>AIV</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p18296514411"><a name="p18296514411"></a><a name="p18296514411"></a>In AI Core separation mode, the Vector Core in a Cube Core and Vector Core combination.</p></td>
</tr>
<tr id="row14591183171120"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p149411830163412"><a name="p149411830163412"></a><a name="p149411830163412"></a>Ascend IR</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p18278153423413"><a name="p18278153423413"></a><a name="p18278153423413"></a>Ascend Intermediate Representation, an abstract data structure dedicated to the <span id="ph3793171771119"><a name="ph3793171771119"></a><a name="ph3793171771119"></a>AI processor</span> for expressing computation flows. In this document, unless otherwise specified, IR refers to Ascend IR by default.</p></td>
</tr>
<tr id="row197673719288"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p1438916212517"><a name="p1438916212517"></a><a name="p1438916212517"></a>B1</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p83631628115112"><a name="p83631628115112"></a><a name="p83631628115112"></a>AscendC::TPosition::B1 represents the logical memory on the device used for Cube computation, which stores the right matrix. Its physical storage corresponds to the L1 Buffer of the AI Core.</p></td>
</tr>
<tr id="row157676716283"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p83893219511"><a name="p83893219511"></a><a name="p83893219511"></a>B2</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p1036320282517"><a name="p1036320282517"></a><a name="p1036320282517"></a>AscendC::TPosition::B2 represents the logical memory on the device used for Cube computation, which stores a small block of the right matrix (for example, a block obtained after splitting and adapted to the L0B Buffer capacity). Its physical storage corresponds to the L0B Buffer of the AI Core.</p></td>
</tr>
<tr id="row1535792642912"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p83891722513"><a name="p83891722513"></a><a name="p83891722513"></a>Block</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p19363182810519"><a name="p19363182810519"></a><a name="p19363182810519"></a>Block has multiple meanings in different scenarios. Generally, it refers to the logical core of the AI Core. Typical scenarios are as follows:</p>
<a name="ul1171135516436"></a><a name="ul1171135516436"></a><ul id="ul1171135516436"><li>AI Core logical core: A Block represents a logical core of the AI Core, and its BlockID is a logical number starting from 0.</li><li>DataBlock: A DataBlock represents the data unit processed by one NPU vector computation instruction. Its size is usually 32 bytes, and one instruction can process multiple DataBlocks at the same time.</li><li>Basic block: Represents the typical data block size required for one computation.</li></ul></td>
</tr>
<tr id="row573717544294"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p9389929516"><a name="p9389929516"></a><a name="p9389929516"></a>BlockID</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p3363152814511"><a name="p3363152814511"></a><a name="p3363152814511"></a>The logical number of the AI Core starting from 0. It can be greater than the actual number of hardware cores.</p></td>
</tr>
<tr id="row1029785613291"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p113895215115"><a name="p113895215115"></a><a name="p113895215115"></a>numBlocks</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p3363122814513"><a name="p3363122814513"></a><a name="p3363122814513"></a>The number of logical AI Core cores involved in the computation. It is specified by the developer when calling the kernel function, and its value is generally equal to or greater than the actual number of physical cores.</p></td>
</tr>
<tr id="row6977143384316"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p193897255119"><a name="p193897255119"></a><a name="p193897255119"></a>BiasTable Buffer</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p16363828195111"><a name="p16363828195111"></a><a name="p16363828195111"></a>Bias storage, a physical storage unit inside the AI Core, typically used to store the bias data required for Cube computation. It corresponds to the logical memory AscendC::TPosition::C2.</p></td>
</tr>
<tr id="row21839410462"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p17392132125120"><a name="p17392132125120"></a><a name="p17392132125120"></a>Broadcast</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p1336662865110"><a name="p1336662865110"></a><a name="p1336662865110"></a>Broadcast, a tensor operation mechanism. Through broadcasting, a smaller tensor can be automatically expanded to match the shape of a larger tensor.</p></td>
</tr>
<tr id="row233554054812"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p183891219518"><a name="p183891219518"></a><a name="p183891219518"></a>C1</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p1536342817515"><a name="p1536342817515"></a><a name="p1536342817515"></a>AscendC::TPosition::C1 represents the logical memory on the device used for Cube computation, used to store bias data. Its physical storage corresponds to the L1 Buffer or Unified Buffer of the AI Core.</p></td>
</tr>
<tr id="row8118833105319"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p03899218513"><a name="p03899218513"></a><a name="p03899218513"></a>C2</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p43631128155117"><a name="p43631128155117"></a><a name="p43631128155117"></a>AscendC::TPosition::C2 represents the logical memory on the device used for Cube computation, used to store small blocks of bias data (for example, blocks split and adapted to the BT Buffer capacity). Its physical storage corresponds to the BT Buffer or L0C Buffer of the AI Core.</p></td>
</tr>
<tr id="row9352180102618"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p13897219515"><a name="p13897219515"></a><a name="p13897219515"></a>C2PIPE2GM</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p11363328205112"><a name="p11363328205112"></a><a name="p11363328205112"></a>AscendC::TPosition::C2PIPE2GM represents the logical memory on the device used for Cube computation, used to store quantization parameters. Its physical storage corresponds to the Fixpipe Buffer of the AI Core.</p></td>
</tr>
<tr id="row188451714278"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p984591162710"><a name="p984591162710"></a><a name="p984591162710"></a>Cache Line</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p5845111192711"><a name="p5845111192711"></a><a name="p5845111192711"></a><span>The smallest data unit in the cache (DCache, ICache, and L2 Cache).</span></p></td>
</tr>
<tr id="row134761151194914"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p747615119496"><a name="p747615119496"></a><a name="p747615119496"></a>Core</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p647645118493"><a name="p647645118493"></a><a name="p647645118493"></a>A compute core with an independent Scalar Compute unit. The Scalar Compute unit handles functions such as instruction issue within the core, and is also called the scheduling unit within the core.</p></td>
</tr>
<tr id="row1883165882917"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p438914216513"><a name="p438914216513"></a><a name="p438914216513"></a>CO1</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p1136332885113"><a name="p1136332885113"></a><a name="p1136332885113"></a>AscendC::TPosition::CO1 represents the logical memory on the device used for Cube computation, and is used to store small-block matrix computation results (such as the blocks of a partitioned matrix computation result). Its physical storage corresponds to the L0C Buffer of the AI Core.</p></td>
</tr>
<tr id="row17964259192913"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p123893216511"><a name="p123893216511"></a><a name="p123893216511"></a>CO2</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p2363122813516"><a name="p2363122813516"></a><a name="p2363122813516"></a>AscendC::TPosition::CO2 represents the logical memory on the device used for Cube computation, and is used to store matrix computation results (such as the final computation result of the original matrix). Its physical storage corresponds to Global Memory or the Unified Buffer of the AI Core.</p></td>
</tr>
<tr id="row11948134812470"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p1739332105114"><a name="p1739332105114"></a><a name="p1739332105114"></a>Compute</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p103671328135113"><a name="p103671328135113"></a><a name="p103671328135113"></a>One of the three typical stages in the Ascend C operator programming paradigm, responsible for completing the computation task.</p></td>
</tr>
<tr id="row28884323015"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p638917214510"><a name="p638917214510"></a><a name="p638917214510"></a>CopyIn</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p15363928125117"><a name="p15363928125117"></a><a name="p15363928125117"></a>One of the three typical stages in the Ascend C operator programming paradigm, responsible for moving the data to be computed from Global Memory to Local Memory.</p></td>
</tr>
<tr id="row5866759309"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p83891127511"><a name="p83891127511"></a><a name="p83891127511"></a>CopyOut</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p936319285516"><a name="p936319285516"></a><a name="p936319285516"></a>One of the three typical stages in the Ascend C operator programming paradigm, responsible for moving the computation results from Local Memory to Global Memory.</p></td>
</tr>
<tr id="row279412713304"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p238916214518"><a name="p238916214518"></a><a name="p238916214518"></a>Core ID</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p936319287518"><a name="p936319287518"></a><a name="p936319287518"></a>The physical number of an AI Core, which corresponds one-to-one with the actual number of hardware cores.</p></td>
</tr>
<tr id="row1873713181358"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p1038918214519"><a name="p1038918214519"></a><a name="p1038918214519"></a>Cube</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p63631028135120"><a name="p63631028135120"></a><a name="p63631028135120"></a>The Cube Compute unit on the AI Core, responsible for executing matrix operations. Taking the float16 data type as an example, each Cube execution can complete the multiplication of two 16x16 float16 matrices.</p></td>
</tr>
<tr id="row42601047115019"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p167952883310"><a name="p167952883310"></a><a name="p167952883310"></a>Cube Core</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p379518810338"><a name="p379518810338"></a><a name="p379518810338"></a>A Cube computation core dedicated to Cube computation. It consists of the Scalar scheduling unit, Cube computation unit, data transfer unit, and others, and does not include the vector computation unit.</p></td>
</tr>
<tr id="row12254185094916"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p7389327518"><a name="p7389327518"></a><a name="p7389327518"></a><span id="ph519472813129"><a name="ph519472813129"></a><a name="ph519472813129"></a>DataBlock</span></p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p2363182819512"><a name="p2363182819512"></a><a name="p2363182819512"></a>The data unit processed by a vector computation instruction. Its size is usually 32 bytes. A single execution of a vector computation instruction can process multiple DataBlocks simultaneously.</p></td>
</tr>
<tr id="row15687144135113"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p43893265112"><a name="p43893265112"></a><a name="p43893265112"></a><span id="ph28101837141319"><a name="ph28101837141319"></a><a name="ph28101837141319"></a>DataBlock Stride</span></p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p436452875113"><a name="p436452875113"></a><a name="p436452875113"></a>The interval between DataBlocks within a single Repeat of a vector computation instruction, that is, the number of DataBlocks between the start data address of the next processing and the start data address of the current processing.</p></td>
</tr>
<tr id="row820213103529"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p1138914214512"><a name="p1138914214512"></a><a name="p1138914214512"></a><span id="ph968401071411"><a name="ph968401071411"></a><a name="ph968401071411"></a>DCache</span></p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p33641628185111"><a name="p33641628185111"></a><a name="p33641628185111"></a>Data Cache.</p>
<p id="p16885155718240"><a name="p16885155718240"></a><a name="p16885155718240"></a>Used to cache data segments that the Scalar Compute unit may repeatedly access in the near future, so as to improve access efficiency.</p></td>
</tr>
<tr id="row151691317115211"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p63903225111"><a name="p63903225111"></a><a name="p63903225111"></a>Device</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p73641728195117"><a name="p73641728195117"></a><a name="p73641728195117"></a>A Device refers to the hardware device on which the Ascend AI processor is installed. It connects to the host side through a PCIe interface and provides neural network computation capabilities for the host. If multiple Devices exist, the memory resources of the multiple Devices cannot be shared.</p></td>
</tr>
<tr id="row03541811362"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p74101612165"><a name="p74101612165"></a><a name="p74101612165"></a>Dim3</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p184107121360"><a name="p184107121360"></a><a name="p184107121360"></a>Dim3<span> is a data type used to define the three-dimensional thread structure in a single thread block.</span>Use AscendC::Simt::Dim3 to specify the values of the three dimensions when starting SIMT VF. The product of the three dimensions must be less than or equal to 2048.</p></td>
</tr>
<tr id="row563612835210"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p1939019214512"><a name="p1939019214512"></a><a name="p1939019214512"></a>DMA</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p936482813514"><a name="p936482813514"></a><a name="p936482813514"></a>Direct Memory Access, a direct memory access unit.</p>
<p id="p12364828145113"><a name="p12364828145113"></a><a name="p12364828145113"></a>Responsible for data transfer, including data transfer between Global Memory and Local Memory and data transfer between Local Memory at different levels, and contains transfer units such as MTE2 and MTE3.</p></td>
</tr>
<tr id="row102651096566"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p33906275114"><a name="p33906275114"></a><a name="p33906275114"></a>DoubleBuffer/DB</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p1436412820518"><a name="p1436412820518"></a><a name="p1436412820518"></a>Double buffering, a common optimization method in the parallel field, which improves the parallelism of data processing by creating multiple buffers that hold data.</p></td>
</tr>
<tr id="row12134356195014"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p153921829516"><a name="p153921829516"></a><a name="p153921829516"></a>Elementwise</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p16366102818511"><a name="p16366102818511"></a><a name="p16366102818511"></a>An element-wise operation is an operation performed independently on each element of a tensor. The result of each element depends only on the corresponding input element.</p></td>
</tr>
<tr id="row656842083115"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p139015219516"><a name="p139015219516"></a><a name="p139015219516"></a>Fixpipe</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p1736412815511"><a name="p1736412815511"></a><a name="p1736412815511"></a>A unit in the AI Core that transfers the Cube computation results from the L0C Buffer to Global Memory or the L1 Buffer, and performs operations such as quantization and activation along the way during the transfer.</p></td>
</tr>
<tr id="row1939121952620"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p1439018219518"><a name="p1439018219518"></a><a name="p1439018219518"></a>Fixpipe Buffer</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p1236412285516"><a name="p1236412285516"></a><a name="p1236412285516"></a>A physical storage unit inside the AI Core, typically used to store data such as quantization parameters required during Fixpipe transfer, corresponding to the logical memory AscendC::TPosition::C2PIPE2GM.</p></td>
</tr>
<tr id="row1548192912206"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p1439018225112"><a name="p1439018225112"></a><a name="p1439018225112"></a>Global Memory/GM</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p836432813513"><a name="p836432813513"></a><a name="p836432813513"></a>The main memory on the device side, which is the external storage of the AI Core. It is used to store large-scale data, but the access pattern needs to be optimized to improve performance.</p></td>
</tr>
<tr id="row146147528542"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p924411179552"><a name="p924411179552"></a><a name="p924411179552"></a>GlobalTensor</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p1024421712551"><a name="p1024421712551"></a><a name="p1024421712551"></a>A tensor that stores global data in Global Memory.</p></td>
</tr>
<tr id="row69311325201"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p73900215115"><a name="p73900215115"></a><a name="p73900215115"></a>Host</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p4364162819519"><a name="p4364162819519"></a><a name="p4364162819519"></a>Refers to the X86 server or ARM server connected to the device side. It uses the NN (Neural-Network) computing capability provided by the Device to complete services.</p></td>
</tr>
<tr id="row4443202662915"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p13390927519"><a name="p13390927519"></a><a name="p13390927519"></a>ICache</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p436492875119"><a name="p436492875119"></a><a name="p436492875119"></a>Instruction Cache.</p>
<p id="p636442816517"><a name="p636442816517"></a><a name="p636442816517"></a>Used to cache recently or frequently used instructions. During extreme performance optimization, attention must be paid to reducing ICache misses.</p></td>
</tr>
<tr id="row1435321083018"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p339013245117"><a name="p339013245117"></a><a name="p339013245117"></a>InferShape</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p2364122855112"><a name="p2364122855112"></a><a name="p2364122855112"></a>Operator shape inference, which is used only in GE graph mode. During the actual network model generation, the tensor shape and datatype are inferred first. In this way, the data type and shape of each tensor can be known before graph execution, and their correctness can be verified in advance. At the same time, the output tensor description of the operator, including the tensor shape, data type, and data layout format, is inferred in advance. Therefore, memory can be statically allocated for all tensors during the operator graph construction preparation phase, avoiding the overhead of dynamic memory allocation.</p></td>
<tr id="row2010"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p2010a"><a name="p2010a"></a><a name="p2010a"></a>ISASI</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p2010b"><a name="p2010b"></a><a name="p2010b"></a>Instruction Set Architecture Special Interface (hardware architecture-related interface). This type of interface does not guarantee compatibility across hardware versions.</p></td>
</tr>
</tr>
<tr id="row179123019146"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p1639032195115"><a name="p1639032195115"></a><a name="p1639032195115"></a>Kernel</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p14364182813519"><a name="p14364182813519"></a><a name="p14364182813519"></a>A kernel function is a parallel function executed on a Device. Kernel functions are modified by __global__, and multiple kernels execute the same kernel function in parallel. The main difference is that different kernel functions have different BlockIDs at runtime.</p></td>
</tr>
<tr id="row34231332175613"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p614234735610"><a name="p614234735610"></a><a name="p614234735610"></a>Kernel Launch</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p1814254735611"><a name="p1814254735611"></a><a name="p1814254735611"></a>The process of submitting a kernel program to the hardware for startup and execution.</p></td>
</tr>
<tr id="row8135612193319"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p9390122165118"><a name="p9390122165118"></a><a name="p9390122165118"></a>L0A Buffer</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p1136472835113"><a name="p1136472835113"></a><a name="p1136472835113"></a>A physical storage unit inside the AI Core, typically used to store the left matrix of Cube computation, corresponding to the logical memory AscendC::TPosition::A2.</p></td>
</tr>
<tr id="row7277111220337"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p15390127512"><a name="p15390127512"></a><a name="p15390127512"></a>L0B Buffer</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p18364328105110"><a name="p18364328105110"></a><a name="p18364328105110"></a>A physical storage unit inside the AI Core, typically used to store the right matrix of Cube computation, corresponding to the logical memory AscendC::TPosition::B2.</p></td>
</tr>
<tr id="row1642151219334"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p1439092115117"><a name="p1439092115117"></a><a name="p1439092115117"></a>L0C Buffer</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p83644289519"><a name="p83644289519"></a><a name="p83644289519"></a>A physical storage unit inside the AI Core, typically used to store the results of Cube computation. It corresponds to the logical memory AscendC::TPosition::CO1.</p></td>
</tr>
<tr id="row20555141263319"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p133905218518"><a name="p133905218518"></a><a name="p133905218518"></a>L1 Buffer</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p1836472812518"><a name="p1836472812518"></a><a name="p1836472812518"></a>A physical storage unit inside the AI Core with a relatively large space, typically used to cache the input activation of Cube computation. The input of Cube computation generally needs to be moved from GM to L1 Buffer, and then moved to L0A Buffer and L0B Buffer respectively. L1 Buffer corresponds to the logical memory AscendC::TPosition::A1 and AscendC::TPosition::B1.</p></td>
</tr>
<tr id="row6690141203319"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p039012215517"><a name="p039012215517"></a><a name="p039012215517"></a>L2 Cache</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p2364528135110"><a name="p2364528135110"></a><a name="p2364528135110"></a>A level-2 cache dedicated to storing frequently accessed data to reduce reads and writes to Global Memory.</p></td>
</tr>
<tr id="row137585435619"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p116271956965"><a name="p116271956965"></a><a name="p116271956965"></a>Lane</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p0627175617615"><a name="p0627175617615"></a><a name="p0627175617615"></a>Each thread in a Warp is called a Lane.</p></td>
</tr>
<tr id="row1825111273311"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p23901024519"><a name="p23901024519"></a><a name="p23901024519"></a>LCM</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p736462845114"><a name="p736462845114"></a><a name="p736462845114"></a>Local Cache Memory. AscendC::TPosition::LCM represents a temporarily shared Unified Buffer space and implements the same function as VECCALC.</p></td>
</tr>
<tr id="row11628103863017"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p193901420518"><a name="p193901420518"></a><a name="p193901420518"></a>Local Memory</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p1436492835113"><a name="p1436492835113"></a><a name="p1436492835113"></a>Internal storage of the AI Core, including storage units such as L1 Buffer, L0A Buffer, L0B Buffer, L0C Buffer, and Unified Buffer.</p></td>
</tr>
<tr id="row887110515115"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p63939216514"><a name="p63939216514"></a><a name="p63939216514"></a>LocalTensor</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p2036742812516"><a name="p2036742812516"></a><a name="p2036742812516"></a>A Tensor that stores local data in the Local Memory of the AI Core.</p></td>
</tr>
<tr id="row1648494123011"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p4390102165119"><a name="p4390102165119"></a><a name="p4390102165119"></a><span id="ph20434165721418"><a name="ph20434165721418"></a><a name="ph20434165721418"></a>Mask</span></p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p836502819511"><a name="p836502819511"></a><a name="p836502819511"></a>Controls the elements that participate in computation within each Repeat of a vector computation instruction. It can be set in continuous mode or bit-by-bit mode.</p></td>
</tr>
<tr id="row1946012254571"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p17150629105717"><a name="p17150629105717"></a><a name="p17150629105717"></a>MaskReg</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p54310520429"><a name="p54310520429"></a><a name="p54310520429"></a><span>A mask register, which is a 256-bit register used to indicate which elements of a RegTensor participate in computation.</span></p></td>
</tr>
<tr id="row11578133217591"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p2245734145919"><a name="p2245734145919"></a><a name="p2245734145919"></a>Membase</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p8245123418590"><a name="p8245123418590"></a><a name="p8245123418590"></a>Membase refers to a memory-based architecture. In the Membase architecture, all operations are performed based on memory, which means that each computation requires loading data from Local Memory, moving the results back to Local Memory after the computation is complete, and temporarily storing all intermediate computation results in Local Memory.</p></td>
</tr>
<tr id="row1183124313018"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p339019220518"><a name="p339019220518"></a><a name="p339019220518"></a>MTE1</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p2365728115117"><a name="p2365728115117"></a><a name="p2365728115117"></a>Memory Transfer Engine 1, the data transfer engine of AI Core, is responsible for moving data from L1 Buffer to L0A Buffer or L0B Buffer. Note: Capabilities may vary depending on the hardware.</p></td>
</tr>
<tr id="row16717617876"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p53901325511"><a name="p53901325511"></a><a name="p53901325511"></a>MTE2</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p73651928205114"><a name="p73651928205114"></a><a name="p73651928205114"></a>Memory Transfer Engine 2, the data transfer engine of AI Core, is responsible for moving data from GM to L1 Buffer, L0A Buffer, L0B Buffer, Unified Buffer, and so on. Note: Capabilities may vary depending on the hardware.</p></td>
</tr>
<tr id="row112041123578"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p183901322511"><a name="p183901322511"></a><a name="p183901322511"></a>MTE3</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p203651128105117"><a name="p203651128105117"></a><a name="p203651128105117"></a>Memory Transfer Engine 3, the data transfer engine of AI Core, is responsible for moving data from Unified Buffer to Global Memory, L1 Buffer, and so on. Note: Capabilities may vary depending on the hardware.</p></td>
</tr>
<tr id="row682759181911"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p039117217510"><a name="p039117217510"></a><a name="p039117217510"></a>NC1HWC0</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p16365202815111"><a name="p16365202815111"></a><a name="p16365202815111"></a>A five-dimensional data format in which C0 is strongly related to the hardware architecture. Using this format can improve the computation efficiency of matrix multiplication.</p></td>
</tr>
<tr id="row144071518151914"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p43911229510"><a name="p43911229510"></a><a name="p43911229510"></a>NCHW</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p1136552885119"><a name="p1136552885119"></a><a name="p1136552885119"></a>Stores feature map data in the order of [Batch, Channels, Height, Width].</p></td>
</tr>
<tr id="row160511120201"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p18391922519"><a name="p18391922519"></a><a name="p18391922519"></a>ND</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p163651228205110"><a name="p163651228205110"></a><a name="p163651228205110"></a>Normal format, N-dimensional tensor.</p></td>
</tr>
<tr id="row121101049142113"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p13391162155120"><a name="p13391162155120"></a><a name="p13391162155120"></a>NHWC</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p19365152817516"><a name="p19365152817516"></a><a name="p19365152817516"></a>Stores feature map data in the order of [Batch, Height, Width, Channels].</p></td>
</tr>
<tr id="row1523465132213"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p163913255117"><a name="p163913255117"></a><a name="p163913255117"></a>NPU</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p536512288518"><a name="p536512288518"></a><a name="p536512288518"></a>Neural-Network Processing Unit. It adopts a "data-driven parallel computing" architecture and is dedicated to processing a large number of computation tasks in AI applications.</p></td>
</tr>
<tr id="row18798193253419"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p139111235116"><a name="p139111235116"></a><a name="p139111235116"></a>OP</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p12355171320142"><a name="p12355171320142"></a><a name="p12355171320142"></a><span id="ph11733205010219"><a name="ph11733205010219"></a><a name="ph11733205010219"></a>Operator (OP for short), the basic unit that performs a specific mathematical operation in deep learning algorithms, such as activation functions (for example, ReLU), convolution (Conv), pooling, and normalization (for example, Softmax). A neural network model can be built by combining these operators.</span></p></td>
</tr>
<tr id="row1367214344341"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p2039132115120"><a name="p2039132115120"></a><a name="p2039132115120"></a>OpType</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p1336532855110"><a name="p1336532855110"></a><a name="p1336532855110"></a>Operator type, a general term for a class of operators. For example, a network may contain multiple Add operators named Add1 and Add2, but the OpType of these operators is Add.</p></td>
</tr>
<tr id="row9853173793419"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p1339122115117"><a name="p1339122115117"></a><a name="p1339122115117"></a>Pipe</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p736512865115"><a name="p736512865115"></a><a name="p736512865115"></a>One of the core concepts of the Ascend C programming paradigm. It is used to uniformly manage resources such as Device-side memory. A Kernel function must initialize exactly one Pipe object.</p></td>
</tr>
<tr id="row296512384350"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p83911328515"><a name="p83911328515"></a><a name="p83911328515"></a>Preload</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p12365192815518"><a name="p12365192815518"></a><a name="p12365192815518"></a>Before a computation task starts, loads the necessary instructions or data into the cache in advance to reduce the latency of instruction or data access and improve computation efficiency.</p></td>
</tr>
<tr id="row8661714840"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p203927210513"><a name="p203927210513"></a><a name="p203927210513"></a>Reduce</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p236772855119"><a name="p236772855119"></a><a name="p236772855119"></a>A dimension reduction operation used to reduce the dimensions of a multi-dimensional tensor. Common dimension reduction operations include sum, average, maximum, and minimum.</p></td>
</tr>
<tr id="row1877813214458"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p1749733654515"><a name="p1749733654515"></a><a name="p1749733654515"></a>RegTensor</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p15497113614513"><a name="p15497113614513"></a><a name="p15497113614513"></a>A vector data register, which is the basic unit of Reg vector computation.</p></td>
</tr>
<tr id="row4658910165520"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p177001414195518"><a name="p177001414195518"></a><a name="p177001414195518"></a>Regbase</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p670014143555"><a name="p670014143555"></a><a name="p670014143555"></a>Regbase refers to a register-based architecture. Under the Regbase architecture, intermediate results can be temporarily stored in registers, eliminating the overhead of moving data out to Local Memory.</p></td>
</tr>
<tr id="row2348241193510"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p93911215511"><a name="p93911215511"></a><a name="p93911215511"></a>Repeat</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p13365162816519"><a name="p13365162816519"></a><a name="p13365162816519"></a>When a vector computation instruction is executed once, eight DataBlocks are read for computation, which is called an iteration (Repeat). Typically, the instruction needs to be executed multiple times in a loop to complete the reading and computation of all data.</p></td>
</tr>
<tr id="row10793143918422"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p7391102185115"><a name="p7391102185115"></a><a name="p7391102185115"></a><span id="ph298011116157"><a name="ph298011116157"></a><a name="ph298011116157"></a>Repeat Stride</span></p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p153651028175110"><a name="p153651028175110"></a><a name="p153651028175110"></a>When a vector computation instruction is executed in a loop, the number of DataBlocks between the start data address of the next Repeat and the start data address of the current Repeat.</p></td>
</tr>
<tr id="row17437417428"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p53913295119"><a name="p53913295119"></a><a name="p53913295119"></a><span id="ph947954111517"><a name="ph947954111517"></a><a name="ph947954111517"></a>Repeat Times</span></p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p14365162814515"><a name="p14365162814515"></a><a name="p14365162814515"></a>The number of times a vector computation instruction is executed in a loop.</p></td>
</tr>
<tr id="row7497125215425"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p1739112219516"><a name="p1739112219516"></a><a name="p1739112219516"></a>Scalar</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p536582815514"><a name="p536582815514"></a><a name="p536582815514"></a>The scalar computation unit on the AI Core, which is mainly responsible for scalar data operations and instruction dispatch to other units (such as the MTE data transfer unit, Vector computation unit, and Cube computation unit).</p></td>
</tr>
<tr id="row042117534180"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p578205931815"><a name="p578205931815"></a><a name="p578205931815"></a>SIMD</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p1978959171815"><a name="p1978959171815"></a><a name="p1978959171815"></a>SIMD: Single Instruction, Multiple Data. A single instruction performs the same operation on multiple data elements simultaneously.</p></td>
</tr>
<tr id="row192274569188"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p16781759101812"><a name="p16781759101812"></a><a name="p16781759101812"></a>SIMT</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p1678145918188"><a name="p1678145918188"></a><a name="p1678145918188"></a>SIMT: Single Instruction, Multiple Threads. A single instruction is executed in parallel by multiple threads, with each thread processing different data.</p></td>
</tr>
<tr id="row918145672910"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p203911727517"><a name="p203911727517"></a><a name="p203911727517"></a>SPMD</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p11365428135119"><a name="p11365428135119"></a><a name="p11365428135119"></a>Single-Program Multiple-Data. A parallel programming model whose core idea is to execute the same program in parallel on multiple cores, with each core processing different data.</p></td>
</tr>
<tr id="row10731203883213"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p146755313210"><a name="p146755313210"></a><a name="p146755313210"></a>SSBuffer</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p46765383220"><a name="p46765383220"></a><a name="p46765383220"></a>A physical storage unit inside the AI Core. In separation mode, Cube Core and Vector Core can use SSBuffer for 1:2 communication between the Cube Core and the Scalar unit of the Vector Core.</p></td>
</tr>
<tr id="row146061856161511"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p1192714320165"><a name="p1192714320165"></a><a name="p1192714320165"></a>SuperKernel</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p5927133215168"><a name="p5927133215168"></a><a name="p5927133215168"></a>SuperKernel is a binary fusion technology for operators. Unlike source code fusion, it focuses on the binary scheduling scheme of kernel functions and performs in-depth optimization. Based on compiled binary code, it fuses and creates a super kernel function (SuperKernel), which invokes multiple other kernel functions (sub-kernels) as sub-functions. Compared with dispatching a single operator, SuperKernel reduces task scheduling wait time and scheduling overhead, and further optimizes operator header overhead by utilizing idle resources between tasks.</p></td>
</tr>
<tr id="row164027130332"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p0391924511"><a name="p0391924511"></a><a name="p0391924511"></a>Tensor</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p11365162845114"><a name="p11365162845114"></a><a name="p11365162845114"></a>A tensor is a container for operator computation data. It is an N-dimensional data structure, the most common of which are scalars, vectors, or matrices. The elements of a tensor can contain integer values, floating-point values, or string values.</p></td>
</tr>
<tr id="row83614614434"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p122491541124316"><a name="p122491541124316"></a><a name="p122491541124316"></a>Thread Block</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p192491541204311"><a name="p192491541204311"></a><a name="p192491541204311"></a>A thread block supports a maximum of 2048 threads. SIMT VF runs one thread block (Block) on an AI Core at a time.</p></td>
</tr>
<tr id="row1368731323316"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p1839114215515"><a name="p1839114215515"></a><a name="p1839114215515"></a>Tiling</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p20365122835111"><a name="p20365122835111"></a><a name="p20365122835111"></a>Tiling refers to the splitting and blocking of data. When the amount of data to be computed is large, the data needs to be split across multiple cores, and each core also needs to compute in blocks multiple times.</p></td>
</tr>
<tr id="row88301113203316"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p239116225114"><a name="p239116225114"></a><a name="p239116225114"></a>TilingData</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p0366202835113"><a name="p0366202835113"></a><a name="p0366202835113"></a>TilingData refers to the parameters related to data splitting and blocking (such as the block size of each transfer and the number of loops). Given the limited Scalar computation capability on the device side, Tiling parameters are generally computed on the Host side and then transferred to the device side for use by the Kernel function.</p></td>
</tr>
<tr id="row174292016362"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p183911529514"><a name="p183911529514"></a><a name="p183911529514"></a>TilingFunc</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p2366228115118"><a name="p2366228115118"></a><a name="p2366228115118"></a>The default function provided by the operator project for computing Tiling on the Host side.</p></td>
</tr>
<tr id="row296561343316"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p63920210519"><a name="p63920210519"></a><a name="p63920210519"></a>TilingKey</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p4366102865120"><a name="p4366102865120"></a><a name="p4366102865120"></a>Used to distinguish special implementations of different versions of the Kernel function. Different TilingKeys are compiled into different binaries.</p></td>
</tr>
<tr id="row166912537816"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p2427951919"><a name="p2427951919"></a><a name="p2427951919"></a>TPosition</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p2427151997"><a name="p2427151997"></a><a name="p2427151997"></a>When managing physical memory at different levels, Ascend C uses an abstract logical position (TPosition) to represent storage at each level, replacing the concept of on-chip physical storage to hide the hardware architecture. TPosition types include VECIN, VECOUT, VECCALC, A1, A2, B1, B2, CO1, CO2, and so on. Among them, VECIN, VECCALC, and VECOUT are mainly used for vector programming, while A1, A2, B1, B2, CO1, and CO2 are used for matrix programming.</p></td>
</tr>
<tr id="row10122151493319"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p1392229512"><a name="p1392229512"></a><a name="p1392229512"></a>TSCM</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p153165032012"><a name="p153165032012"></a><a name="p153165032012"></a>AscendC::TPosition::TSCM represents the logical memory corresponding to the L1 Buffer space. Developers need to manage it themselves to efficiently utilize hardware resources. It is mainly used for Matmul computation. For example, developers can cache a copy of TSCM data and flexibly configure it as the A matrix, B matrix, or Bias matrix of a Matmul operation in different usage scenarios, thereby achieving memory reuse and computation efficiency optimization.</p></td>
</tr>
<tr id="row2199154185916"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p183881676591"><a name="p183881676591"></a><a name="p183881676591"></a>UnalignRegForLoad</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p63881878597"><a name="p63881878597"></a><a name="p63881878597"></a>An unaligned register used as a buffer to optimize the overhead of continuous unaligned address access between UB and RegTensor. It applies to continuous unaligned load-in scenarios.</p></td>
</tr>
<tr id="row11318565912"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p13388117145918"><a name="p13388117145918"></a><a name="p13388117145918"></a>UnalignRegForStore</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p838817725912"><a name="p838817725912"></a><a name="p838817725912"></a>An unaligned register used as a buffer to optimize the overhead of continuous unaligned address access between UB and RegTensor. It applies to continuous unaligned load-out scenarios.</p></td>
</tr>
<tr id="row13357140113713"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p17392122195118"><a name="p17392122195118"></a><a name="p17392122195118"></a>Unified Buffer/UB</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p936692819517"><a name="p936692819517"></a><a name="p936692819517"></a>An internal storage unit of the AI Core, mainly used for vector computation. It corresponds to the logical memory AscendC::TPosition::VECIN, AscendC::TPosition::VECOUT, and AscendC::TPosition::VECCALC.</p></td>
</tr>
<tr id="row354602815226"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p23927295115"><a name="p23927295115"></a><a name="p23927295115"></a>VECCALC</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p236617287516"><a name="p236617287516"></a><a name="p236617287516"></a>Vector Calculation, AscendC::TPosition::VECCALC represents the logical memory on the device used for vector computation, used to store temporary variables. Its physical storage corresponds to the Unified Buffer of the AI Core.</p></td>
</tr>
<tr id="row5265131443316"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p3392192155118"><a name="p3392192155118"></a><a name="p3392192155118"></a>VECIN</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p536692845113"><a name="p536692845113"></a><a name="p536692845113"></a>Vector Input, AscendC::TPosition::VECIN represents the logical memory on the device used for vector computation, used to store the input data of vector computation. Its physical storage corresponds to the Unified Buffer of the AI Core.</p></td>
</tr>
<tr id="row11418214173314"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p17392182195114"><a name="p17392182195114"></a><a name="p17392182195114"></a>VECOUT</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p173668282513"><a name="p173668282513"></a><a name="p173668282513"></a>Vector Output, AscendC::TPosition::VECOUT represents the logical memory on the device used for vector computation, used to store the output data of vector computation. Its physical storage corresponds to the Unified Buffer of the AI Core.</p></td>
</tr>
<tr id="row895317489610"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p203923211517"><a name="p203923211517"></a><a name="p203923211517"></a>Vector</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p536613280517"><a name="p536613280517"></a><a name="p536613280517"></a>The Vector Compute unit on the AI Core, responsible for executing vector operations. Its computing power is lower than that of the Cube, but its flexibility is higher than that of the Cube (for example, it supports mathematical operations such as reciprocal and square root).</p></td>
</tr>
<tr id="row20565514143314"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p18392142145120"><a name="p18392142145120"></a><a name="p18392142145120"></a>Vector Core</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p3245121495210"><a name="p3245121495210"></a><a name="p3245121495210"></a>A vector computation core that focuses on vector computation. It consists of the Scalar scheduling unit, vector Compute unit, and data transfer unit, and does not include the Cube computation unit.</p></td>
</tr>
<tr id="row1124815485717"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p127871649478"><a name="p127871649478"></a><a name="p127871649478"></a>VF</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p107871249378"><a name="p107871249378"></a><a name="p107871249378"></a>A Vector Function is a group of consecutive vector instructions called by a scalar compute unit, marked with __simt_vf__ or __simd_vf__.</p></td>
</tr>
<tr id="row20305163910554"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p1576518408558"><a name="p1576518408558"></a><a name="p1576518408558"></a>VL</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p1676564017553"><a name="p1676564017553"></a><a name="p1676564017553"></a>Vector Length, the bit width of RegTensor, typically 256 bytes.</p></td>
</tr>
<tr id="row204821946194411"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p2701104724419"><a name="p2701104724419"></a><a name="p2701104724419"></a>Warp</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p470114734418"><a name="p470114734418"></a><a name="p470114734418"></a>Each thread block is divided into multiple warps. A warp is a set of threads that execute the same instruction, and each warp contains 32 threads. Multiple warps of a block are scheduled in sequence to the same AIV core in the AI Core for execution.</p></td>
</tr>
<tr id="row1987221443311"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p1939220235111"><a name="p1939220235111"></a><a name="p1939220235111"></a>Workspace</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p14366628165116"><a name="p14366628165116"></a><a name="p14366628165116"></a>Generally refers to a pre-allocated, temporarily used Global Memory area for storing intermediate results or temporary data.</p></td>
</tr>
<tr id="row6896278492"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p93891429517"><a name="p93891429517"></a><a name="p93891429517"></a>CPU domain debugging</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p1236310283512"><a name="p1236310283512"></a><a name="p1236310283512"></a>A twin debugging method provided by Ascend C that simulates the execution and debugging of the device-side Kernel function on the CPU, used only for debugging the functionality and precision of the operator.</p></td>
</tr>
<tr id="row134674514426"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p73891426518"><a name="p73891426518"></a><a name="p73891426518"></a>Basic block</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p33638285516"><a name="p33638285516"></a><a name="p33638285516"></a>Typical data block size required for a single computation.</p></td>
</tr>
<tr id="row10594182515423"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p1472173404214"><a name="p1472173404214"></a><a name="p1472173404214"></a>Static Tensor programming</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p672534174219"><a name="p672534174219"></a><a name="p672534174219"></a>Compared with the Pipe-based programming approach, the static Tensor programming approach avoids the TPipe memory management initialization process (about hundreds of nanoseconds), thereby reducing runtime overhead and helping developers achieve ultimate performance. It provides higher flexibility by directly constructing a LocalTensor with a specified address and storage location and passing it to APIs such as computation and data movement for programming.</p></td>
</tr>
<tr id="row1490079161018"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p53938211517"><a name="p53938211517"></a><a name="p53938211517"></a>Kernel direct invocation</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p163671728125117"><a name="p163671728125117"></a><a name="p163671728125117"></a>A simple and direct way to invoke a Kernel.</p>
<p id="p13676289512"><a name="p13676289512"></a><a name="p13676289512"></a>After the Kernel-side operator implementation and the Host-side Tiling implementation are completed, the operator Kernel can be directly invoked through the runtime APIs. In this approach, Tiling development is not restricted by the CANN framework, making it simple and direct, and it is mostly used for rapid verification of operator functions.</p></td>
</tr>
<tr id="row144191142323"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p239115255119"><a name="p239115255119"></a><a name="p239115255119"></a>NPU domain debugging</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p13651928175111"><a name="p13651928175111"></a><a name="p13651928175111"></a>A twin debugging method provided by Ascend C, which refers to debugging based on NPU simulation software or NPU hardware.</p></td>
</tr>
<tr id="row6660193119114"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p24291240121113"><a name="p24291240121113"></a><a name="p24291240121113"></a>Tiling offloading</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p14195143652317"><a name="p14195143652317"></a><a name="p14195143652317"></a>Tiling offloading refers to offloading Tiling computation to the AI CPU on the Device side for execution, so that the entire computation process is efficiently completed on the Device side.</p></td>
</tr>
<tr id="row5262142310553"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p16262172385511"><a name="p16262172385511"></a><a name="p16262172385511"></a>Separation mode</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p5262923145516"><a name="p5262923145516"></a><a name="p5262923145516"></a>A working mode of AI Core in which the Cube computation unit and the vector computation unit are scheduled by independent Scalar scheduling units and deployed separately on the Cube Core and Vector Core. Cube Cores and Vector Cores are combined at a certain ratio (1:N), and such a combination is regarded as one AI Core. The number of AI Core cores is determined by the Cube Cores.</p></td>
</tr>
<tr id="row1493915201218"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p839210212512"><a name="p839210212512"></a><a name="p839210212512"></a>Twin debugging</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p1336619286512"><a name="p1336619286512"></a><a name="p1336619286512"></a>An operator debugging method provided by Ascend C, which supports precision debugging in the CPU domain and precision/performance debugging in the NPU domain.</p></td>
</tr>
<tr id="row15874154915120"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p83923215110"><a name="p83923215110"></a><a name="p83923215110"></a>Pipeline task</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p936614282513"><a name="p936614282513"></a><a name="p936614282513"></a>The Ascend C programming paradigm is a pipelined programming paradigm that divides the processing program in an operator core into multiple pipeline tasks. A pipeline task refers to a parallel task scheduled by the main program in a single-core processing program. Within a kernel function, pipeline tasks can be used to implement parallel data processing and further improve performance.</p></td>
</tr>
<tr id="row715815113320"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p143925216518"><a name="p143925216518"></a><a name="p143925216518"></a>Continuous mode</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p7366628145115"><a name="p7366628145115"></a><a name="p7366628145115"></a>One of the selectable modes when using Mask to control the elements participating in computation in each Repeat of vector computation. It indicates that the preceding consecutive elements participate in the computation.</p></td>
</tr>
<tr id="row0600555135513"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p1160035565519"><a name="p1160035565519"></a><a name="p1160035565519"></a>Coupled mode</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p18600855195510"><a name="p18600855195510"></a><a name="p18600855195510"></a>A working mode of AI Core in which a single Scalar scheduling unit schedules both the Cube computation unit and the vector computation unit, and all units are deployed on one AI Core.</p></td>
</tr>
<tr id="row745021593315"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p123921027513"><a name="p123921027513"></a><a name="p123921027513"></a>Fusion operator</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p1536610285518"><a name="p1536610285518"></a><a name="p1536610285518"></a>A fusion operator is formed by fusing multiple independent small operators. It is functionally equivalent to the small operators but usually delivers better performance. You can freely fuse Vector and Cube operators based on specific algorithms according to your actual service scenarios to achieve performance gains.</p></td>
</tr>
<tr id="row1960081563312"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p2039215275110"><a name="p2039215275110"></a><a name="p2039215275110"></a>Operator graph integration</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p1836642816510"><a name="p1836642816510"></a><a name="p1836642816510"></a>Operator graph integration refers to running operators in GE graph mode. In graph mode, all operators are first constructed into a graph, and then the graph is delivered to the <span id="ph196874123168"><a name="ph196874123168"></a><a name="ph196874123168"></a>AI processor</span> through GE for execution.</p></td>
</tr>
<tr id="row8742315133310"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p1539252135115"><a name="p1539252135115"></a><a name="p1539252135115"></a>Operator prototype</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p43663288512"><a name="p43663288512"></a><a name="p43663288512"></a>An operator prototype is an abstract description of an operator. It defines the inputs, outputs, attributes, and other information of the operator.</p></td>
</tr>
<tr id="row1090021553312"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p103922245119"><a name="p103922245119"></a><a name="p103922245119"></a>Communication-computation fusion operator</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p736620283511"><a name="p736620283511"></a><a name="p736620283511"></a>A communication-computation fusion operator fuses collective communication tasks and computation tasks. During operator execution, the computation and communication tasks can achieve partial pipeline parallelism, thereby improving performance.</p></td>
</tr>
<tr id="row19291228388"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p1765196133017"><a name="p1765196133017"></a><a name="p1765196133017"></a>Reg vector computation</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p13324155813017"><a name="p13324155813017"></a><a name="p13324155813017"></a>The Reg vector computation APIs are developed for the RegBase architecture. Through these APIs, you can directly operate the registers involved in Vector computation on the chip to achieve greater flexibility and better performance.</p></td>
</tr>
<tr id="row136541613319"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p7392202195118"><a name="p7392202195118"></a><a name="p7392202195118"></a>Bitwise mode</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p9366112835114"><a name="p9366112835114"></a><a name="p9366112835114"></a>One of the selectable modes when using Mask to control the elements participating in computation in each Repeat of vector computation. It controls which elements participate in computation on a bit-by-bit basis. A bit value of 1 indicates that the element participates in computation, and 0 indicates that it does not.</p></td>
</tr>
<tr id="row1209516203316"><td class="cellrowborder" valign="top" width="22.978822978822976%" headers="mcps1.2.3.1.1 "><p id="p539217295116"><a name="p539217295116"></a><a name="p539217295116"></a>Custom operator project</p></td>
<td class="cellrowborder" valign="top" width="77.02117702117702%" headers="mcps1.2.3.1.2 "><p id="p16366152855115"><a name="p16366152855115"></a><a name="p16366152855115"></a>An operator project generated by Ascend C using the msOpGen tool.</p></td>
</tr>
</tbody>
</table>

