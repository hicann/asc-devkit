# ACLRTC Kernel 常量化编译样例

## 概述

本样例展示 Kernel 级常量化编译的完整流程：通过标准算子包构建流程生成 Kernel 编译资源，调用 `aclrtcKernelSpecialization` 将 `MatmulCustom` Kernel 的 Tiling 参数固化到 Kernel 中，加载特化后的 ELF 并执行。

样例中的输入、特化参数和 Kernel 启动参数均为固定值，便于对照接口调用关系。也可使用算子调优工具对比常量化编译前后的 Kernel 耗时与流水线指标。

## 本样例支持的产品及CANN软件版本

**表1**  支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
| --- | --- |
| Ascend 950PR&950DT系列产品 | 9.3.0 |
| Atlas A3系列产品 | 9.3.0 |
| Atlas A2系列产品 | 9.3.0 |

## 目录结构介绍

```text
rtc_kernel_specialization/
├── CMakeLists.txt                 # Host 编译工程
├── main.cpp                       # Kernel 常量化编译、加载、执行和结果校验
├── runtime_resources.h            # Runtime 资源释放
├── custom_op/                     # MatmulCustom 算子及 Kernel 源码
│   ├── CMakeLists.txt             # 算子包构建配置
│   ├── op_host/
│   │   ├── CMakeLists.txt
│   │   └── matmul_custom_host.cpp
│   └── op_kernel/
│       ├── CMakeLists.txt
│       ├── matmul_custom.cpp
│       └── matmul_custom_tiling.h
└── README.md
```

## 样例描述

Kernel 执行 `C = A × B`，三个矩阵的形状均为 `[512, 512]`，采用 ND 格式，不转置、不加 Bias。输入 A、B 为 FP16，输出 C 为 FP32。A 的元素从 -2 到 2 按 0.25 间隔循环取值，B 为单位矩阵，因此预期输出 C 等于 A。Kernel 有 5 个参数：`a`、`b`、`c`、`workspace` 和 `tiling`。本样例仅将第 5 个参数 `tiling` 作为常量参数；Host 调用 Matmul Tiling API 按当前设备生成参数，并打印实际块数和分块尺寸。

## 样例实现

`main.cpp` 按以下顺序调用接口：

1. 初始化 ACL、Context、Stream，为输入和输出申请普通 Host 内存及 Device 内存，并为 Tiling 申请 Device 内存。
2. 准备矩阵 A、B 和 Host 侧参考结果，将 A、B 拷贝到 Device，调用 Matmul Tiling API 生成 Tiling 参数，并按需申请 Device workspace。
3. 默认模式下，填充 `aclrtcKernelSpecRequest`，设置第 5 个参数的 Host 地址和字节数，通过 `options` 分别传入 `-g` 和 `-fno-ident`，用于生成调试信息并禁止生成编译器标识信息，调用 `aclrtcKernelSpecialization` 生成特化 ELF；直接传入已有二进制文件路径时跳过此步骤。
4. 使用 `aclrtBinaryLoadFromFile` 加载指定的 Kernel 二进制，通过 `aclrtBinaryGetFunction` 获取 Kernel 函数。
5. 将 Tiling 数据拷贝到 Device，初始化输出缓冲区，按原始五参数顺序调用 `aclrtLaunchKernelWithArgsArray`。
6. 同步 Stream，将输出拷贝回 Host 并校验结果。
7. 卸载二进制，释放 Host 和 Device 内存，销毁 Stream、Context，并完成 ACL 去初始化。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  1. 构建算子包和 Kernel Spec 编译资源：

     本样例已在 `custom_op/CMakeLists.txt` 的 `npu_op_package` 中配置 `KERNEL_SPEC_MODE Normal`，用于生成普通 Kernel 的常量化编译资源。算子工程的编译配置方法请参见[基本流程](../../../../../docs/zh/guide/programming_guide/advanced_programming/aclnn_operator_development/compilation_and_deployment/basic_process.md)。

     将 `ascendxxx` 替换为实际运行的 AI 处理器系列。`ASCEND_COMPUTE_UNIT` 区分大小写，请使用构建工具支持的全小写取值。

     ```bash
     rm -rf build/operator
     cmake -S custom_op -B build/operator -DASCEND_COMPUTE_UNIT=ascendxxx
     cmake --build build/operator --target binary package -j8
     ```

     构建完成后会生成 Kernel 编译资源、Manifest 和算子安装包 `custom_opp_*.run`。

  2. 将算子包安装到样例的 `build/opp` 目录：

     ```bash
     rm -rf build/opp
     mkdir -p build/opp
     bash build/operator/custom_opp_*.run --quiet --install-path="$(pwd)/build/opp"
     source build/opp/vendors/customize/bin/set_env.bash
     export ASCEND_CUSTOM_OPP_PATH="$(pwd)/build/opp/vendors/customize"
     ```

  3. 从本次构建生成的 Manifest 获取 `resourceId` 和 Kernel 入口名。请在同一终端继续执行后续步骤，运行 `demo` 时需要使用本步骤取得的两个值。

     ```bash
     manifest=$(find build/operator -type f -name '*_manifest.json' -print -quit)
     resource_id=$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1]))["resource_id"])' "$manifest")
     kernel_entry=$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1]))["kernels"][0]["kernel_name"])' "$manifest")
     printf 'resourceId=%s\nkernelEntry=%s\n' "$resource_id" "$kernel_entry"
     ```

  4. 编译 Host 程序：

     ```bash
     rm -rf build/host
     cmake -S . -B build/host
     cmake --build build/host -j8
     ```

     根目录的 `CMakeLists.txt` 会编译示例程序，生成 `build/host/demo`。

  5. 在 NPU 上运行：

     ```bash
     device_id=3
     ./build/host/demo \
         "$resource_id" \
         "$kernel_entry" \
         build/specialized_kernel.elf \
         "$device_id"
     ```

     参数依次为资源 ID、Kernel 入口名、特化 ELF 输出路径和设备 ID。请将 `device_id` 设置为可用的设备 ID。

     > [!WARNING]
     > **注意**：本样例仅支持 NPU 运行模式，不支持 CPU 调试或 SIM 仿真模式。

