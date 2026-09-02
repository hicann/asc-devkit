# Utils API列表

**表1**  C++标准库API列表

| 接口名 | 功能描述 |
| --- | --- |
| [max](cpp_stdlib/algorithms/max.md) | 比较相同数据类型的两个数中的最大值。 |
| [min](cpp_stdlib/algorithms/min.md) | 比较相同数据类型的两个数中的最小值。 |
| [abs](cpp_stdlib/math_functions/abs.md) | 获取输入数据的绝对值。 |
| [ceil\_div](cpp_stdlib/math_functions/ceil_div.md) | 计算两个整数a和b相除后向上取整的结果。 |
| [CeilDivision](cpp_stdlib/math_functions/CeilDivision.md) | 计算两个整数num1和num2相除后向上取整的结果。 |
| [sqrt](cpp_stdlib/math_functions/sqrt.md) | 计算输入数据的平方根。 |
| [integer_sequence](cpp_stdlib/general_utils/integer_sequence.md) | 用于生成一个整数序列。 |
| [tuple](cpp_stdlib/container_functions/tuple.md) | 允许存储多个不同类型元素的容器。 |
| [get](cpp_stdlib/container_functions/get.md) | 从tuple容器中提取指定位置的元素。 |
| [make_tuple](cpp_stdlib/container_functions/make_tuple.md) | 用于便捷地创建tuple对象。 |
| [is_convertible](cpp_stdlib/type_traits/is_convertible.md) | 在程序编译时判断两个类型之间是否可以进行隐式转换。 |
| [is_base_of](cpp_stdlib/type_traits/is_base_of.md) | 在程序编译时判断一个类型是否为另一个类型的基类。 |
| [is_same](cpp_stdlib/type_traits/is_same.md) | 在程序编译时判断两个类型是否完全相同。 |
| [is_void](cpp_stdlib/type_traits/is_void.md) | 在程序编译时，检测一个类型是否为void类型。 |
| [is_integral](cpp_stdlib/type_traits/is_integral.md) | 在程序编译时，检测一个类型是否为整数类型。 |
| [is_floating_point](cpp_stdlib/type_traits/is_floating_point.md) | 在程序编译时，检测一个类型是否为浮点类型。 |
| [is_array](cpp_stdlib/type_traits/is_array.md) | 在程序编译时，检测一个类型是否为数组类型。 |
| [is_pointer](cpp_stdlib/type_traits/is_pointer.md) | 在程序编译时，判断一个类型是否为指针类型。 |
| [is_reference](cpp_stdlib/type_traits/is_reference.md) | 在程序编译时，检测一个类型是否为引用类型。 |
| [is_const](cpp_stdlib/type_traits/is_const.md) | 在程序编译时，检测一个类型是否为const限定的类型。 |
| [remove_const](cpp_stdlib/type_traits/remove_const.md) | 在程序编译时，对传入的模板参数类型移除const限定符。 |
| [remove_volatile](cpp_stdlib/type_traits/remove_volatile.md) | 在程序编译时，对传入的模板参数类型移除volatile限定符。 |
| [remove_cv](cpp_stdlib/type_traits/remove_cv.md) | 在程序编译时，对传入的模板参数类型移除const限定符或volatile限定符，或同时移除这两种限定符。 |
| [remove_reference](cpp_stdlib/type_traits/remove_reference.md) | 在程序编译时，从给定类型中移除引用限定符。 |
| [remove_pointer](cpp_stdlib/type_traits/remove_pointer.md) | 在程序编译时，从给定类型中移除指针限定符。 |
| [add_const](cpp_stdlib/type_traits/add_const.md) | 在程序编译时，为指定类型添加const限定符。 |
| [add_volatile](cpp_stdlib/type_traits/add_volatile.md) | 在程序编译时，为指定类型添加volatile限定符。 |
| [add_cv](cpp_stdlib/type_traits/add_cv.md) | 在程序编译时，为指定类型添加const和volatile限定符。 |
| [add_pointer](cpp_stdlib/type_traits/add_pointer.md) | 在程序编译时，为指定类型添加指针限定符。 |
| [add_lvalue_reference](cpp_stdlib/type_traits/add_lvalue_reference.md) | 在程序编译时，为指定类型添加左值引用限定符。 |
| [add_rvalue_reference](cpp_stdlib/type_traits/add_rvalue_reference.md) | 在程序编译时，为指定类型添加右值引用限定符。 |
| [enable_if](cpp_stdlib/type_traits/enable_if.md) | 在程序编译时根据某个条件启用或禁用特定的函数模板、类模板或模板特化。 |
| [conditional](cpp_stdlib/type_traits/conditional.md) | 在程序编译时根据一个布尔条件从两个类型中选择一个类型。 |
| [integral_constant](cpp_stdlib/type_traits/integral_constant.md) | 用于封装一个编译时常量整数值，是标准库中许多类型特性和编译时计算的基础组件。 |

**表2**  平台信息获取API列表

| 接口名 | 功能描述 |
| --- | --- |
| [PlatformAscendC](platform_info/PlatformAscendC/PlatformAscendC.md) | 在实现Host侧的Tiling函数时，可能需要获取一些硬件平台的信息，来支撑Tiling的计算，比如获取硬件平台的核数等信息。PlatformAscendC类提供获取这些平台信息的功能。 |
| [PlatformAscendCManager](platform_info/PlatformAscendCManager.md) | 基于Kernel Launch算子工程，通过基础调用（Kernel Launch）方式调用算子的场景下，可能需要获取硬件平台相关信息，比如获取硬件平台的核数。PlatformAscendCManager类提供获取平台信息的功能。 |

