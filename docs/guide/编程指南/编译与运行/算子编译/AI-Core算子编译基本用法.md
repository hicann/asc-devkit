# AI Core编译基本用法<a name="section229217121411"></a>
当开发者完成一个Kernel核函数的编写，并在Host侧通过<<<\>\>\>方式完成核函数的启动调用后，就需要进行算子源码编译与算子运行，接下来我们介绍通过bisheng编译器完成算子编译的基本用法。

## AI Core SIMD编译
使用bisheng编译Ascend C源文件的基本命令如下，其中--npu-arch=dav-\<npu architecture\>用于指定AI处理器架构版本：
```shell
  bisheng <source_file>.asc -o <output_file> --npu-arch=dav-<npu architecture>
```

bisheng可以通过-I \<path\>指定头文件路径、-L \<path\> 指定链接库路径、-l \<library\>指定需要链接的动态库或静态库、-D\<macro\>=\<value\>指定宏定义，示例如下：
```shell
  bisheng add_kernel.asc -I <path_to_include> -L <path_to_library> -l <library> -o <output_file> -D<macro>=<value> --npu-arch=dav-<npu architecture>
```

AI Core SIMD的基本编译流程如下：Host代码使用Host编译器编译成Host二进制，AI Core SIMD代码分为Cube代码和Vector代码，需分别编译成Cube二进制和Vector二进制，先将Cube二进制和Vector二进制链接成Fatbin文件，再与Host二进制合并生成可执行二进制。
![aicore编译流程示意图](../../../figures/aicore_compilation.png)

- 异构编译
  ```shell
  // ----- add_kernel.asc -----
  //  指定核函数在AI Core上执行
  __global__ __vector__ add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z)

  bisheng -c add_kernel.asc -o add_kernel.o --npu-arch=dav-xxxx
  bisheng -c main.cpp -o main.o -I${INSTALL_DIR}/include
  bisheng add_kernel.o main.o -o main
  // 或
  bisheng main.cpp add_kernel.asc -o main --npu-arch=dav-xxxx
  ```
  > [!NOTE]说明
  > ${INSTALL_DIR}请替换为CANN软件安装后文件存储路径。以root用户安装为例，安装后文件默认存储路径为：/usr/local/Ascend/cann。
  
- 单独编译

  bisheng异构编译默认采用的是全程序编译模式，该模式要求单个源文件X.asc中编译的设备程序没有任何未解析的外部设备函数和变量引用。设备函数可以调用其他编译单元中定义的设备函数或访问其中定义的设备变量，但必须在bisheng命令行中指定`-dc`编译选项，才能启用不同编译单元间设备代码的链接功能。这种支持跨编译单元链接设备代码与符号的能力，被称为单独编译。

  单独编译能实现更灵活的代码组织、缩短编译耗时，并生成体积更小的可执行文件；相较于全程序编译，它会增加一定的编译构建复杂度。此外，设备代码链接可能会影响程序性能，这也是该模式未被设为默认模式的原因。链接时优化（LTO）可有效降低单独编译带来的性能损耗。

  单独编译的强制要求：
  - 在一个编译单元中定义的非常量设备变量，在其他编译单元中引用时，必须使用extern关键字声明；
  - 所有常量设备变量，定义和跨单元引用时都必须使用extern关键字；
  - 所有Ascend C源文件（.asc）都必须使用`-dc`选项编译。

  下方示例中，add_compute.asc定义了变量和函数，add_kernel.asc对其进行引用；两个文件将分别编译，最终链接为完整可执行文件。
  ```c++
  // ------- add_compute.asc -------
  __gm__ int dev_var = 5;
  __aicore__ void add_compute(...);

  // ------- add_kernel.asc -------
  extern __gm__ int dev_var;
  __aicore__  void add_compute(...);
  __global__  __vector__ void add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z) {
    dev_var = 0;
    add_compute(...);
  }
  ```

  ```shell
  bisheng -dc add_compute.asc -o add_compute.o --npu-arch=dav-xxxx
  bisheng -dc add_kernel.asc -o add_kernel.o --npu-arch=dav-xxxx
  bisheng -c main.cpp -o main.o -I${INSTALL_DIR}/include
  bisheng add_compute.o add_kernel.o main.o -o program
  ```
  
  > [!NOTE]说明
  > ${INSTALL_DIR}请替换为CANN软件安装后文件存储路径。以root用户安装为例，安装后文件默认存储路径为：/usr/local/Ascend/cann。
  
- 动态库编译
   ```shell
  bisheng -shared add_kernel.asc -o libadd_kernel.so -fPIC --npu-arch=dav-xxxx
  ```

- 静态库编译
  ```shell
  bisheng -lib add_kernel.asc -o libadd_kernel.a --npu-arch=dav-xxxx
  ```

## AI Core SIMT编译

