# optype_collector Tool<a name="ZH-CN_TOPIC_OPTYPE_COLLECTOR"></a>

<!-- md-trans-meta sourceCommit=c891f0c5d5b2ac25c5683ff40810fa381d654ece translatedAt=2026-10-23T14:49:59.367Z pushedAt=2026-11-03T07:05:13.442Z nl=2 -->

The optype_collector tool is used to collect the OpType information of a specified AI processor from built-in/custom operator packages, and to detect OpType duplicate name issues between custom operator packages and built-in operator packages, as well as between custom operator packages, helping developers identify name conflicts before installing or delivering custom operators.

The tool supports scanning the following information sources:

- The CANN built-in operator package in the `CANN` installation directory.
- The custom operator package in the `CANN` installation directory.
- One or more custom operator package paths specified by `ASCEND_CUSTOM_OPP_PATH`.

>[!NOTE] Note
>- OpType indicates the operator type name, which is a key identifier in operator prototypes, operator implementations, and operator package management. The OpType of a custom operator should avoid duplicating that of a built-in operator or another custom operator.
>- The conflict detection mode covers the following two scenarios:
>    - OpType duplicate names between a custom operator package and the CANN built-in operator package.
>    - OpType duplicate names between different custom operator packages.

## Tool Installation<a name="section_optype_collector_install"></a>

1.  Install the optype_collector tool.

    The tool is released with the CANN software package. After CANN is installed by referring to [Environment Preparation](../../getting_started/environment_setup.md), you can use the tool.

2.  Set the environment variables.

    -   When the Ascend-cann-toolkit package is installed by the root user

        ```
        source /usr/local/Ascend/cann/set_env.sh
        ```

    -   When the Ascend-cann-toolkit package is installed by a non-root user

        ```
        source ${HOME}/Ascend/cann/set_env.sh
        ```

    Multiple custom operator package paths are separated by the system path separator.

    ```
    export ASCEND_CUSTOM_OPP_PATH=/home/custom_opp
    ```

3.  Check whether the tool is installed successfully.

    Run the following command. If the help information is displayed normally, the tool environment is normal and the functions can be used properly.

    ```
    optype_collector -h
    ```

## Command Format<a name="section_optype_collector_command"></a>

In this document, `{soc_version}` indicates the AI processor model.


-   The configuration is described as follows:

    >[!NOTE] Note 
    ><!-- npu="910b,910,310p,310b" id1 -->
    >- For the following products: run the `npu-smi info` command on the server where the AI processor is installed to query and obtain the Name information. The actual configuration value is AscendName. For example, if the Name value is xxxyy, the actual configuration value is Ascendxxxyy.<br><br>
    >    <!-- npu="910b" id4 -->
    >    Atlas A2 products  
    >    <!-- end id4 -->
    >
    >    <!-- npu="310b" id5 -->
    >    Atlas 200I/500 A2 inference products  
    >    <!-- end id5 -->
    >
    >    <!-- npu="310p" id6 -->
    >    Atlas inference products  
    >    <!-- end id6 -->
    >
    >    <!-- npu="910" id7 -->
    >    Atlas training products<br><br> 
    >    <!-- end id7 -->
    ><!-- end id1 -->
    ><!-- npu="A3" id2 -->
    >- For <term>Atlas A3 products</term>, run the `npu-smi info -t board -i <id> -c <chip_id>` command on the server where the AI processor is installed to query and obtain the Chip Name and NPU Name information. The actual configuration value is `Chip Name_NPU Name`. For example, if the Chip Name value is Ascendxxx and the NPU Name value is 1234, the actual configuration value is Ascendxxx_1234. Where:<br>`id`: device ID. The NPU ID obtained via the `npu-smi info -l` command is the device ID, which is represented as `<id>` in the command.<br>`chip_id`: chip ID. The Chip ID obtained via the `npu-smi info -m` command is the chip ID, which is represented as `<chip_id>` in the command.<br><br>
    ><!-- end id2 -->
    ><!-- npu="950" id3 -->
    >- For Ascend 950PR&950DT products, run the `npu-smi info -t board -i <id>` command on the server where the AI processor is installed to query and obtain the Chip Name and NPU Name information. The actual configuration value is `Chip Name_NPU Name`. For example, if the Chip Name value is Ascendxxx and the NPU Name value is 1234, the actual configuration value is Ascendxxx_1234.<br>Where: `id` is the device ID. The NPU ID obtained via the `npu-smi info -l` command is the device ID, which is represented as `<id>` in the command.
    ><!-- end id3 -->

-   **Command for outputting the built-in operator OpType list**

    ```
    optype_collector {soc_version}
    optype_collector {soc_version} --builtin
    ```

    **Table 1** Parameters<a name="table_optype_collector_builtin_params"></a>

    | Parameter (Case-Sensitive) | Optional/Required | Description |
    | ---- | ---- | ---- |
    | `{soc_version}` | Required | Specifies the AI processor model to be scanned. You can fill in the AI processor model of a product supported by CANN. |
    | `--builtin` | Optional | Outputs only the OpType list in the CANN built-in operator package. If no list mode is specified, this mode is used by default. |

