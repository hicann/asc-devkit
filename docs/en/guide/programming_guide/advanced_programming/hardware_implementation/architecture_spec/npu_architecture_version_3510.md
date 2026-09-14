# NPU Architecture Version 3510<a name="ZH-CN_TOPIC_0000002468380701"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-26T11:43:17.899Z pushedAt=2026-09-02T08:13:53.461Z -->

This section describes the hardware architecture and functional description of the NPU whose `__NPU_ARCH__` version number is [3510](../../../language_extension/SIMD-BuiltIn_keyword.md#table65291052154114).

## Hardware Architecture Diagram<a name="section2189135013710"></a>

As shown in the following figure, in this architecture, the AI Core is divided into two independent cores, AIC and AIV, which are used for cube computation and vector computation, respectively. The ratio of AIC cores to AIV cores is 1:2. Each core has its own Scalar unit and can independently load its own code segment.

![](../../../../figures/hardware_architecture.png)

The key features of this architecture are as follows:

-   Add the data paths L0C Buffer -\> Unified Buffer and Unified Buffer <-\> L1 Buffer.
-   Remove the data paths Global Memory -\> L0A Buffer and Global Memory -\> L0B Buffer.
-   Remove the data path L1 Buffer -\> Global Memory.
-   SSBuffer, used for inter-core communication between AIC and AIV.
-   Add the SIMD Register File storage hierarchy. In SIMD programs, data is moved from the Unified Buffer to registers for computation, and the intermediate results generated can be computed directly in registers without being moved back to the Unified Buffer.
-   Add SIMT-related hardware units. The SIMT-related hardware units are described as follows:

    <a name="table45836437171"></a>
    <table><thead align="left"><tr id="row1558324391711"><th class="cellrowborder" valign="top" width="18.85%" id="mcps1.1.3.1.1"><p id="p558318431177"><a name="p558318431177"></a><a name="p558318431177"></a>SIMT Hardware Unit Name</p>
    </th>
    <th class="cellrowborder" valign="top" width="81.15%" id="mcps1.1.3.1.2"><p id="p3583443191718"><a name="p3583443191718"></a><a name="p3583443191718"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row358314316175"><td class="cellrowborder" valign="top" width="18.85%" headers="mcps1.1.3.1.1 "><p id="p1158311438177"><a name="p1158311438177"></a><a name="p1158311438177"></a>SIMT DCache</p>
    </td>
    <td class="cellrowborder" valign="top" width="81.15%" headers="mcps1.1.3.1.2 "><p id="p658313438176"><a name="p658313438176"></a><a name="p658313438176"></a>SIMT accesses to GM must go through the SIMT DCache. SIMT supports a maximum of 128 KB Data Cache. The Data Cache directly reuses the UB as cache lines. All external memory accesses by SIMT are performed at a granularity of 128 B.</p>
    </td>
    </tr>
    <tr id="row97402054191814"><td class="cellrowborder" valign="top" width="18.85%" headers="mcps1.1.3.1.1 "><p id="p17741125491813"><a name="p17741125491813"></a><a name="p17741125491813"></a>Warp Scheduler</p>
    </td>
    <td class="cellrowborder" valign="top" width="81.15%" headers="mcps1.1.3.1.2 "><p id="p1974115414186"><a name="p1974115414186"></a><a name="p1974115414186"></a>Implements hardware multi-thread scheduling.</p>
    <p id="p129229500325"><a name="p129229500325"></a><a name="p129229500325"></a>Each AIV in SIMT has four Warp Schedulers.</p>
    </td>
    </tr>
    <tr id="row1943773001919"><td class="cellrowborder" valign="top" width="18.85%" headers="mcps1.1.3.1.1 "><p id="p104371530101917"><a name="p104371530101917"></a><a name="p104371530101917"></a>SIMT Register File</p>
    </td>
    <td class="cellrowborder" valign="top" width="81.15%" headers="mcps1.1.3.1.2 "><p id="p74371930131917"><a name="p74371930131917"></a><a name="p74371930131917"></a>Provides a total capacity of 128 KB of ultra-large registers for SIMT applications. The number of registers available to each thread depends on the number of threads, as follows:</p>
    <a name="ul3236101542314"></a><a name="ul3236101542314"></a><ul id="ul3236101542314"><li>1025 to 2048 threads: 16 registers.</li><li>513 to 1024 threads: 32 registers.</li><li>257 to 512 threads: 64 registers.</li><li>1 to 256 threads: 127 registers.</li></ul>
    </td>
    </tr>
    </tbody>
    </table>

## Compute Unit<a name="section8490181519513"></a>

**Separate deployment of Cube compute units and Vector compute units**

In this architecture, Cube compute units and Vector compute units are deployed on AIC cores and AIV cores respectively. Each core has its own Scalar unit and can independently load its own code segment.

**Vector compute unit**

-   The Vector compute unit supports the U8, U16, U32, S8, S16, S32, BF16, FP16, and FP32 data types.
-   The Vector compute unit can process 256 bytes of data per cycle.
-   The data processed by the Vector compute unit comes from registers.
-   In this architecture version, the mask value passed in through the high-dimensional sharding API is converted into MaskReg and passed to the Vector compute unit, whereas in [NPU architecture version 2201](../../../language_extension/SIMD-BuiltIn_keyword.md#table65291052154114), the mask value is read from a special mask register.

    **Figure 1**  High-dimensional sharding in NPU architecture version 220<a name="fig7285103215410"></a>  
    ![](../../../../figures/npu_architecture_version_220_high_dimensional_sharding.png "High-dimensional sharding in NPU architecture version 220")

    **Figure 2**  High-dimensional sharing in this architecture version<a name="fig7787845174216"></a>  
    ![](../../../../figures/high_dimensional_sharding_in_this_architecture_version.png "High-dimensional sharding in this architecture version")

**Cube compute unit**

-   The Cube compute unit supports FP32/FP16/BF16/HiF8/FP8_E4M3/FP8_E5M2/U8/S8. It completes a 16x16 by 16x16 cube multiplication of the float16 data type per cycle; for the int8_t data type, it completes a 16*32 by 32*16 cube multiplication per cycle.
-   The storage units accessible to the Cube compute unit are the L0A Buffer, L0B Buffer, and L0C Buffer. The L0A Buffer stores the left cube, the L0B Buffer stores the right cube, and the L0C Buffer stores the results and intermediate results of cube multiplication.

**Scalar unit**

-   The Scalar unit supports the U16/S16/U32/S32/U64/S64/FP64 data types.
-   In the Regbase architecture, the Aux Scalar compute unit separately handles Scalar computation inside SIMD_VF functions, while the Scalar compute unit handles Scalar computation outside SIMD_VF functions.

## Storage Units<a name="section6500173264510"></a>

**Obtaining the memory space size of a storage unit**

Developers can query the memory space size of each storage unit through the [platform information](../../../../../api/Utils-API/platform_info/platform_information.md) API.

**Minimum access granularity (alignment requirement) of each storage unit**

<a name="table12348145512210"></a>
<table><thead align="left"><tr id="row3348175522218"><th class="cellrowborder" valign="top" width="7.5200000000000005%" id="mcps1.1.4.1.1"><p id="p1680716381143"><a name="p1680716381143"></a><a name="p1680716381143"></a>Core</p></th>
<th class="cellrowborder" valign="top" width="46.22%" id="mcps1.1.4.1.2"><p id="p5348145515229"><a name="p5348145515229"></a><a name="p5348145515229"></a>Storage Unit</p></th>
<th class="cellrowborder" valign="top" width="46.26%" id="mcps1.1.4.1.3"><p id="p16348125520221"><a name="p16348125520221"></a><a name="p16348125520221"></a>Alignment Requirement</p></th>
</tr>
</thead>
<tbody><tr id="row11348135516221"><td class="cellrowborder" valign="top" width="7.5200000000000005%" headers="mcps1.1.4.1.1 "><p id="p48071138245"><a name="p48071138245"></a><a name="p48071138245"></a>AIV</p></td>
<td class="cellrowborder" valign="top" width="46.22%" headers="mcps1.1.4.1.2 "><p id="p63481755122216"><a name="p63481755122216"></a><a name="p63481755122216"></a><span id="ph15348135502220"><a name="ph15348135502220"></a><a name="ph15348135502220"></a>Unified Buffer</span></p></td>
<td class="cellrowborder" valign="top" width="46.26%" headers="mcps1.1.4.1.3 "><p id="p1634825512226"><a name="p1634825512226"></a><a name="p1634825512226"></a>32-byte alignment.</p></td>
</tr>
<tr id="row1534865520225"><td class="cellrowborder" rowspan="6" valign="top" width="7.5200000000000005%" headers="mcps1.1.4.1.1 "><p id="p567812161851"><a name="p567812161851"></a><a name="p567812161851"></a>AIC</p></td>
<td class="cellrowborder" valign="top" width="46.22%" headers="mcps1.1.4.1.2 "><p id="p7349175582217"><a name="p7349175582217"></a><a name="p7349175582217"></a><span id="ph1234918559222"><a name="ph1234918559222"></a><a name="ph1234918559222"></a>L1 Buffer</span></p></td>
<td class="cellrowborder" valign="top" width="46.26%" headers="mcps1.1.4.1.3 "><p id="p2034975519227"><a name="p2034975519227"></a><a name="p2034975519227"></a>32-byte alignment.</p></td>
</tr>
<tr id="row14349855162220"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="p2349175522213"><a name="p2349175522213"></a><a name="p2349175522213"></a><span id="ph12349175572211"><a name="ph12349175572211"></a><a name="ph12349175572211"></a>L0A Buffer</span></p></td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="p12349105510223"><a name="p12349105510223"></a><a name="p12349105510223"></a>512-byte alignment.</p></td>
</tr>
<tr id="row83498556224"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="p143491455162210"><a name="p143491455162210"></a><a name="p143491455162210"></a><span id="ph5349135515225"><a name="ph5349135515225"></a><a name="ph5349135515225"></a>L0B Buffer</span></p></td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="p234945516225"><a name="p234945516225"></a><a name="p234945516225"></a>512-byte alignment.</p></td>
</tr>
<tr id="row13491655162217"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="p43494555226"><a name="p43494555226"></a><a name="p43494555226"></a><span id="ph17349155514229"><a name="ph17349155514229"></a><a name="ph17349155514229"></a>L0C Buffer</span></p></td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="p14349125512220"><a name="p14349125512220"></a><a name="p14349125512220"></a>64-byte alignment.</p></td>
</tr>
<tr id="row15518316118"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="p13625119517"><a name="p13625119517"></a><a name="p13625119517"></a>BiasTable Buffer</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="p1462519119"><a name="p1462519119"></a><a name="p1462519119"></a>64-byte alignment.</p></td>
</tr>
<tr id="row16156956113"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="p126251896119"><a name="p126251896119"></a><a name="p126251896119"></a>Fixpipe Buffer</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="p16251295111"><a name="p16251295111"></a><a name="p16251295111"></a>64-byte alignment.</p></td>
</tr>
</tbody>
</table>

**Recommended data layout formats for each storage unit**

-   The following fractal formats are recommended for L0A Buffer, L0B Buffer, and L0C Buffer respectively:

    -   L0A Buffer: FRACTAL\_NZ (due to hardware structure changes, the fractal of L0A Buffer is changed to NZ in this architecture)
    -   L0B Buffer: FRACTAL\_ZN
    -   L0C Buffer: FRACTAL\_NZ  
    These formats are optimized for compute-intensive tasks such as cube multiplication, which can significantly improve computation efficiency.

-   For the L1 Buffer cache, the FRACTAL\_NZ format is recommended. When the L1 Buffer uses the NZ format, the format conversion overhead can be reduced when data is moved to the L0A/L0B Buffers (which need to be converted to the ZN format respectively).
-   The Unified Buffer has no requirement on the data format.

**Access conflicts of storage units**

The UB structure of this NPU architecture version is shown in the following figure. When multiple operations attempt to access the same bank or bank group of the Unified Buffer at the same time, bank conflicts may occur, including read-write conflicts, write-write conflicts, and read-read conflicts. Such conflicts cause access queuing and degrade performance. In NPU architecture version 2201, a bank group has only one set of read and write ports, and at most one read or one write can be completed per cycle. In this NPU architecture version, each bank group has two sets of read and write ports, allowing at most 2 reads and 0 writes, or 1 read and 1 write, at the same time. The related read/write constraints are as follows:

-   **Read-write conflict**: A read operation and a write operation attempt to access the same bank at the same time.
-   **Write-write conflict**: Multiple write operations attempt to access the same bank group at the same time.
-   **Read-read conflict**: Two read operations attempt to access the same bank at the same time, or more than two read operations attempt to access the same bank group at the same time.

**Figure 3**  UB bank diagram of this architecture version<a name="fig59155716511"></a>  
![](../../../../figures/ub_bank_diagram_of_this_architecture_version.png "UB bank diagram of this architecture version")

**Register**

-   RegTensor

    RegTensor is used to store Reg vector computation data. The bit width of RegTensor is Vector Length (VL) (256 bytes).

-   MaskReg

    MaskReg is used to indicate which elements participate in the computation. Its width is one-eighth of RegTensor (VL/8).

-   UnalignRegForLoad & UnalignRegForStore

    UnalignRegForLoad and UnalignRegForStore are used as buffers to optimize the overhead of consecutive unaligned address access between UB and RegTensor. Before reading an unaligned address, UnalignRegForLoad and UnalignRegForStore should be initialized through the LoadUnAlignPre API, and then the LoadUnAlign API is used. When writing an unaligned address, use the StoreUnAlign API first, and then use the StoreUnAlignPost API for post-processing.

-   AddrReg

    AddrReg is short for Address Register, a register used to store address offsets. AddrReg should be initialized through [CreateAddrReg](../../../../../api/SIMD-API/basic_api/reg_vector_compute/register_data_types/AddrReg.md), and then used within a loop to store address offsets. AddrReg increments by the configured stride in each loop iteration.

## Data Movement Unit<a name="section1073952984611"></a>

**Alignment requirements for data movement**

Because the moved data is used for computation, there are requirements on the size of the moved data. The data moved to the Unified Buffer must be aligned by DataBlock, and data movement to other storage units must follow the [fractal requirements](../../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout_format.md). For example, when data is moved from the L1 Buffer to the L0A Buffer, the data format must be converted from NZ to ZN, and the size of the moved data must be aligned by the fractal size. If the remaining size of the L1 Buffer is less than one fractal, an exception occurs during hardware execution.

**MTE hard channel**

-   AIV adds a hard channel between the Unified Buffer and the L1 Buffer.
-   Adds support for the Loop mode for GM-to-Unified Buffer movement and Unified Buffer-to-GM movement. In Loop mode, each loop can be in Normal mode or Compact mode. For details about Normal mode and Compact mode, see [DataCopyPad\(ISASI\)](../../../../../api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopyPad_GMToUB.md).
    -   A single loop moves data in Normal mode.

        If the length of a single data block is 32B-aligned, no padding needs to be inserted, and multiple data blocks can be moved through multiple repeats.

        ![](../../../../figures/loop_normal.png)

        If the length of a single data block is not 32B-aligned, padding must be inserted after each data block to make it 32B-aligned before movement.

        ![](../../../../figures/forloop.png)

    -   A single loop moves data in Compact mode.

        In Compact mode, a group of data blocks can be moved at a time. When the total length of these data blocks is 32B-aligned, no padding needs to be inserted at the end.

        ![](../../../../figures/loop_normal-19.png)

        When the length of a group of data blocks is not 32B-aligned, padding needs to be inserted after this group of data blocks so that the total length is 32B-aligned.

        ![](../../../../figures/loop_comact.png)

**Support for Fixpipe hardware acceleration**

Fixpipe is an acceleration module in the NPU that hardens typical operations. It is located inside the AIC and works with the Cube compute unit to complete inline computation. Its main functions are as follows:

-   Quantization and dequantization: including S4/S8/S32/FP16/FP32/FP8\_E4M3/HiF8/BF16.
-   ReLU functions, including typical activation functions such as ReLU, PReLU, and Leaky ReLU.
-   Data format conversion, including:
    -   Channel merge and channel split can convert the fractal size to ensure that the fractal output to the L1 Buffer/GM meets the requirement.
    -   Inline NZ2ND and NZ2DN conversion of the L0C source is supported.

Channel merge supports the S8, U8, S4, and U4 data types, while channel split supports the FP32 data type.

-   Channel merge (S8 and U8 data types)

    For conversion to the S8 or U8 target data type, the fractal cube is converted from 16x16 to 16x32 by hardware. If the number of output channels N is an even multiple of 16, every two adjacent 16x16 fractal cubes in the N direction are merged into one 16x32 fractal cube. If N is an odd multiple of 16, channels 1 to (N–16) are merged, and the last 16 channels remain unmerged.

    As shown below, the target data type is S8, M is 32, and N is 48. First, the first two columns of 16x16 fractal cubes are merged into one 16x32 cube, and then the remaining 16x16 fractal cubes are directly moved into the L1 Buffer.

    ![](../../../../figures/Fixpipe_int8-channel_merge1.png)

-   Channel merge (S4 and U4 data types):

    For conversion to the S4 or U4 target data type, the fractal cube is converted from 16x16 to 16x64 by hardware. If the number of output channels N is a multiple of 64, every four adjacent 16x16 fractal cubes in the N direction are merged into one single 16x64 fractal cube.

    For example, here the target data type is S4, M is 32, and N is 64. First, the 16x16 fractal matrices in row 1 are merged into one 16x64 matrix, and then the 16x16 fractal matrices in row 2 are also merged.

    In this case, N must be configured as a multiple of 64.

    ![](../../../../figures/Fixpipe_int4_channel_merge1.png)

-   FP32 channel split:

    For the target type FP32, the fractal cube can be converted from 16x16 to 16x8 by hardware. If Channel split is enabled, each 16x16 fractal cube is split into two 16x8 fractal cubes.

    As shown in the following figure, here the target data type is FP32, M is 64, and N is 32. It is split into 16 16x8 fractals.

    ![](../../../../figures/Fixpipe_F32_channel_split1.png)

## AIC-AIV Inter-Core Communication<a name="section1965819584325"></a>

This architecture supports AIC:AIV inter-core communication at ratios of 1:1 and 1:2. Inter-core communication is performed through SSBuf, which differs from the NPU220 architecture, where inter-core communication is completed through GM.

![](../../../../figures/1b2.png)

## Synchronization Control<a name="section41753664816"></a>

-   Intra-core synchronization

    Because the execution units inside the AI Core (such as the MTE2 data movement unit and the Vector compute unit) run in an asynchronous and parallel manner, data dependencies may exist when reading from and writing to Local Memory (such as the Unified Buffer). To ensure data consistency and computation correctness, synchronization control is required to coordinate the operation timing.

    Take the process in which MTE2 moves data from GM to the UB, the Vector compute unit performs the Abs computation, and then the data is moved back to GM as an example. The following synchronization conditions must be met:

    1.  Data movement and computation order
        -   Start the Abs computation of the Vector unit only after the GM→UB data movement is complete (to avoid data loss caused by starting the computation before the movement is complete);
        -   Perform the UB→GM data movement only after the Vector computation is complete (to ensure that the result data is ready).

    2.  Synchronization rules for loop-based data movement and computation scenarios
        -   Start new data movement only after the preceding computation is complete: when the previous computation is not complete, new data movement must not be triggered (to prevent old data in the UB from being overwritten);
        -   Start new computation only after the preceding data has been moved out: when the previous data has not been completely moved out of the UB, new computation tasks must not be triggered (to avoid overwrite conflicts in the target memory area).

    The synchronization control process is shown in the following figure:

    ![](../../../../figures/0000000_intra_core_sync_final_23.png)

    In the preceding figure, ID1, ID2, ID3, ID4, ID5, and ID6 represent event IDs (EventIDs). Each EventID corresponds to the movement status of a block of stored data, ensuring the correctness and consistency of data operations.

    Note the following points:

    -   It is recommended that you obtain an EventID through the [AllocEventID](../../../../../api/SIMD-API/basic_api/resource_management/TPipe/AllocEventID.md) or [FetchEventID](../../../../../api/SIMD-API/basic_api/resource_management/TPipe/FetchEventID.md) API to ensure its validity and effectiveness.
    -   The number of EventIDs is limited. After use, call ReleaseEventID immediately to release the resource to avoid EventID exhaustion, which may affect normal system running.
    -   SetFlag and WaitFlag must be used in pairs, and their parameters (including template parameters and event IDs) must be exactly the same. If they do not match, computation on the current core may become abnormal, or operator execution on the next core may be affected, causing a timeout issue.

        For example, `SetFlag<HardEvent::S_MTE3>(1)` and `SetFlag<HardEvent::MTE3_MTE1>(1)` do not set the same EventID because their template parameters are different. Only when both the template parameters and the event ID are exactly the same do they represent the same EventID.

    -   Do not set the same EventID consecutively, because this may cause the event status to become confused or fail to be processed correctly.
    -   It is not recommended to manually insert a TEventID. Do not manually insert TEventIDs 6 and 7, because they may be reserved by the system or used for special purposes.

-   Inter-core synchronization

    When different cores operate on the same global memory, data dependency issues such as read-after-write, write-after-read, and write-after-write may occur, requiring inter-core synchronization control.

    Inter-core synchronization control is divided into the following modes, as shown in the following figure:

    -   Mode 0: Synchronization control between AI Core cores. For the AIC scenario, all AIC cores are synchronized. The instructions following CrossCoreWaitFlag are executed only after all AIC cores have executed CrossCoreSetFlag. For the AIV scenario, all AIV cores are synchronized. The instructions following CrossCoreWaitFlag are executed only after all AIV cores have executed CrossCoreSetFlag.
    -   Mode 1: Synchronization control between AIV cores within an AI Core. The instructions following CrossCoreWaitFlag are executed only after both AIV cores have executed CrossCoreSetFlag.
    -   Mode 2: Synchronization control between AIC and AIV within an AI Core (1:2). After the AIC core executes CrossCoreSetFlag, the instructions following CrossCoreWaitFlag on the two AIVs continue to execute. After both AIVs execute CrossCoreSetFlag, the instructions following CrossCoreWaitFlag on the AIC can be executed.
    -   Mode 4: Synchronization control between AIC and AIV within an AI Core (1:1). AIV0 and AIV1 can independently trigger the AIC to wait. For example, after the AIC core executes CrossCoreSetFlag, the instructions following CrossCoreWaitFlag on AIV0 continue to execute. After AIV0 executes CrossCoreSetFlag, the instructions following CrossCoreWaitFlag on the AIC can be executed.

    ![](../../../../figures/inter_core_sync.png)

    For example, after the AIC moves the computation result in L0C to GM, the AIV needs to move the data in GM to UB. In this case, you can use the CrossCoreSetFlag and CrossCoreWaitFlag commands to ensure that the data is moved from GM to UB only after it is successfully moved from L0C to GM. The process is shown in the following figure.

    ![](../../../../figures/new_inter_core_sync_1.png)

    The [CrossCoreSetFlag](../../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md) and [CrossCoreWaitFlag](../../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md) APIs are used together. When using them, you need to pass in the inter-core synchronization flag ID \(flagId\), that is, ID1 in the preceding figure. Each ID corresponds to a counter with an initial value of 0. After CrossCoreSetFlag is executed, the counter corresponding to the ID increases by 1. When CrossCoreWaitFlag is executed, if the corresponding counter value is 0, execution is blocked. If the corresponding counter is greater than 0, the counter decreases by 1 and subsequent instructions start to execute. The value range of flagId is 0 to 10.

    Note the following points:

    -   **Use in pairs**

        CrossCoreSetFlag and CrossCoreWaitFlag must be used in pairs. Otherwise, operator timeout may occur.

    -   **Consistency requirement**

        The template parameters and flagId of CrossCoreSetFlag must be exactly the same as those of CrossCoreWaitFlag. Otherwise, they are regarded as different flagIds. For example, CrossCoreSetFlag<0x0, PIPE\_MTE3\>\(0x8\) and CrossCoreSetFlag<0x2, PIPE\_FIX\>\(0x8\) do not set the same flagId.

    -   **Avoiding consecutive setting**

        Consecutive setting of the same flagId is not allowed, to prevent counter state confusion.

    -   **Conflict with high-level API usage**

        The internal implementation of the Matmul high-level API uses this API for inter-core synchronization control. Therefore, it is not recommended that developers use this API and the Matmul high-level API at the same time. Otherwise, there is a risk of flagId conflict.

    -   **Counter limit**

        The counter of the same flagId can be set up to 15 times.

    -   **Default pipeline type**

        CrossCoreWaitFlag does not require explicitly setting the pipeline type of the instruction. PIPE\_S is used by default.