使用bisheng编译器编译Ascend C SIMT源文件时，需要在AI Core SIMD编译的基本命令上增加`--enable-simt`，示例如下：

```shell
  bisheng <source_file>.asc -o <output_file> --npu-arch=dav-<npu architecture> --enable-simt
```

AI Core SIMT的基本编译流程如下：Host代码使用Host编译器编译成Host二进制，AI Core SIMT代码编译成SIMT二进制，先将SIMT二进制链接成Fatbin文件，再与Host二进制合并生成可执行二进制。
![aicore simt编译流程示意图](../../../figures/aicore_simt_comilation.png)

## 基本编译命令汇总
|编译方式|AI Core SIMD编译命令| AI Core SIMT编译命令|
|-|-|-|
|异构编译| bisheng <source_file>.asc -o \<output_file\> --npu-arch=dav-\<npu architecture\> | bisheng <source_file>.asc -o <output_file> --npu-arch=dav-\<npu architecture\> **--enable-simt**|
|独立编译|bisheng **-dc** <source_file>.asc -o \<output_file\>**.o** --npu-arch=dav-\<npu architecture\>| bisheng **-dc** <source_file>.asc -o <output_file>**.o** --npu-arch=dav-\<npu architecture\> **--enable-simt**|
|动态库编译|bisheng **-shared** <source_file>.asc -o \<output_file\>**.so** --npu-arch=dav-\<npu architecture\> **-fPIC**| bisheng **-shared** <source_file>.asc -o <output_file>**.so** --npu-arch=dav-\<npu architecture\> **--enable-simt** **-fPIC**|
|静态库编译|bisheng **-lib** <source_file>.asc -o \<output_file\>**.a** --npu-arch=dav-\<npu architecture\>| bisheng **-lib** <source_file>.asc -o <output_file>**.a** --npu-arch=dav-\<npu architecture\> **--enable-simt**|


