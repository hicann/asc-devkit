# 性能调优<a name="ZH-CN_TOPIC_0000002529073929"></a>

## 性能采集与分析工具<a name="section20894115120010"></a>

通过毕昇编译器编译生成可执行程序后，使用msOpProf工具采集Ascend C算子在AI处理器上执行的性能数据，进行性能精细调优。

[msOpProf工具](https://gitcode.com/Ascend/msopprof/blob/master/docs/zh/user_guide/msopprof_user_guide.md)包含msopprof和msopprof simulator两种使用方式，协助用户定位算子内存、算子代码以及算子指令的异常，实现全方位的算子调优。两种使用方式的详细说明请参考表1。

**表1 msopprof和msopprof simulator功能说明表**

| 功能名称 | 适用场景 | 使用方式 | 展示的图形 |
|--|--|--| -- |
| msopprof | 适用于上板性能分析（实际运行环境中的性能分析），可协助用户定位算子内存和性能瓶颈。 | 直接分析运行中的算子，无需额外配置，适合在板环境中快速定位算子性能问题。 | 计算内存热力图<br>Roofline瓶颈分析图<br>Cache热力图<br>通算流水图<br>算子代码热点图|
|msopprof simulator| 适用于仿真性能分析，可协助用户分析算子指令和代码热点问题。|需要参考msopprof simulator配置，配置环境变量（如LD_LIBRARY_PATH）和编译选项（如添加-g生成调试信息），适合在仿真环境中详细分析算子行为。|指令流水图<br>算子代码热点图<br>内存通路吞吐率波形图|

## NPU域上板性能分析<a name="section107378341021"></a>

算子程序通过毕昇编译器编译生成可执行程序后，可以通过msopprof在NPU上完成性能采集。以SIMD编程场景为例，使用msOpProf工具采集上板性能数据大致步骤如下：

1.  参考[AI Core SIMD编译](../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md)，编译add算子样例，生成可执行文件。

    dav-后为NPU架构版本号，请根据实际情况进行替换，各产品型号对应的架构版本号请通过[对应关系表](../language_extension/simd_builtin_keywords.md#npu-arch)进行查询。

    ```
    bisheng add_custom.asc -o add_custom --npu-arch=dav-2201   
    ```

2.  使用msopprof调用算子可执行文件进行性能采集。

    ```
    msopprof ./add_custom
    ```

3.  查看性能数据，了解当前算子性能瓶颈。

    ```
    性能数据文件夹结构示例：
    OPPROF_{timestamp}_XXX
    ├──dump                       # 原始的性能数据，用户无需关注
    ├──ArithmeticUtilization.csv  # cube/vector指令cycle占比，建议优化算子逻辑，减少冗余计算指令
    ├──L2Cache.csv                # L2 Cache命中率，影响MTE2，建议合理规划数据搬运逻辑，增加命中率
    ├──Memory.csv                 # UB，L1和主存储器读写带宽速率，单位GB/s
    ├──MemoryL0.csv               # L0A，L0B，和L0C读写带宽速率，单位GB/s
    ├──MemoryUB.csv               # Vector和Scalar到UB的读写带宽速率，单位GB/s
    ├──OpBasicInfo.csv            # 算子基础信息
    ├──PipeUtilization.csv        # pipe类指令耗时和占比，建议优化数据搬运逻辑，提高带宽利用率
    ├──ResourceConflictRatio.csv  # UB上的bank group、bank conflict和资源冲突率在所有指令中的占比，建议减少/避免对于同一个bank读写冲突或bank group的读读冲突
    └──visualize_data.bin         # MindStudio Insight呈现文件
    ```
    
**表2 msopprof文件介绍**
<a name="table244174535419"></a>
<table><thead align="left"><tr id="row17451145145411"><th class="cellrowborder" valign="top" width="24.54%" id="mcps1.2.3.1.1"><p id="p18451345115414"><a name="p18451345115414"></a><a name="p18451345115414"></a>名称</p>
</th>
<th class="cellrowborder" valign="top" width="75.46000000000001%" id="mcps1.2.3.1.2"><p id="p4451245115415"><a name="p4451245115415"></a><a name="p4451245115415"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row194514510541"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p04514519549"><a name="p04514519549"></a><a name="p04514519549"></a>dump文件夹</p>
</td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p6451456545"><a name="p6451456545"></a><a name="p6451456545"></a>原始的性能数据，用户无需关注。
</td>
</tr>
<tr id="row74518458542"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p14520459540"><a name="p14520459540"></a><a name="p14520459540"></a>ArithmeticUtilization.csv</p>
</td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p145114515419"><a name="p145114515419"></a><a name="p145114515419"></a>Cube和Vector类型的指令耗时和占比，可参考ArithmeticUtilization（Cube及Vector类型指令耗时和占比）。</p>
</td>
</tr>
<tr id="row124534535416"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p144516455548"><a name="p144516455548"></a><a name="p144516455548"></a>L2Cache.csv</p>
</td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p845134505416"><a name="p845134505416"></a><a name="p845134505416"></a>L2 Cache命中率，可参考L2Cache（L2 Cache命中率）。
</td>
</tr>
<tr id="row124534535416"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p144516455548"><a name="p144516455548"></a><a name="p144516455548"></a>Memory.csv</p>
</td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p845134505416"><a name="p845134505416"></a><a name="p845134505416"></a>UB/L1/L2/主存储器采集内存读写带宽速率，可参考Memory（内存读写带宽速率）。
</td>
</tr>

<tr id="row124534535416"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p144516455548"><a name="p144516455548"></a><a name="p144516455548"></a>MemoryL0.csv</p>
</td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p845134505416"><a name="p845134505416"></a><a name="p845134505416"></a>L0A/L0B/L0C采集内存读写带宽速率，可参考MemoryL0（L0读写带宽速率）。
</td>
</tr>

<tr id="row124534535416"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p144516455548"><a name="p144516455548"></a><a name="p144516455548"></a>MemoryUB.csv</p>
</td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p845134505416"><a name="p845134505416"></a><a name="p845134505416"></a>mte/vector/scalar采集ub读写带宽速率，可参考MemoryUB（UB读写带宽速率）。
</td>
</tr>

<tr id="row124534535416"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p144516455548"><a name="p144516455548"></a><a name="p144516455548"></a>PipeUtilization.csv</p>
</td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p845134505416"><a name="p845134505416"></a><a name="p845134505416"></a>采集计算单元和搬运单元耗时和占比，可参考PipeUtilization（计算单元和搬运单元耗时占比）。
</td>
</tr>

<tr id="row124534535416"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p144516455548"><a name="p144516455548"></a><a name="p144516455548"></a>ResourceConflictRatio.csv</p>
</td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p845134505416"><a name="p845134505416"></a><a name="p845134505416"></a>UB上的bank group、bank conflict和资源冲突在所有指令中的占比，可参考ResourceConflictRatio（资源冲突占比）。
</td>
</tr>

<tr id="row124534535416"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p144516455548"><a name="p144516455548"></a><a name="p144516455548"></a>OpBasicInfo.csv</p>
</td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p845134505416"><a name="p845134505416"></a><a name="p845134505416"></a>算子基础信息，包含算子名称、block dim和耗时等信息，可参考OpBasicInfo（算子基础信息）。
</td>
</tr>

<tr id="row124534535416"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p144516455548"><a name="p144516455548"></a><a name="p144516455548"></a>visualize_data.bin</p>
</td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p845134505416"><a name="p845134505416"></a><a name="p845134505416"></a>算子基础信息、计算单元负载、热点函数和Roofline瓶颈分析等信息的可视化呈现文件。visualize_data.bin可通过MindStudio Insight工具进行可视化展示。
</td>
</tr>
<tr id="row124534535416"><td class="cellrowborder" valign="top" width="24.54%" headers="mcps1.2.3.1.1 "><p id="p144516455548"><a name="p144516455548"></a><a name="p144516455548"></a>trace.json</p>
</td>
<td class="cellrowborder" valign="top" width="75.46000000000001%" headers="mcps1.2.3.1.2 "><p id="p845134505416"><a name="p845134505416"></a><a name="p845134505416"></a>通算流水可视化呈现文件，Chrome浏览器具体请参考通算流水图。
</td>
</tr>
</tbody>
</table>

对于SIMT编程场景，只需遵循[AI Core SIMT编译](../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md)指导进行算子编译，生成可执行文件后，按照上述步骤2和步骤3使用msOpProf工具执行程序，以获取算子执行的性能数据。

## NPU域仿真性能分析<a name="section75259502193"></a>

在非昇腾设备上，通过毕昇编译器仿真编译后生成可执行程序，可以通过msopprof simulator完成性能流水仿真。当前仅支持SIMD编程场景，SIMT编程场景不支持。

-   **通过CMake方式仿真编译算子**
    -   灵活控制不同的target是否开启仿真编译。修改CMakeList，使用target\_link\_libraries与target\_link\_directories手动配置链接库与路径：

        ```
        find_package(ASC REQUIRED)
        project(kernel_samples LANGUAGES ASC)
        
        add_executable(demo
            add_custom.asc
        )
        
        set_target_properties(demo PROPERTIES 
            LINK_FLAGS "-Wl,--disable-new-dtags"  ## 由于仿真库会依赖当前目录的其他非链接的so，需要开启RPATH传递依赖目录
        )
        
        target_link_libraries(demo PRIVATE
            runtime_camodel npu_drv_camodel  ## 仿真编译需要链接的so
            m
        )
        
        # ${INSTALL_DIR}请替换为CANN软件安装后文件存储路径。以root用户安装为例，安装后文件默认存储路径为：/usr/local/Ascend/cann。
        target_link_directories(demo PRIVATE
            ${INSTALL_DIR}/tools/simulator/dav_2201/lib  ##仿真库所在的目录，其中dav_2201目录名称与芯片版本相关 
        )
        
        target_compile_options(demo PRIVATE
            $<$<COMPILE_LANGUAGE:ASC>: --npu-arch=dav-2201>
        )
        ```

        其中仿真库所在目录与NPU架构版本号之间的关系如下，目录名使用**下划线**连接“dav”和架构版本号。

        其中仿真编译所依赖的库介绍如下，开启仿真编译时，需要优先链接，确保优先使用仿真库的符号，防止出现运行时coredump等异常情况。

        **表3 仿真编译依赖库介绍**

        <a name="table84131311195116"></a>
        <table><thead align="left"><tr id="row4413911115113"><th class="cellrowborder" valign="top" width="45.97%" id="mcps1.2.3.1.1"><p id="p1841381120511"><a name="p1841381120511"></a><a name="p1841381120511"></a>名称</p>
        </th>
        <th class="cellrowborder" valign="top" width="54.03%" id="mcps1.2.3.1.2"><p id="p15413191116515"><a name="p15413191116515"></a><a name="p15413191116515"></a>作用描述</p>
        </th>
        </tr>
        </thead>
        <tbody><tr id="row94131311115113"><td class="cellrowborder" valign="top" width="45.97%" headers="mcps1.2.3.1.1 "><p id="p8413161115114"><a name="p8413161115114"></a><a name="p8413161115114"></a>libruntime_camodel.so</p>
        </td>
        <td class="cellrowborder" valign="top" width="54.03%" headers="mcps1.2.3.1.2 "><p id="p2041381120511"><a name="p2041381120511"></a><a name="p2041381120511"></a>camodel仿真运行时库，负责NPU算子仿真环境的运行时功能支持。</p>
        </td>
        </tr>
        <tr id="row841315115517"><td class="cellrowborder" valign="top" width="45.97%" headers="mcps1.2.3.1.1 "><p id="p16413211185110"><a name="p16413211185110"></a><a name="p16413211185110"></a>libnpu_drv_camodel.so</p>
        </td>
        <td class="cellrowborder" valign="top" width="54.03%" headers="mcps1.2.3.1.2 "><p id="p18413161165117"><a name="p18413161165117"></a><a name="p18413161165117"></a>仿真驱动库，对真实的驱动接口打桩，模拟真实NPU驱动行为，提供硬件交付的接口仿真。</p>
        </td>
        </tr>
        </tbody>
        </table>

    -   通过向CMake传入变量CMAKE\_ASC\_RUN\_MODE和CMAKE\_ASC\_ARCHITECTURES来统一开启仿真编译。命令示例如下：

        sim表示开启仿真编译，dav-后为NPU架构版本号，请根据实际情况进行填写。

        ```
        cmake -B build -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201
        ```

        >[!NOTE]说明
        >使用命令行往CMake传入变量的方式全局生效，会对CMakeList中所有的target开启sim模式。

-   **通过命令行方式仿真编译算子可执行程序**

    ```
    # 编译算子: bisheng  [算子源文件] -o [输出产物名称] --npu-arch=[NPU架构版本号]，--run-mode=sim
    bisheng add_custom.asc -o add_custom --npu-arch=dav-2201 --run-mode=sim   

    ```

-   **性能流水仿真**

    使用msopprof simulator并获取仿真数据。

    ```
    msopprof simulator --soc-version=Ascendxxxyy ./add_custom
    ```
    
    仿真数据说明

    ```
    OPPROF_{timestamp}_XXX
    ├──dump                    # 原始的性能数据，用户无需关注
    └──simulator               # 算子基础信息
       ├──core0.cubecore0
       ├──...
       ├──core23.cubecore0
       ├──trace.json           # Edge/Chrome Trace Viewer/Perfetto呈现文件
       └──visualize_data.bin   # MindStudio Insight呈现文件
    ```
**表4 msopprof simulator文件介绍**
| 名称 | 说明 |
|--|--|
| dump文件夹 | 原始仿真生成的dump数据存放文件夹。 |
| core*_code_exe.csv | 代码行耗时，*代表0~n核，以便用户快速确定编写的代码中最耗时的部分，可参考代码行耗时数据文件。 |
| core*_instr_exe.csv|  代码指令详细信息，*代表0~n核，以便用户快速确定最耗时的指令，可参考代码指令信息文件。 |
| visualize_data.bin | 仿真流水图和仿真热点函数等信息可视化呈现文件。
|trace.json | 仿真指令流水图文件，包括每个核的子文件以及全部核的汇总文件，可参考指令流水图和内存通路吞吐率波形图。 |

更多详细的信息可以参考[《msOpProf用户指南》](https://gitcode.com/Ascend/msopprof/blob/master/docs/zh/user_guide/msopprof_user_guide.md)。
