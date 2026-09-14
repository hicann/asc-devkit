# NPU On-Board Debugging<a name="ZH-CN_TOPIC_0000001663882793"></a>

<!-- md-trans-meta sourceCommit=84d4771c758afa4058a229c863ddc58cf59e3dd3 translatedAt=2026-08-26T11:02:10.031Z pushedAt=2026-08-31T07:09:45.640Z -->

NPU on-board debugging mainly includes on-board data printing, msSanitizer memory exception detection, and msDebug single-step debugging. Data printing provides two methods: printf and DumpTensor. DumpTensor is a feature exclusive to SIMD programming and is used to print the data of a specified tensor.

## Printing Data Through printf<a name="section1962503317229"></a>

**printf** is mainly used to print scalar and string information, and is supported by both SIMT programming and SIMD programming.

The following is an example of **printf**. For details about how to use the **printf** API and its specific constraints, see [printf](../../../../api/Utils-API/tuning_interface/printf-290.md).

```
printf("fmt string %d", 0x123);
```

> [!NOTE] Note
> The printing function of the **printf** API affects the actual running performance of operators to a certain extent, and is usually used in the debugging phase. Developers can disable the printing function as required. For details, see [printf](../../../../api/Utils-API/tuning_interface/printf-290.md).

## Debugging SIMD Programming Through DumpTensor Printing<a name="section16452184823718"></a>

DumpTensor is a printing function unique to the SIMD programming scenario. It is used to print the data of a specified tensor during on-board debugging in the NPU domain.

**The specific usage is as follows**:

Call the DumpTensor API at the location in the operator kernel-side implementation code where log information needs to be output to print the relevant content.

As shown below, **srcLocal** indicates the tensor to be printed; 5 indicates the user-defined additional information, such as the current code line number; and **dataLen** indicates the number of elements. For details about the usage and specific constraints of the DumpTensor API, see [DumpTensor](../../../../api/SIMD-API/basic_api/debug_interface/onboard_print/DumpTensor.md).

```
DumpTensor(srcLocal,5, dataLen);
```

During dumping, a corresponding header DumpHead (32 bytes) is added before the dump information of each block core to record the core number and resource usage information. A header DumpTensorHead (32 bytes) is also added before the tensor data of each dump to record the relevant information of the tensor. The following is an example of the printing result:

```
DumpTensor: desc=5, addr=0, data_type=float16, position=UB, dump_size=32
[19.000000, 4.000000, 38.000000, 50.000000, 39.000000, 67.000000, 84.000000, 98.000000, 21.000000, 36.000000, 18.000000, 46.000000, 10.000000, 92.000000, 26.000000, 38.000000, 39.000000, 9.000000, 82.000000, 37.000000, 35.000000, 65.000000, 97.000000, 59.000000, 89.000000, 63.000000, 70.000000, 57.000000, 35.000000, 3.000000, 16.000000,
42.000000]
DumpTensor: desc=5, addr=100, data_type=float16, position=UB, dump_size=32
[6.000000, 34.000000, 52.000000, 38.000000, 73.000000, 38.000000, 35.000000, 14.000000, 67.000000, 62.000000, 30.000000, 49.000000, 86.000000, 37.000000, 84.000000, 18.000000, 38.000000, 18.000000, 44.000000, 21.000000, 86.000000, 99.000000, 13.000000, 79.000000, 84.000000, 9.000000, 48.000000, 74.000000, 52.000000, 99.000000, 80.000000,
53.000000]
...
DumpTensor: desc=5, addr=0, data_type=float16, position=UB, dump_size=32
[35.000000, 41.000000, 41.000000, 22.000000, 84.000000, 49.000000, 60.000000, 0.000000, 90.000000, 14.000000, 67.000000, 80.000000, 16.000000, 46.000000, 16.000000, 83.000000, 6.000000, 70.000000, 97.000000, 28.000000, 97.000000, 62.000000, 80.000000, 22.000000, 53.000000, 37.000000, 23.000000, 58.000000, 65.000000, 28.000000, 4.000000,
29.000000]

```

