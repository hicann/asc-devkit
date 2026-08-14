# CPU域孪生调试<a name="ZH-CN_TOPIC_0000001664002637"></a>

本节介绍CPU域调试的方法：CPU侧验证核函数，gdb调试、使用printf命令打印。当前SIMT编程场景不支持。

>[!NOTE]说明
>CPU调测过程中，配置日志相关环境变量，可以记录程序的运行过程及异常信息，有助于开发者进行功能调测。
>关于环境变量的使用约束以及详细说明，可参见[《环境变量参考》](https://www.hiascend.com/document/redirect/CannCommunityEnvRef)中“辅助功能 \> 日志”章节。

## CPU侧验证核函数<a name="section1227643165914"></a>

在非昇腾设备上，开发者可以利用CPU仿真环境先行进行算子开发和测试，并在准备就绪后，利用昇腾设备进行加速计算。在[编译与运行](../../编译与运行/编译与运行.md)章节，我们已经介绍了算子Kernel程序NPU域的编译运行。相比于NPU域的算子运行逻辑，CPU域调试将算子Kernel程序以Host程序的形式进行编译，此时算子Kernel程序链接CPU调测库，执行编译生成的可执行文件，可以完成算子CPU域的运行验证。CPU侧的运行程序，通过GDB通用调试工具进行单步调试，可以精准验证程序执行流程是否符合预期。

**图1**  CPU域和NPU域的核函数运行逻辑对比<a name="fig39851716019"></a>  
![](../../../figures/cpu_npu.png "CPU域和NPU域的核函数运行逻辑对比")

推荐使用CMake编译方式，可在最小化修改的情况下快速开启CPU域孪生调试功能。

1.  通过在CMake配置阶段传入变量CMAKE\_ASC\_RUN\_MODE和CMAKE\_ASC\_ARCHITECTURES即可开启CPU域编译。命令示例如下：

    ```
    cmake -B build -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201
    ```

    cpu表示开启CPU域编译，dav-后为NPU架构版本号，请根据实际情况参考[对应关系表](../../语言扩展层/SIMD-BuiltIn关键字.md#npu-arch)进行填写。

    其他CMakeLists.txt项目配置[通过CMake编译](../../编译与运行/算子编译/AI-Core算子编译基本用法.md#ZH-CN_TOPIC_0000002428982142)进行编写。

>[!NOTE]说明 
>为了实现CPU域与NPU域代码归一，框架在CPU域中仅对部分acl接口进行适配，开发者在使用CPU域调测功能时，仅支持使用如下acl接口，并且不支持用户自行链接**ascendcl库**：
>- 有实际功能接口，支持CPU域调用
>    - aclDataTypeSize、aclFloat16ToFloat、aclFloatToFloat16。
>    - aclrtMalloc、aclrtFree、aclrtMallocHost、aclrtFreeHost、aclrtMemset、aclrtMemsetAsync、aclrtMemcpy、aclrtMemcpyAsync、aclrtMemcpy2d、aclrtMemcpy2dAsync、aclrtCreateContext、aclrtDestroyContext。
>- 无实际功能接口，打桩实现。
>    - Profiling数据采集
>        aclprofInit、aclprofSetConfig、aclprofStart、aclprofStop、aclprofFinalize。
>    - 系统配置
>        aclInit、aclFinalize、aclrtGetVersion。
>    - 运行时管理
>        aclrtSetDevice、aclrtResetDevice、aclrtCreateStream、aclrtCreateStreamWithConfig、aclrtDestroyStream、aclrtDestroyStreamForce、aclrtSynchronizeStream、aclrtCreateContext、aclrtDestroyContext。

> [!CAUTION] 注意
> 在CPU孪生调试模式下，编译器不识别Ascend C提供的[SIMD-BuiltIn关键字](../../语言扩展层/SIMD-BuiltIn关键字.md)及[SIMT-BuiltIn关键字](../../语言扩展层/SIMT-BuiltIn关键字.md)，包括
> - **函数执行空间限定符**：如`__aicore__`、`__global__`、`__host__`等;
> - **函数标记宏**：如`__simd_vf__`、`__simd_callee__`、`__simt_vf__`等；
> - **地址空间限定符**：如`__gm__` 、`__ubuf__`等。
>
> 这些关键字与Device侧执行强相关，在CPU孪生调试模式下无需实际生效，因此系统框架将这些关键字定义为空。
> 由于这种置空处理，**仅通过BuiltIn关键字区分的函数签名将被视为相同**，会导致重定义编译错误。
> 
> 典型问题场景包括：
> 1. **Device侧函数与Host侧函数同名**
> 
> <blockquote>
> 如下例中，`add`函数仅通过`__aicore__`区分运行域，在CPU孪生调试模式下两个声明将冲突：
> 	<pre><code class="language-asc">__aicore__ inline void add(int x, int y);  // Device侧
> void add(int x, int y);                    // Host侧（等同于 __host__）</code></pre>
> </blockquote>
>
> 2. **参数类型仅有地址空间限定符不同**
> 
> <blockquote>
> 如下例中，两个Device侧函数仅在指针的地址空间限定符上不同：
> 	<pre><code class="language-asc">__aicore__ inline void process(__gm__ void* data, int size);
> __aicore__ inline void process(__ubuf__ void* data, int size); </code></pre>
> </blockquote>
> 
> **在孪生调试场景下需特别注意**：请勿定义仅依赖BuiltIn关键字来区分的函数。

## gdb调试<a name="section13838280458"></a>

可使用**gdb**单步调试算子计算精度。由于cpu调测已转为多进程调试，每个核都会拉起独立的子进程，故gdb需要转换成子进程调试的方式。针对耦合架构，每个AI Core会拉起1个子进程。针对分离架构，默认每个AI Core会拉起3个子进程，1个Cube，2个Vector。

-   调试单独一个子进程

    启动gdb，示例中的add\_custom\_cpu为CPU域的算子可执行文件，参考[修改并执行一键式编译运行脚本](../../附录/基于样例工程完成Kernel直调.md#section188001652105215)，将一键式编译运行脚本中的run-mode设置成cpu，即可编译生成CPU域的算子可执行文件。

    gdb启动后，首先设置跟踪子进程，之后再打断点，就会停留在子进程中，但是这种方式只会停留在遇到断点的第一个子进程中，其余子进程和主进程会继续执行直到退出。涉及到核间同步的算子无法使用这种方法进行调试。

    ```
    gdb --args add_custom_cpu  // 启动gdb，add_custom_cpu为算子可执行文件
    (gdb) set follow-fork-mode child
    ```

-   调试多个子进程

    如果涉及到核间同步，那么需要能同时调试多个子进程。

    在gdb启动后，首先设置调试模式为只调试一个进程，挂起其他进程。设置的命令如下：

    ```
    (gdb) set detach-on-fork off
    ```

    查看当前调试模式的命令为：

    ```
    (gdb) show detach-on-fork
    ```

    中断gdb程序要使用捕捉事件的方式，即gdb程序捕捉fork这一事件并中断。这样在每一次起子进程时就可以中断gdb程序。设置的命令为：

    ```
    (gdb) catch fork
    ```

    当执行r后，可以查看当前的进程信息：

    ```
    (gdb) info inferiors
      Num  Description
    * 1    process 19613
    ```

    可以看到，当第一次执行fork的时候，程序断在了主进程fork的位置，子进程还未生成。

    执行c后，再次查看info inferiors，可以看到此时第一个子进程已经启动。

    ```
    (gdb) info inferiors
      Num  Description 
    * 1    process 19613
      2    process 19626
    ```

    这个时候可以使用切换到第二个进程，也就是第一个子进程，再打上断点进行调试，此时主进程是暂停状态：

    ```
    (gdb) inferior 2
    [Switching to inferior 2 [process 19626] ($HOME/demo)]
    (gdb) info inferiors
      Num  Description
      1    process 19613
    * 2    process 19626
    ```

    请注意，inferior后跟的数字是进程的序号，而不是进程号。

    如果遇到同步阻塞，可以切换回主进程继续生成子进程，然后再切换到新的子进程进行调试，等到同步条件完成后，再切回第一个子进程继续执行。

如下是调试一个单独子进程的命令样例：

```
gdb --args add_custom_cpu
set follow-fork-mode child
break add_custom.cpp:45
run
list
backtrace
print i
break add_custom.cpp:56
continue
display xLocal
quit
```

## 使用printf打印命令打印<a name="section1829231615466"></a>

在代码中直接编写printf\(...\)来观察数值的输出。样例代码如下：

```
printf("xLocal size: %d\n", xLocal.GetSize()); 
printf("tileLength: %d\n", tileLength);
```
