# assert<a name="ZH-CN_TOPIC_0000001920717221"></a>

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
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->

## 功能说明<a name="section259105813316"></a>

头文件路径为：`"utils/debug/asc_assert.h"`。

该接口实现CPU/NPU域assert断言功能。算子执行中，如果assert内部条件判断不为真，则输出assert条件并将输入的信息格式化打印在屏幕上。

在算子核函数（Kernel）侧实现代码中需要增加断言的地方使用assert检查代码，并格式化输出一些调测信息。示例如下：

```cpp
int assertFlag = 10;

assert(assertFlag == 10);
```

> [!CAUTION]注意 
> 该接口主要用于调试分析，开启后会对算子性能产生一定影响，通常在调试阶段使用，生产环境建议关闭。<br>
> 默认情况下，调用该接口就会打印相关内容，开发者可以参考[关闭ASCENDC_DUMP说明](../disable_ascendc_dump.md)，按需关闭该接口功能。

## 函数原型<a name="section2067518173415"></a>

```cpp
define assert(expr)
define assert(expr, __gm__ const char *fmt, Args&&... args)
```

## 参数说明<a name="section158061867342"></a>

**表1**  参数说明

| 参数名称 | 输入/输出 | 描述 |
| ------ | ------ | ------ |
| expr | 输入 | assert断言是否终止程序的条件。为true则程序继续执行，为false则终止程序。 |
| fmt | 输入 | 格式控制字符串，包含两种类型的对象：普通字符和转换说明。<br>•普通字符将原样不动地打印输出。<br>•转换说明并不直接输出而是用于控制printf中参数的转换和打印。每个转换说明都由一个百分号字符（%）开始，以转换说明结束，从而说明输出数据的类型。<br>•支持的转换类型包括：<br>&ensp;&ensp;▪%d / %i：输出十进制数，支持打印的数据类型：bool、int8_t、int16_t、int32_t、int64_t。<br>&ensp;&ensp;▪%f：输出实数，支持打印的数据类型：float、half。<br>&ensp;&ensp;▪%x：输出十六进制整数，支持打印的数据类型：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、int64_t、uint64_t。<br>&ensp;&ensp;▪%s：输出字符串。<br>&ensp;&ensp;▪%u：输出unsigned类型数据，支持打印的数据类型：bool、uint8_t、uint16_t、uint32_t、uint64_t。<br>&ensp;&ensp;▪%p：输出指针地址。<br>**注意**：<br>•上文列出的数据类型是NPU域调试支持的数据类型，CPU域调试时，支持的数据类型和C/C++规范保持一致。<br>•在转换类型为%x，即输出十六进制整数时，NPU域上的输出为64位，CPU域上的输出为32位。 |
| args | 输入 | 附加参数，个数和类型可变的参数列表：根据不同的fmt字符串，函数可能需要一系列的附加参数，每个参数包含了一个要被插入的值，替换了fmt参数中指定的每个%标签。参数的个数应与%标签的个数相同。 |

## 数据类型

无

## 返回值说明<a name="section640mcpsimp"></a>	 

无

## 约束说明<a name="section43265506459"></a>

- 单次调用本接口打印的数据总量不可超过30KB。使用时应注意，如果超出这个限制，则数据不会被打印。
- 在cmake工程的release模式下，默认使能NDEBUG宏定义，assert打印信息失效。

## 调用示例<a name="section82241477610"></a>

```cpp
int assertFlag = 10;
// 断言条件
assert(assertFlag != 10);
```

程序运行时会触发assert，打印效果如下：

```plain
[ASSERT] /home/.../add_custom.cpp:44: Assertion `assertFlag != 10' 
```