**表3**  Tiling调测API列表

| 接口名 | 功能描述 |
| --- | --- |
| [OpTilingRegistry](Tiling_tuning/OpTilingRegistry/OpTilingRegistry.md) | OpTilingRegistry类属于context_ascendc命名空间，主要用于加载Tiling实现的动态库，并获取算子的Tiling函数指针以进行调试和验证。 |
| [ContextBuilder](Tiling_tuning/ContextBuilder/ContextBuilder.md) | ContextBuilder类提供一系列的API接口，支持手动构造类来验证Tiling函数以及KernelContext类用于TilingParse函数的验证。 |

**表4**  Tiling模板编程API列表

| 接口名 | 功能描述 |
| --- | --- |
| [模板参数定义](Tiling_template_programming/template_params_definition.md) | 通过该类接口进行模板参数ASCENDC_TPL_ARGS_DECL和模板参数组合ASCENDC_TPL_ARGS_SEL（即可使用的模板）的定义。 |
| [GET_TPL_TILING_KEY](Tiling_template_programming/GET_TPL_TILING_KEY.md) | Tiling模板编程时，开发者通过调用此接口自动生成TilingKey。该接口将传入的模板参数通过定义的位宽，转成二进制，按照顺序组合后转成uint64数值，即TilingKey。 |
| [ASCENDC_TPL_SEL_PARAM](Tiling_template_programming/ASCENDC_TPL_SEL_PARAM.md) | Tiling模板编程时，开发者通过调用此接口自动生成并配置TilingKey。 |

**表5**  RTC API列表

| 接口名 | 功能描述 |
| --- | --- |
| [aclrtcCompileProg](RTC/aclrtcCompileProg.md) | 编译接口，编译指定的程序。 |
| [aclrtcCreateProg](RTC/aclrtcCreateProg.md) | 通过给定的参数，创建编译程序的实例。 |
| [aclrtcDestroyProg](RTC/aclrtcDestroyProg.md) | 销毁编译程序的实例。 |
| [aclrtcGetBinData](RTC/aclrtcGetBinData.md) | 获取编译后的二进制数据。 |
| [aclrtcGetBinDataSize](RTC/aclrtcGetBinDataSize.md) | 获取编译的二进制数据大小。用于在[aclrtcGetBinData](RTC/aclrtcGetBinData.md)获取二进制数据时分配对应大小的内存空间。 |
| [aclrtcGetCompileLogSize](RTC/aclrtcGetCompileLogSize.md) | 获取编译日志的大小。用于在[aclrtcGetCompileLog](RTC/aclrtcGetCompileLog.md)获取日志内容时分配对应大小的内存空间。 |
| [aclrtcGetCompileLog](RTC/aclrtcGetCompileLog.md) | 获取编译日志的内容，以字符串形式保存。 |

**表6**  log API列表

| 接口名 | 功能描述 |
| --- | --- |
| [ASC_CPU_LOG](log/ASC_CPU_LOG.md) | 提供Host侧打印Log的功能。开发者可以在算子的TilingFunc代码中使用ASC_CPU_LOG_XXX接口来输出相关内容。 |

**表7**  调测接口列表

| 接口名 | 功能描述 |
| --- | --- |
| [printf](tuning_interface/printf.md) | 在算子核函数（Kernel）侧的实现代码中，需要输出日志信息时，调用printf接口打印相关内容。 |
| [assert/ascendc_assert](tuning_interface/assert.md) | 用于在算子核函数（Kernel）中检查指定条件。条件不成立时，接口打印断言失败信息并触发异常。 |
| [__trap](tuning_interface/__trap.md) | 在SIMT VF实现代码中调用此接口会中断算子的运行。 |
| [clock](tuning_interface/clock.md) | 本接口在SIMT VF调试场景中提供Clock时间戳功能，用于记录从程序启动到接口调用时刻所经历的时钟周期数（Cycle Count），便于精确分析执行延迟和性能瓶颈。 |
| [asc\_dump](tuning_interface/asc_dump.md) | 将对应内存上的数据打印出来，同时支持打印自定义的附加信息（仅支持uint32\_t类型的信息），比如打印当前行号等。 |
| [asc\_time\_stamp](tuning_interface/asc_time_stamp.md) | 提供时间戳打点功能，用于在算子核函数（Kernel）代码中标记关键执行点。 |
| [asc\_prof\_start](tuning_interface/asc_prof_start.md) | 设置性能数据采集信号启动，和asc\_prof\_stop配合使用，用于指定需要调优的代码段范围。 |
| [asc\_prof\_stop](tuning_interface/asc_prof_stop.md) | 设置性能数据采集信号停止，和asc\_prof\_start配合使用，用于指定需要调优的代码段范围。 |
| [asc\_mark\_stamp](tuning_interface/asc_mark_stamp.md) | 在算子执行过程中标记特定位置，便于后期通过流水图分析代码执行路径与性能热点。 |
| [TRACE\_START](tuning_interface/TRACE_START.md) | 通过仿真器进行算子性能仿真时，可对算子任意运行阶段打点，从而分析不同指令的流水图，以便进一步性能调优。 |
| [TRACE\_STOP](tuning_interface/TRACE_STOP.md) | 通过仿真器进行算子性能仿真时，可对算子任意运行阶段打点，从而分析不同指令的流水图，以便进一步性能调优。 |
