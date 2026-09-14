# Basic Architecture<a name="ZH-CN_TOPIC_0000002380477945"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T11:35:40.581Z pushedAt=2026-09-02T09:39:17.721Z -->

As shown in the following figure, operators developed based on Ascend C run on the AI Core. The Ascend C programming model is introduced based on the abstraction of the AI Core hardware architecture. Understanding the hardware architecture helps developers better understand the programming model. For advanced developers who need to achieve high-performance programming, knowledge of the hardware architecture is even more essential, and much of the content in the operator practice reference is introduced based on this chapter.

![](../../../figures/overview.png)

The AI Core is responsible for executing cube- and vector-computation-intensive tasks. It consists of the following components:

-   **Compute units**: include the Cube compute unit, Vector compute unit, and Scalar compute unit.
-   **Storage unitd**: include L1 Buffer, L0A Buffer, L0B Buffer, L0C Buffer, Unified Buffer, BiasTable Buffer, Fixpipe Buffer, and other storage units designed for efficient computation.
-   **DMA unitd**: include MTE1, MTE2, MTE3, and FixPipe, used for efficient data transfer between different storage units.

Taking the Atlas A2 training products/Atlas A2 inference products as an example, the hardware architecture diagram is as follows:

![](../../../figures/separated_architecture.png)

This chapter first introduces the key concepts and terms related to the hardware architecture, as well as the working modes of the AI Core, laying the foundation for understanding the subsequent content. Then, taking the Atlas A2 training products/Atlas A2 inference products as an example, it provides an introduction to the basic architecture of the AI Core: it first introduces the basic functions and structures of the compute unit, storage unit, and DMA unit, and then helps developers gain an in-depth understanding of how the hardware architecture works through typical data flow and control flow examples. For the specific architecture specifications and details corresponding to different product models, see the subsequent [Architecture Specifications](./architecture_spec/architecture_spec.md) chapter.

## Key Concepts and Terms<a name="section16011278116"></a>

-   Core

    A compute core that has an independent Scalar compute unit. The Scalar compute unit is the scheduling unit within the core and is responsible for functions such as instruction issue within the core.

-   AI Core

    The compute core of an AI processor, responsible for executing cube- and vector-computation-intensive tasks.

-   Cube Core

    A cube compute core dedicated to cube computation. It consists of the Scalar scheduling unit, Cube compute unit, DMA unit, and others, and does not include a Vector compute unit.

-   Vector Core

    A vector compute core dedicated to vector computation. It consists of the Scalar scheduling unit, Vector compute unit, DMA unit, and others, and does not include a Cube compute unit.