> [!NOTE] Note
> The DumpTensor API printing function affects the actual running performance of the operator to a certain extent and is usually used in the debugging phase. Developers can disable the printing function as required. For details, see [DumpTensor](../../../../api/SIMD-API/basic_api/debug_interface/onboard_print/DumpTensor.md).

## Using the msSanitizer Tool for Exception Detection<a name="section931475414217"></a>

The msSanitizer tool is an exception detection tool based on the AI processor. It provides four sub-functions for single-operator development scenarios: memory detection, race detection, uninitialized detection, and synchronization detection. Currently, it supports program debugging only in SIMD programming scenarios, and does not support program debugging in SIMT programming scenarios.

-   Memory detection: During operator development, the tool helps you locate memory issues such as illegal read/write, multi-core trampling, unaligned access, memory leaks, and illegal release. It also supports memory detection for the CANN software stack, helping you identify the module where a software stack memory exception occurs.
-   Race detection: The tool helps you locate data race issues that may be caused by race risks, including intra-core and inter-core race issues. Intra-core races include inter-pipeline races and intra-pipeline races.
-   Uninitialized detection: The tool helps you locate dirty data read issues that may be caused by uninitialized memory.
-   Synchronization detection: The tool helps you locate the issue where a subsequent operator fails to synchronize due to unpaired synchronization instructions in a preceding operator.

