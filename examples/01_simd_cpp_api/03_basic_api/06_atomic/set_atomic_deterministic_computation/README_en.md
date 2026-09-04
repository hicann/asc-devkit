# Deterministic Computation with Data Movement Atomic Operations Example
## Overview
This example first introduces the necessity and specific implementation scheme of deterministic computation in the scenario of data movement with inline atomic operations, and then describes how to apply the above scheme in single AIV core, multi-AIV core, and multi-AIC core scenarios respectively.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure
```
├── set_atomic_deterministic_computation
│   ├── figures                                   // Image resource directory for README
│   ├── scripts
│   │   ├── gen_data.py                           // Input data and ground truth generation script
│   │   └── verify_result.py                      // Verification script to check output data against ground truth
│   ├── CMakeLists.txt                            // Build configuration file
│   ├── data_utils.h                              // Data read/write functions
│   ├── set_atomic_add_multi_aic.h                // Multi-AIC core synchronization implementation
│   ├── set_atomic_add_multi_aiv.h                // Multi-AIV core synchronization implementation
│   ├── set_atomic_add_single_aiv.h               // Single-AIV core synchronization implementation
│   ├── set_atomic_deterministic_computation.asc  // Ascend C example implementation & invocation example
│   └── README.md                                 // Example description document
```

## 1. Deterministic Computation

### 1.1 Necessity of Deterministic Computation

Floating-point accumulation does not satisfy mathematical commutativity and associativity. Different accumulation orders may produce different computation results. This can be verified with the following data:

```python
x = 1.0
y = 1e16
z = -1e16

# Different accumulation orders produce different results
left = (x + y) + z    # Add x and y first, then add z
right = x + (y + z)   # Add y and z first, then add x
```

Due to floating-point precision limitations, `(1.0 + 1e16) + (-1e16)` and `1.0 + (1e16 + (-1e16))` produce different results:
- The former: `1.0 + 1e16` loses precision, the result is close to `1e16`, then adding `-1e16` yields `0.0`
- The latter: `1e16 + (-1e16)` is exactly `0.0`, then adding `1.0` yields `1.0`

Therefore, in a parallel computing environment, a synchronization mechanism must ensure a fixed accumulation order to guarantee result determinism. Deterministic computation refers to a computation process that always produces completely consistent output results under the same input conditions, regardless of the number of executions or the execution environment. Deterministic computation provides guarantees for system stability and experimental verifiability.

### 1.2 Overview of Deterministic Computation

To illustrate the problem of non-deterministic computation in atomic operation scenarios, we construct the following common deterministic computation scenario: First, initialize GM through a single set of floating-point data transfer; then, launch atomic accumulation operations; finally, accumulate multiple sets of floating-point data on GM through multiple data transfers. The specific pseudocode is as follows:

```
(1) Transfer data data0 to GM;    // Data transfer, overwrites the original random values in GM, expected GM data is data0
(2) SetAtomicAdd();         // Enable atomic accumulation, subsequent transfers from UB/L0C/L1 to GM all perform atomic accumulation
(3) Transfer data1 to GM;    // Data transfer with inline atomic operation, expected GM data is data0 + data1
(4) Transfer data2 to GM;    // Data transfer with inline atomic operation, expected GM data is data0 + data1 + data2
(5) Transfer data3 to GM;    // Data transfer with inline atomic operation, expected GM data is data0 + data1 + data2 + data3
```

As shown in the figure below, the developer's expected result: the order of instruction issuance strictly corresponds to the actual instruction execution order. After executing this code segment multiple times, regardless of how many times it is executed, the final GM data is always data0 + data1 + data2 + data3, with completely consistent results, achieving deterministic computation.

![Deterministic computation scenario](figures/确定性计算场景，GM上数据变化过程.png)

Figure 1: Deterministic computation scenario, data change process on GM

