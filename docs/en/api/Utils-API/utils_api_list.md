# Utils API List

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T09:49:46.093Z pushedAt=2026-09-10T06:40:33.523Z -->

**Table 1** C++ standard library API list

| API Name | Function Description |
| --- | --- |
| [max](cpp_stdlib/algorithms/max-267.md) | Compares two numbers of the same data type and returns the maximum value. |
| [min](cpp_stdlib/algorithms/min-268.md) | Compares two numbers of the same data type and returns the minimum value. |
| [abs](cpp_stdlib/math_functions/abs1.md) | Obtains the absolute value of the input data. |
| [sqrt](cpp_stdlib/math_functions/sqrt1.md) | Calculates the square root of the input data. |
| [integer_sequence](cpp_stdlib/general_utils/integer_sequence.md) | Used to generate an integer sequence. |
| [tuple](cpp_stdlib/container_functions/tuple.md) | A container that can store multiple elements of different types. |
| [get](cpp_stdlib/container_functions/get1.md) | Extracts the element at a specified position from a tuple container. |
| [make_tuple](cpp_stdlib/container_functions/make_tuple.md) | Used to conveniently create tuple objects. |
| [is_convertible](cpp_stdlib/type_traits/is_convertible.md) | Determines at compile time whether an implicit conversion is possible between two types. |
| [is_base_of](cpp_stdlib/type_traits/is_base_of.md) | Determines at compile time whether one type is a base class of another type. |
| [is_same](cpp_stdlib/type_traits/is_same.md) | Determines at compile time whether two types are exactly the same. |
| [is_void](cpp_stdlib/type_traits/is_void.md) | Detects at compile time whether a type is the void type. |
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
| [conditional](cpp_stdlib/type_traits/conditional.md) | Selects one type from two types at compile time based on a Boolean condition. |
| [integral_constant](cpp_stdlib/type_traits/integral_constant.md) | Used to encapsulate a compile-time constant integer value. It is a fundamental component of many type_traits and compile-time computations in the standard library. |

**Table 2** Platform information acquisition API list

| API Name | Function Description |
| --- | --- |
| [PlatformAscendC](platform_info/PlatformAscendC/PlatformAscendC.md) | When implementing the Tiling function on the Host side, you may need to obtain some hardware platform information to support Tiling computation, such as the number of cores of the hardware platform. The PlatformAscendC class provides the capability to obtain such platform information. |
| [PlatformAscendCManager](platform_info/PlatformAscendCManager.md) | In the scenario where an operator is invoked through the basic call (Kernel Launch) method based on a Kernel Launch operator project, you may need to obtain hardware platform information, such as the number of cores of the hardware platform. The PlatformAscendCManager class provides the capability to obtain platform information. |

**Table 3** Tiling debugging API list

| API Name | Function Description |
| --- | --- |
| [OpTilingRegistry](Tiling_tuning/OpTilingRegistry/OpTilingRegistry.md) | The OpTilingRegistry class belongs to the context_ascendc namespace. It is mainly used to load the dynamic library that implements Tiling and obtain the Tiling function pointer of the operator for debugging and verification. |
| [ContextBuilder](Tiling_tuning/ContextBuilder/ContextBuilder.md) | The ContextBuilder class provides a series of APIs to support manually constructing classes for verifying the Tiling function and the KernelContext class used for TilingParse function verification. |

**Table 4** Tiling template programming API list

| API Name | Function Description |
| --- | --- |
| [Template Parameter Definition](Tiling_template_programming/template_parameter_definition.md) | Defines the template parameter ASCENDC_TPL_ARGS_DECL and the template parameter combination ASCENDC_TPL_ARGS_SEL (that is, the usable template) through this class of APIs. |
| [GET_TPL_TILING_KEY](Tiling_template_programming/GET_TPL_TILING_KEY.md) | During Tiling template programming, developers call this API to automatically generate the TilingKey. This API converts the passed template parameters into binary according to the defined bit width, combines them in order, and then converts them into a uint64 value, which is the TilingKey. |
| [ASCENDC_TPL_SEL_PARAM](Tiling_template_programming/ASCENDC_TPL_SEL_PARAM.md) | During Tiling template programming, developers call this API to automatically generate and configure the TilingKey. |

**Table 5** RTC API list

| API Name | Function Description |
| --- | --- |
| [aclrtcCompileProg](RTC/aclrtcCompileProg.md) | Compilation API that compiles the specified program. |
| [aclrtcCreateProg](RTC/aclrtcCreateProg.md) | Creates an instance of the compilation program with the given parameters. |
| [aclrtcDestroyProg](RTC/aclrtcDestroyProg.md) | Destroys the instance of the compilation program. |
| [aclrtcGetBinData](RTC/aclrtcGetBinData.md) | Obtains the compiled binary data. |
| [aclrtcGetBinDataSize](RTC/aclrtcGetBinDataSize.md) | Obtains the size of the compiled binary data. Used to allocate memory of the corresponding size when obtaining binary data through [aclrtcGetBinData](RTC/aclrtcGetBinData.md). |
| [aclrtcGetCompileLogSize](RTC/aclrtcGetCompileLogSize.md) | Obtains the size of the compilation log. Used to allocate memory of the corresponding size when obtaining log content through [aclrtcGetCompileLog](RTC/aclrtcGetCompileLog.md). |
| [aclrtcGetCompileLog](RTC/aclrtcGetCompileLog.md) | Obtains the content of the compilation log and saves it as a string. |

**Table 6**  Log API list

| API Name | Function Description |
| --- | --- |
| [ASC_CPU_LOG](log/ASC_CPU_LOG.md) | Provides the function of printing logs on the Host side. Developers can use the ASC_CPU_LOG_XXX API in the TilingFunc code of an operator to output related content. |

**Table 7**  Debugging API list

| API Name | Function Description |
| --- | --- |
| [printf](tuning_interface/printf-290.md) | In the implementation code on the Kernel side of an operator, call the printf API to print related content when log information needs to be output. |
| [assert](tuning_interface/assert-291.md) | This API provides the assert function in SIMT VF debugging scenarios. In the SIMT VF implementation code on the Kernel side of an operator, if the internal condition of assert is not true, the assert condition is output and the input information is formatted and printed on the screen. |
| [__trap](tuning_interface/__trap.md) | Calling this API in the SIMT VF implementation code interrupts the execution of the operator. |
| [clock](tuning_interface/clock.md) | This API provides the Clock timestamp function in SIMT VF debugging scenarios. It records the number of clock cycles (Cycle Count) elapsed from program startup to the moment the API is called, facilitating precise analysis of execution latency and performance bottlenecks. |