- 执行结果

  执行成功时输出：

  ```text
  Numerical check passed: 262144 float elements
  Example passed
  ```

## 性能调试

### 性能采集

使用当前 CANN 环境中的 `msprof op` 采集 Kernel 任务耗时和流水线指标。请先完成“编译运行”，并在样例根目录的同一终端执行以下步骤。

1. **指定待采集的两个 Kernel 二进制**

   ```bash
   original_kernel_path=$(find build/operator/op_kernel -type f -name "${kernel_entry%_*}.o" -print -quit)
   specialized_kernel_path=build/specialized_kernel.elf
   ls -l "$original_kernel_path" "$specialized_kernel_path"
   mkdir -p build/profiling
   profile_root=$(mktemp -d build/profiling/kernel-XXXXXX)
   ```

   `mktemp -d` 创建本次采集的独立目录，将 `XXXXXX` 自动替换为随机字符，并把实际目录路径保存到 `profile_root`；无需手动替换 `XXXXXX`。本步骤只执行一次，5 轮报告保存在同一目录下。

   原始二进制来自本次算子包构建，常量化 ELF 来自“编译运行”第 5 步。两者须使用相同的输入、Tiling 和启动参数；重新构建算子包、修改 Tiling 或更换目标设备后，须重新生成常量化 ELF。

2. **采集原始 Kernel 与常量化 Kernel**

   设置当前轮次后执行两条采集命令。

   ```bash
   round=1
   msprof op \
       --output="$profile_root/round-$round-original" \
       --warm-up=5 --launch-count=1 \
       --replay-mode=kernel --aic-metrics=PipeUtilization \
       ./build/host/demo \
       "$resource_id" "$kernel_entry" "$specialized_kernel_path" "$device_id" "$original_kernel_path"

   msprof op \
       --output="$profile_root/round-$round-specialized" \
       --warm-up=5 --launch-count=1 \
       --replay-mode=kernel --aic-metrics=PipeUtilization \
       ./build/host/demo \
       "$resource_id" "$kernel_entry" "$specialized_kernel_path" "$device_id" "$specialized_kernel_path"
   ```

   调用格式为 `demo resource_id kernel_entry output_kernel_path device_id [input_kernel_path]`。末尾传入 `input_kernel_path` 时，直接加载该文件并跳过常量化编译，此时不使用 `output_kernel_path`。输入路径须指向已存在的普通文件，路径无效时报错退出。文件路径说明如下：

   - `original_kernel_path`：算子包构建生成的原始 Kernel `.o` 文件路径。
   - `specialized_kernel_path`：`aclrtcKernelSpecialization` 生成的 ELF 文件路径。若文件已重命名或移动，请将变量设置为实际路径。

   两种产物使用相同的执行、校验流程。采集参数说明如下：

   - `--warm-up=5`：由工具执行 5 次 Kernel 预热。
   - `--launch-count=1`：采集程序下发的一个 Kernel。此参数限制采集的 Kernel 数量，不指定重复测量次数。
   - `--replay-mode=kernel`：由工具重放 Kernel，以采集 `PipeUtilization` 指标。工具按 PMU 事件分组完成采集。

   每条命令结束后，可用 `echo $?` 检查退出码为 0，并确认输出包含 `Numerical check passed`、`Example passed` 和 `Warm Up enabled. times:5`。校验失败或采集失败的数据不计入结果。

3. **重复采集 5 轮**

   完成第 1 轮后，仅重复第 2 步，将 `round=1` 依次改为 `round=2`、`round=3`、`round=4`、`round=5`；不要重复第 1 步创建目录。第 1、3、5 轮先采集原始 Kernel，第 2、4 轮先采集常量化 Kernel。两条命令串行执行，每轮分别得到两个 Kernel 的一组采集结果；所有轮次复用同一个常量化 ELF。

### 查看采集数据

每个输出目录下生成 `OPPROF_*` 子目录。用以下命令定位 CSV 文件，再使用电子表格软件打开：

