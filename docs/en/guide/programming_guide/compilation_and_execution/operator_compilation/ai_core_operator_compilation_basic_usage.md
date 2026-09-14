# AI Core Compilation Basics<a name="section229217121411"></a>

<!-- md-trans-meta sourceCommit=84d4771c758afa4058a229c863ddc58cf59e3dd3 translatedAt=2026-08-26T10:53:28.701Z pushedAt=2026-08-31T03:17:37.432Z -->

After a developer finishes writing a kernel function and launches it on the host side using the <<<\>\>\> syntax, the operator source code needs to be compiled and the operator needs to be run. This section describes the basic usage of compiling operators with the BiSheng Compiler.

## AI Core SIMD Compilation
The basic command for compiling Ascend C source files using BiSheng is as follows, where **--npu-arch=dav-\<npu architecture\>** specifies the AI processor architecture version:
```shell
  bisheng <source_file>.asc -o <output_file> --npu-arch=dav-<npu architecture>
```

BiSheng can use **-I \<path\>** to specify the header file path, **-L \<path\>** to specify the library path, **-l \<library\>** to specify the dynamic or static library to be linked, and **-D\<macro\>=\<value\>** to specify a macro definition. An example is as follows:
```shell
  bisheng add_kernel.asc -I <path_to_include> -L <path_to_library> -l <library> -o <output_file> -D<macro>=<value> --npu-arch=dav-<npu architecture>
```

The basic compilation process of AI Core SIMD is as follows: the host code is compiled into a host binary by the host compiler. The AI Core SIMD code is divided into cube code and vector code, which must be compiled into a cube binary and a vector binary respectively. The cube binary and vector binary are first linked into a Fatbin file, which is then merged with the host binary to generate the executable binary.
![AI Core compilation process](../../../figures/aicore_compilation.png)

- Heterogeneous compilation
  ```shell
  // ----- add_kernel.asc -----
  //  Specify that the kernel function runs on the AI Core.
  __global__ __vector__ add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z)

  bisheng -c add_kernel.asc -o add_kernel.o --npu-arch=dav-xxxx
  bisheng -c main.cpp -o main.o -I${INSTALL_DIR}/include
  bisheng add_kernel.o main.o -o main
  // or
  bisheng main.cpp add_kernel.asc -o main --npu-arch=dav-xxxx
  ```
  > [!NOTE] Note
  > Replace *${INSTALL_DIR}* with the actual storage path of the CANN software after installation. For example, when installed by the **root** user, the default storage path after installation is **/usr/local/Ascend/cann**.

- Standalone compilation

  BiSheng heterogeneous compilation uses the whole-program compilation mode by default. This mode requires that the device program compiled from a single source file X.asc has no unresolved external device function or variable references. A device function can call device functions defined in other compilation units or access device variables defined in them, but the `-dc` compilation option must be specified on the BiSheng command line to enable linking of device code across different compilation units. This capability of supporting cross-compilation-unit linking of device code and symbols is called standalone compilation.

Standalone compilation enables more flexible code organization, shorter compilation time, and smaller executable files. Compared with whole-program compilation, it adds a certain degree of complexity to the compilation and build process. In addition, device code linking may affect program performance, which is why this mode is not set as the default mode. Link-time optimization (LTO) can effectively reduce the performance loss introduced by standalone compilation.

Mandatory requirements for standalone compilation:
- A non-constant device variable defined in one compilation unit must be declared with the extern keyword when referenced in another compilation unit.
- All constant device variables must be declared with the extern keyword both when defined and when referenced across compilation units.
- All Ascend C source files (.asc) must be compiled with the `-dc` option.

In the following example, **add_compute.asc** defines variables and functions, and **add_kernel.asc** references them. The two files are compiled separately and finally linked into a complete executable file.
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

> [!NOTE] Note
> Replace *${INSTALL_DIR}* with the path where CANN software files are stored after installation. For example, if you install CANN as the **root** user, the default storage path is **/usr/local/Ascend/cann**.

- Dynamic library compilation
   ```shell
  bisheng -shared add_kernel.asc -o libadd_kernel.so -fPIC --npu-arch=dav-xxxx
  ```

- Static library compilation
  ```shell
  bisheng -lib add_kernel.asc -o libadd_kernel.a --npu-arch=dav-xxxx
  ```

## AI Core SIMT Compilation

When compiling Ascend C SIMT source files with the BiSheng Compiler, add `--enable-simt` to the basic command for AI Core SIMD compilation. The following is an example:

```shell
  bisheng <source_file>.asc -o <output_file> --npu-arch=dav-<npu architecture> --enable-simt
```

The basic compilation process of AI Core SIMT is as follows: the host code is compiled into a host binary by the host compiler, and the AI Core SIMT code is compiled into a SIMT binary. The SIMT binary is first linked into a Fatbin file, which is then merged with the host binary to generate an executable binary.
![AI Core SIMT compilation process](../../../figures/aicore_simt_comilation.png)