However, in practice, if the developer does not intervene, the execution order of these instructions may change each time the program runs, ultimately causing the GM data to be inconsistent with the expected result. Two possible instruction execution orders and their corresponding execution flows are listed below.

#### 1.2.1 Non-deterministic Computation, Result 1

![Non-deterministic computation scenario 1](figures/非确定性计算场景1，GM上数据变化过程.png)

Figure 2: Non-deterministic computation scenario 1, data change process on GM

As shown in the figure, the instruction execution flow in this scenario is as follows:
1. Initial state, GM data is: random values;
2. Transfer data0 to GM, GM data is initialized to: data0;
3. Execute SetAtomicAdd, enabling atomic accumulation for subsequent transfer instructions, GM data is: data0;
4. Three transfer instructions with inline atomic operations are out of order. The actual execution order is "transfer data2 -> transfer data3 -> transfer data1", and the final GM data is: data0 + data2 + data3 + data1.

**Cause of non-deterministic computation 1**:
Transfer instructions with inline atomic operations are out of order. Because floating-point addition does not satisfy associativity, that is, (a+b)+c != a+(b+c), the final GM data data0 + data2 + data3 + data1 deviates from the expected data0 + data1 + data2 + data3.

The prerequisites for transfer instructions with inline atomic operations being out of order to cause result deviation are the following three conditions:
- The atomic operation type is atomic accumulation (maximum and minimum operations satisfy associativity)
- The atomic operation data type is floating-point (integer addition satisfies associativity)
- There are 3 or more transfer instructions with inline atomic operations (floating-point addition satisfies commutativity)

#### 1.2.2 Non-deterministic Computation, Result 2

![Non-deterministic computation scenario 2](figures/非确定性计算场景2，GM上数据变化过程.png)

Figure 3: Non-deterministic computation scenario 2, data change process on GM

As shown in the figure, the instruction execution flow in this scenario is as follows:
1. Initial state, GM data is: random values;
2. Execute SetAtomicAdd, enabling atomic accumulation for subsequent transfer instructions, GM data is: random values;
3. Execute two transfer instructions with inline atomic operations in sequence, execution order is "transfer data1 -> transfer data2", GM data is: random values + data1 + data2;
4. Transfer data0 to GM, the accumulated result on GM is overwritten by data0, GM data is: data0;
5. Finally execute the data3 transfer, final GM data is: data0 + data3.

**Cause of non-deterministic computation 2**:
Out-of-order execution between the normal transfer instruction before enabling atomic accumulation and the transfer instructions with atomic operations causes the data that has already undergone atomic operations on GM to be incorrectly overwritten by data0, resulting in non-deterministic computation results.

This type of out-of-order result deviation does not require any prerequisites. Developers do not need to distinguish atomic operation types, atomic operation data types, or consider whether the number of transfer instructions with inline atomic operations reaches 3 or more.

### 1.3 Deterministic Computation Implementation Scheme

Based on the two root causes of non-deterministic computation, the following describes the implementation scheme for deterministic computation from both aspects. The core idea is to insert appropriate synchronization between instructions so that the relevant instructions execute in the expected deterministic order each time the program runs, ultimately ensuring that the program output is always the same. Specifically, this includes the following two aspects:

- Insert synchronization between the transfer instruction before enabling atomic accumulation and the instruction that enables atomic operations
  As shown in the following pseudocode, inserting synchronization between instructions (1) and (2) ensures that the initial GM value before starting atomic operations meets expectations.
- Synchronization between multiple transfer instructions after enabling atomic accumulation
  Inserting synchronization between instructions (3) and (4), and between (4) and (5), ensures that the floating-point accumulation order meets expectations.

Synchronization between the instruction that enables atomic operations and subsequent transfer instructions does not need to be inserted by the developer.

#### 1.3.1 Intra-core Synchronization Implementation

