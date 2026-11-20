# Utils API List

<!-- md-trans-meta sourceCommit=08a15e3295a53ae78c5e373a7b6b6c8beb2aa1eb translatedAt=2026-11-13T09:33:24.198Z pushedAt=2026-11-16T07:07:41.620Z nl=2 -->

**Table 1** C++ Standard Library API List

| Name | Description |
| --- | --- |
| [max](cpp_stdlib/algorithms/max.md) | Compares two numbers of the same data type and returns the maximum value. |
| [min](cpp_stdlib/algorithms/min.md) | Compares two numbers of the same data type and returns the minimum value. |
| [abs](cpp_stdlib/math_functions/abs.md) | Obtains the absolute value of the input. |
| [ceil\_div](cpp_stdlib/math_functions/ceil_div.md) | Computes the result of rounding up the division of two integers, a and b. |
| [CeilDivision](cpp_stdlib/math_functions/CeilDivision.md) | Computes the result of rounding up the division of two integers, num1 and num2. |
| [sqrt](cpp_stdlib/math_functions/sqrt.md) | Returns the square root of the input data. |
| [integer_sequence](cpp_stdlib/general_utils/integer_sequence.md) | Used to generate an integer sequence. |
| [tuple](cpp_stdlib/container_functions/tuple.md) | A container that stores multiple elements of different types. |
| [get](cpp_stdlib/container_functions/get.md) | Extracts the element at a specified position from a tuple container. |
| [make_tuple](cpp_stdlib/container_functions/make_tuple.md) | Used to conveniently create a tuple object. |
| [is_convertible](cpp_stdlib/type_traits/is_convertible.md) | Determines at compile time whether an implicit conversion is possible between two types. |
| [is_base_of](cpp_stdlib/type_traits/is_base_of.md) | Determines at compile time whether one type is a base class of another type. |
| [is_same](cpp_stdlib/type_traits/is_same.md) | Determines at compile time whether two types are exactly the same. |
| [is_void](cpp_stdlib/type_traits/is_void.md) | Detects at compile time whether a type is void. |
| [is_integral](cpp_stdlib/type_traits/is_integral.md) | Detects at compile time whether a type is an integer type. |
| [is_floating_point](cpp_stdlib/type_traits/is_floating_point.md) | Detects at compile time whether a type is a floating-point type. |
| [is_array](cpp_stdlib/type_traits/is_array.md) | Detects at compile time whether a type is an array type. |
| [is_pointer](cpp_stdlib/type_traits/is_pointer.md) | Determines at compile time whether a type is a pointer type. |
| [is_reference](cpp_stdlib/type_traits/is_reference.md) | Detects at compile time whether a type is a reference type. |
| [is_const](cpp_stdlib/type_traits/is_const.md) | Detects at compile time whether a type is const-qualified. |
| [remove_const](cpp_stdlib/type_traits/remove_const.md) | Removes the const qualifier from the passed template parameter type at compile time. |
| [remove_volatile](cpp_stdlib/type_traits/remove_volatile.md) | Removes the volatile qualifier from the passed template parameter type at compile time. |
| [remove_cv](cpp_stdlib/type_traits/remove_cv.md) | Removes the const qualifier, the volatile qualifier, or both from the passed template parameter type at compile time. |
| [remove_reference](cpp_stdlib/type_traits/remove_reference.md) | Removes the reference qualifier from a given type at compile time. |
| [remove_pointer](cpp_stdlib/type_traits/remove_pointer.md) | Removes the pointer qualifier from a given type at compile time. |
| [add_const](cpp_stdlib/type_traits/add_const.md) | Adds the const qualifier to a specified type at compile time. |
| [add_volatile](cpp_stdlib/type_traits/add_volatile.md) | Adds the volatile qualifier to a specified type at compile time. |
| [add_cv](cpp_stdlib/type_traits/add_cv.md) | Adds the const and volatile qualifiers to a specified type at compile time. |
| [add_pointer](cpp_stdlib/type_traits/add_pointer.md) | Adds the pointer qualifier to a specified type at compile time. |
| [add_lvalue_reference](cpp_stdlib/type_traits/add_lvalue_reference.md) | Adds the lvalue reference qualifier to a specified type at compile time. |
| [add_rvalue_reference](cpp_stdlib/type_traits/add_rvalue_reference.md) | Adds the rvalue reference qualifier to a specified type at compile time. |
| [enable_if](cpp_stdlib/type_traits/enable_if.md) | Enables or disables a specific function template, class template, or template specialization at compile time based on a condition. |
| [conditional](cpp_stdlib/type_traits/conditional.md) | Selects one of two types at compile time based on a Boolean condition. |
| [integral_constant](cpp_stdlib/type_traits/integral_constant.md) | Encapsulates a compile-time constant integer value and serves as the foundation for many type traits and compile-time computations in the standard library. |

**Table 2** Platform Information Retrieval API List

| Name | Description |
| --- | --- |
| [PlatformAscendC](platform_info/PlatformAscendC/PlatformAscendC.md) | When implementing the Tiling function on the host side, you may need to obtain some hardware platform information, such as the number of cores, to support Tiling computation. The PlatformAscendC class provides the functionality to obtain such platform information. |
| [PlatformAscendCManager](platform_info/PlatformAscendCManager.md) | In the scenario of invoking operators through the basic call (Kernel Launch) method based on a Kernel Launch operator project, you may need to obtain hardware platform information, such as the number of cores. The PlatformAscendCManager class provides the functionality to obtain platform information. |

**Table 3** Tiling Debugging API List

