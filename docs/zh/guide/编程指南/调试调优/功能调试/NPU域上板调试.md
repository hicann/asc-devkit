# NPU域上板调试<a name="ZH-CN_TOPIC_0000001663882793"></a>

NPU域上板调试手段主要包含上板数据打印、msSanitizer内存异常检测和msDebug单步调试等功能，数据打印包括printf、DumpTensor两种方式，其中，DumpTensor是SIMD编程独有功能，用于打印指定Tensor的数据。

## 通过printf打印数据<a name="section1962503317229"></a>

printf主要用于打印标量和字符串信息，SIMT编程及SIMD编程均支持。

printf示例如下，printf接口的使用说明和具体约束请参考[printf](../../../../api/Utils-API/tuning_interface/printf.md)。

```
printf("fmt string %d", 0x123);
```

>[!NOTE]说明
>printf接口打印功能会对算子实际运行的性能带来一定影响，通常在调测阶段使用。开发者可以按需关闭打印功能。具体方法请参考[printf](../../../../api/Utils-API/tuning_interface/printf.md)。

## SIMD编程通过DumpTensor打印进行调试<a name="section16452184823718"></a>

DumpTensor是SIMD编程场景独有的打印功能，用于NPU域上板打印指定Tensor的数据。

**具体的使用方法如下**：

在算子kernel侧实现代码中需要输出日志信息的地方调用DumpTensor接口打印相关内容。

如下所示，srcLocal表示待打印的Tensor；5表示用户的自定义附加信息，比如当前的代码行号；dataLen表示元素个数。DumpTensor接口的使用说明和具体约束请参考[DumpTensor](../../../../api/SIMD-API/basic_api/debug_interface/onboard_print/DumpTensor.md)。

```
DumpTensor(srcLocal,5, dataLen);
```

Dump时，每个block核的dump信息前会增加对应信息头DumpHead（32字节大小），用于记录核号和资源使用信息；每次Dump的Tensor数据前也会添加信息头DumpTensorHead（32字节大小），用于记录Tensor的相关信息。打印结果的样例如下：

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

> [!NOTE]说明
> DumpTensor接口打印功能会对算子实际运行的性能带来一定影响，通常在调测阶段使用。开发者可以按需关闭打印功能。具体方法请参考[DumpTensor](../../../../api/SIMD-API/basic_api/debug_interface/onboard_print/DumpTensor.md)。

## 使用msSanitizer工具进行异常检测<a name="section931475414217"></a>

msSanitizer工具是基于AI处理器的异常检测工具，包含了单算子开发场景下的内存检测、竞争检测、未初始化检测和同步检测四个子功能。当前只支持SIMD编程场景的程序调试，暂不支持SIMT编程场景的程序调试。

-   内存检测：工具可以在用户开发算子的过程中，协助定位非法读写、多核踩踏、非对齐访问、内存泄漏以及非法释放等内存问题。同时工具也支持对CANN软件栈的内存检测，帮助用户定界软件栈内存异常发生的模块。
-   竞争检测：工具可以协助用户定位由于竞争风险可能导致的数据竞争问题，包含核内竞争和核间竞争问题。其中，核内竞争包含流水间竞争和流水内竞争。
-   未初始化检测：工具可以协助用户定位由于内存未初始化可能导致的脏数据读取问题。
-   同步检测：工具可以协助用户定位由于前序算子中的未配对同步指令导致的后续算子同步失败的问题。

