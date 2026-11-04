# Performance Tuning<a name="ZH-CN_TOPIC_0000002529073929"></a>

<!-- md-trans-meta sourceCommit=f83e857605e3844db8b9de0442007cc115278a47 translatedAt=2026-10-23T15:10:28.343Z pushedAt=2026-11-03T07:05:13.464Z nl=2 -->

## Performance Collection and Analysis Tool<a name="section20894115120010"></a>

After an executable program is generated through compilation with the BiSheng Compiler, use the msOpProf tool to collect the performance data of Ascend C operators executed on the AI processor for fine-grained performance tuning.

The [msOpProf tool](https://gitcode.com/Ascend/msopprof/blob/26.2.0/docs/en/user_guide/msopprof_user_guide.md) provides two usage modes, msopprof and msopprof simulator, to help you locate anomalies in operator memory, operator code, and operator instructions, enabling comprehensive operator tuning. For details about the two usage modes, see Table 1.

**Table 1** Function description of msopprof and msopprof simulator

| Function Name | Application Scenario | Usage | Displayed Graph |
|--|--|--| -- |
| msopprof | Applicable to on-board performance analysis (performance analysis in the actual running environment), helping you locate operator memory and performance bottlenecks. | Directly analyzes running operators without additional configuration, suitable for quickly locating operator performance issues in the on-board environment. | Compute memory heatmap<br>Roofline bottleneck analysis graph<br>Cache heatmap<br>General-purpose pipeline graph<br>Operator code hotspot graph|
|msopprof simulator| Applicable to simulation performance analysis, helping you analyze operator instructions and code hotspot issues.|Requires reference to the msopprof simulator configuration, configuring environment variables (such as LD_LIBRARY_PATH) and compilation options (such as adding -g to generate debugging information), suitable for detailed analysis of operator behavior in the simulation environment.|Instruction pipeline graph<br>Operator code hotspot graph<br>Memory path throughput rate waveform graph|

## On-Board Performance Analysis in the NPU Domain<a name="section107378341021"></a>

After the operator program is compiled by the BiSheng Compiler to generate an executable program, you can use msOpProf to collect performance data on the NPU. Taking the SIMD programming scenario as an example, the general steps for collecting on-board performance data using the msOpProf tool are as follows:

1.  Refer to [AI Core SIMD Compilation](../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md) to compile the add operator sample and generate an executable file.

    The value after dav- is the NPU architecture version. Replace it with the actual value. For the architecture version corresponding to each product model, see the [mapping table](../language_extension/simd_builtin_keywords.md#npu-arch).

    ```
    bisheng add_custom.asc -o add_custom --npu-arch=dav-2201   
    ```

2.  Use msOpProf to call the operator executable file for performance collection.

    ```
    msopprof ./add_custom
    ```

3.  View the performance data to identify the current operator performance bottleneck.

    ```
    Example of the performance data folder structure:
    OPPROF_{timestamp}_XXX
    ├──dump                       # Raw performance data, which you do not need to pay attention to
    ├──ArithmeticUtilization.csv  # Proportion of cube/vector instruction cycles. It is recommended to optimize the operator logic to reduce redundant computation instructions
    ├──L2Cache.csv                # L2 cache hit rate, which affects MTE2. It is recommended to properly plan the data transfer logic to increase the hit rate
    ├──Memory.csv                 # Read/write bandwidth rate of Unified Buffer (UB), L1, and main memory, in GB/s
    ├──MemoryL0.csv               # Read/write bandwidth rate of L0A, L0B, and L0C, in GB/s
    ├──MemoryUB.csv               # Read/write bandwidth rate from Vector and Scalar to UB, in GB/s
    ├──OpBasicInfo.csv            # Operator basics information
    ├──PipeUtilization.csv        # Time consumption and proportion of pipe instructions. It is recommended to optimize the data transfer logic to improve bandwidth utilization.
    ├──ResourceConflictRatio.csv  # Proportion of bank group, bank conflict, and resource conflict rates on the UB among all instructions. It is recommended to reduce or avoid read/write conflicts on the same bank or read/read conflicts on the same bank group.
    └──visualize_data.bin         # File for presentation in MindStudio Insight
    ```
    
**Table 2** msopprof files
<a name="table244174535419"></a>
<table><thead align="left"><tr id="row17451145145411"><th class="cellrowborder" valign="top" width="24.54%" id="mcps1.2.3.1.1"><p id="p18451345115414"><a name="p18451345115414"></a><a name="p18451345115414"></a>Name</p></th>
<th class="cellrowborder" valign="top" width="75.46000000000001%" id="mcps1.2.3.1.2"><p id="p4451245115415"><a name="p4451245115415"></a><a name="p4451245115415"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row194514510541"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p04514519549"><a name="p04514519549"></a><a name="p04514519549"></a>dump folder</p></td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p6451456545"><a name="p6451456545"></a><a name="p6451456545"></a>Raw performance data. Users do not need to pay attention to it.
</p></td>
</tr>
<tr id="row74518458542"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p14520459540"><a name="p14520459540"></a><a name="p14520459540"></a>ArithmeticUtilization.csv</p></td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p145114515419"><a name="p145114515419"></a><a name="p145114515419"></a>Time consumption and proportion of Cube and Vector instructions. For details, see ArithmeticUtilization (time consumption and proportion of Cube and Vector instructions).</p></td>
</tr>
<tr id="row124534535416"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p144516455548"><a name="p144516455548"></a><a name="p144516455548"></a>L2Cache.csv</p></td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p845134505416"><a name="p845134505416"></a><a name="p845134505416"></a>L2 Cache hit rate. For details, see L2Cache (L2 Cache hit rate).
</p></td>
</tr>
<tr id="row124534535416"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p144516455548"><a name="p144516455548"></a><a name="p144516455548"></a>Memory.csv</p></td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p845134505416"><a name="p845134505416"></a><a name="p845134505416"></a>Collects the memory read/write bandwidth rate of UB/L1/L2/main memory. For details, see Memory (memory read/write bandwidth rate).
</p></td>
</tr>

<tr id="row124534535416"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p144516455548"><a name="p144516455548"></a><a name="p144516455548"></a>MemoryL0.csv</p></td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p845134505416"><a name="p845134505416"></a><a name="p845134505416"></a>Collects the memory read/write bandwidth rate of L0A/L0B/L0C. For details, see MemoryL0 (L0 read/write bandwidth rate).
</p></td>
</tr>

<tr id="row124534535416"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p144516455548"><a name="p144516455548"></a><a name="p144516455548"></a>MemoryUB.csv</p></td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p845134505416"><a name="p845134505416"></a><a name="p845134505416"></a>Collects the UB read/write bandwidth rate of mte/vector/scalar. For details, see MemoryUB (UB read/write bandwidth rate).
</p></td>
</tr>

<tr id="row124534535416"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p144516455548"><a name="p144516455548"></a><a name="p144516455548"></a>PipeUtilization.csv</p></td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p845134505416"><a name="p845134505416"></a><a name="p845134505416"></a>Collects the duration and proportion of compute units and transfer units. For details, see PipeUtilization (duration and proportion of compute units and transfer units).
</p></td>
</tr>

<tr id="row124534535416"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p144516455548"><a name="p144516455548"></a><a name="p144516455548"></a>ResourceConflictRatio.csv</p></td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p845134505416"><a name="p845134505416"></a><a name="p845134505416"></a>Proportion of bank group, bank conflict, and resource conflict on the UB among all instructions. For details, see ResourceConflictRatio.
</p></td>
</tr>

<tr id="row124534535416"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p144516455548"><a name="p144516455548"></a><a name="p144516455548"></a>OpBasicInfo.csv</p></td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p845134505416"><a name="p845134505416"></a><a name="p845134505416"></a>Operator basics, including the operator name, block dim, and duration. For details, see OpBasicInfo.
</p></td>
</tr>

<tr id="row124534535416"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p144516455548"><a name="p144516455548"></a><a name="p144516455548"></a>visualize_data.bin</p></td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p845134505416"><a name="p845134505416"></a><a name="p845134505416"></a>File that visualizes operator basics, compute unit load, hotspot functions, and Roofline bottleneck analysis. visualize_data.bin can be visualized using the MindStudio Insight tool.
</p></td>
</tr>
<tr id="row124534535416"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p144516455548"><a name="p144516455548"></a><a name="p144516455548"></a>trace.json</p></td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p845134505416"><a name="p845134505416"></a><a name="p845134505416"></a>File that visualizes the general-purpose pipeline. For details about the Chrome browser, see the general-purpose pipeline diagram.
</p></td>
</tr>
</tbody>
</table>

For the SIMT programming scenario, you only need to follow the [AI Core SIMT compilation](../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md) guide to compile the operator code. After the executable file is generated, use the msOpProf tool to run the program by following step 2 and step 3 above to obtain the performance data of operator execution.

## NPU Domain Simulation Performance Analysis<a name="section75259502193"></a>

On non-Ascend devices, an executable program is generated after simulation compilation using the BiSheng Compiler, and performance pipeline simulation can be completed through msopprof simulator. Currently, only the SIMD programming scenario is supported; the SIMT programming scenario is not supported.

-   **Simulation compilation of operators through CMake**
    -   Flexibly control whether simulation compilation is enabled for different targets. Modify CMakeList and use target\_link\_libraries and target\_link\_directories to manually configure the link libraries and paths:

        ```
        find_package(ASC REQUIRED)
        project(kernel_samples LANGUAGES ASC)
        
        add_executable(demo
            add_custom.asc
        )
        
        set_target_properties(demo PROPERTIES 
            LINK_FLAGS "-Wl,--disable-new-dtags"  ## Because the simulation library depends on other non-linked .so files in the current directory, enable RPATH to pass the dependency directory
        )
        
        target_link_libraries(demo PRIVATE
            runtime_camodel npu_drv_camodel  ## .so files to be linked for simulation compilation
            m
        )
        
        # Replace ${INSTALL_DIR} with the path where CANN software is installed. For example, if you install CANN as the root user, the default installation path is /usr/local/Ascend/cann.
        target_link_directories(demo PRIVATE
            ${INSTALL_DIR}/tools/simulator/dav_2201/lib  ## Directory where the simulation libraries are located. The directory name dav_2201 is related to the chip version. 
        )
        
        target_compile_options(demo PRIVATE
            $<$<COMPILE_LANGUAGE:ASC>: --npu-arch=dav-2201>
        )
        ```

        The relationship between the directory where the simulation library is located and the NPU architecture version number is as follows. The directory name uses an **underscore** to connect "dav" and the architecture version number.

        The libraries that simulation compilation depends on are described as follows. When simulation compilation is enabled, they need to be linked first to ensure that the symbols of the simulation library are used preferentially, preventing exceptions such as runtime coredump.

        **Table 3** Simulation compilation dependency libraries

        <a name="table84131311195116"></a>
        <table><thead align="left"><tr id="row4413911115113"><th class="cellrowborder" valign="top" width="45.97%" id="mcps1.2.3.1.1"><p id="p1841381120511"><a name="p1841381120511"></a><a name="p1841381120511"></a>Name</p>
        </th>
        <th class="cellrowborder" valign="top" width="54.03%" id="mcps1.2.3.1.2"><p id="p15413191116515"><a name="p15413191116515"></a><a name="p15413191116515"></a>Description</p>
        </th>
        </tr>
        </thead>
        <tbody><tr id="row94131311115113"><td class="cellrowborder" valign="top" width="45.97%" headers="mcps1.2.3.1.1 "><p id="p8413161115114"><a name="p8413161115114"></a><a name="p8413161115114"></a>libruntime_camodel.so</p>
        </td>
        <td class="cellrowborder" valign="top" width="54.03%" headers="mcps1.2.3.1.2 "><p id="p2041381120511"><a name="p2041381120511"></a><a name="p2041381120511"></a>camodel simulation runtime library, which provides runtime function support for the NPU operator simulation environment.</p>
        </td>
        </tr>
        <tr id="row841315115517"><td class="cellrowborder" valign="top" width="45.97%" headers="mcps1.2.3.1.1 "><p id="p16413211185110"><a name="p16413211185110"></a><a name="p16413211185110"></a>libnpu_drv_camodel.so</p>
        </td>
        <td class="cellrowborder" valign="top" width="54.03%" headers="mcps1.2.3.1.2 "><p id="p18413161165117"><a name="p18413161165117"></a><a name="p18413161165117"></a>Simulation driver library, which stubs the real driver APIs, simulates the behavior of the real NPU driver, and provides interface simulation for hardware delivery.</p>
        </td>
        </tr>
        </tbody>
        </table>

    -   Enable simulation compilation uniformly by passing the variables CMAKE\_ASC\_RUN\_MODE and CMAKE\_ASC\_ARCHITECTURES to CMake. The following is an example command:

        sim indicates that simulation compilation is enabled. The value after dav- is the NPU architecture version. Set it based on the actual situation.

        ```
        cmake -B build -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201
        ```

        >[!NOTE] Note
        > Passing variables to CMake through the command line takes effect globally and enables sim mode for all targets in the CMakeList.

-   **Build the operator executable program through simulation compilation on the command line**

    ```
    # Compile the operator: bisheng  [operator source file] -o [output artifact name] --npu-arch=[NPU architecture version], --run-mode=sim
    bisheng add_custom.asc -o add_custom --npu-arch=dav-2201 --run-mode=sim   

    ```

-   **Performance pipeline simulation**

    Use msopprof simulator and obtain the simulation data.

    ```
    msopprof simulator --soc-version=Ascendxxxyy ./add_custom
    ```
    
    Simulation data description

    ```
    OPPROF_{timestamp}_XXX
    ├──dump                    # Raw performance data, which users do not need to care about
    └──simulator               # Operator basics
       ├──core0.cubecore0
       ├──...
       ├──core23.cubecore0
       ├──trace.json           # File for presentation in Edge/Chrome Trace Viewer/Perfetto
       └──visualize_data.bin   # File for presentation in MindStudio Insight
    ```
**Table 4** msopprof simulator files

| Name | Description |
|--|--|
| dump folder | Folder that stores the dump data generated by the original simulation. |
| core*_code_exe.csv | Time consumed by each code line. * indicates cores 0 to n. This file helps you quickly identify the most time-consuming part of the code you write. For details, see the code line time-consumption data file. |
| core*_instr_exe.csv|  Detailed information about code instructions. * indicates cores 0 to n. This file helps you quickly identify the most time-consuming instructions. For details, see the code instruction information file. |
| visualize_data.bin | File that visualizes information such as simulation pipeline diagrams and simulation hotspot functions. |trace.json | Simulation instruction pipeline diagram file, including the subfile of each core and the summary file of all cores. For details, see the instruction pipeline diagram and memory path throughput waveform diagram. |

For more details, see [msOpProf User Guide](https://gitcode.com/Ascend/msopprof/blob/26.2.0/docs/en/user_guide/msopprof_user_guide.md).
