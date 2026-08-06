# printf<a name="ZH-CN_TOPIC_0000001665961730"></a>

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
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section259105813316"></a>

头文件路径为：`"utils/debug/asc_printf.h"`。

该接口提供CPU域/NPU域调试场景下的格式化输出功能。

在算子kernel侧实现代码中需要输出日志信息的地方调用printf接口打印相关内容。样例如下：

```cpp
#include "kernel_operator.h"
AscendC::printf("fmt string %d\n", 0x123);
```
> [!CAUTION]注意
> 该接口主要用于调试分析，开启后会对算子性能产生一定影响，通常在调试阶段使用，生产环境建议关闭。<br>
> 默认情况下，调用该接口就会打印相关内容，开发者可以参考[关闭ASCENDC_DUMP说明](../关闭ASCENDC_DUMP说明.md)，按需关闭该接口功能。

## 函数原型<a name="section2067518173415"></a>

```cpp
template <class... Args>
__aicore__ inline void printf(__gm__ const char* fmt, Args&&... args)
template <class... Args>
__aicore__ inline void PRINTF(__gm__ const char* fmt, Args&&... args) // 此接口为保证兼容性而保留，不建议在新代码中使用。请优先使用printf替代。
```

## 参数说明<a name="section158061867342"></a>

**表**  参数说明

| 参数名称 | 输入/输出 | 描述 |
| ------ | ------ | ------ |
| fmt | 输入 | 格式控制字符串，包含两种类型的对象：普通字符和转换说明。<br>•普通字符将原样不动地打印输出。<br>•转换说明并不直接输出而是用于控制printf中参数的转换和打印。每个转换说明都由一个百分号字符（%）开始，以转换说明结束，从而说明输出数据的类型。<br>•支持的转换类型包括：<br>&ensp;&ensp;▪%d / %i：输出十进制数，支持打印的数据类型：bool、int8_t、int16_t、int32_t、int64_t。<br>&ensp;&ensp;▪%f：输出实数，支持打印的数据类型：half、float、bfloat16。<br>&ensp;&ensp;▪%x：输出十六进制整数，支持打印的数据类型：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、int64_t、uint64_t。<br>&ensp;&ensp;▪%s：输出字符串。<br>&ensp;&ensp;▪%u：输出unsigned类型数据，支持打印的数据类型：bool、uint8_t、uint16_t、uint32_t、uint64_t。<br>&ensp;&ensp;▪%p：输出指针地址。<br>**注意**：<br>•上文列出的数据类型是NPU域调试支持的数据类型，CPU域调试时，支持的数据类型和C/C++规范保持一致。<br>•在转换类型为%x，即输出十六进制整数时，NPU域上的输出为64位，CPU域上的输出为32位。 |
| args | 输入 | 附加参数，个数和类型可变的参数列表：根据不同的fmt字符串，函数可能需要一系列的附加参数，每个参数包含了一个要被插入的值，替换了fmt参数中指定的每个%标签。参数的个数应与%标签的个数相同。 |

## 返回值说明<a name="section640mcpsimp"></a> 
 
 无

## 约束说明<a name="section43265506459"></a>

可以参考utils目录SIMD约束下的[printf](../../../../Utils-API/调测接口/printf-290.md)说明。

## 调用示例<a name="section82241477610"></a>

```cpp
#include "kernel_operator.h"

// 整型打印：
AscendC::printf("fmt string %d\n", 0x123);

// 浮点型打印：
float a = 3.14;
AscendC::printf("fmt string %f\n", a);

// 指针打印：
int b = 0x123;
int *c = &b;
AscendC::printf("TEST %p\n", c);
```

NPU模式下，程序运行时打印效果如下：

```plain
fmt string 291
fmt string 291
fmt string 3.140000
fmt string 3.140000
TEST 0x357f9c
TEST 0x357f9c
```