具体使用方法请参考[《msSanitizer用户指南》](https://gitcode.com/Ascend/mssanitizer/blob/master/docs/zh/user_guide/mssanitizer_user_guide.md)。

**异常检测程序编译**
```shell
// bisheng编译时增加sanitizer命令行
bisheng add_custom.asc -o add_custom --sanitizer -lineinfo
```
**异常检测工具的使用**
```shell
// 同时开启多种异常检测能力
mssanitizer --tool=memcheck --tool=racecheck --tool=initcheck --tool=synccheck ./add_custom
```
**内存异常报告解析**

内存检测异常报告会输出多种不同类型的异常信息，以下将对非法读写给出了简单的异常信息示例：
```shell
====== ERROR: illegal read of size 224  // 异常的基本信息,包含非法读写的类型以及被非法访问的字节数,非法读写包括read(非法读取)和write(非法写入)
======    at 0x12c0c0015000 on GM in add_custom_kernel  // 异常发生的内存位置信息，包含发生的核函数名、地址空间与内存地址，此处的内存地址指一次内存访问中的首地址
======    in block aiv(0) on device 0  // 异常代码对应Vector核的block索引
======    code in pc current 0x77c (serialNo:10) // 当前异常发生的pc指针和调用api行为的序列号
======    #0 ${ASCEND_HOME_PATH}/asc/impl/basic_api/dav_c220/kernel_operator_data_copy_impl.h :58:9  // 以下为异常发生代码的调用栈，包含文件名、行号和列号
======    #1 ${ASCEND_HOME_PATH}/asc/impl/basic_api/kernel_operator_data_copy_intf_impl.h:58:9
======    #2 ${ASCEND_HOME_PATH}/asc/include/basic_api/kernel_operator_data_copy_intf.h:443:5
======    #3 illegal_read_and_write/add_custom.cpp:18:5
```

## 使用msDebug工具进行算子调试<a name="section2072113416285"></a>

msDebug是一款面向昇腾设备的算子调试工具，用于调试NPU侧运行的算子程序，为算子开发人员提供调试手段，当前只支持SIMD编程场景的程序调试，暂不支持SIMT编程场景的程序调试。msDebug工具支持调试所有的昇腾算子，包含Ascend C算子（Vector、Cube以及融合算子）程序。具体功能包括断点设置、打印变量和内存、单步调试、中断运行、核切换、检查程序状态、调试信息展示、解析Core dump文件，用户可根据实际情况进行选择。

### msdebug调试进程

#### 调试程序编译
```shell
// bisheng编译时增加debug命令行
bisheng add_custom.asc -o add_custom  -O0 -g
```
#### 调试程序运行
```shell
// 使用msdebug进行调试
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

#### 断点设置
```shell
(msdebug) b matmul_leakyrelu_kernel.cpp:114
Breakpoint 1: where = device_debugdata`_ZN17MatmulLeakyKernelIDhDhffE7CopyOutEj_mix_aiv + 240 at matmul_leakyrelu_kernel.cpp:114:14, address = 0x000000000000ff88
```

#### 内存与变量打印
```shell
#  打印LocalTensor
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

#### 单步调试
```shell
(msdebug) s
Process 177943 stopped
[Switching to focus on Kernel matmul_leakyrelu_custom, CoreId 44, Type aiv]
* thread #1, name = 'matmul_leakyrelu', stop reason = step over   //   通过回显可查看pc的位置，表示单步成功 
    frame #0: 0x000000000000f048 device_debugdata`_ZN17MatmulLeakyKernelIDhDhffE10CalcOffsetEiiRK11TCubeTilingRiS4_S4_S4__mix_aiv(this=0x0000000000217b60, blockIdx=0, usedCoreNum=2, tiling=0x0000000000217e28, offsetA=0x00000000002175c8, offsetB=0x00000000002175c4, offsetC=0x00000000002175c0, offsetBias=0x00000000002175bc) at matmul_leakyrelu_kernel.cpp:130:18
   127      offsetA = mCoreIndx * tiling.Ka * tiling.singleCoreM;
   128      offsetB = nCoreIndx * tiling.singleCoreN;
   129      offsetC = mCoreIndx * tiling.N * tiling.singleCoreM + nCoreIndx * tiling.singleCoreN;
-> 130      offsetBias = nCoreIndx * tiling.singleCoreN;
   131  }
   ```

#### 调试信息展示
   ```shell
    (msdebug) ascend info cores
  CoreId  Type  Device Stream Task Block         PC               stop reason
   12     aic      1     3     0     0     0x12c0c00f03b0         breakpoint 1.2
*  44     aiv      1     3     0     0     0x12c0c00f8048         step over               //* 代表当前正在运行的核
   45     aiv      1     3     0     0     0x12c0c00f801c         breakpoint 1.2
   ```

### 解析异常算子dump文件
通过环境变量开启算子异常时Dump出Core文件。
```shell
# aic_err_detail_dump: 导出AI Core的内存存储、寄存器和调用栈信息。
export ASCEND_DUMP_SCENE=aic_err_detail_dump
# 指定异常算子Dump信息的存储路径，可配置为绝对路径或执行程序的相对路径。
export ASCEND_DUMP_PATH=./
```
使用msdebug解析core文件：
```shell
msdebug --core output2/extra-info/data-dump/0/xxx.core add_custom
```
更多详细信息可以参考[《msDebug用户指南》](https://gitcode.com/Ascend/msdebug/blob/master/docs/zh/user_guide/msdebug_user_guide.md)。