For details about how to use the tool, see [*msSanitizer User Guide*](https://gitcode.com/Ascend/mssanitizer/blob/26.1.0/docs/en/user_guide/mssanitizer_user_guide.md).

**Compiling the Exception Detection Program**
```shell
// Add the sanitizer command line during BiSheng compilation.
bisheng add_custom.asc -o add_custom --sanitizer -lineinfo
```
**Using the Exception Detection Tool**
```shell
// Enable multiple exception detection capabilities simultaneously.
mssanitizer --tool=memcheck --tool=racecheck --tool=initcheck --tool=synccheck ./add_custom
```
**Parsing Memory Exception Reports**

A memory detection exception report outputs multiple types of exception information. The following provides a simple example of exception information for an illegal read/write:
```shell
====== ERROR: illegal read of size 224  // Basic information about the exception, including the type of illegal read/write and the number of bytes illegally accessed. Illegal read/write includes read (illegal read) and write (illegal write).
======    at 0x12c0c0015000 on GM in add_custom_kernel  // Memory location information where the exception occurs, including the kernel function name, address space, and memory address. The memory address here refers to the first address of a memory access.
======    in block aiv(0) on device 0  // Block index of the vector core corresponding to the exception code.
======    code in pc current 0x77c (serialNo:10) // PC pointer where the current exception occurs and the serial number of the API call behavior.
======    #0 ${ASCEND_HOME_PATH}/asc/impl/basic_api/dav_c220/kernel_operator_data_copy_impl.h :58:9  // The following is the call stack of the code where the exception occurs, including the file name, line number, and column number.
======    #1 ${ASCEND_HOME_PATH}/asc/impl/basic_api/kernel_operator_data_copy_intf_impl.h:58:9
======    #2 ${ASCEND_HOME_PATH}/asc/include/basic_api/kernel_operator_data_copy_intf.h:443:5
======    #3 illegal_read_and_write/add_custom.cpp:18:5
```

## Using msDebug to Debug Operators<a name="section2072113416285"></a>

msDebug is an operator debugging tool for Ascend devices. It is used to debug operator programs running on the NPU side and provides debugging methods for operator developers. Currently, it supports only program debugging in SIMD programming scenarios, and does not support program debugging in SIMT programming scenarios. msDebug supports debugging all Ascend operators, including Ascend C operator (vector, cube, and fusion operator) programs. Its specific functions include breakpoint setting, variable and memory printing, single-step debugging, interrupt execution, core switching, program status checking, debugging information display, and core dump file parsing. You can select the required functions based on actual conditions.

### msDebug Debugging Process

#### Compiling the Debug Program
```shell
// Add the debug command line when compiling with BiSheng.
bisheng add_custom.asc -o add_custom  -O0 -g
```
#### Running the Debug Program
```shell
// Use msDebug for debugging.
$ msdebug ./add_custom
msdebug(MindStudio Debugger) is part of MindStudio Operator-dev Tools.
The tool provides developers with a mechanism for debugging Ascend kernels running on actual hardware.
This enables developers to debug Ascend kernels without being affected by potential changes brought by simulation and emulation environments.
(msdebug) target create "python3"
Current executable set to '${INSTALL_DIR}/projects/application' (aarch64).

(msdebug) b matmul_leakyrelu_kernel.cpp:114
Breakpoint 1: where = device_debugdata`_ZN17MatmulLeakyKernelIDhDhffE7CopyOutEj_mix_aiv + 240 at matmul_leakyrelu_kernel.cpp:114:14, address = 0x000000000000ff88
(msdebug) run
```

#### Setting Breakpoints
```shell
(msdebug) b matmul_leakyrelu_kernel.cpp:114
Breakpoint 1: where = device_debugdata`_ZN17MatmulLeakyKernelIDhDhffE7CopyOutEj_mix_aiv + 240 at matmul_leakyrelu_kernel.cpp:114:14, address = 0x000000000000ff88
```

#### Printing Memory and Variables
```shell
#  Print LocalTensor
(msdebug) p reluOutLocal
(AscendC::LocalTensor<float>) $2 = {
  AscendC::BaseLocalTensor<float> = {
    address_ = (dataLen = 131072, bufferAddr = 0, bufferHandle = "", logicPos = '\n')
  }
  shapeInfo_ = {
    shapeDim = '\0'
    originalShapeDim = '\0'
    shape = ([0] = 0, [1] = 1092616192, [2] = 4800, [3] = 1473680, [4] = 0, [5] = 1473888, [6] = 0, [7] = 1471968)
    originalShape = ([0] = 0, [1] = 3222199212, [2] = 4800, [3] = 1, [4] = 0, [5] = 1473376, [6] = 0, [7] = 1473376)
    dataFormat = ND
  }
}
```

#### Performing Single-Step Debugging
```shell
(msdebug) s
Process 177943 stopped
[Switching to focus on Kernel matmul_leakyrelu_custom, CoreId 44, Type aiv]
* thread #1, name = 'matmul_leakyrelu', stop reason = step over   //   The PC location can be viewed through the echo, indicating that the single step succeeded.
    frame #0: 0x000000000000f048 device_debugdata`_ZN17MatmulLeakyKernelIDhDhffE10CalcOffsetEiiRK11TCubeTilingRiS4_S4_S4__mix_aiv(this=0x0000000000217b60, blockIdx=0, usedCoreNum=2, tiling=0x0000000000217e28, offsetA=0x00000000002175c8, offsetB=0x00000000002175c4, offsetC=0x00000000002175c0, offsetBias=0x00000000002175bc) at matmul_leakyrelu_kernel.cpp:130:18
   127      offsetA = mCoreIndx * tiling.Ka * tiling.singleCoreM;
   128      offsetB = nCoreIndx * tiling.singleCoreN;
   129      offsetC = mCoreIndx * tiling.N * tiling.singleCoreM + nCoreIndx * tiling.singleCoreN;
-> 130      offsetBias = nCoreIndx * tiling.singleCoreN;
   131  }
   ```

#### Displaying Debugging Information
   ```shell
    (msdebug) ascend info cores
  CoreId  Type  Device Stream Task Block         PC               stop reason
   12     aic      1     3     0     0     0x12c0c00f03b0         breakpoint 1.2
*  44     aiv      1     3     0     0     0x12c0c00f8048         step over               //* indicates the core currently running
   45     aiv      1     3     0     0     0x12c0c00f801c         breakpoint 1.2
   ```

### Parsing the Exception Operator Dump File
Enable core file dumping upon operator exception through environment variables.
```shell
# aic_err_detail_dump: Export the memory storage, registers, and call stack information of the AI Core.
export ASCEND_DUMP_SCENE=aic_err_detail_dump
# Specify the storage path of the exception operator dump information, which can be configured as an absolute path or a path relative to the executable program.
export ASCEND_DUMP_PATH=./
```
Use msDebug to parse the core file:
```shell
msdebug --core output2/extra-info/data-dump/0/xxx.core add_custom
```
For more details, see [*msDebug User Guide*](https://gitcode.com/Ascend/msdebug/blob/26.1.0/docs/en/user_guide/msdebug_user_guide.md).