| Name | Description |
| --- | --- |
| [OpTilingRegistry](Tiling_tuning/OpTilingRegistry/OpTilingRegistry.md) | The OpTilingRegistry class belongs to the context_ascendc namespace and is mainly used to load the dynamic library of the Tiling implementation and obtain the operator's Tiling function pointer for debugging and verification. |
| [ContextBuilder](Tiling_tuning/ContextBuilder/ContextBuilder.md) | The ContextBuilder class provides a series of APIs to support manually constructing classes for verifying the Tiling function, and the KernelContext class is used for verifying the TilingParse function. |

**Table 4** Tiling Template Programming API List

| Name | Description |
| --- | --- |
| [Template Parameter Definition](Tiling_template_programming/template_params_definition.md) | Defines the template parameter ASCENDC_TPL_ARGS_DECL and the template parameter combination ASCENDC_TPL_ARGS_SEL (that is, the usable template) through this type of API. |
| [GET_TPL_TILING_KEY](Tiling_template_programming/GET_TPL_TILING_KEY.md) | In Tiling Template Programming, you can call this API to automatically generate a TilingKey. The API converts the passed template parameters into binary form according to the defined bit widths, combines them in order, and then converts the result into a uint64 value, which is the TilingKey. |
| [ASCENDC_TPL_SEL_PARAM](Tiling_template_programming/ASCENDC_TPL_SEL_PARAM.md) | During Tiling template programming, you can call this API to automatically generate and configure the TilingKey. |

**Table 5** RTC API List

| Name | Description |
| --- | --- |
| [aclrtcCompileProg](RTC/aclrtcCompileProg.md) | Compilation API that compiles the specified program. |
| [aclrtcCreateProg](RTC/aclrtcCreateProg.md) | Creates an instance of the compilation program with the given parameters. |
| [aclrtcDestroyProg](RTC/aclrtcDestroyProg.md) | Destroys the compiled program instance. |
| [aclrtcGetBinData](RTC/aclrtcGetBinData.md) | Obtains the compiled binary data. |
| [aclrtcGetBinDataSize](RTC/aclrtcGetBinDataSize.md) | Obtains the size of the compiled binary data. It is used to allocate memory of the corresponding size when obtaining binary data through [aclrtcGetBinData](RTC/aclrtcGetBinData.md). |
| [aclrtcGetCompileLogSize](RTC/aclrtcGetCompileLogSize.md) | Obtains the size of the compilation log. It is used to allocate memory of the corresponding size when obtaining the log content through [aclrtcGetCompileLog](RTC/aclrtcGetCompileLog.md). |
| [aclrtcGetCompileLog](RTC/aclrtcGetCompileLog.md) | Obtains the content of the compilation log and saves it as a string. |

**Table 6**  log API list

| Name | Description |
| --- | --- |
| [ASC_CPU_LOG](log/ASC_CPU_LOG.md) | Provides the log printing function on the Host side. You can use the ASC_CPU_LOG_XXX API in the TilingFunc code of the operator to output the relevant content. |

**Table 7**  Debugging API list

| Name | Description |
| --- | --- |
| [ASC\_HOST\_CHECK\_CONDITION](tuning_interface/ASC_HOST_CHECK_CONDITION.md) | Used to check whether the condition on the Host side is satisfied. When the condition is not satisfied, the API outputs diagnostic information and calls the `assert` macro in the C++ standard library `<cassert>`. |
| [printf](tuning_interface/printf.md) | In the implementation code on the operator kernel function (Kernel) side, call the printf API to print the relevant content when log information needs to be output. |
| [assert/ascendc_assert](tuning_interface/assert.md) | Used to check the specified condition in the operator kernel function (Kernel). When the condition is not met, the API prints the assertion failure information and triggers an exception. |
| [__trap](tuning_interface/__trap.md) | Calling this API in the SIMT VF implementation code interrupts the execution of the operator. |
| [clock](tuning_interface/clock.md) | This API provides the Clock timestamp function in SIMD and SIMT debugging scenarios. It records the number of clock cycles (Cycle Count) elapsed from program startup to the moment the API is called, enabling precise analysis of execution latency and performance bottlenecks. |
| [asc\_dump](tuning_interface/asc_dump.md) | Prints the data in the corresponding memory, and also supports printing custom additional information (only information of the uint32\_t type is supported), such as the current line number. |
| [asc\_time\_stamp](tuning_interface/asc_time_stamp.md) | Provides the timestamp marking function, which is used to mark key execution points in the operator kernel function (Kernel) code. |
| [asc\_prof\_start](tuning_interface/asc_prof_start.md) | Starts the performance data collection signal, and is used together with asc\_prof\_stop to specify the code segment range to be tuned. |
| [asc\_prof\_stop](tuning_interface/asc_prof_stop.md) | Stops the performance data collection signal, and is used together with asc\_prof\_start to specify the code segment range to be tuned. |
| [asc\_mark\_stamp](tuning_interface/asc_mark_stamp.md) | Marks a specific position during operator execution, facilitating later analysis of the code execution path and performance hotspots through the pipeline graph. |
| [TRACE\_START](tuning_interface/TRACE_START.md) | When performing operator performance simulation through the simulator, you can mark any execution stage of the operator to analyze the pipeline graphs of different instructions for further performance tuning. |
| [TRACE\_STOP](tuning_interface/TRACE_STOP.md) | When performing operator performance simulation through the simulator, you can mark any execution stage of the operator to analyze the pipeline graphs of different instructions for further performance tuning. |
