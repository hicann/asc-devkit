# optype_collector工具<a name="ZH-CN_TOPIC_OPTYPE_COLLECTOR"></a>

optype_collector工具用于采集内置/自定义算子包中指定AI处理器的OpType信息，并检测自定义算子包与内置算子包、自定义算子包之间的OpType重名问题，帮助开发者在自定义算子安装或交付前提前发现命名冲突。

工具支持扫描以下信息来源：

- `CANN`安装目录下的CANN内置算子包。
- `CANN`安装目录下的自定义算子包。
- `ASCEND_CUSTOM_OPP_PATH`指定的一个或多个自定义算子包路径。

>[!NOTE]说明
>- OpType表示算子类型名称，是算子原型、算子实现和算子包管理中的关键标识。自定义算子OpType应避免与内置算子或其他自定义算子重复。
>- 冲突检测模式会覆盖如下两类场景：
>    - 自定义算子包与CANN内置算子包之间的OpType重名。
>    - 不同自定义算子包之间的OpType重名。

## 工具安装<a name="section_optype_collector_install"></a>

1.  安装optype_collector工具。

    工具跟随CANN软件包发布，参考[环境准备](../../getting_started/environment_setup.md)完成CANN安装后，即可使用该工具。

2.  设置环境变量。

    -   root用户安装Ascend-cann-toolkit包时

        ```
        source /usr/local/Ascend/cann/set_env.sh
        ```

    -   非root用户安装Ascend-cann-toolkit包时

        ```
        source ${HOME}/Ascend/cann/set_env.sh
        ```

    多个自定义算子包路径使用系统路径分隔符分隔。

    ```
    export ASCEND_CUSTOM_OPP_PATH=/home/custom_opp
    ```

3.  检查工具是否安装成功。

    执行如下命令，若能正常显示帮助信息，则表示工具环境正常，功能可正常使用。

    ```
    optype_collector -h
    ```

## 命令格式<a name="section_optype_collector_command"></a>

本文中的`{soc_version}`表示AI处理器的型号。


-   配置说明如下：

    >[!NOTE]说明 
    ><!-- npu="910b,910,310p,310b" id1 -->
    >- 针对如下产品：在安装AI处理器的服务器执行`npu-smi info`命令进行查询，获取Name信息。实际配置值为AscendName，例如Name取值为xxxyy，实际配置值为Ascendxxxyy。<br><br>
    >    <!-- npu="910b" id4 -->
    >    Atlas A2 训练系列产品/Atlas A2 推理系列产品  
    >    <!-- end id4 -->
    >
    >    <!-- npu="310b" id5 -->
    >    Atlas 200I/500 A2 推理产品  
    >    <!-- end id5 -->
    >
    >    <!-- npu="310p" id6 -->
    >    Atlas 推理系列产品  
    >    <!-- end id6 -->
    >
    >    <!-- npu="910" id7 -->
    >    Atlas 训练系列产品<br><br> 
    >    <!-- end id7 -->
    ><!-- end id1 -->
    ><!-- npu="A3" id2 -->
    >- 针对<term>Atlas A3 训练系列产品</term>/<term>Atlas A3 推理系列产品</term>，在安装AI处理器的服务器执行`npu-smi info -t board -i <id> -c <chip_id>`命令进行查询，获取Chip Name和NPU Name信息，实际配置值为`Chip Name_NPU Name`。例如Chip Name取值为Ascendxxx，NPU Name取值为1234，实际配置值为Ascendxxx_1234。其中：<br>`id`：设备id，通过`npu-smi info -l`命令查出的NPU ID即为设备id，命令中表示为`<id>`。<br>`chip_id`：芯片id，通过`npu-smi info -m`命令查出的Chip ID即为芯片id，命令中表示为`<chip_id>`。<br><br>
    ><!-- end id2 -->
    ><!-- npu="950" id3 -->
    >- 针对Ascend 950PR/Ascend 950DT，在安装AI处理器的服务器执行`npu-smi info -t board -i <id>`命令进行查询，获取Chip Name和NPU Name信息，实际配置值为`Chip Name_NPU Name`。例如Chip Name取值为Ascendxxx，NPU Name取值为1234，实际配置值为Ascendxxx_1234。<br>其中：`id`为设备id，通过`npu-smi info -l`命令查出的NPU ID即为设备id，命令中表示为`<id>`。
    ><!-- end id3 -->