## 常用的编译选项<a name="ZH-CN_TOPIC_0000002462746461"></a>
<a name="zh-cn_topic_0000001856506454_table2716123812212"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001856506454_row13875038172111"><th class="cellrowborder" valign="top" width="33.63636363636363%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001856506454_p38753382219"><a name="zh-cn_topic_0000001856506454_p38753382219"></a><a name="zh-cn_topic_0000001856506454_p38753382219"></a><strong id="zh-cn_topic_0000001856506454_b138752389214"><a name="zh-cn_topic_0000001856506454_b138752389214"></a><a name="zh-cn_topic_0000001856506454_b138752389214"></a>选项</strong></p>
</th>
<th class="cellrowborder" valign="top" width="9.676767676767676%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001856506454_p0875138132119"><a name="zh-cn_topic_0000001856506454_p0875138132119"></a><a name="zh-cn_topic_0000001856506454_p0875138132119"></a><strong id="zh-cn_topic_0000001856506454_b2875113818214"><a name="zh-cn_topic_0000001856506454_b2875113818214"></a><a name="zh-cn_topic_0000001856506454_b2875113818214"></a>是否必需</strong></p>
</th>
<th class="cellrowborder" valign="top" width="56.686868686868685%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001856506454_p168751138102115"><a name="zh-cn_topic_0000001856506454_p168751138102115"></a><a name="zh-cn_topic_0000001856506454_p168751138102115"></a><strong id="zh-cn_topic_0000001856506454_b198756387215"><a name="zh-cn_topic_0000001856506454_b198756387215"></a><a name="zh-cn_topic_0000001856506454_b198756387215"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001856506454_row18875183852112"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001856506454_p1987573810215"><a name="zh-cn_topic_0000001856506454_p1987573810215"></a><a name="zh-cn_topic_0000001856506454_p1987573810215"></a>-help</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001856506454_p587515381215"><a name="zh-cn_topic_0000001856506454_p587515381215"></a><a name="zh-cn_topic_0000001856506454_p587515381215"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001856506454_p138752383219"><a name="zh-cn_topic_0000001856506454_p138752383219"></a><a name="zh-cn_topic_0000001856506454_p138752383219"></a>查看帮助。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001856506454_row20875103872119"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p1068416586398"><a name="p1068416586398"></a><a name="p1068416586398"></a>--npu-arch</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p106841658103918"><a name="p106841658103918"></a><a name="p106841658103918"></a>是</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p1268425803914"><a name="p1268425803914"></a><a name="p1268425803914"></a>编译时指定的<span id="ph17911124171120"><a name="ph17911124171120"></a><a name="ph17911124171120"></a>AI处理器</span>架构，取值为dav-&lt;arch-version&gt;，其中&lt;arch-version&gt;为NPU架构版本号，各产品型号对应的架构版本号请通过<a href="../../语言扩展层/SIMD-BuiltIn关键字.md#table65291052154114">对应关系表</a>进行查询。</p>
</td>
</tr>
<tr id="row15492131482712"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p4492414142719"><a name="p4492414142719"></a><a name="p4492414142719"></a>--npu-soc</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p15492151411271"><a name="p15492151411271"></a><a name="p15492151411271"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p11492114142717"><a name="p11492114142717"></a><a name="p11492114142717"></a>编译时指定的<span id="ph12176116185918"><a name="ph12176116185918"></a><a name="ph12176116185918"></a>AI处理器</span>型号，npu-soc和npu-arch同时配置时，优先采用npu-arch的配置。</p>
<p id="p15587811201611"><a name="p15587811201611"></a><a name="p15587811201611"></a><span id="ph1195412562597"><a name="ph1195412562597"></a><a name="ph1195412562597"></a>AI处理器</span>的型号请通过如下方式获取：</p>
<a name="ul1124912113117"></a><a name="ul1124912113117"></a><ul id="ul1124912113117"><li>针对如下产品：在安装<span id="zh-cn_topic_0000001265392790_ph196874123168"><a name="zh-cn_topic_0000001265392790_ph196874123168"></a><a name="zh-cn_topic_0000001265392790_ph196874123168"></a>AI处理器</span>的服务器执行<strong id="zh-cn_topic_0000001265392790_b17687612191618"><a name="zh-cn_topic_0000001265392790_b17687612191618"></a><a name="zh-cn_topic_0000001265392790_b17687612191618"></a>npu-smi info</strong>命令进行查询，获取<strong id="zh-cn_topic_0000001265392790_b10161437131915"><a name="zh-cn_topic_0000001265392790_b10161437131915"></a><a name="zh-cn_topic_0000001265392790_b10161437131915"></a>Name</strong>信息。实际配置值为AscendName，例如<strong id="zh-cn_topic_0000001265392790_b16284944181920"><a name="zh-cn_topic_0000001265392790_b16284944181920"></a><a name="zh-cn_topic_0000001265392790_b16284944181920"></a>Name</strong>取值为<em id="zh-cn_topic_0000001265392790_i1478775919179"><a name="zh-cn_topic_0000001265392790_i1478775919179"></a><a name="zh-cn_topic_0000001265392790_i1478775919179"></a>xxxyy</em>，实际配置值为Ascend<em id="zh-cn_topic_0000001265392790_i1678775901719"><a name="zh-cn_topic_0000001265392790_i1678775901719"></a><a name="zh-cn_topic_0000001265392790_i1678775901719"></a>xxxyy</em>。<p id="zh-cn_topic_0000001265392790_p3529538154519"><a name="zh-cn_topic_0000001265392790_p3529538154519"></a><a name="zh-cn_topic_0000001265392790_p3529538154519"></a><span id="zh-cn_topic_0000001265392790_ph1483216010188"><a name="zh-cn_topic_0000001265392790_ph1483216010188"></a><a name="zh-cn_topic_0000001265392790_ph1483216010188"></a><term id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term11962195213215"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term11962195213215"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term11962195213215"></a>Atlas A2 训练系列产品</term>/<term id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term184716139811"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term184716139811"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term184716139811"></a>Atlas A2 推理系列产品</term></span></p>
<p id="zh-cn_topic_0000001265392790_p1193517020467"><a name="zh-cn_topic_0000001265392790_p1193517020467"></a><a name="zh-cn_topic_0000001265392790_p1193517020467"></a><span id="zh-cn_topic_0000001265392790_ph783112021813"><a name="zh-cn_topic_0000001265392790_ph783112021813"></a><a name="zh-cn_topic_0000001265392790_ph783112021813"></a><term id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term354143892110"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term354143892110"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term354143892110"></a>Atlas 200I/500 A2 推理产品</term></span></p>
<p id="zh-cn_topic_0000001265392790_p78319051815"><a name="zh-cn_topic_0000001265392790_p78319051815"></a><a name="zh-cn_topic_0000001265392790_p78319051815"></a><span id="zh-cn_topic_0000001265392790_ph1383116081815"><a name="zh-cn_topic_0000001265392790_ph1383116081815"></a><a name="zh-cn_topic_0000001265392790_ph1383116081815"></a><term id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term4363218112215"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term4363218112215"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term4363218112215"></a>Atlas 推理系列产品</term></span></p>
<p id="zh-cn_topic_0000001265392790_p38313021813"><a name="zh-cn_topic_0000001265392790_p38313021813"></a><a name="zh-cn_topic_0000001265392790_p38313021813"></a><span id="zh-cn_topic_0000001265392790_ph58317041819"><a name="zh-cn_topic_0000001265392790_ph58317041819"></a><a name="zh-cn_topic_0000001265392790_ph58317041819"></a><term id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term71949488213"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term71949488213"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term71949488213"></a>Atlas 训练系列产品</term></span></p>
</li><li>针对如下产品，在安装<span id="zh-cn_topic_0000001265392790_ph17911124171120"><a name="zh-cn_topic_0000001265392790_ph17911124171120"></a><a name="zh-cn_topic_0000001265392790_ph17911124171120"></a>AI处理器</span>的服务器执行<strong id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_b206066255591"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_b206066255591"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_b206066255591"></a>npu-smi info -t board -i </strong><em id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_i16609202515915"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_i16609202515915"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_i16609202515915"></a>id</em><strong id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_b14358631175910"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_b14358631175910"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_b14358631175910"></a> -c </strong><em id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_i16269732165915"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_i16269732165915"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_i16269732165915"></a>chip_id</em>命令进行查询，获取<strong id="zh-cn_topic_0000001265392790_b11257114917192"><a name="zh-cn_topic_0000001265392790_b11257114917192"></a><a name="zh-cn_topic_0000001265392790_b11257114917192"></a>Chip Name</strong>和<strong id="zh-cn_topic_0000001265392790_b72671651121916"><a name="zh-cn_topic_0000001265392790_b72671651121916"></a><a name="zh-cn_topic_0000001265392790_b72671651121916"></a>NPU Name</strong>信息，实际配置值为Chip Name_NPU Name。例如<strong id="zh-cn_topic_0000001265392790_b13136111611203"><a name="zh-cn_topic_0000001265392790_b13136111611203"></a><a name="zh-cn_topic_0000001265392790_b13136111611203"></a>Chip Name</strong>取值为Ascend<em id="zh-cn_topic_0000001265392790_i68701996189"><a name="zh-cn_topic_0000001265392790_i68701996189"></a><a name="zh-cn_topic_0000001265392790_i68701996189"></a>xxx</em>，<strong id="zh-cn_topic_0000001265392790_b51347352112"><a name="zh-cn_topic_0000001265392790_b51347352112"></a><a name="zh-cn_topic_0000001265392790_b51347352112"></a>NPU Name</strong>取值为1234，实际配置值为Ascend<em id="zh-cn_topic_0000001265392790_i82901912141813"><a name="zh-cn_topic_0000001265392790_i82901912141813"></a><a name="zh-cn_topic_0000001265392790_i82901912141813"></a>xxx</em><em id="zh-cn_topic_0000001265392790_i154501458102213"><a name="zh-cn_topic_0000001265392790_i154501458102213"></a><a name="zh-cn_topic_0000001265392790_i154501458102213"></a>_</em>1234。其中：<a name="zh-cn_topic_0000001265392790_ul2747601334"></a><a name="zh-cn_topic_0000001265392790_ul2747601334"></a><ul id="zh-cn_topic_0000001265392790_ul2747601334"><li>id：设备id，通过<strong id="zh-cn_topic_0000001265392790_b83171930133314"><a name="zh-cn_topic_0000001265392790_b83171930133314"></a><a name="zh-cn_topic_0000001265392790_b83171930133314"></a>npu-smi info -l</strong>命令查出的NPU ID即为设备id。</li><li>chip_id：芯片id，通过<strong id="zh-cn_topic_0000001265392790_b18888204343317"><a name="zh-cn_topic_0000001265392790_b18888204343317"></a><a name="zh-cn_topic_0000001265392790_b18888204343317"></a>npu-smi info -m</strong>命令查出的Chip ID即为芯片id。</li></ul>
<p id="zh-cn_topic_0000001265392790_p1790216395447"><a name="zh-cn_topic_0000001265392790_p1790216395447"></a><a name="zh-cn_topic_0000001265392790_p1790216395447"></a><span id="zh-cn_topic_0000001265392790_ph2272194216543"><a name="zh-cn_topic_0000001265392790_ph2272194216543"></a><a name="zh-cn_topic_0000001265392790_ph2272194216543"></a>Ascend 950PR/Ascend 950DT</span></p>
<p id="zh-cn_topic_0000001265392790_p12136131554410"><a name="zh-cn_topic_0000001265392790_p12136131554410"></a><a name="zh-cn_topic_0000001265392790_p12136131554410"></a><span id="zh-cn_topic_0000001265392790_ph13754548217"><a name="zh-cn_topic_0000001265392790_ph13754548217"></a><a name="zh-cn_topic_0000001265392790_ph13754548217"></a><term id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term1253731311225"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term1253731311225"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term1253731311225"></a>Atlas A3 训练系列产品</term>/<term id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term131434243115"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term131434243115"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term131434243115"></a>Atlas A3 推理系列产品</term></span></p>
</li></ul>
</td>
</tr>
<tr id="row463118312408"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p172818333408"><a name="p172818333408"></a><a name="p172818333408"></a>-x</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p112810333405"><a name="p112810333405"></a><a name="p112810333405"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p1147317623814"><a name="p1147317623814"></a><a name="p1147317623814"></a>指定编译语言,如：-x asc,表示指定为Ascend C编程语言。</p>
</td>
</tr>
<tr id="row95881839113910"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p1752513455396"><a name="p1752513455396"></a><a name="p1752513455396"></a>-o &lt;file&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p1052564512393"><a name="p1052564512393"></a><a name="p1052564512393"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p14525174511393"><a name="p14525174511393"></a><a name="p14525174511393"></a>指定输出文件的名称和位置。</p>
</td>
</tr>
<tr id="row14244162782319"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p12244627192312"><a name="p12244627192312"></a><a name="p12244627192312"></a>-c</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p14244162712317"><a name="p14244162712317"></a><a name="p14244162712317"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p1824452712312"><a name="p1824452712312"></a><a name="p1824452712312"></a>编译生成目标文件。</p>
</td>
</tr>
<tr id="row14244162782319"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p12244627192312"><a name="p12244627192312"></a><a name="p12244627192312"></a>-dc</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p14244162712317"><a name="p14244162712317"></a><a name="p14244162712317"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p1824452712312"><a name="p1824452712312"></a><a name="p1824452712312"></a>编译生成relocatable目标文件。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000001856506454_row11875938112115"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p3893173113412"><a name="p3893173113412"></a><a name="p3893173113412"></a>-shared，--shared</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p98931836342"><a name="p98931836342"></a><a name="p98931836342"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001856506454_p128758389219"><a name="zh-cn_topic_0000001856506454_p128758389219"></a><a name="zh-cn_topic_0000001856506454_p128758389219"></a>编译生成动态链接库。</p>
</td>
</tr>
<tr id="row149091846161211"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p19268319526"><a name="p19268319526"></a><a name="p19268319526"></a>-lib，--cce-build-static-lib</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p690994614128"><a name="p690994614128"></a><a name="p690994614128"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p5909184615121"><a name="p5909184615121"></a><a name="p5909184615121"></a>编译生成静态链接库。编译器会将Device侧的代码进行编译链接，生成Device侧二进制文件，随后将该文件作为Host侧编译的输入进行编译，最后链接生成静态链接库。</p>
</td>
</tr>
<tr id="row8304638153012"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p183051238103015"><a name="p183051238103015"></a><a name="p183051238103015"></a>-g</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p18305153833013"><a name="p18305153833013"></a><a name="p18305153833013"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p230573823018"><a name="p230573823018"></a><a name="p230573823018"></a>编译时增加调试信息。</p>
</td>
</tr>
<tr id="row168573353120"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p1085713312317"><a name="p1085713312317"></a><a name="p1085713312317"></a>--sanitizer</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p685743143118"><a name="p685743143118"></a><a name="p685743143118"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p1961764455616"><a name="p1961764455616"></a><a name="p1961764455616"></a>编译时增加代码正确性校验信息。使用sanitizer选项时，需要同步添加-g选项，且不能在-O0场景下使用。</p>
<p id="p485717333110"><a name="p485717333110"></a><a name="p485717333110"></a>注意，启用该选项后GlobalTensor默认使用L2 Cache，无法通过AscendC::SetL2CacheHint接口设置不使用L2 Cache的模式。</p>
</td>
</tr>
<tr id="row236103412553"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p114041048134211"><a name="p114041048134211"></a><a name="p114041048134211"></a>-fPIC</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p34047482426"><a name="p34047482426"></a><a name="p34047482426"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p940474884218"><a name="p940474884218"></a><a name="p940474884218"></a>告知编译器产生位置无关代码。</p>
</td>
</tr>
<tr id="row1757181163816"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p1854419204220"><a name="p1854419204220"></a><a name="p1854419204220"></a>-O</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p2054319164211"><a name="p2054319164211"></a><a name="p2054319164211"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001856506454_p1187523802118"><a name="zh-cn_topic_0000001856506454_p1187523802118"></a><a name="zh-cn_topic_0000001856506454_p1187523802118"></a>用于指定编译器的优化级别，当前支持-O3，-O2，-O0。</p>
</td>
</tr>
<tr id="row16786622645"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p1584686172114"><a name="p1584686172114"></a><a name="p1584686172114"></a><span>--run-mode=</span>sim</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p158467611219"><a name="p158467611219"></a><a name="p158467611219"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p08911647152012"><a name="p08911647152012"></a><a name="p08911647152012"></a>sim模式：链接时用户添加仿真模式对应的实现库，实现代码在仿真模式下运行，可以查看仿真相关日志，方便用户性能调试。</p>
</td>
</tr>
<tr id="row109271176132"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p1092701741311"><a name="p1092701741311"></a><a name="p1092701741311"></a>--enable-simt</p>
</td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p49274178137"><a name="p49274178137"></a><a name="p49274178137"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p139275178134"><a name="p139275178134"></a><a name="p139275178134"></a>SIMT编程场景，指定SIMT方式编译。</p>
</td>
</tr>
</tbody>
</table>