```bash
find "$profile_root" -name 'OpBasicInfo.csv' -print
find "$profile_root" -name 'PipeUtilization.csv' -print
```

**表2**  性能指标说明

| 文件 | 字段 | 含义与统计方法 |
| --- | --- | --- |
| `OpBasicInfo.csv` | `Task Duration(us)` | 一次采集的 Kernel 任务起止时间之差 |
| `PipeUtilization.csv` | `aic_time(us)` | 各 AI Cube Core 的执行耗时 |
| `PipeUtilization.csv` | `aic_scalar_time(us)` | Scalar 耗时 |
| `PipeUtilization.csv` | `aic_cube_time(us)` | Cube 矩阵乘加耗时 |
| `PipeUtilization.csv` | `aic_mte1_time(us)` | L1 至 L0A/L0B 的搬运指令耗时，不含搬运等待时间 |
| `PipeUtilization.csv` | `aic_mte2_time(us)` | GM 至 AI Core 的搬运指令耗时 |
| `PipeUtilization.csv` | `aic_mte3_time(us)` | AI Core 至 GM 的搬运指令耗时 |
| `PipeUtilization.csv` | `aic_fixpipe_time(us)` | L0C 至 GM/L1 的结果搬运指令耗时；启用 UnitFlag 时包含指令等待时间 |

表中以耗时作为对比指标；对应的流水线周期占比可在同一 CSV 的 `*_ratio` 字段中查看。

- **逐核数据**：`PipeUtilization.csv` 中的 `block_id`、`sub_block_id` 标识计算核，每行不是一次独立测量。本样例按列对有效 AI Core 数据求算术平均值（电子表格可使用 `AVERAGE`），得到本轮各流水线指标。
- **轮次均值**：分别对原始 Kernel 和常量化 Kernel 的 5 轮结果求算术平均值。任务耗时和流水线耗时分别统计；缺失或标为 `NA`、`N/A` 的数据不能按 0 处理。
- **对比计算**：变化率 =（常量化平均耗时 − 原始平均耗时）÷ 原始平均耗时 × 100%；负值表示耗时减少，正值表示耗时增加。加速比 = 原始平均耗时 ÷ 常量化平均耗时。
- **统计范围**：仅比较设备 Kernel 任务和流水线指标，不包含常量化编译、Host 初始化、二进制加载、Host-Device 拷贝、Host 下发和同步 API 耗时。预热由工具完成，不需要从 CSV 中手动删除预热记录。
- **结果解读**：工具重放的缓存条件与业务连续执行可能不同。比较时使用相同工具参数；各流水线可重叠，不能相加得到任务耗时。

### 性能实测

**表3**  性能测试配置

| 项目 | 实测配置 |
| --- | --- |
| 产品 | Atlas A2系列产品 |
| Host | aarch64，Ubuntu 24.04.4 LTS |
| 软件 | CANN 9.3.0，驱动 25.5.1 |
| 矩阵规格 | M = N = K = 512；A、B 为 ND 格式 FP16，C 为 ND 格式 FP32；B 为单位矩阵 |
| Tiling 和编译选项 | 两个 Kernel 使用相同的 Tiling 参数及 `-g`、`-fno-ident` 编译选项，分核数和分块尺寸以样例输出为准 |
| 采集方式 | `msprof op`，`PipeUtilization`，Kernel 重放，工具预热 5 次，每个 Kernel 独立采集 5 轮 |

以下数据按上述配置独立采集 5 轮，每轮分别采集原始版本和常量化版本的 1 个设备 Kernel 任务。

**表4**  各轮 Kernel 任务耗时（Task Duration）

| 轮次 | 原始版本 Task Duration（μs） | 常量化版本 Task Duration（μs） |
| --- | ---: | ---: |
| 1 | 9.32 | 7.28 |
| 2 | 9.38 | 7.56 |
| 3 | 8.60 | 7.74 |
| 4 | 9.04 | 7.38 |
| 5 | 8.90 | 7.70 |
| **平均** | **9.048** | **7.532** |

流水线指标先在每轮内对有效计算核取均值，再对 5 轮结果取均值。

**表5**  Kernel 任务及流水线平均耗时

| 指标 | 原始均值（μs） | 常量化均值（μs） | 变化幅度 |
| --- | ---: | ---: | ---: |
| `Task Duration(us)` | 9.048 | 7.532 | -16.76% |
| `aic_time(us)` | 7.786 | 6.441 | -17.27% |
| `aic_scalar_time(us)` | 4.640 | 2.330 | -49.79% |
| `aic_cube_time(us)` | 1.449 | 1.450 | +0.07% |
| `aic_mte1_time(us)` | 1.703 | 1.684 | -1.14% |
| `aic_mte2_time(us)` | 4.138 | 4.606 | +11.31% |
| `aic_mte3_time(us)` | 0.001 | 0.001 | 0.00% |
| `aic_fixpipe_time(us)` | 2.858 | 3.006 | +5.16% |

- 5 轮中常量化版本的 Task Duration 均低于原始版本，平均下降 16.76%。Scalar 平均下降 49.79%，与常量化减少运行时标量处理的预期一致。