```cpp
// The entire atomic accumulation executes within the same core, controlling the execution order of 5 instructions as "(1)->(2)->(3)->(4)->(5)"
(1) Transfer data data0 to GM;    // Data transfer, overwrites the original random values in GM, expected GM data is data0
Intra-core synchronization
(2) SetAtomicAdd();         // Enable atomic accumulation, subsequent transfers from UB/L0C/L1 to GM all perform atomic accumulation
// No synchronization needed between instructions (2) and (3)
(3) Transfer data1 to GM;    // Data transfer after enabling atomic accumulation, expected GM data is data0 + data1
Intra-core synchronization
(4) Transfer data2 to GM;    // Data transfer after enabling atomic accumulation, expected GM data is data0 + data1 + data2
Intra-core synchronization
(5) Transfer data3 to GM;    // Data transfer after enabling atomic accumulation, expected GM data is data0 + data1 + data2 + data3
```
The pipeline types of transfer instructions and atomic operation enabling instructions are shown in the following table. When the above instructions execute within the same core, developers insert [single-pipeline synchronization](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/API/ascendcopapi/atlasascendc_api_07_0271.html) or [multi-pipeline synchronization](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/API/ascendcopapi/atlasascendc_api_07_0270.html) as needed. Refer to the SCENARIO_NUM=1 branch in the example.
<table border="1" style="text-align: left;">
  <tr>
    <th style="padding: 8px;">Instruction Name</th>
    <th style="padding: 8px;">Pipeline Type</th>
  </tr>
  <tr>
    <td style="padding: 8px;">DataCopy</td>
    <td style="padding: 8px;">PIPE_MTE3</td>
  </tr>
  <tr>
    <td style="padding: 8px;">Fixpipe</td>
    <td style="padding: 8px;">PIPE_FIX</td>
  </tr>
  <tr>
    <td style="padding: 8px;">SetAtomicAdd/SetAtomicMax/SetAtomicMin</td>
    <td style="padding: 8px;">PIPE_S</td>
  </tr>
</table>

#### 1.3.2 Inter-core Synchronization Implementation

As shown in the following pseudocode, when the above instructions execute in different cores, the intra-core synchronization above must be replaced with inter-core synchronization.

```cpp
// The entire atomic accumulation executes across 4 different cores, controlling the execution order of the 4 cores as "core 0->core 1->core 2->core 3"
if (GetBlockIdx == 0) {
   Transfer data data0 to GM;
   Inter-core synchronization
} else if (GetBlockIdx == 1) {
   Inter-core synchronization
   SetAtomicAdd();
   Transfer data1 to GM;
   Inter-core synchronization
} else if (GetBlockIdx == 2) {
   Inter-core synchronization
   SetAtomicAdd();
   Transfer data2 to GM;
   Inter-core synchronization
} else if (GetBlockIdx == 3) {
   Inter-core synchronization
   SetAtomicAdd();
   Transfer data3 to GM;
}
```
Since no hardware synchronization API is currently available for controlling execution order between different cores, inter-core synchronization in deterministic computation scenarios must be implemented through software. The software synchronization schemes differ for three scenarios: pure Vector examples, pure Cube examples, and Mix (containing both Vector and Cube computation) examples, as shown in the following table.
<table border="1" style="text-align: left;">
  <tr>
    <th style="padding: 8px;">Example Type</th>
    <th style="padding: 8px;">Software Synchronization Scheme</th>
    <th style="padding: 8px;">Description</th>
  </tr>
  <tr>
    <td style="padding: 8px;" rowspan="2">Pure Vector Example</td>
    <td style="padding: 8px;">Scheme 1: Combine multiple pairs of IBSet and IBWait APIs to achieve synchronization among multiple AIVs. Refer to the SCENARIO_NUM=2 branch in the example.</td>
    <td style="padding: 8px;">Scheme 1 supports specifying a subset of AIVs to participate in synchronization and controlling the execution order of each AIV.</td>
  </tr>
  <tr>
    <td style="padding: 8px;">Scheme 2: Use InitDetermineComputeWorkspace, NotifyNextBlock, and WaitPreBlock APIs together to ensure all AIV cores execute in ascending blockIdx order.</td>
    <td style="padding: 8px;">Scheme 2 requires all AIVs to participate in synchronization, and the execution order is fixed to ascending blockIdx.</td>
  </tr>
  <tr>
    <td style="padding: 8px;">Pure Cube Example</td>
    <td style="padding: 8px;">Implement inter-core synchronization through semaphores in GM. First establish synchronization between a pair of cores, then extend to multiple cores. Refer to the SCENARIO_NUM=3 branch in the example.</td>
    <td style="padding: 8px;">When the Scalar unit accesses GM, multi-core data consistency must be considered.<br>The "core" here can be AIV or AIC.</td>
  </tr>