更多的编译命令和用法可以参见[《毕昇编译器用户指南》](https://www.hiascend.com/document/redirect/CannCommunityBiSheng)。

## CMake方式编译<a name="ZH-CN_TOPIC_0000002428982142"></a>

项目中可以使用CMake来更简便地使用毕昇编译器编译Ascend C算子，生成可执行文件、动态库、静态库或二进制文件。

以下是CMake脚本的示例及其核心步骤说明：

```cmake
# 1、设置编译配置变量
# CMAKE_ASC_ARCHITECTURES：指定NPU架构版本，当前默认值为dav-2201，可通过-DCMAKE_ASC_ARCHITECTURES=dav-xxxx覆盖默认值
set(CMAKE_ASC_ARCHITECTURES "dav-2201" CACHE STRING "NPU architecture")

# 2、find_package(ASC)是CMake中用于查找和配置Ascend C编译工具链的命令
find_package(ASC)

# 3、指定项目支持的语言包括ASC，ASC表示支持使用毕昇编译器对Ascend C编程语言进行编译
project(kernel_samples LANGUAGES ASC)

# 4、使用CMake接口编译可执行文件、动态库、静态库、二进制文件
add_executable(demo
    add_custom.asc
)
```

编译AI Core SIMT代码时，需要在编译选项中增加`--enable-simt`，或通过`CMAKE_ASC_ENABLE_SIMT`设置为ON启用SIMT编译模式，示例如下，完整样例请参考：[LINK](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/00_introduction/00_quickstart/hello_world_simt)。

```cmake
# CMAKE_ASC_ARCHITECTURES：指定NPU架构版本，可通过-DCMAKE_ASC_ARCHITECTURES=dav-xxxx覆盖默认值
set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU architecture")
# CMAKE_ASC_ENABLE_SIMT：启用SIMT编译模式，需要在find_package(ASC)前设置
set(CMAKE_ASC_ENABLE_SIMT ON)

find_package(ASC)
project(kernel_samples LANGUAGES ASC)

add_executable(demo
    add_custom.asc
)
```

以下是动态库、静态库编译示例，同时展示如何将源文件切换为用语言ASC编译：

-   编译.asc文件生成动态库

    ```cmake
    # 3、使用CMake接口编译动态库
    add_library(kernel_shared SHARED
        cube.asc
        vector.asc
        mix.asc
    )
    ```

-   编译.asc文件生成静态库

    ```cmake
    # 3、使用CMake接口编译静态库
    add_library(kernel_lib STATIC
          cube.asc
          vector.asc
          mix.asc
    )
    ```
**表1常用的CMAKE配置变量说明**
| 变量名称 | 配置说明 |
|--|--|
| CMAKE_BUILD_TYPE | 编译模式选项，可配置为：Release或Debug。Release版本，不包含调试信息，编译最终发布的版本。Debug版本，包含调试信息，便于开发者开发和调试。配置其他值时CMake会发出警告，但不中止构建。 |
| CMAKE_ASC_COMPILER | 指定Ascend C的编译器路径，默认由CMake自动探测。路径不存在时配置阶段报错终止。 |
| CMAKE_ASC_SOURCE_FILE_EXTENSIONS | ASC源文件扩展名列表，默认为asc。 |
| CMAKE_ASC_STANDARD | 指定编译使用的C++标准版本，默认值为17。设置低于17的值时CMake会发出警告。 |
| CMAKE_ASC_ARCHITECTURES | 指定目标NPU架构版本，取值为`dav-<arch-version>`（如dav-2201、dav-3510）。NPU运行模式下，CMake会自动将该值以`--npu-arch=<value>`注入编译命令。 |
| CMAKE_ASC_RUN_MODE | 指定算子运行模式，支持npu（默认，在NPU设备上运行）、cpu（CPU调试模式）、sim（仿真模式）。不同模式下CMake会自动调整链接库和编译选项。 |
| CMAKE_ASC_FLAGS | 为所有构建类型（Debug，Release等）添加的通用编译选项。 |
| CMAKE_ASC_FLAGS\_DEBUG | 专用于Debug构建的编译选项，默认为`-O0 -g`。 |
| CMAKE_ASC_FLAGS\_RELEASE | 专用于Release构建的编译选项，默认为`-O3 -DNDEBUG`。 |
| CMAKE_ASC_COMPILER_LAUNCHER | 编译命令的前置启动器（如ccache、distcc），未设置时直接调用编译器。 |
| CMAKE_ASC_LINKER_LAUNCHER | 链接命令的前置启动器，未设置时直接执行链接。 |
| CMAKE_ASC_COMPILER_AR | 静态库归档工具，默认使用系统ar工具。设置后，静态库的归档命令将使用该工具替代默认ar。 |
| CMAKE_ASC_COMPILER_LINKER | 链接驱动，默认使用bisheng编译器作为链接驱动，仅在有明确替换需求时设置。 |
| CMAKE_ASC_ENABLE_SIMT | 是否启用SIMT编译模式，设置为ON时在编译命令中自动注入--enable-simt选项，默认为OFF。 |
| CMAKE_INSTALL_PREFIX | 为CMake的内置变量，用于指定CMake执行install命令时，安装的路径前缀。|

>[!CAUTION]注意
> 在CMake工程中，影响Ascend C语言初始化、编译器探测和编译规则生成的变量，
> 需要在`find_package(ASC)`前设置，或在执行`cmake`配置命令时通过`-D`指定。
> 这些变量在Ascend C语言初始化过程中会被读取，若在`find_package(ASC)`之后再设置，
> 可能不会影响已经生成的编译规则。

## 其他编译相关说明

### 内置编译宏开关<a name="section57020345148"></a>

内置编译宏开关列表如下：
 -   **ASCENDC\_DUMP** 用于控制Dump开关，默认开关打开，开发者调用printf/DumpTensor/assert后会有信息打印（需要注意直调工程的kernel文件内存在host函数，如果在host函数内调用了printf接口，也会触发kernel内的printf相关初始化动作，进而影响kernel的执行性能\)；设置为0后，表示开关关闭。示例如下：
     ```shell
     # 关闭所有算子的printf打印功能
     bisheng <source_file>.asc -o <output_file> --npu-arch=dav-<npu architecture> -DASCENDC_DUMP=0
     ```
 -   **ASCENDC\_DEBUG** 用于控制Ascend C API的调测开关，默认开关关闭；增加该编译宏后，表示开关打开，此时接口内部的assert校验生效，校验不通过会有assert日志打屏。开启该功能会对算子实际运行的性能带来一定影响，通常在调测阶段使用。示例如下：
     ```shell
     bisheng <source_file>.asc -o <output_file> --npu-arch=dav-<npu architecture> -DASCENDC_DEBUG
     ```
     当前ASCENDC\_DEBUG功能支持的产品型号为：
     Atlas 推理系列产品
     Atlas A2 训练系列产品/Atlas A2 推理系列产品

 -   **ENABLE\_CV\_COMM\_VIA\_SSBUF** 用于控制是否使用SSBuffer以及UB到L1 Buffer的硬通道，在涉及CV通信（AIC和AIV）或使用数据搬运API时需关注此选项。开启该选项可以提高相关API的性能或拓展使用更多功能。默认开关关闭；设置为true后，表示开关打开。示例如下：
     ```shell
     bisheng <source_file>.asc -o <output_file> --npu-arch=dav-<npu architecture> -DENABLE_CV_COMM_VIA_SSBUF=true
     ```
     仅在Ascend 950PR/Ascend 950DT支持该选项。
     -   从其它硬件平台移植到此平台的算子，开关默认关闭以保持兼容性。
     -   在该平台新开发的算子，以下场景需要打开：使用矩阵计算Matmul高阶API，且使用SetTensorScaleA等接口，这些接口属于Ascend 950PR/Ascend 950DT新增的功能，其内部实现使用了SSBuffer；使用DataCopy接口从UB拷贝数据到L1 Buffer。

 -   **NO\_OVERLAP\_IN\_MULTI\_REPEAT**
  该编译选项用于在没有地址重叠的情况下移除不必要的内存同步指令，以提升性能。针对Ascend 950PR/Ascend 950DT，使用基础API的高维切分计算API时，默认会插入内存同步指令以确保在地址重叠等复杂场景下的数据正确性，但这些同步指令会带来性能开销。在追求极致性能的场景下，如果您可以确定代码在任何情况下都不会发生内存重叠，可以使用此选项。