-   **输出内置算子OpType清单的命令**

    ```
    optype_collector {soc_version}
    optype_collector {soc_version} --builtin
    ```

    **表1** 参数说明<a name="table_optype_collector_builtin_params"></a>

    | 参数（区分大小写） | 可选/必选 | 说明 |
    | ---- | ---- | ---- |
    | `{soc_version}` | 必选 | 指定待扫描的AI处理器型号，可填写CANN支持的产品AI处理器型号。 |
    | `--builtin` | 可选 | 仅输出CANN内置算子包中的OpType清单。未指定清单模式时，默认使用该模式。 |

-   **输出自定义算子OpType清单的命令**

    ```
    optype_collector {soc_version} --custom
    ```

    **表2** 参数说明<a name="table_optype_collector_custom_params"></a>

    | 参数（区分大小写） | 可选/必选 | 说明 |
    | ---- | ---- | ---- |
    | `{soc_version}` | 必选 | 指定待扫描的AI处理器型号，可填写CANN支持的产品AI处理器型号。 |
    | `--custom` | 必选 | 仅输出自定义算子包中的OpType清单。工具会扫描`CANN包安装目录`和`ASCEND_CUSTOM_OPP_PATH`指定路径下的自定义算子包。 |

-   **输出内置算子与自定义算子OpType清单的命令**

    ```
    optype_collector {soc_version} --all
    ```

    **表3** 参数说明<a name="table_optype_collector_all_params"></a>

    | 参数（区分大小写） | 可选/必选 | 说明 |
    | ---- | ---- | ---- |
    | `{soc_version}` | 必选 | 指定待扫描的AI处理器型号，可填写CANN支持的产品AI处理器型号。 |
    | `--all` | 必选 | 同时输出CANN内置算子包和自定义算子包中的OpType清单。 |

-   **检测OpType重名冲突的命令**

    ```
    optype_collector --detect-conflicts {soc_version}
    ```

    **表4** 参数说明<a name="table_optype_collector_detect_params"></a>

    | 参数（区分大小写） | 可选/必选 | 说明 |
    | ---- | ---- | ---- |
    | `--detect-conflicts {soc_version}` | 必选 | 检测指定AI处理器版本下自定义算子OpType是否与CANN内置算子OpType或其他自定义算子OpType重名。`{soc_version}`表示待检测的AI处理器型号，可填写CANN支持的产品AI处理器型号。 |

## 返回值<a name="section_optype_collector_return_code"></a>

<a name="table_optype_collector_return_code"></a>
| 返回值 | 说明 |
| ---- | ---- |
| `0` | 执行成功，且冲突检测模式下未发现OpType重名冲突。 |
| `1` | 冲突检测模式下发现OpType重名冲突。 |
| `2` | 参数错误、环境变量缺失、目标AI处理器包不存在等阻塞类错误。 |

## 输出说明<a name="section_optype_collector_output"></a>

工具执行后会根据命令模式输出如下信息。

<a name="table_optype_collector_output"></a>
| 输出项 | 说明 |
| ---- | ---- |
| `[Scan Info]` | 展示本次扫描的AI处理器版本、`ASCEND_HOME_PATH`和`ASCEND_CUSTOM_OPP_PATH`。 |
| `[Sources]` | 展示命中的信息来源，包括来源类型、状态、OpType数量、配置文件数量、输入AI处理器型号和扫描路径。 |
| `[OpType List]` | 清单模式下输出OpType统计信息和OpType列表。 |
| `[Conflict Summary]` | 冲突检测模式下输出内置算子OpType数量、自定义算子包数量和冲突分组数量。 |
| `[Conflict n]` | 冲突检测模式下输出第n组冲突的包信息、冲突数量和冲突OpType列表。 |
| `[Warnings]` | 输出非阻塞类告警信息，如未设置`ASCEND_CUSTOM_OPP_PATH`、JSON文件解析失败等。 |
| `[Errors]` | 输出阻塞类错误信息，如CANN包环境变量未设置、目标AI处理器包不存在等。 |

