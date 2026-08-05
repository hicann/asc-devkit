# printf

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->

## 功能说明

本接口提供SIMD和SIMT编程调试场景下的格式化输出功能。

在算子Kernel侧的实现代码中，需要输出日志信息时，调用printf接口打印相关内容。

> [!CAUTION]注意
>printf（PRINTF）接口打印功能会对算子实际运行的性能带来一定影响，通常在调测阶段使用。开发者可以按需通过设置ASCENDC\_DUMP=0的方式关闭打印功能。

## 函数原型

```
template <class... Args>
__aicore__ inline void printf( const __gm__ char* fmt, Args&&... args)
```

<!-- npu="950" id9 -->
以下接口为simd\_vf中所使用的printf接口，仅支持Ascend 950PR/Ascend 950DT。

```
template <class... Args>
__simd_callee__ inline void printf( const __ubuf__ char* fmt, Args&&... args)
```
<!-- end id9 -->

<!-- npu="950" id10 -->
以下接口为SIMT中所使用的printf接口，仅支持Ascend 950PR/Ascend 950DT。

```
template <class... Args>
static __attribute__((noinline)) void printf(const char* fmt, Args&&... args);
```
<!-- end id10 -->

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| fmt | 输入 | 格式控制字符串，包含两种类型的对象：普通字符和转换说明。<br><br>普通字符将原样不动地打印输出。<br>转换说明并不直接输出而是用于控制printf中参数的转换和打印。每个转换说明都由一个百分号字符（%）开始，以转换说明结束，从而说明输出数据的类型。<br>支持的转换类型包括：<br>%d、%ld、%lld、%i、%li、%lli：输出十进制数，支持打印的数据类型：int8_t、int16_t、int32_t、int64_t<br>%f、%F：输出浮点数，支持打印的数据类型：float、half、bfloat16_t<br>%x、%lx、%llx：输出十六进制整数，支持打印的数据类型：int8_t、int16_t、int32_t、int64_t、uint8_t、uint16_t、uint32_t、uint64_t<br>%s：输出字符串<br>%u、%lu、%llu：输出unsigned类型数据，支持打印的数据类型：uint8_t、uint16_t、uint32_t、uint64_t<br>%p：输出指针地址<br><br>注意：上文列出的数据类型是NPU域调试支持的数据类型，CPU域调试时，支持的数据类型和C/C++规范保持一致。 |
| args | 输入 | 附加参数，个数和类型可变的参数列表：根据不同的fmt字符串，函数可能需要一系列的附加参数，每个参数包含了一个要被插入的值，替换了fmt参数中指定的每个%标签。参数的个数应与%标签的个数相同。 |

## 返回值说明

无

## 约束说明

-   本接口不支持打印除换行符之外的其他转义字符。
<!-- npu="950,A3,910b" id14 -->
-   SIMT场景的支持情况如下：
      <!-- npu="950" id11 -->
    - Ascend 950PR/Ascend 950DT：支持。
      <!-- end id11 -->
      <!-- npu="A3" id12 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持。
      <!-- end id12 -->
      <!-- npu="910b" id13 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持。
      <!-- end id13 -->
<!-- end id14 -->
-   SIMT中printf功能需要占用额外的Global Memory空间用于数据缓存，缓存空间大小默认为2MB。您可以通过acl.json中的"simt\_printf\_fifo\_size"字段进行配置，配置范围最小为1MB，最大为64MB。当打印数据量较大时，建议增加缓存空间。
-   SIMT中使用printf接口会增加算子运行时间，请控制打印数据量，避免算子超时。建议在代码中判断线程ID，仅在部分线程中打印调试信息，减少重复内容的打印，更有利于调试。
-   SIMD场景下，单次调用本接口打印的数据总量不可超过打印大小限制，默认为32KB。使用时应注意，如果超出这个限制，则数据不会被打印。您可以通过acl.json中的"simd\_printf\_fifo\_size\_per\_core"字段进行配置，配置范围最小为1KB，最大为64MB。当打印数据量较大时，建议增加缓存空间。pytorch调用和算子入图场景暂不支持该配置。
-   SIMD场景下，根据算子执行方式的不同，printf的打印结果输出方式不同。动态图或者单算子直调场景下，待输出内容会被解析并打印在屏幕上；静态图场景下，整图算子需要全下沉到NPU侧执行，无法直接调用接口打印出单个算子的信息，因此需要在模型执行完毕后，将待输出内容落盘在dump文件中，dump文件需要通过工具解析为可读内容。
    -   dump文件落盘路径按照优先级排列如下：
        -   如果开启了Data Dump功能，dump文件落盘到开发者配置的dump\_path路径下。如何开启Dump功能依赖于具体的网络运行方式。以TensorFlow在线推理为例，通过enable\_dump、dump\_path、dump\_mode等参数进行配置。配置方式可参考《TensorFlow 2.6.5模型迁移》中的API参考 \> TF Adapter接口（2.x）\> npu.global\_options \> 配置参数说明章节。
        -   如果未开启Data Dump功能，但配置了ASCEND\_WORK\_PATH环境变量，dump文件落盘到ASCEND\_WORK\_PATH下的printf目录下。
        -   如果未开启Data Dump功能也没有配置ASCEND\_WORK\_PATH环境变量，dump文件落盘到当前程序执行目录下的printf路径下。

    -   落盘dump文件需要使用工具解析为用户可读内容：

        使用show\_kernel\_debug\_data工具将dump二进制文件解析为用户可读内容，命令格式如下。

        ```
        show_kernel_debug_data  bin_file  output_dir
        ```