-   AIC

    In AI Core [separated mode](#li188191010204418), it is the Cube Core in a combination of a Cube Core and a Vector Core.

-   AIV

    In AI Core [separated mode](#li188191010204418), it is the Vector Core in a combination of a Cube Core and a Vector Core.

## AI Core Working Modes<a name="section1574769433"></a>

-   <a name="li188191010204418"></a>Separated Mode

    A working mode of the AI Core in which the Cube compute unit and the Vector compute unit each correspond to an independent Scalar scheduling unit and are deployed separately on the Cube Core and the Vector Core. The Cube Core and Vector Core are combined at a certain ratio (1:N), and such a combination is regarded as one AI Core. The number of AI Cores is determined by the number of Cube Cores.

    **Figure 1**  Separated mode diagram (the value of N is subject to the value obtained through the hardware platform information acquisition API)<a name="fig4925816477"></a>  
    ![](../../../figures/separation_mode_diagram_n_value_based_on_hardware_platform_information_acquisition_interface.png "Separated mode diagram (the value of N is subject to the value obtained through the hardware platform information acquisition API)")

-   <a name="li1414517184416"></a>Coupling Mode

    A working mode of the AI Core in which the Cube compute unit and the Vector compute unit correspond to the same Scalar scheduling unit and are deployed on one AI Core.

    **Figure 2**  Coupled mode diagram<a name="fig583116255714"></a>  
    ![](../../../figures/coupled_mode_diagram.png "Coupled mode diagram")

>[!NOTE]Note 
>In Ascend C programming, the working modes of different products are as follows:
>- Atlas inference products: coupled mode
>- Atlas training products: couplws mode
>- Atlas A2 training products/Atlas A2 inference products: separated mode
>- Atlas A3 training products/Atlas A3 inference products: separated mode
>- Ascend 950PR/Ascend 950DT: separated mode
>- Atlas 200I/500 A2 inference products: coupled mode
>Note: For Atlas 200I/500 A2 inference products, the hardware supports both coupled and separated working modes. In coupled mode, developers only need to pay attention to the number of AI Cores, without paying attention to the number of Vector Cores and Cube Cores. In separated mode, developers need to pay attention to the number of AI Cores, Vector Cores, and Cube Cores. In Ascend C programming scenarios, only coupled mode is supported.

## Compute Unit<a name="section2853918419"></a>

The compute unit is the core unit in the AI Core that provides powerful computing capabilities. There are three basic **compute units**: the Cube compute unit, the Vector compute unit, and the Scalar compute unit, which perform different types of data computation in the AI Core.

-   Cube

    The Cube compute unit is responsible for cube computation. Taking the float16 data type as an example, each Cube compute unit execution can complete the multiplication of two 16x16 float16 cubes. As shown in the following figure, the highlighted part is the Cube compute unit and the storage units it accesses, where L0A stores the left cube, L0B stores the right cube, and L0C stores the result and intermediate results of the cube multiplication.

    **Figure 3**  Cube compute unit data access<a name="fig643417174212"></a>  
    ![](../../../figures/cube_compute_unit_data_access.png "Cube compute unit data access")

-   Vector

    The Vector compute unit is responsible for vector computation. It executes vector instructions, similar to traditional Single Instruction Multiple Data (SIMD) instructions. Each vector instruction can complete the same type of operation on multiple operands. The Vector compute unit can quickly complete the addition or multiplication of two float16 vectors. Vector instructions support multiple iterations of execution and also support direct operations on vectors with intervals.

    As shown in the following figure, the source data and destination data of all vector computations must be stored in the Unified Buffer. The start address and operation length of vector instructions have alignment requirements, usually 32B alignment. For specific alignment requirements, see the constraint description in the API.

    **Figure 4**  Vector compute unit data access<a name="fig172397519590"></a>  
    ![](../../../figures/vector_compute_unit_data_access.png "Vector compute unit data access")

-   Scalar

    The Scalar compute unit is responsible for scalar data operations of various types and program flow control. Functionally, it can be regarded as a small CPU that completes loop control of the entire program, branch judgment, address and parameter calculation for Cube/Vector and other instructions, and basic arithmetic operations. It can also control the pipeline of other execution units in the AI Core by inserting synchronization symbols into the event synchronization module. Compared with the host CPU, the Scalar in the AI Core has weaker computing capability and is mainly used to issue instructions. Therefore, in actual application scenarios, Scalar computation should be minimized as much as possible. For example, during performance tuning, minimize branch judgment such as if/else and variable operations.

    As shown in the following figure: when the Scalar compute unit executes scalar operation instructions, it executes standard Arithmetic Logic Unit (ALU) statements. The code segment and data segment (stack space) required by the ALU both come from GM. The ICache (Instruction Cache) is used to cache the code segment, and its cache size is related to the hardware specification, for example, 16K or 32K, loaded in units of 2K. The DCache (Data Cache) is used to cache the data segment, and its size is also related to the hardware specification, for example, 16K, loaded in units of Cache Line (64 bytes). Considering that intra-core access is the most efficient, ensure that the code segment and data segment are cached in the ICache and DCache as much as possible to avoid off-core access. At the same time, since the data loading unit differs, you can consider the size of data loaded at a time during programming to improve loading efficiency. For example, when the DCache loads data, the loading efficiency is the highest when the start address of the data memory is aligned with the Cache Line (64 bytes).

    **Figure 5**  Scalar access to instructions and data<a name="fig9952125415559"></a>  
    ![](../../../figures/scalar_access_to_instructions_and_data.png "Scalar access to instructions and data")

    >[!NOTE]
    >The hardware provides L2Cache to cache data (including the code segment and data segment) accessed from GM, thereby accelerating the access speed and improving access efficiency. The off-core L2Cache loads data in units of Cache Line. Depending on the hardware specifications, the Cache Line size differs (128/256/512 bytes, etc.).

## Storage Units and DMA Units<a name="section123639375417"></a>

For the compute resources in an AI processor to deliver strong computing power, it is essential to ensure that input data reaches the compute units in a timely and accurate manner. This requires a carefully designed storage system to guarantee the data supply needed by the compute units.

As shown in the following figure, an AI Core contains multiple levels of internal storage. The AI Core must load data from external storage into its internal storage before it can perform the corresponding computation. The main internal storage of an AI Core includes the L1 Buffer, L0 Buffer, Unified Buffer, and so on. To support data transfer and movement within the AI Core, the AI Core also contains Memory Transfer Engine (MTE) DMA units, which can perform on-the-fly data format/type conversion during the transfer process.

For details about the internal storage units and DMA units, see [Table 1](#table1692510612218) and [Table 2](#table288493152012).

**Figure 6** Storage units<a name="fig2032418113610"></a>  
![](../../../figures/storage_unit.png "Storage units")

**Table 1** Storage units

<a name="table1692510612218"></a>
<table><thead align="left"><tr id="row1692576132110"><th class="cellrowborder" valign="top" width="30.11%" id="mcps1.2.3.1.1"><p id="p189252069218"><a name="p189252069218"></a><a name="p189252069218"></a>Storage Unit</p></th>
<th class="cellrowborder" valign="top" width="69.89%" id="mcps1.2.3.1.2"><p id="p17925161215"><a name="p17925161215"></a><a name="p17925161215"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row892518618215"><td class="cellrowborder" valign="top" width="30.11%" headers="mcps1.2.3.1.1 "><p id="p1292586202112"><a name="p1292586202112"></a><a name="p1292586202112"></a>L1 Buffer</p></td>
<td class="cellrowborder" valign="top" width="69.89%" headers="mcps1.2.3.1.2 "><p id="p1492517613219"><a name="p1492517613219"></a><a name="p1492517613219"></a>L1 Buffer, a general-purpose internal storage, is a relatively large data staging area in the AI Core. It can temporarily store data that the Cube compute unit needs to use repeatedly, thereby reducing the number of reads and writes from the bus.</p></td>
</tr>
<tr id="row1925146182113"><td class="cellrowborder" valign="top" width="30.11%" headers="mcps1.2.3.1.1 "><p id="p1192512632115"><a name="p1192512632115"></a><a name="p1192512632115"></a>L0A Buffer / L0B Buffer</p></td>
<td class="cellrowborder" valign="top" width="69.89%" headers="mcps1.2.3.1.2 "><p id="p192611610219"><a name="p192611610219"></a><a name="p192611610219"></a>Input of Cube instructions.</p></td>
</tr>
<tr id="row1192610642119"><td class="cellrowborder" valign="top" width="30.11%" headers="mcps1.2.3.1.1 "><p id="p109268610212"><a name="p109268610212"></a><a name="p109268610212"></a>L0C Buffer</p></td>
<td class="cellrowborder" valign="top" width="69.89%" headers="mcps1.2.3.1.2 "><p id="p49269611212"><a name="p49269611212"></a><a name="p49269611212"></a>Output of Cube instructions, but also part of the input when accumulation computation is performed.</p></td>
</tr>
<tr id="row9926467213"><td class="cellrowborder" valign="top" width="30.11%" headers="mcps1.2.3.1.1 "><p id="p1692614642117"><a name="p1692614642117"></a><a name="p1692614642117"></a>Unified Buffer</p></td>
<td class="cellrowborder" valign="top" width="69.89%" headers="mcps1.2.3.1.2 "><p id="p59267692111"><a name="p59267692111"></a><a name="p59267692111"></a>Unified buffer, serving as the input and output of vector and scalar computations.</p></td>
</tr>
<tr id="row185871713151111"><td class="cellrowborder" valign="top" width="30.11%" headers="mcps1.2.3.1.1 "><p id="p8186194819417"><a name="p8186194819417"></a><a name="p8186194819417"></a><span>BT Buffer</span></p></td>
<td class="cellrowborder" valign="top" width="69.89%" headers="mcps1.2.3.1.2 "><p id="p7587111381113"><a name="p7587111381113"></a><a name="p7587111381113"></a><span>BiasTable</span><span> buffer, </span><span>stores the </span><span>Bias in Cube computation.</span></p></td>
</tr>
<tr id="row13135416151111"><td class="cellrowborder" valign="top" width="30.11%" headers="mcps1.2.3.1.1 "><p id="p17491298118"><a name="p17491298118"></a><a name="p17491298118"></a><span>FP Buffer</span></p></td>
<td class="cellrowborder" valign="top" width="69.89%" headers="mcps1.2.3.1.2 "><p id="p1213671651111"><a name="p1213671651111"></a><a name="p1213671651111"></a><span>Fixpipe</span><span> Buffer, </span><span>stores quantization parameters, </span><span>Relu</span><span> parameters, etc</span><span>.</span></p></td>
</tr>
</tbody>
</table>

**Table 2**  Introduction to DMA units

<a name="table288493152012"></a>
<table><thead align="left"><tr id="row19884103142012"><th class="cellrowborder" valign="top" width="30.049999999999997%" id="mcps1.2.3.1.1"><p id="p288473114204"><a name="p288473114204"></a><a name="p288473114204"></a>DMA Unit</p></th>
<th class="cellrowborder" valign="top" width="69.95%" id="mcps1.2.3.1.2"><p id="p8884531162011"><a name="p8884531162011"></a><a name="p8884531162011"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row199091854122011"><td class="cellrowborder" valign="top" width="30.049999999999997%" headers="mcps1.2.3.1.1 "><p id="p990975412200"><a name="p990975412200"></a><a name="p990975412200"></a>MTE1</p></td>
<td class="cellrowborder" valign="top" width="69.95%" headers="mcps1.2.3.1.2 "><p id="p69911469917"><a name="p69911469917"></a><a name="p69911469917"></a>Responsible for data transfer on the following paths:</p>
<a name="ul15750125414917"></a><a name="ul15750125414917"></a><ul id="ul15750125414917"><li>L1-&gt;L0A/L0B</li><li><span>L1-</span><span>&gt;</span><span>BT Buffer</span></li></ul></td>
</tr>
<tr id="row83637216"><td class="cellrowborder" valign="top" width="30.049999999999997%" headers="mcps1.2.3.1.1 "><p id="p16440784214"><a name="p16440784214"></a><a name="p16440784214"></a>MTE2</p></td>
<td class="cellrowborder" valign="top" width="69.95%" headers="mcps1.2.3.1.2 "><p id="p12680536151314"><a name="p12680536151314"></a><a name="p12680536151314"></a>Responsible for data transfer on the following paths:</p>
<a name="ul1420163841318"></a><a name="ul1420163841318"></a><ul id="ul1420163841318"><li>GM-&gt;{L1, L0A/B}. On this path, data is transferred based on fractal size, and performance is better when the transfer meets Cache Line size alignment.</li><li>GM-&gt;UB. Performance is better when data is transferred based on Cache Line size.</li></ul></td>
</tr>
<tr id="row153848520214"><td class="cellrowborder" valign="top" width="30.049999999999997%" headers="mcps1.2.3.1.1 "><p id="p121221999218"><a name="p121221999218"></a><a name="p121221999218"></a>MTE3</p></td>
<td class="cellrowborder" valign="top" width="69.95%" headers="mcps1.2.3.1.2 "><p id="p1683722417444"><a name="p1683722417444"></a><a name="p1683722417444"></a>Responsible for data transfer on the following paths:</p>
<a name="ul43441132124411"></a><a name="ul43441132124411"></a><ul id="ul43441132124411"><li>UB -&gt; GM</li></ul></td>
</tr>
<tr id="row217791862110"><td class="cellrowborder" valign="top" width="30.049999999999997%" headers="mcps1.2.3.1.1 "><p id="p61776186219"><a name="p61776186219"></a><a name="p61776186219"></a>FixPipe</p></td>
<td class="cellrowborder" valign="top" width="69.95%" headers="mcps1.2.3.1.2 "><p id="p18986147134310"><a name="p18986147134310"></a><a name="p18986147134310"></a>Responsible for data transfer on the following paths, during which in-band data format/type conversion can be performed:</p>
<a name="ul189261456194111"></a><a name="ul189261456194111"></a><ul id="ul189261456194111"><li>L0C-&gt;{GM/L1}</li><li>L1-&gt;FP Buffer</li></ul></td>
</tr>
</tbody>
</table>

>[!NOTE] Note
>- Different types of AI processors have different storage unit sizes. Developers can obtain the size by calling the [GetCoreMemSize](../../../../api/Utils-API/platform_info/PlatformAscendC/GetCoreMemSize.md) API.
>- All data read from or written to GM through the DMA unit is cached in L2Cache by default to accelerate access and improve access efficiency. The off-core L2Cache loads data in units of Cache Line. Depending on the hardware specification, the Cache Line size varies (128/256/512 bytes, etc.).

## Typical Data Flow<a name="section147001014511"></a>

-   The typical data flow of vector computation is as follows:

    GM → UB → Vector → UB → GM

    ![](../../../figures/separated_architecture_10.png)

-   The typical data flow of Cube computation is as follows:

    -   GM → L1 → L0A/L0B → Cube → L0C → FixPipe → GM
    -   GM → L1 → L0A/L0B → Cube → L0C → FixPipe → L1

    ![](../../../figures/separated_architecture_11.png)

## Typical Instruction Flow<a name="section1296816241258"></a>

Multiple instructions enter the ICache (Instruction Cache) from the system memory through the bus interface. The subsequent instruction execution process has two possibilities depending on the instruction type:

-   If the instruction is a Scalar instruction, it is directly executed by the Scalar unit.
-   Other instructions are scheduled by the Scalar unit to independent classification sequences (Vector instruction sequence, Cube instruction sequence, MTE1/MTE2/MTE3 instruction sequence, and so on), and then executed by the corresponding execution units.

**Figure 7** Instruction classification processing mechanism<a name="fig1993349810"></a>  
![](../../../figures/instruction_classification_processing_mechanism.png "Instruction classification processing mechanism")

Instructions in the same instruction sequence are executed in the order in which they enter the sequence. Different instruction sequences can be executed in parallel, and the parallel execution of multiple instruction sequences improves the overall execution efficiency. For possible data dependencies during parallel execution, synchronization instructions are inserted through the event synchronization module to control pipeline synchronization. Two APIs, PipeBarrier and SetFlag/WaitFlag, are provided to ensure that instructions within a sequence and between sequences are executed according to their logical relationships.

-   PipeBarrier itself is an instruction used to constrain the execution order within a sequence (although instructions are executed sequentially, it does not mean that the previous instruction has finished executing when the next instruction starts). The PipeBarrier instruction ensures that all data reads and writes in the preceding instructions are completed before the subsequent instructions start executing.
-   SetFlag/WaitFlag are two instructions. In the SetFlag/WaitFlag instructions, you can specify the relationship between a pair of instruction sequences, which implements a "lock" mechanism between the two sequences. The mechanism works as follows:
    -   SetFlag: After all read and write operations of the preceding instructions are completed, the current instruction starts executing and sets the corresponding flag bit in the hardware to 1.
    -   WaitFlag: When this instruction is executed, if the corresponding flag bit is found to be 0, the subsequent instructions of this sequence are blocked; if the corresponding flag bit is found to be 1, the corresponding flag bit is set to 0, and the subsequent instructions start executing.

Ascend C provides synchronization control APIs, which developers can use to perform synchronization control on their own. Note that in most cases, developers do not need to pay attention to synchronization when programming based on the programming model and paradigm described in the [programming model](../../programming_model/programming_model_overview.md), because the programming model handles synchronization control for them. Using the programming model and paradigm is the recommended programming approach, and performing synchronization control on your own may introduce a certain degree of programming complexity.

However, we still hope that developers can understand the basic principles of synchronization, so that they can better understand how to design parallel computing programs later. In a few cases, developers need to manually insert synchronization. You can refer to [when developers need to manually insert synchronization](../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/intra_core_synchronization_capability_overview.md) for details.