## Summary of Basic Compilation Commands
|Compilation Method|AI Core SIMD Compilation Command|AI Core SIMT Compilation Command|
|-|-|-|
|Heterogeneous compilation| bisheng <source_file>.asc -o \<output_file\> --npu-arch=dav-\<npu architecture\> | bisheng <source_file>.asc -o <output_file> --npu-arch=dav-\<npu architecture\> **--enable-simt**|
|Standalone compilation|bisheng **-dc** <source_file>.asc -o \<output_file\>**.o** --npu-arch=dav-\<npu architecture\>| bisheng **-dc** <source_file>.asc -o <output_file>**.o** --npu-arch=dav-\<npu architecture\> **--enable-simt**|
|Dynamic library compilation|bisheng **-shared** <source_file>.asc -o \<output_file\>**.so** --npu-arch=dav-\<npu architecture\> **-fPIC**| bisheng **-shared** <source_file>.asc -o <output_file>**.so** --npu-arch=dav-\<npu architecture\> **--enable-simt** **-fPIC**|
|Static library compilation|bisheng **-lib** <source_file>.asc -o \<output_file\>**.a** --npu-arch=dav-\<npu architecture\>| bisheng **-lib** <source_file>.asc -o <output_file>**.a** --npu-arch=dav-\<npu architecture\> **--enable-simt**|