### 内置链接库<a name="section57020345148"></a>

毕昇编译器默认链接的库文件，列表如下：
<a name="table201231542115513"></a>
<table><thead align="left"><tr id="row171231542205510"><th class="cellrowborder" valign="top" width="23.98%" id="mcps1.2.3.1.1"><p id="p11123114295513"><a name="p11123114295513"></a><a name="p11123114295513"></a>名称</p>
</th>
<th class="cellrowborder" valign="top" width="76.02%" id="mcps1.2.3.1.2"><p id="p1412374225512"><a name="p1412374225512"></a><a name="p1412374225512"></a>作用描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row5123842135514"><td class="cellrowborder" valign="top" width="23.98%" headers="mcps1.2.3.1.1 "><p id="p1212364212559"><a name="p1212364212559"></a><a name="p1212364212559"></a>libascendc_runtime.a</p>
</td>
<td class="cellrowborder" valign="top" width="76.02%" headers="mcps1.2.3.1.2 "><p id="p1112394218551"><a name="p1112394218551"></a><a name="p1112394218551"></a>Ascend C算子参数等组装库。</p>
</td>
</tr>
<tr id="row612324285519"><td class="cellrowborder" valign="top" width="23.98%" headers="mcps1.2.3.1.1 "><p id="p01231423552"><a name="p01231423552"></a><a name="p01231423552"></a>libruntime.so</p>
</td>
<td class="cellrowborder" valign="top" width="76.02%" headers="mcps1.2.3.1.2 "><p id="p8123164255511"><a name="p8123164255511"></a><a name="p8123164255511"></a>Runtime运行库。</p>
</td>
</tr>
<tr id="row1612374285512"><td class="cellrowborder" valign="top" width="23.98%" headers="mcps1.2.3.1.1 "><p id="p2012315425551"><a name="p2012315425551"></a><a name="p2012315425551"></a>libprofapi.so</p>
</td>
<td class="cellrowborder" valign="top" width="76.02%" headers="mcps1.2.3.1.2 "><p id="p12123164265514"><a name="p12123164265514"></a><a name="p12123164265514"></a>Ascend C算子运行性能数据采集库。</p>
</td>
</tr>
<tr id="row10123134212552"><td class="cellrowborder" valign="top" width="23.98%" headers="mcps1.2.3.1.1 "><p id="p812374235515"><a name="p812374235515"></a><a name="p812374235515"></a>libunified_dlog.so</p>
</td>
<td class="cellrowborder" valign="top" width="76.02%" headers="mcps1.2.3.1.2 "><p id="p412314426554"><a name="p412314426554"></a><a name="p412314426554"></a>CANN日志收集库。</p>
</td>
</tr>
<tr id="row1012384210552"><td class="cellrowborder" valign="top" width="23.98%" headers="mcps1.2.3.1.1 "><p id="p15123104219559"><a name="p15123104219559"></a><a name="p15123104219559"></a>libmmpa.so</p>
</td>
<td class="cellrowborder" valign="top" width="76.02%" headers="mcps1.2.3.1.2 "><p id="p13123242135519"><a name="p13123242135519"></a><a name="p13123242135519"></a>CANN系统接口库。</p>
</td>
</tr>
<tr id="row17124154245516"><td class="cellrowborder" valign="top" width="23.98%" headers="mcps1.2.3.1.1 "><p id="p612484265518"><a name="p612484265518"></a><a name="p612484265518"></a>libascend_dump.so</p>
</td>
<td class="cellrowborder" valign="top" width="76.02%" headers="mcps1.2.3.1.2 "><p id="p101241842175512"><a name="p101241842175512"></a><a name="p101241842175512"></a>CANN维测信息库。</p>
</td>
</tr>
<tr id="row6124164213551"><td class="cellrowborder" valign="top" width="23.98%" headers="mcps1.2.3.1.1 "><p id="p111246426558"><a name="p111246426558"></a><a name="p111246426558"></a>libc_sec.so</p>
</td>
<td class="cellrowborder" valign="top" width="76.02%" headers="mcps1.2.3.1.2 "><p id="p31241442185512"><a name="p31241442185512"></a><a name="p31241442185512"></a>CANN安全函数库。</p>
</td>
</tr>
<tr id="row171241342175514"><td class="cellrowborder" valign="top" width="23.98%" headers="mcps1.2.3.1.1 "><p id="p6124124218556"><a name="p6124124218556"></a><a name="p6124124218556"></a>liberror_manager.so</p>
</td>
<td class="cellrowborder" valign="top" width="76.02%" headers="mcps1.2.3.1.2 "><p id="p61248424557"><a name="p61248424557"></a><a name="p61248424557"></a>CANN错误信息管理库。</p>
</td>
</tr>
<tr id="row512404213550"><td class="cellrowborder" valign="top" width="23.98%" headers="mcps1.2.3.1.1 "><p id="p151243425553"><a name="p151243425553"></a><a name="p151243425553"></a>libascendcl.so</p>
</td>
<td class="cellrowborder" valign="top" width="76.02%" headers="mcps1.2.3.1.2 "><p id="p1012424213555"><a name="p1012424213555"></a><a name="p1012424213555"></a>acl相关接口库。</p>
</td>
</tr>
</tbody>
</table>

### 高阶API常用链接库
在使用高阶API时，必须链接以下库，因为这些库是高阶API功能所依赖的。在其他场景下，可以根据具体需求选择是否链接这些库。
|链接库名称|作用描述|使用场景|动态库路径|
|--|--|--|--|
|libtiling_api.a|Tiling函数相关库。|使用高阶API相关的Tiling接口时需要链接。|${ASCEND_HOME_PATH}/lib64|
|libregister.so|Tiling函数相关库。|使用高阶API相关的Tiling接口时需要链接。|${ASCEND_HOME_PATH}/lib64|
|libgraph_base.so|基础数据结构和接口库。|调用ge::Shape，ge::DataType等基础结构体时需要链接。|${ASCEND_HOME_PATH}/lib64|
|libplatform.so|硬件平台信息库。|使用PlatformAscendC相关硬件平台信息接口时需要链接。|${ASCEND_HOME_PATH}/lib64|