-   **Command for outputting the custom operator OpType list**

    ```
    optype_collector {soc_version} --custom
    ```

    **Table 2** Parameters<a name="table_optype_collector_custom_params"></a>

    | Parameter (Case-Sensitive) | Optional/Required | Description |
    | ---- | ---- | ---- |
    | `{soc_version}` | Required | Specifies the AI processor model to be scanned. You can fill in the AI processor model of a product supported by CANN. |
    | `--custom` | Required | Outputs only the OpType list in the custom operator package. The tool scans the custom operator packages in the `CANN package installation directory` and the path specified by `ASCEND_CUSTOM_OPP_PATH`. |

-   **Command for outputting the OpType lists of built-in operators and custom operators**

    ```
    optype_collector {soc_version} --all
    ```

    **Table 3** Parameters<a name="table_optype_collector_all_params"></a>

    | Parameter (Case-Sensitive) | Optional/Required | Description |
    | ---- | ---- | ---- |
    | `{soc_version}` | Required | Specifies the AI processor model to be scanned. You can fill in the AI processor model of a product supported by CANN. |
    | `--all` | Required | Outputs the OpType lists of both the CANN built-in operator package and the custom operator package. |

-   **Command for detecting OpType name conflicts**

    ```
    optype_collector --detect-conflicts {soc_version}
    ```

    **Table 4** Parameters<a name="table_optype_collector_detect_params"></a>

    | Parameter (Case-Sensitive) | Optional/Required | Description |
    | ---- | ---- | ---- |
    | `--detect-conflicts {soc_version}` | Required | Detects whether the OpType of a custom operator duplicates the OpType of a CANN built-in operator or another custom operator under the specified AI processor version. `{soc_version}` indicates the AI processor model to be detected. The AI processor model supported by CANN can be filled in. |

## Return Value<a name="section_optype_collector_return_code"></a>

<a name="table_optype_collector_return_code"></a>

| Return Value | Description |
| ---- | ---- |
| `0` | The execution succeeds, and no OpType name conflict is found in conflict detection mode. |
| `1` | An OpType name conflict is found in conflict detection mode. |
| `2` | A blocking error occurs, such as a parameter error, a missing environment variable, or the absence of the target AI processor package. |

## Output Description<a name="section_optype_collector_output"></a>

After execution, the tool outputs the following information based on the command mode.

<a name="table_optype_collector_output"></a>

| Output Item | Description |
| ---- | ---- |
| `[Scan Info]` | Displays the AI processor version, `ASCEND_HOME_PATH`, and `ASCEND_CUSTOM_OPP_PATH` of this scan. |
| `[Sources]` | Displays the matched information sources, including source type, status, OpType quantity, configuration file quantity, input AI processor model, and scan path. |
| `[OpType List]` | Outputs OpType statistics and the OpType list in list mode. |
| `[Conflict Summary]` | Outputs the number of built-in operator OpTypes, the number of custom operator packages, and the number of conflict groups in conflict detection mode. |
| `[Conflict n]` | Outputs the package information, conflict quantity, and conflicting OpType list of the nth conflict group in conflict detection mode. |
| `[Warnings]` | Outputs non-blocking warning information, such as `ASCEND_CUSTOM_OPP_PATH` not being set or JSON file parsing failures. |
| `[Errors]` | Outputs blocking error information, such as the CANN package environment variable not being set or the target AI processor package not existing. |

## Example<a name="section_optype_collector_examples"></a>

The following examples use `{soc_version}` as an example. Before running the command, ensure that the CANN environment variables have been configured. To scan external custom operator packages, ensure that `ASCEND_CUSTOM_OPP_PATH` has been set correctly.

-   **View the built-in operator OpType list**

    ```
    optype_collector {soc_version} --builtin
    ```

    After you run the preceding command, the terminal prints the built-in operator OpType list. The screen display information is in the following form:

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

-   **View the custom operator OpType list**

    ```
    optype_collector {soc_version} --custom
    ```

    After you run the preceding command, the terminal prints the custom operator OpType list. If multiple custom operator packages are matched, the tool displays them by scan source. The screen display information is in the following form:

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

-   **View all OpType lists**

    ```
    optype_collector {soc_version} --all
    ```

    After you run the preceding command, the terminal prints the OpType lists in both the built-in operator packages and the custom operator packages. This command is suitable for checking whether an OpType already exists in the current CANN environment or custom operator packages.

-   **Detect OpType name conflicts**

    ```
    optype_collector --detect-conflicts {soc_version}
    ```

    After you run the preceding command, the tool detects whether the custom operator OpTypes conflict with the built-in operator OpTypes or with each other. If a conflict is found, the return value is `1`. The screen display information is in the following form:

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

    If no conflict is found, the tool returns `0`, and the screen display information is in the following form:

    ```
    [Conflict Summary]
      SoC                    : {soc_version}
      Built-in OpTypes       : 3
      Custom packages        : 1
      Custom vs Built-in     : 0 group(s)
      Custom vs Custom       : 0 group(s)
      Result                 : No duplicate OpType conflicts found.
    ```