## Common Compilation Options<a name="ZH-CN_TOPIC_0000002462746461"></a>
<a name="zh-cn_topic_0000001856506454_table2716123812212"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001856506454_row13875038172111"><th class="cellrowborder" valign="top" width="33.63636363636363%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001856506454_p38753382219"><a name="zh-cn_topic_0000001856506454_p38753382219"></a><a name="zh-cn_topic_0000001856506454_p38753382219"></a><strong id="zh-cn_topic_0000001856506454_b138752389214"><a name="zh-cn_topic_0000001856506454_b138752389214"></a><a name="zh-cn_topic_0000001856506454_b138752389214"></a>Option</strong></p></th>
<th class="cellrowborder" valign="top" width="9.676767676767676%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001856506454_p0875138132119"><a name="zh-cn_topic_0000001856506454_p0875138132119"></a><a name="zh-cn_topic_0000001856506454_p0875138132119"></a><strong id="zh-cn_topic_0000001856506454_b2875113818214"><a name="zh-cn_topic_0000001856506454_b2875113818214"></a><a name="zh-cn_topic_0000001856506454_b2875113818214"></a>Required</strong></p></th>
<th class="cellrowborder" valign="top" width="56.686868686868685%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001856506454_p168751138102115"><a name="zh-cn_topic_0000001856506454_p168751138102115"></a><a name="zh-cn_topic_0000001856506454_p168751138102115"></a><strong id="zh-cn_topic_0000001856506454_b198756387215"><a name="zh-cn_topic_0000001856506454_b198756387215"></a><a name="zh-cn_topic_0000001856506454_b198756387215"></a>Description</strong></p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001856506454_row18875183852112"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001856506454_p1987573810215"><a name="zh-cn_topic_0000001856506454_p1987573810215"></a><a name="zh-cn_topic_0000001856506454_p1987573810215"></a>-help</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001856506454_p587515381215"><a name="zh-cn_topic_0000001856506454_p587515381215"></a><a name="zh-cn_topic_0000001856506454_p587515381215"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001856506454_p138752383219"><a name="zh-cn_topic_0000001856506454_p138752383219"></a><a name="zh-cn_topic_0000001856506454_p138752383219"></a>View help.</p></td>
</tr>
<tr id="zh-cn_topic_0000001856506454_row20875103872119"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p1068416586398"><a name="p1068416586398"></a><a name="p1068416586398"></a>--npu-arch</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p106841658103918"><a name="p106841658103918"></a><a name="p106841658103918"></a>Yes</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p1268425803914"><a name="p1268425803914"></a><a name="p1268425803914"></a>Architecture of the <span id="ph17911124171120"><a name="ph17911124171120"></a><a name="ph17911124171120"></a>AI processor</span> specified during compilation. The value is dav-&lt;arch-version&gt;, where &lt;arch-version&gt; is the NPU architecture version number. For the architecture version number corresponding to each product model, see the <a href="../../language_extension/SIMD-BuiltIn_keyword.md#table65291052154114">mapping table</a>.</p></td>
</tr>
<tr id="row15492131482712"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p4492414142719"><a name="p4492414142719"></a><a name="p4492414142719"></a>--npu-soc</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p15492151411271"><a name="p15492151411271"></a><a name="p15492151411271"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p11492114142717"><a name="p11492114142717"></a><a name="p11492114142717"></a>Model of the <span id="ph12176116185918"><a name="ph12176116185918"></a><a name="ph12176116185918"></a>AI processor</span> specified during compilation. When both npu-soc and npu-arch are configured, the npu-arch configuration takes precedence.</p>
<p id="p15587811201611"><a name="p15587811201611"></a><a name="p15587811201611"></a>Obtain the model of the <span id="ph1195412562597"><a name="ph1195412562597"></a><a name="ph1195412562597"></a>AI processor</span> as follows:</p>
<a name="ul1124912113117"></a><a name="ul1124912113117"></a><ul id="ul1124912113117"><li>For the following products, run the <strong id="zh-cn_topic_0000001265392790_b17687612191618"><a name="zh-cn_topic_0000001265392790_b17687612191618"></a><a name="zh-cn_topic_0000001265392790_b17687612191618"></a>npu-smi info</strong> command on the server where the <span id="zh-cn_topic_0000001265392790_ph196874123168"><a name="zh-cn_topic_0000001265392790_ph196874123168"></a><a name="zh-cn_topic_0000001265392790_ph196874123168"></a>AI processor</span> is installed to query the <strong id="zh-cn_topic_0000001265392790_b10161437131915"><a name="zh-cn_topic_0000001265392790_b10161437131915"></a><a name="zh-cn_topic_0000001265392790_b10161437131915"></a>Name</strong> information. The actual configuration value is AscendName. For example, if the value of <strong id="zh-cn_topic_0000001265392790_b16284944181920"><a name="zh-cn_topic_0000001265392790_b16284944181920"></a><a name="zh-cn_topic_0000001265392790_b16284944181920"></a>Name</strong> is <em id="zh-cn_topic_0000001265392790_i1478775919179"><a name="zh-cn_topic_0000001265392790_i1478775919179"></a><a name="zh-cn_topic_0000001265392790_i1478775919179"></a>xxxyy</em>, the actual configuration value is Ascend<em id="zh-cn_topic_0000001265392790_i1678775901719"><a name="zh-cn_topic_0000001265392790_i1678775901719"></a><a name="zh-cn_topic_0000001265392790_i1678775901719"></a>xxxyy</em>.<p id="zh-cn_topic_0000001265392790_p3529538154519"><a name="zh-cn_topic_0000001265392790_p3529538154519"></a><a name="zh-cn_topic_0000001265392790_p3529538154519"></a><span id="zh-cn_topic_0000001265392790_ph1483216010188"><a name="zh-cn_topic_0000001265392790_ph1483216010188"></a><a name="zh-cn_topic_0000001265392790_ph1483216010188"></a><term id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term11962195213215"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term11962195213215"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term11962195213215"></a>Atlas A2 training products</term>/<term id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term184716139811"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term184716139811"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term184716139811"></a>Atlas A2 inference products</term></span></p>
<p id="zh-cn_topic_0000001265392790_p1193517020467"><a name="zh-cn_topic_0000001265392790_p1193517020467"></a><a name="zh-cn_topic_0000001265392790_p1193517020467"></a><span id="zh-cn_topic_0000001265392790_ph783112021813"><a name="zh-cn_topic_0000001265392790_ph783112021813"></a><a name="zh-cn_topic_0000001265392790_ph783112021813"></a><term id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term354143892110"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term354143892110"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term354143892110"></a>Atlas 200I/500 A2 inference products</term></span></p>
<p id="zh-cn_topic_0000001265392790_p78319051815"><a name="zh-cn_topic_0000001265392790_p78319051815"></a><a name="zh-cn_topic_0000001265392790_p78319051815"></a><span id="zh-cn_topic_0000001265392790_ph1383116081815"><a name="zh-cn_topic_0000001265392790_ph1383116081815"></a><a name="zh-cn_topic_0000001265392790_ph1383116081815"></a><term id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term4363218112215"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term4363218112215"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term4363218112215"></a>Atlas inference products</term></span></p>
<p id="zh-cn_topic_0000001265392790_p38313021813"><a name="zh-cn_topic_0000001265392790_p38313021813"></a><a name="zh-cn_topic_0000001265392790_p38313021813"></a><span id="zh-cn_topic_0000001265392790_ph58317041819"><a name="zh-cn_topic_0000001265392790_ph58317041819"></a><a name="zh-cn_topic_0000001265392790_ph58317041819"></a><term id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term71949488213"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term71949488213"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term71949488213"></a>Atlas training products</term></span></p>
</li><li>For the following products, run the <strong id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_b206066255591"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_b206066255591"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_b206066255591"></a>npu-smi info -t board -i </strong><em id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_i16609202515915"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_i16609202515915"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_i16609202515915"></a>id</em><strong id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_b14358631175910"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_b14358631175910"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_b14358631175910"></a> -c </strong><em id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_i16269732165915"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_i16269732165915"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001264656721_zh-cn_topic_0000001117597244_i16269732165915"></a>chip_id</em> command on the server where the <span id="zh-cn_topic_0000001265392790_ph17911124171120"><a name="zh-cn_topic_0000001265392790_ph17911124171120"></a><a name="zh-cn_topic_0000001265392790_ph17911124171120"></a>AI processor</span> is installed to query the <strong id="zh-cn_topic_0000001265392790_b11257114917192"><a name="zh-cn_topic_0000001265392790_b11257114917192"></a><a name="zh-cn_topic_0000001265392790_b11257114917192"></a>Chip Name</strong> and <strong id="zh-cn_topic_0000001265392790_b72671651121916"><a name="zh-cn_topic_0000001265392790_b72671651121916"></a><a name="zh-cn_topic_0000001265392790_b72671651121916"></a>NPU Name</strong> information. The actual configuration value is Chip Name_NPU Name. For example, if the value of <strong id="zh-cn_topic_0000001265392790_b13136111611203"><a name="zh-cn_topic_0000001265392790_b13136111611203"></a><a name="zh-cn_topic_0000001265392790_b13136111611203"></a>Chip Name</strong> is Ascend<em id="zh-cn_topic_0000001265392790_i68701996189"><a name="zh-cn_topic_0000001265392790_i68701996189"></a><a name="zh-cn_topic_0000001265392790_i68701996189"></a>xxx</em> and the value of <strong id="zh-cn_topic_0000001265392790_b51347352112"><a name="zh-cn_topic_0000001265392790_b51347352112"></a><a name="zh-cn_topic_0000001265392790_b51347352112"></a>NPU Name</strong> is 1234, the actual configuration value is Ascend<em id="zh-cn_topic_0000001265392790_i82901912141813"><a name="zh-cn_topic_0000001265392790_i82901912141813"></a><a name="zh-cn_topic_0000001265392790_i82901912141813"></a>xxx</em><em id="zh-cn_topic_0000001265392790_i154501458102213"><a name="zh-cn_topic_0000001265392790_i154501458102213"></a><a name="zh-cn_topic_0000001265392790_i154501458102213"></a>_</em>1234. Where:<a name="zh-cn_topic_0000001265392790_ul2747601334"></a><a name="zh-cn_topic_0000001265392790_ul2747601334"></a><ul id="zh-cn_topic_0000001265392790_ul2747601334"><li>id: device ID. The NPU ID queried by the <strong id="zh-cn_topic_0000001265392790_b83171930133314"><a name="zh-cn_topic_0000001265392790_b83171930133314"></a><a name="zh-cn_topic_0000001265392790_b83171930133314"></a>npu-smi info -l</strong> command is the device ID.</li><li>chip_id: chip ID. The Chip ID queried by the <strong id="zh-cn_topic_0000001265392790_b18888204343317"><a name="zh-cn_topic_0000001265392790_b18888204343317"></a><a name="zh-cn_topic_0000001265392790_b18888204343317"></a>npu-smi info -m</strong> command is the chip ID.</li></ul>
<p id="zh-cn_topic_0000001265392790_p1790216395447"><a name="zh-cn_topic_0000001265392790_p1790216395447"></a><a name="zh-cn_topic_0000001265392790_p1790216395447"></a><span id="zh-cn_topic_0000001265392790_ph2272194216543"><a name="zh-cn_topic_0000001265392790_ph2272194216543"></a><a name="zh-cn_topic_0000001265392790_ph2272194216543"></a>Ascend 950PR/Ascend 950DT</span></p>
<p id="zh-cn_topic_0000001265392790_p12136131554410"><a name="zh-cn_topic_0000001265392790_p12136131554410"></a><a name="zh-cn_topic_0000001265392790_p12136131554410"></a><span id="zh-cn_topic_0000001265392790_ph13754548217"><a name="zh-cn_topic_0000001265392790_ph13754548217"></a><a name="zh-cn_topic_0000001265392790_ph13754548217"></a><term id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term1253731311225"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term1253731311225"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term1253731311225"></a>Atlas A3 training products</term>/<term id="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term131434243115"><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term131434243115"></a><a name="zh-cn_topic_0000001265392790_zh-cn_topic_0000001312391781_term131434243115"></a>Atlas A3 inference products</term></span></p>
</li></ul></td>
</tr>
<tr id="row463118312408"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p172818333408"><a name="p172818333408"></a><a name="p172818333408"></a>-x</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p112810333405"><a name="p112810333405"></a><a name="p112810333405"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p1147317623814"><a name="p1147317623814"></a><a name="p1147317623814"></a>Specifies the compilation language. For example, -x asc specifies the Ascend C programming language.</p></td>
</tr>
<tr id="row95881839113910"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p1752513455396"><a name="p1752513455396"></a><a name="p1752513455396"></a>-o &lt;file&gt;</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p1052564512393"><a name="p1052564512393"></a><a name="p1052564512393"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p14525174511393"><a name="p14525174511393"></a><a name="p14525174511393"></a>Specifies the name and location of the output file.</p></td>
</tr>
<tr id="row14244162782319"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p12244627192312"><a name="p12244627192312"></a><a name="p12244627192312"></a>-c</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p14244162712317"><a name="p14244162712317"></a><a name="p14244162712317"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p1824452712312"><a name="p1824452712312"></a><a name="p1824452712312"></a>Compiles and generates an object file.</p></td>
</tr>
<tr id="row14244162782319"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p12244627192312"><a name="p12244627192312"></a><a name="p12244627192312"></a>-dc</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p14244162712317"><a name="p14244162712317"></a><a name="p14244162712317"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p1824452712312"><a name="p1824452712312"></a><a name="p1824452712312"></a>Compiles and generates a relocatable object file.</p></td>
</tr>
<tr id="zh-cn_topic_0000001856506454_row11875938112115"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p3893173113412"><a name="p3893173113412"></a><a name="p3893173113412"></a>-shared, --shared</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p98931836342"><a name="p98931836342"></a><a name="p98931836342"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001856506454_p128758389219"><a name="zh-cn_topic_0000001856506454_p128758389219"></a><a name="zh-cn_topic_0000001856506454_p128758389219"></a>Compiles and generates a dynamic link library.</p></td>
</tr>
<tr id="row149091846161211"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p19268319526"><a name="p19268319526"></a><a name="p19268319526"></a>-lib, --cce-build-static-lib</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p690994614128"><a name="p690994614128"></a><a name="p690994614128"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p5909184615121"><a name="p5909184615121"></a><a name="p5909184615121"></a>Compiles and generates a static library. The compiler compiles and links the code on the Device side to generate a Device-side binary file, then uses this file as the input for Host-side compilation, and finally links to generate a static library.</p></td>
</tr>
<tr id="row8304638153012"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p183051238103015"><a name="p183051238103015"></a><a name="p183051238103015"></a>-g</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p18305153833013"><a name="p18305153833013"></a><a name="p18305153833013"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p230573823018"><a name="p230573823018"></a><a name="p230573823018"></a>Adds debugging information during compilation.</p></td>
</tr>
<tr id="row168573353120"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p1085713312317"><a name="p1085713312317"></a><a name="p1085713312317"></a>--sanitizer</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p685743143118"><a name="p685743143118"></a><a name="p685743143118"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p1961764455616"><a name="p1961764455616"></a><a name="p1961764455616"></a>Adds code correctness verification information during compilation. When using the sanitizer option, you need to add the -g option at the same time, and it cannot be used in the -O0 scenario.</p>
<p id="p485717333110"><a name="p485717333110"></a><a name="p485717333110"></a>Note that after this option is enabled, GlobalTensor uses L2 Cache by default, and the mode of not using L2 Cache cannot be set through the AscendC::SetL2CacheHint API.</p></td>
</tr>
<tr id="row236103412553"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p114041048134211"><a name="p114041048134211"></a><a name="p114041048134211"></a>-fPIC</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p34047482426"><a name="p34047482426"></a><a name="p34047482426"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p940474884218"><a name="p940474884218"></a><a name="p940474884218"></a>Instructs the compiler to generate position-independent code.</p></td>
</tr>
<tr id="row1757181163816"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p1854419204220"><a name="p1854419204220"></a><a name="p1854419204220"></a>-O</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p2054319164211"><a name="p2054319164211"></a><a name="p2054319164211"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001856506454_p1187523802118"><a name="zh-cn_topic_0000001856506454_p1187523802118"></a><a name="zh-cn_topic_0000001856506454_p1187523802118"></a>Specifies the optimization level of the compiler. Currently, -O3, -O2, and -O0 are supported.</p></td>
</tr>
<tr id="row16786622645"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p1584686172114"><a name="p1584686172114"></a><a name="p1584686172114"></a><span>--run-mode=</span>sim</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p158467611219"><a name="p158467611219"></a><a name="p158467611219"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p08911647152012"><a name="p08911647152012"></a><a name="p08911647152012"></a>Simulation mode: during linking, the user adds the implementation library corresponding to the simulation mode so that the code runs in simulation mode. Simulation-related logs can be viewed, facilitating performance debugging.</p></td>
</tr>
<tr id="row109271176132"><td class="cellrowborder" valign="top" width="33.63636363636363%" headers="mcps1.1.4.1.1 "><p id="p1092701741311"><a name="p1092701741311"></a><a name="p1092701741311"></a>--enable-simt</p></td>
<td class="cellrowborder" valign="top" width="9.676767676767676%" headers="mcps1.1.4.1.2 "><p id="p49274178137"><a name="p49274178137"></a><a name="p49274178137"></a>No</p></td>
<td class="cellrowborder" valign="top" width="56.686868686868685%" headers="mcps1.1.4.1.3 "><p id="p139275178134"><a name="p139275178134"></a><a name="p139275178134"></a>For SIMT programming scenarios, specifies SIMT-based compilation.</p></td>
</tr>
</tbody>
</table>