</table>
The following figure shows how two cores perform inter-core synchronization through semaphores in GM:

![Software synchronization scheme flow chart for a pair of cores](figures/一对核之间软件同步方案流程图.png)

Figure 4: Software synchronization scheme flow chart for a pair of cores

- After the previous core completes data transfer or enables atomic operations, it writes the value 1 to the semaphore in the inter-core shared GM through the Scalar unit, indicating that its task is complete. Intra-core synchronization must also be inserted in the previous core:
  - When the previous core has multiple transfer instructions, intra-core synchronization 1 must be inserted between them.
  - Before the Scalar unit writes data to GM, all preceding transfer instructions must have completed execution, so intra-core synchronization 2 must also be inserted between them.
- Before the current core executes its transfer task, it continuously reads the semaphore value through the Scalar unit. If the semaphore is not equal to 1, the current core enters a blocking wait state; when the semaphore is detected to be 1, the current core unblocks and begins executing its data transfer or atomic operation. To ensure that the current core does not execute transfer instructions before the semaphore equals 1, intra-core synchronization 3 must be inserted before the transfer instructions.

#### 1.3.3 Scalar Unit Access to Semaphores on GM

The Scalar unit accesses semaphores on GM through two access methods:

1. **Access through DCache**
   Use the GetValue and SetValue member functions of GlobalTensor. In this case, developers must manually call the [DataCacheCleanAndInvalid](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/API/ascendcopapi/atlasascendc_api_07_0177.html) API to ensure data consistency across multiple cores.

2. **Access bypassing DCache**
   Use [WriteGmBypassDCache](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/API/ascendcopapi/atlasascendc_api_07_00089.html) and ReadGmBypassDCache. This method ensures multi-core data consistency without additional operations.

Performance difference between the two schemes: bypassing DCache has relatively poorer performance, but if the amount of data read from or written to GM is small, the bypass-DCache method can be considered.
The inter-core synchronization scheme also requires intra-core synchronization. The roles of the three intra-core synchronization points are described as follows:

- **Intra-core synchronization 1 (optional)**: When multiple data transfer instructions exist within a core, this synchronization ensures that each transfer operation executes strictly in order.
- **Intra-core synchronization 2 (required)**: Only after all tasks of the previous core have completed is the Scalar unit allowed to write 1 to the global memory semaphore.
- **Intra-core synchronization 3 (required)**: After the Scalar unit detects that the semaphore has been updated to 1, the current core starts executing subsequent tasks.

## 2. Example Description
### 2.1 Scenario Configuration Description
<table border="1" style="text-align: left;">
  <tr>
    <td>SCENARIO_NUM Value</td>
    <td>Business Scenario</td>
    <td>Kernel Function</td>
    <td>Synchronization Mode Used</td>
  </tr>
  <tr>
    <td>1</td>
    <td>Deterministic computation within a single AIV core</td>
    <td>set_atomic_add_single_aiv_custom</td>
    <td>PipeBarrier ensures ordering</td>
  </tr>
  <tr>
    <td>2</td>
    <td>Deterministic computation across multiple AIV cores</td>
    <td>set_atomic_add_multi_aiv_custom</td>
    <td>IBSet/IBWait AIV inter-core synchronization</td>
  </tr>
  <tr>
    <td>3</td>
    <td>Deterministic computation across multiple AIC cores</td>
    <td>set_atomic_add_multi_aic_custom</td>
    <td>GM semaphore inter-core synchronization</td>
  </tr>
