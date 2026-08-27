# CAmodel Simulation Example Based on Add

## Overview

This example uses Add computation to demonstrate the compilation, execution, result verification, and performance data collection flow of Ascend C programs in CAmodel simulation mode. Users can obtain simulation performance data through `msopprof simulator` to analyze performance bottlenecks and improve performance analysis efficiency.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Version
- \>= CANN 9.1.0

## Directory Structure

```
├── 07_simulator
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write helper functions
│   ├── README.md               // Example description
│   ├── scripts                 // Input data generation and result verification scripts
│   └── add.asc                 // SIMT Add invocation example
```

## Example Description

- Example Function

  This example implements element-wise addition of two input tensors to produce an output tensor based on the Ascend C SIMT programming model. The computation formula is as follows:

  ```text
  z = x + y
  ```

  In this example, the shape of `x` and `y` is `[48,256]` and the data type is `float`. `scripts/gen_data.py` is used to generate input data and golden data. After the example runs, the result is written to `output/output.bin`, and then `scripts/verify_result.py` performs result verification.

- Example Specifications

  <table border="2">
  <caption>Table 1: Add Example Specification Description</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">Add</td></tr>
  <tr><td align="center">Field</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">x</td><td align="center">[48,256]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[48,256]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">Example Output</td><td align="center">z</td><td align="center">[48,256]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">add_custom</td></tr>
  </table>

## Build and Run

Run the following steps in the root directory of this example to build and execute the program.

- Configure Environment Variables  
  Configure environment variables based on the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, it defaults to `/usr/local/Ascend`.

- Run the Example

  Run the following commands in this example directory.

  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j
  python3 ../scripts/gen_data.py
  msopprof simulator --soc-version=<soc_version> ./demo
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin
  ```

  Select the corresponding `CMAKE_ASC_ARCHITECTURES` parameter based on the NPU hardware architecture being tested.

  | Option | Description |
  |------|------|
  | `CMAKE_ASC_RUN_MODE` | Set to `sim` to enable NPU simulation mode |
  | `CMAKE_ASC_ARCHITECTURES` | Specify the NPU architecture version. This example only supports `dav-3510` (Ascend 950PR/Ascend 950DT) |

  > The `-g` compilation option has been added in CMakeLists.txt to generate debug information, enabling the simulator to collect instruction pipeline diagrams. To use the simulation tuning feature in your own project, ensure this option is also added during compilation.

  When accuracy verification is successful, the output is as follows:

  ```bash
  test pass!
  ```

## Simulation Tuning

Based on `./demo`, use `msopprof simulator` for simulation performance analysis to generate visualized instruction pipeline diagrams and other information. The command is as follows:

```bash
msopprof simulator --soc-version=<soc_version> ./demo
```

> Obtain the AI processor model `<soc_version>` through the following method:
>
> - Ascend 950PR/Ascend 950DT
>   - For the above product models, run the `npu-smi info -t board -i <id> -c <chip_id>` command on the server with the Ascend AI Processor installed to query and obtain the **Chip Name** and **NPU Name** information. The actual configuration value is Chip Name_NPU Name. For example, if **Chip Name** is Ascendxxx and **NPU Name** is 1234, the actual configuration value is Ascendxxx_1234.
>
> Where `id` is the device ID, obtained from the NPU ID shown by the `npu-smi info -l` command; `chip_id` is the chip ID, obtained from the Chip ID shown by the `npu-smi info -m` command.

After the command completes, a folder named `OPPROF_{timestamp}_XXX` is generated in the current directory. The output structure is as follows:

```
OPPROF_{timestamp}_XXX/
├── dump                    // Raw performance data, no user attention needed
└── simulator
    ├── core*.veccore*/     // Simulation instruction pipeline diagram files for each vector core
    └── visualize_data.bin  // MindStudio Insight presentation file
```

After execution, view the instruction pipeline diagram through the following methods:

- **MindStudio Insight**: Open `visualize_data.bin` for visual presentation.

  For more details, refer to the msOpProf tool usage: [MindStudio Tool Tuning (msOpProf) Quick Start](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/optool/docs/zh/quick_start/msopprof_quick_start.md).