For more compilation commands and usage, see [*BiSheng Compiler User Guide*](https://www.hiascend.com/document/redirect/CannCommunityBiSheng).

## CMake Compilation<a name="ZH-CN_TOPIC_0000002428982142"></a>

In a project, you can use CMake to compile Ascend C operators with the BiSheng Compiler more conveniently, generating executable files, dynamic libraries, static libraries, or binary files.

The following shows an example of a CMake script and the description of its core steps:

```cmake
# 1. Set compilation configuration variables.
# CMAKE_ASC_ARCHITECTURES: specifies the NPU architecture version. The current default value is dav-2201, which can be overridden by -DCMAKE_ASC_ARCHITECTURES=dav-xxxx.
set(CMAKE_ASC_ARCHITECTURES "dav-2201" CACHE STRING "NPU architecture").

# 2. find_package(ASC) is a CMake command used to locate and configure the Ascend C compilation toolchain.
find_package(ASC)

# 3. Specify that the languages supported by the project include ASC. ASC indicates that the BiSheng Compiler is used to compile the Ascend C programming language.
project(kernel_samples LANGUAGES ASC)

# 4. Use CMake interfaces to compile executable files, dynamic libraries, static libraries, and binary files.
add_executable(demo
    add_custom.asc
)
```

When compiling AI Core SIMT code, add `--enable-simt` to the compilation options, or set `CMAKE_ASC_ENABLE_SIMT` to ON to enable the SIMT compilation mode. An example is as follows. For the complete sample, click [here](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/00_introduction/00_quickstart/hello_world_simt).

```cmake
# CMAKE_ASC_ARCHITECTURES: specifies the NPU architecture version, which can be overridden by -DCMAKE_ASC_ARCHITECTURES=dav-xxxx.
set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU architecture")
# CMAKE_ASC_ENABLE_SIMT: enables the SIMT compilation mode. It must be set before find_package(ASC).
set(CMAKE_ASC_ENABLE_SIMT ON)

find_package(ASC)
project(kernel_samples LANGUAGES ASC)

add_executable(demo
    add_custom.asc
)
```

The following shows examples of dynamic library and static library compilation, and how to switch the source file to be compiled with the ASC language:

-   Compile the .asc file to generate a dynamic library.

    ```cmake
    # 3. Use the CMake API to compile a dynamic library.
    add_library(kernel_shared SHARED
        cube.asc
        vector.asc
        mix.asc
    )
    ```

-   Compile the .asc file to generate a static library.

    ```cmake
    # 3. Use the CMake API to compile a static library.
    add_library(kernel_lib STATIC
          cube.asc
          vector.asc
          mix.asc
    )
    ```
**Table 1** Common CMAKE configuration variables
| Variable Name | Description |
|--|--|
| CMAKE_BUILD_TYPE | Compilation mode option. It can be set to Release or Debug. The Release version does not contain debugging information and is used to compile the final release version. The Debug version contains debugging information to facilitate development and debugging. When other values are set, CMake issues a warning but does not abort the build. |
| CMAKE_ASC_COMPILER | Specifies the compiler path of Ascend C. By default, CMake detects it automatically. If the path does not exist, an error is reported and the configuration phase is terminated. |
| CMAKE_ASC_SOURCE_FILE_EXTENSIONS | List of ASC source file extensions. The default value is asc. |
| CMAKE_ASC_STANDARD | Specifies the C++ standard version used for compilation. The default value is 17. When a value lower than 17 is set, CMake issues a warning. |
| CMAKE_ASC_ARCHITECTURES | Specifies the target NPU architecture version. The value is `dav-<arch-version>` (for example, dav-2201 and dav-3510). In NPU run mode, CMake automatically injects this value into the compilation command as `--npu-arch=<value>`. |
| CMAKE_ASC_RUN_MODE | Specifies the operator run mode. Supported values are npu (default, running on an NPU device), cpu (CPU debugging mode), and sim (simulation mode). CMake automatically adjusts the linked libraries and compilation options in different modes. |
| CMAKE_ASC_FLAGS | Common compilation options added for all build types (Debug, Release, and so on). |
| CMAKE_ASC_FLAGS\_DEBUG | Compilation options dedicated to the Debug build. The default value is `-O0 -g`. |
| CMAKE_ASC_FLAGS\_RELEASE | Compilation options dedicated to the Release build. The default value is `-O3 -DNDEBUG`. |
| CMAKE_ASC_COMPILER_LAUNCHER | Pre-launcher of the compilation command (for example, ccache and distcc). When it is not set, the compiler is invoked directly. |
| CMAKE_ASC_LINKER_LAUNCHER | Pre-launcher of the link command. When it is not set, linking is executed directly. |
| CMAKE_ASC_COMPILER_AR | Static library archiving tool. The system ar tool is used by default. After it is set, this tool replaces the default ar in the static library archiving command. |
| CMAKE_ASC_COMPILER_LINKER | Link driver. The bisheng compiler is used as the link driver by default. Set it only when there is an explicit replacement requirement. |
| CMAKE_ASC_ENABLE_SIMT | Whether to enable the SIMT compilation mode. When it is set to ON, the --enable-simt option is automatically injected into the compilation command. The default value is OFF. |
| CMAKE_INSTALL_PREFIX | A built-in CMake variable that specifies the installation path prefix when CMake executes the install command. |

>[!CAUTION] Caution
> In a CMake project, variables that affect Ascend C language initialization, compiler detection, and compilation rule generation
> must be set before `find_package(ASC)`, or specified with `-D` when executing the `cmake` configuration command.
> These variables are read during Ascend C language initialization. If they are set after `find_package(ASC)`,
> they may not affect the compilation rules that have already been generated.

## Other Compilation Notes

### Built-in Compilation Macro Switches<a name="section57020345148"></a>

The built-in compilation macro switches are listed as follows:
 -   **ASCENDC\_DUMP** controls the dump switch. The default switch is enabled. After developers call **printf/DumpTensor/assert**, information is printed. (Note that the kernel file of a direct-call project contains host functions. If **printf** is called in a host function, the printf-related initialization in the kernel is also triggered, which affects the kernel execution performance.) When it is set to 0, the switch is disabled. The following is an example:
     ```shell
     # Disable the printf printing of all operators.
     bisheng <source_file>.asc -o <output_file> --npu-arch=dav-<npu architecture> -DASCENDC_DUMP=0
     ```
 -   **ASCENDC\_DEBUG** controls the debugging switch of the Ascend C API. The default switch is disabled. After this compilation macro is added, the switch is enabled. In this case, the assert check inside the API takes effect. If the check fails, assert logs are printed to the screen. Enabling this function affects the actual running performance of the operator to a certain extent, and it is usually used in the debugging phase. The following is an example:
     ```shell
     bisheng <source_file>.asc -o <output_file> --npu-arch=dav-<npu architecture> -DASCENDC_DEBUG
     ```
     The product models that currently support ASCENDC\_DEBUG are:
     Atlas inference products
     Atlas A2 training products/Atlas A2 inference products

 -   **ENABLE\_CV\_COMM\_VIA\_SSBUF** controls whether to use SSBuffer and the hard channel from UB to the L1 Buffer. Pay attention to this option when CV communication (AIC and AIV) is involved or when data movement APIs are used. Enabling this option can improve the performance of related APIs or extend more functions. The default switch is disabled. When it is set to true, the switch is enabled. The following is an example:
     ```shell
     bisheng <source_file>.asc -o <output_file> --npu-arch=dav-<npu architecture> -DENABLE_CV_COMM_VIA_SSBUF=true
     ```
     This option is supported only on Ascend 950PR/Ascend 950DT.
     -    For operators ported from other hardware platforms to this platform, the switch is disabled by default to maintain compatibility.
     -    For operators newly developed on this platform, the switch needs to be enabled in the following scenarios: using the high-level Matmul API for matrix computation and using APIs such as SetTensorScaleA. These APIs are new functions of Ascend 950PR/Ascend 950DT, and their internal implementation uses SSBuffer, and the DataCopy API is used to copy data from UB to the L1 Buffer.

 -   **NO\_OVERLAP\_IN\_MULTI\_REPEAT**
  This compilation option is used to remove unnecessary memory synchronization instructions when there is no address overlap, so as to improve performance. For Ascend 950PR/Ascend 950DT, when the high-dimensional split computation API of the basic API is used, memory synchronization instructions are inserted by default to ensure data correctness in complex scenarios such as address overlap. However, these synchronization instructions incur performance overhead. In scenarios where ultimate performance is pursued, if you can confirm that no memory overlap occurs in the code under any circumstances, you can use this option.


### Built-in Libraries<a name="section57020345148"></a>

The library files linked by default by the BiSheng Compiler are listed as follows:
<a name="table201231542115513"></a>
<table><thead align="left"><tr id="row171231542205510"><th class="cellrowborder" valign="top" width="23.98%" id="mcps1.2.3.1.1"><p id="p11123114295513"><a name="p11123114295513"></a><a name="p11123114295513"></a>Name</p></th>
<th class="cellrowborder" valign="top" width="76.02%" id="mcps1.2.3.1.2"><p id="p1412374225512"><a name="p1412374225512"></a><a name="p1412374225512"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row5123842135514"><td class="cellrowborder" valign="top" width="23.98%" headers="mcps1.2.3.1.1 "><p id="p1212364212559"><a name="p1212364212559"></a><a name="p1212364212559"></a>libascendc_runtime.a</p></td>
<td class="cellrowborder" valign="top" width="76.02%" headers="mcps1.2.3.1.2 "><p id="p1112394218551"><a name="p1112394218551"></a><a name="p1112394218551"></a>Library for assembling Ascend C operator parameters and other components.</p></td>
</tr>
<tr id="row612324285519"><td class="cellrowborder" valign="top" width="23.98%" headers="mcps1.2.3.1.1 "><p id="p01231423552"><a name="p01231423552"></a><a name="p01231423552"></a>libruntime.so</p></td>
<td class="cellrowborder" valign="top" width="76.02%" headers="mcps1.2.3.1.2 "><p id="p8123164255511"><a name="p8123164255511"></a><a name="p8123164255511"></a>Runtime library.</p></td>
</tr>
<tr id="row1612374285512"><td class="cellrowborder" valign="top" width="23.98%" headers="mcps1.2.3.1.1 "><p id="p2012315425551"><a name="p2012315425551"></a><a name="p2012315425551"></a>libprofapi.so</p></td>
<td class="cellrowborder" valign="top" width="76.02%" headers="mcps1.2.3.1.2 "><p id="p12123164265514"><a name="p12123164265514"></a><a name="p12123164265514"></a>Library for collecting runtime performance data of Ascend C operators.</p></td>
</tr>
<tr id="row10123134212552"><td class="cellrowborder" valign="top" width="23.98%" headers="mcps1.2.3.1.1 "><p id="p812374235515"><a name="p812374235515"></a><a name="p812374235515"></a>libunified_dlog.so</p></td>
<td class="cellrowborder" valign="top" width="76.02%" headers="mcps1.2.3.1.2 "><p id="p412314426554"><a name="p412314426554"></a><a name="p412314426554"></a>CANN log collection library.</p></td>
</tr>
<tr id="row1012384210552"><td class="cellrowborder" valign="top" width="23.98%" headers="mcps1.2.3.1.1 "><p id="p15123104219559"><a name="p15123104219559"></a><a name="p15123104219559"></a>libmmpa.so</p></td>
<td class="cellrowborder" valign="top" width="76.02%" headers="mcps1.2.3.1.2 "><p id="p13123242135519"><a name="p13123242135519"></a><a name="p13123242135519"></a>CANN system API library.</p></td>
</tr>
<tr id="row17124154245516"><td class="cellrowborder" valign="top" width="23.98%" headers="mcps1.2.3.1.1 "><p id="p612484265518"><a name="p612484265518"></a><a name="p612484265518"></a>libascend_dump.so</p></td>
<td class="cellrowborder" valign="top" width="76.02%" headers="mcps1.2.3.1.2 "><p id="p101241842175512"><a name="p101241842175512"></a><a name="p101241842175512"></a>CANN maintenance and test information library.</p></td>
</tr>
<tr id="row6124164213551"><td class="cellrowborder" valign="top" width="23.98%" headers="mcps1.2.3.1.1 "><p id="p111246426558"><a name="p111246426558"></a><a name="p111246426558"></a>libc_sec.so</p></td>
<td class="cellrowborder" valign="top" width="76.02%" headers="mcps1.2.3.1.2 "><p id="p31241442185512"><a name="p31241442185512"></a><a name="p31241442185512"></a>CANN secure function library.</p></td>
</tr>
<tr id="row171241342175514"><td class="cellrowborder" valign="top" width="23.98%" headers="mcps1.2.3.1.1 "><p id="p6124124218556"><a name="p6124124218556"></a><a name="p6124124218556"></a>liberror_manager.so</p></td>
<td class="cellrowborder" valign="top" width="76.02%" headers="mcps1.2.3.1.2 "><p id="p61248424557"><a name="p61248424557"></a><a name="p61248424557"></a>CANN error information management library.</p></td>
</tr>
<tr id="row512404213550"><td class="cellrowborder" valign="top" width="23.98%" headers="mcps1.2.3.1.1 "><p id="p151243425553"><a name="p151243425553"></a><a name="p151243425553"></a>libascendcl.so</p></td>
<td class="cellrowborder" valign="top" width="76.02%" headers="mcps1.2.3.1.2 "><p id="p1012424213555"><a name="p1012424213555"></a><a name="p1012424213555"></a>ACL-related API library.</p></td>
</tr>
</tbody>
</table>

### Commonly Used Libraries for High-Level APIs
When using high-level APIs, you must link the following libraries because these libraries are depended by the high-level API functions. In other scenarios, you can decide whether to link these libraries based on your specific requirements.
|Library Name|Description|Usage Scenario|Dynamic Library Path|
|--|--|--|--|
|libtiling_api.a|Library related to tiling functions.|Link this library when using Tiling APIs related to high-level APIs.|${ASCEND_HOME_PATH}/lib64|
|libregister.so|Library related to tiling functions.|Link this library when using related high-level tiling APIs.|${ASCEND_HOME_PATH}/lib64|
|libgraph_base.so|Library of basic data structures and APIs.|Link this library when calling basic structures such as ge::Shape and ge::DataType.|${ASCEND_HOME_PATH}/lib64|
|libplatform.so|Library of hardware platform information.|Link this library when using hardware platform information APIs related to PlatformAscendC.|${ASCEND_HOME_PATH}/lib64|