## 使用样例<a name="section_optype_collector_examples"></a>

以下样例以`{soc_version}`为例。执行命令前，请确保已完成CANN环境变量配置；如需扫描外部自定义算子包，请确保已正确设置`ASCEND_CUSTOM_OPP_PATH`。

-   **查看内置算子OpType清单**

    ```
    optype_collector {soc_version} --builtin
    ```

    执行上述命令，终端打印内置算子OpType清单，屏显信息形如：

    ```
    [Scan Info]
      SoC                    : {soc_version}
      ASCEND_HOME_PATH       : /usr/local/Ascend/latest
      ASCEND_CUSTOM_OPP_PATH : /home/custom_opp

    [Sources]
      Type     Status  OpTypes  ConfigFiles  SoC  Path
      -------  ------  -------  -----------  ----------  ---------------------------------------------------------------
      builtin  OK      3        1            {soc_version}  {op_soc_path}

    [OpType List]
      SoC                    : {soc_version}
      Mode                   : builtin
      Total OpTypes          : 3

      OpType
      ------
      Add
      MatMul
      Relu
    ```

-   **查看自定义算子OpType清单**

    ```
    optype_collector {soc_version} --custom
    ```

    执行上述命令，终端打印自定义算子OpType清单。若命中多个自定义算子包，工具会按扫描来源分组展示，屏显信息形如：

    ```
    [Scan Info]
      SoC                    : {soc_version}
      ASCEND_HOME_PATH       : /usr/local/Ascend/latest
      ASCEND_CUSTOM_OPP_PATH : /home/custom_opp

    [Sources]
      Type    Status  OpTypes  ConfigFiles  SoC  Path
      ------  ------  -------  -----------  ----------  ---------------------------------------------------------------
      custom  OK      2        1            {soc_version}  {op_soc_path1}
      custom  OK      2        1            {soc_version}  {op_soc_path2}

    [OpType List]
      SoC                    : {soc_version}
      Mode                   : custom
      Total OpTypes          : 3

      [custom] {op_soc_path1}
      OpTypes          : 2
      SoC             : {soc_version}
        OpType
        ------
        Add
        CustomSame

      [custom] {op_soc_path2}
      OpTypes          : 2
      SoC             : {soc_version}
        OpType
        ------
        CustomSame
        OtherCustom
    ```

-   **查看全部OpType清单**

    ```
    optype_collector {soc_version} --all
    ```

    执行上述命令，终端同时打印内置算子包和自定义算子包中的OpType清单。该命令适用于排查某个OpType是否已经在当前CANN环境或自定义算子包中存在。

-   **检测OpType重名冲突**

    ```
    optype_collector --detect-conflicts {soc_version}
    ```

    执行上述命令，工具会检测自定义算子OpType与内置算子OpType、自定义算子OpType之间是否重名。若发现冲突，返回值为`1`，屏显信息形如：

    ```
    [Conflict Summary]
      SoC                    : {soc_version}
      Built-in OpTypes       : 3
      Custom packages        : 2
      Custom vs Built-in     : 1 group(s)
      Custom vs Custom       : 1 group(s)

    [Conflict 1] Custom package conflicts with built-in OpTypes
      Pkg  Type     Vendor    Path
      ---  -------  --------  ---------------------------------------------------------------
      A    custom   vendor_a  {op_soc_path1}
      B    builtin  -         {op_soc_path2}
      Conflict count         : 1
      Conflict OpTypes:
        - Add

    [Conflict 2] Custom package conflicts with another custom package
      Pkg  Type    Vendor    Path
      ---  ------  --------  ---------------------------------------------------------------
      A    custom  vendor_a  {op_soc_path1}
      B    custom  vendor_b  {op_soc_path3}
      Conflict count         : 1
      Conflict OpTypes:
        - CustomSame
    ```

    若未发现冲突，返回值为`0`，屏显信息形如：

    ```
    [Conflict Summary]
      SoC                    : {soc_version}
      Built-in OpTypes       : 3
      Custom packages        : 1
      Custom vs Built-in     : 0 group(s)
      Custom vs Custom       : 0 group(s)
      Result                 : No duplicate OpType conflicts found.
    ```
