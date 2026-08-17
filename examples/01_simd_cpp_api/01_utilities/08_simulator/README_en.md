# CAmodel simulation example description based on MatmulLeakyRelu

## Overview

This sample uses MatmulLeakyRelu fusion computing as a carrier to demonstrate the compilation, running, result verification and performance data collection process of the Ascend C program in the CAmodel simulation mode. Users can obtain simulation performance data through `msopprof simulator`, which can be used to analyze performance bottlenecks and improve performance analysis efficiency.

## Products and CANN software versions supported by this example

| Products | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 training series products/Atlas A3 inference series products | >= CANN 9.0.0 |
| Atlas A2 training series products/Atlas A2 inference series products | >= CANN 9.0.0 |

## Introduction to directory structure

```
├── 08_simulator
│   ├── CMakeLists.txt          // Compile project files
│   ├── data_utils.h            // Data reading and writing auxiliary functions
│   ├── README.md               // Sample description
│   ├── scripts                 // Input data generation and result verification scripts
│   └── simulator.asc           // Ascend C sample implementation and Host side calling sample
```

## Sample description

- Sample functions:
  To implement the fusion calculation of Matmul and LeakyRelu, the calculation formula is as follows:

  Matmul calculation:
  $$
  C_{ij} = \sum_{k} A_{ik} \times B_{kj}
  $$

  LeakyRelu calculation:
  $$
  C_{ij} = \begin{cases}
  C_{ij} & \text{if } C_{ij} \geq 0 \\
  C_{ij} \times 0.001 & \text{if } C_{ij} < 0
  \end{cases}
  $$

  Among them, A is the left matrix with the shape [M, K]; B is the right matrix with the shape [K, N]; C is the output matrix with the shape [M, N].

- Sample specifications:
  The parameters of this example are M = 512, K = 128, N = 128, and 2 Cube cores and 4 Vector cores are called to complete the calculation. The input specifications are as shown in the following table:

  <table>
  <tr><td rowspan="1" align="center"> sample type (OpType) </td><td colspan="4" align="center">Matmul+LeakyRelu fusion </td></tr>
  <tr><td rowspan="3" align="center"> sample input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">A (left matrix) </td><td align="center">[512, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">B (right matrix) </td><td align="center">[128, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center"> sample output</td><td align="center">C</td><td align="center">[512, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  </table>

## Compile and run

Perform the following steps in the root directory of this sample to compile and execute the program.

- Configure environment variables
  Please configure the environment variables according to the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit package in the current environment.

  > To use the msOpProf tool, you need to install the CANN commercial/community version. For detailed information, please refer to [msOpProf Tool Installation Guide ](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/optool/docs/zh/install_guide/msopprof_install_guide.md).

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. If the installation directory is not specified, it will be installed under `/usr/local/Ascend` by default.

- Sample execution

  Execute the following command in this sample directory.
  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j
  python3 ../scripts/gen_data.py
  msopprof simulator --soc-version=Ascend910B1 ./demo
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin
  ```

  Please select the corresponding `CMAKE_ASC_ARCHITECTURES` parameters according to the actual tested NPU hardware architecture.

  | Options | Description |
  |------|------|
  | `CMAKE_ASC_RUN_MODE` | Specify `sim` to enable NPU simulation mode |
  | `CMAKE_ASC_ARCHITECTURES` | Specifies the NPU architecture version number. `dav-2201` corresponds to Atlas A2 training series products/Atlas A2 reasoning series products and Atlas A3 training series products/Atlas A3 reasoning series products. `dav-3510` corresponds to Ascend 950PR/Ascend 950DT |

  When the accuracy comparison is successful, the output is as follows:

  ```bash
  test pass!
  ```

## Simulation tuning

Based on `./demo`, simulation performance analysis can be performed through `msopprof simulator` to generate visual instruction flow chart and other information. The instructions are as follows:

```bash
msopprof simulator --soc-version=<soc_version> ./demo
```

> Please obtain the AI ​​processor model `<soc_version>` through the following methods:
>
>- Atlas A2 training series products/Atlas A2 inference series products
>   - For the above product models: Execute the `npu-smi info` command on the server where the Ascend AI processor is installed to query and obtain the **Name** information. The actual configuration value is AscendName. For example, the value of **Name** is xxxyy, and the actual configuration value is Ascendxxxyy.
>

> - Ascend 950PR/Ascend 950DT
> - Atlas A3 training series products/Atlas A3 inference series products
>   - For the above product models, execute the `npu-smi info -t board -i <id> -c <chip_id>` command on the server where the Ascend AI processor is installed to query and obtain the **Chip Name** and **NPU Name** information. The actual configuration value is Chip Name_NPU Name. For example, the value of **Chip Name** is Ascendxxx, the value of **NPU Name** is 1234, and the actual configuration value is Ascendxxx_1234.
>
> Among them, `id` is the device ID, and the NPU ID found through the `npu-smi info -l` command is the device ID; `chip_id` is the chip ID, and the Chip ID found out through the `npu-smi info -m` command is the chip ID.

After the command is completed, a folder named `OPPROF_{timestamp}_XXX` will be generated in the current directory. The product structure is as follows:

```
OPPROF_{timestamp}_XXX/
├── dump                    // Raw performance data, users do not need to pay attention
└── simulator
    ├── core*.veccore*/     // Simulation instruction flow chart file for each vector core
    ├── trace.json          // Simulation flowchart and hotspot function visual presentation file
    └── visualize_data.bin  // MindStudio Insight presentation file
```

After execution, you can view the instruction flow chart in the following ways:

- **MindStudio Insight**: Open `visualize_data.bin` or `trace.json` for visual presentation.

  For more details, please see how to use the msOpProf tool, [MindStudio Tool Tuning (msOpProf) Quick Start ](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/optool/docs/zh/quick_start/msopprof_quick_start.md).