-   由于在simd vf中不能直接访问gm地址，因此使用simd vf中的printf时，需要先定义一个ubuf的字符串，该字符串即为原来的fmt变量。

    ```
    __ubuf__ const char* fmt = "simd vf: int=%d, uint=%u, float=%f, string=%s\n";
    ```

-   在`simd_vf`场景下，每个AIV核在单次`asc_vf_call`执行期间最多能使用2KB的UB空间；同一次`asc_vf_call`中的所有`simd_vf`的`printf`和`asc_dump`调用共享该预留空间。
-   每次调用`simd_vf`的`printf`时，除格式字符串和参数外，还会固定占用32字节；整条打印数据需要按8字节对齐。
-   `simd_vf`调测接口不会检查上述预留空间是否越界。超过限制可能越界写入预留空间，导致打印结果异常，并可能影响算子执行。

## 需要包含的头文件

使用该接口需要包含"utils/debug/asc\_printf.h"头文件。

```
#include "utils/debug/asc_printf.h"
```

## SIMD调用示例

```
#include "utils/debug/asc_printf.h"

// SIMD printf
__global__ __mix__(1, 2) void hello_world()
{
    // print string
    printf("hello world device\n");
    // print int
    printf("fmt string int: %d\n", 0x123);
    // print float
    float b = 3.14;
    printf("fmt string float: %f\n", b);
}
```

NPU模式下，程序运行时打印效果如下：

```
hello world device
fmt string int: 291
fmt string float: 3.140000
hello world device
fmt string int: 291
fmt string float: 3.140000
hello world device
fmt string int: 291
fmt string float: 3.140000
hello world device
fmt string int: 291
fmt string float: 3.140000
......
```

## SIMD VF调用示例

```
#include "kernel_operator.h"
#include "utils/debug/asc_printf.h"

__simd_vf__ inline void SimdVfPrint()
{
    __ubuf__ const char* fmt = "simd vf: int=%d, uint=%u, float=%f, string=%s\n";
    printf(fmt, 1, 2U, 5.0f, "AscendC");
}
```

NPU模式下，程序运行时打印效果如下：

```
simd vf: int=1, uint=2, float=5.000000, string=AscendC
simd vf: int=1, uint=2, float=5.000000, string=AscendC
simd vf: int=1, uint=2, float=5.000000, string=AscendC
......
```

## SIMT编程场景调用示例

```
#include "simt_api/asc_simt.h"
#include "utils/debug/asc_printf.h"

// 核函数线程启动参数：dim3(8, 2, 8)
__global__ void SimtCompute()
{
    int x = threadIdx.x;
    int y = threadIdx.y;
    int z = threadIdx.z;
    printf("simt: d: (%d, %d, %d), f: %f, s: %s\n", x, y, z, 3.14f, "pass");
}
```

NPU模式下，程序运行时打印效果如下：

```
simt: d: (0, 0, 0), f: 3.140000, s: pass
simt: d: (1, 0, 0), f: 3.140000, s: pass
simt: d: (2, 0, 0), f: 3.140000, s: pass
simt: d: (3, 0, 0), f: 3.140000, s: pass
simt: d: (4, 0, 0), f: 3.140000, s: pass
simt: d: (5, 0, 0), f: 3.140000, s: pass
simt: d: (6, 0, 0), f: 3.140000, s: pass
simt: d: (7, 0, 0), f: 3.140000, s: pass
simt: d: (0, 1, 0), f: 3.140000, s: pass
simt: d: (1, 1, 0), f: 3.140000, s: pass
......
```

## SIMD与SIMT混合编程场景SIMT VF调用示例

```
#include "kernel_operator.h"
#include "simt_api/asc_simt.h"
#include "utils/debug/asc_printf.h"

// asc_vf_call调用时dim3参数：dim3(8, 2, 8)
__simt_vf__ __launch_bounds__(128) inline void SimtCompute()
{
    int x = threadIdx.x;
    int y = threadIdx.y;
    int z = threadIdx.z;
    printf("simt vf: d: (%d, %d, %d), f: %f, s: %s\n", x, y, z, 3.14f, "pass");
}
```

NPU模式下，程序运行时打印效果如下：

```
simt vf: d: (0, 0, 0), f: 3.140000, s: pass
simt vf: d: (1, 0, 0), f: 3.140000, s: pass
simt vf: d: (2, 0, 0), f: 3.140000, s: pass
simt vf: d: (3, 0, 0), f: 3.140000, s: pass
simt vf: d: (4, 0, 0), f: 3.140000, s: pass
simt vf: d: (5, 0, 0), f: 3.140000, s: pass
simt vf: d: (6, 0, 0), f: 3.140000, s: pass
simt vf: d: (7, 0, 0), f: 3.140000, s: pass
simt vf: d: (0, 1, 0), f: 3.140000, s: pass
simt vf: d: (1, 1, 0), f: 3.140000, s: pass
......
```