</table>

### 2.2 Computation Formula and Example Specifications

All three scenarios use the same computation formula:

$$
z = src0 + src2 + src3 + src1
$$

Where:
- `src0` is the GM initial value vector (all zeros)
- `src1`, `src2`, `src3` are the three input vectors participating in atomic accumulation
- The accumulation order is fixed as: `src0` (initial) -> +`src2` -> +`src3` -> +`src1`
- `z` is the final accumulation result

#### 2.2.1 SCENARIO_NUM=1 (Deterministic Computation Within a Single AIV Core)
- **Computation method**: Execute atomic accumulation operations in a fixed sequence within a single AIV core
- **Synchronization mechanism**: Use PipeBarrier to ensure each atomic operation completes before the next one starts

- **Example specifications**:
  <table border="1">
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">SetAtomicAddSingleAiv</td></tr>
  <tr><td rowspan="5" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src0</td><td align="center">[8]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td align="center">src1</td><td align="center">[8]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td align="center">src2</td><td align="center">[8]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td align="center">src3</td><td align="center">[8]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[8]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Core Count (numBlocks)</td><td colspan="4" align="center">1</td></tr>
  </table>

#### 2.2.2 SCENARIO_NUM=2 and SCENARIO_NUM=3 (Deterministic Computation Across Multiple Cores)
The core logic of these two scenarios is the same, with the difference being that the accumulated data is distributed across multiple AIV cores or AIC cores. Compared to the single-core scenario, multi-core scenarios require synchronization between AIV cores or AIC cores, so the input adds a sync_buf parameter for storing inter-core synchronization semaphores on GM, whose initial value must be 0.

- **Example specifications**:
  <table border="1">
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">SetAtomicAddMultiCore</td></tr>
  <tr><td rowspan="6" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src0</td><td align="center">[8]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td align="center">src1</td><td align="center">[8]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td align="center">src2</td><td align="center">[8]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td align="center">src3</td><td align="center">[8]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td align="center">sync_buf</td><td align="center">[256]</td><td align="center">int32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[8]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Core Count (numBlocks)</td><td colspan="4" align="center">4</td></tr>
  </table>

### 2.3 Notes

#### (1) When `SCENARIO_NUM=3`, the example does not support Ascend 950PR/Ascend 950DT.
When `SCENARIO_NUM=1` and `SCENARIO_NUM=2`, the example supports Ascend 950PR/Ascend 950DT.
When `SCENARIO_NUM=3`, it calls `DataCopy` to transfer data from L1 to GM. However, under the Ascend 950PR/Ascend 950DT architecture, the DataCopy API does not support the L1 Buffer -> GM path. Therefore, when `SCENARIO_NUM=3`, the example does not support Ascend 950PR/Ascend 950DT. To support Ascend 950PR/Ascend 950DT, refer to the compatibility scheme in the [basic API migration guide](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_compatibility_10_00005.html).

#### (2) When `SCENARIO_NUM=2`, the example does not support static tensor programming.
When SCENARIO_NUM=2, it calls `IBSet` and `IBWait` for inter-core synchronization. However, the internal implementation of these two APIs requires the TPipe framework for intra-core synchronization. Therefore, when SCENARIO_NUM=2, the example does not support static tensor programming.

## Build and Run
Run the following steps in the root directory of this example to build and run it.
- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  SCENARIO_NUM=1 # Default demonstrates deterministic computation within a single AIV core
  mkdir -p build && cd build;   # Create and enter the build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=${SCENARIO_NUM};make -j;  # Build the project
  python3 ../scripts/gen_data.py
  ./demo                        # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct, confirming algorithm logic correctness
  ```
  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
