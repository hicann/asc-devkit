# RTC<a name="ZH-CN_TOPIC_0000002462620725"></a>

<!-- md-trans-meta sourceCommit=f83e857605e3844db8b9de0442007cc115278a47 translatedAt=2026-10-23T15:06:59.938Z pushedAt=2026-11-03T07:05:13.459Z nl=2 -->

RTC is the Ascend C runtime compilation library. Through the [aclrtc](../../../../api/Utils-API/RTC/RTC.md) APIs, it dynamically compiles intermediate code into target machine code at program runtime, improving program execution performance.

The runtime compilation library provides the following core APIs:
-   aclrtcCreateProg: Creates an aclrtcProg program instance based on input parameters (such as Ascend C source code expressed as a string).
-   aclrtcAddNameExpr (optional): registers the name expression of the kernel function to be exported, supporting template parameters (such as "kernel function ::add_custom\<float\>""). This step can be skipped for non-template kernel functions.
-   aclrtcCompileProg: compiles the given program and supports user-defined compilation options, such as specifying the [NPU architecture version](../../language_extension/simd_builtin_keywords.md#npu-arch): --npu-arch=dav-2201. For supported compilation options, see [*Bisheng Compiler*](https://www.hiascend.com/document/detail/en/CANNCommunityEdition/920/compiler/BishengCompiler/atlas_bisheng_10_0001.html).
-   aclrtcGetBinDataSize: Obtains the size of the compiled device-side binary data.
-   aclrtcGetBinData: Obtains the compiled device-side binary data.
-   aclrtcGetLoweredName (optional): obtains the mangled name of the compiled kernel function, which is used to find the kernel function handle through aclrtBinaryGetFunction. This step can be skipped for non-template kernel functions.
-   aclrtcDestroyProg: Destroys the given program after the compilation and execution process ends.

After compilation, call the following APIs (only the core APIs are listed) to load and execute the kernel function. For the complete process and detailed API descriptions, see the "Kernel Function Loading and Execution" section in [Runtime API](https://gitcode.com/cann/runtime/blob/9.2.0/docs/en/api_ref/README.md).
1.  Call aclInit, aclrtSetDevice, and other APIs to initialize the runtime environment and specify the device.
2.  Parse the operator binary data obtained by aclrtcGetBinData through aclrtBinaryLoadFromData. When loading, you can specify the binary type through ACL_RT_BINARY_LOAD_OPT_MAGIC, such as ACL_RT_BINARY_MAGIC_ELF_AICORE.
3.  Call aclrtBinaryGetFunction to obtain the kernel function handle.
4.  Call aclrtLaunchKernelWithArgsArray to start the computation task of the corresponding operator on the created stream in the argument array mode. For a kernel function without parameters, you can pass an empty argument array. For a kernel function with parameters, ensure that the elements of the argument array are arranged in the order of the kernel function input parameters, and each element points to the parameter value on the host side.
5.  Call aclrtSynchronizeStream, aclrtBinaryUnLoad, aclrtDestroyStream, aclrtResetDevice, aclFinalize, and other APIs to complete synchronization and resource release.

The following sample demonstrates how to use the aclrtc APIs to compile and run a Kernel, which calls printf for printing. For the complete sample, see [LINK](../../../../../../examples/01_simd_cpp_api/02_features/05_aclrtc/rtc_hello_world/README_en.md).


```c++
#include <cstdio>
#include <vector>
#include "acl/acl.h"
#include "acl/acl_rt_compile.h" // Header file required for using the aclrtc APIs.

#ifndef ACL_RTC_NPU_ARCH
#define ACL_RTC_NPU_ARCH "dav-2201"
#endif

#define ASCENDC_CHECK(expr) do {               \
    aclError ret = (expr);                     \
    if (ret != ACL_SUCCESS) {                  \
        fprintf(stderr,                        \
            "Ascend Error: %s:%d code=%d %s\n",\
            __FILE__, __LINE__,                \
            ret, aclGetRecentErrMsg());        \
        return ret;                            \
    }                                          \
} while(0)

const char *src = R""""(
#include "utils/debug/asc_printf.h"
extern "C" __global__ __vector__ void hello_world()
{
    printf("Hello World!!!\n");
}
)"""";

int main(int argc, char *argv[])
{
    aclrtcProg prog;
    ASCENDC_CHECK(aclrtcCreateProg(&prog, src, "hello_world.asc", 0, nullptr, nullptr));

    // aclrtc process: pass the compilation options of the BiSheng compiler and call aclrtcCompileProg for compilation.
    const char *options[] = {
        "--npu-arch=" ACL_RTC_NPU_ARCH,
    };
    int numOptions = sizeof(options) / sizeof(options[0]);
    aclError ret = aclrtcCompileProg(prog, 1, options);
    if (ret != ACL_SUCCESS) {
        size_t size = 0;
        (void)aclrtcGetCompileLogSize(prog, &size);
        char log[size] = {0};
        (void)aclrtcGetCompileLog(prog, log);
        printf("Compile Error Log : %s", log);
    }
    // aclrtc process: obtain the binary content and size on the Device side.
    size_t binDataSizeRet;
    ASCENDC_CHECK(aclrtcGetBinDataSize(prog, &binDataSizeRet));
    std::vector<char> deviceELF(binDataSizeRet);
    ASCENDC_CHECK(aclrtcGetBinData(prog, deviceELF.data()));

    ASCENDC_CHECK(aclInit(nullptr));
    ASCENDC_CHECK(aclrtSetDevice(0));
    aclrtStream stream = nullptr;
    ASCENDC_CHECK(aclrtCreateStream(&stream));

    aclrtBinHandle binHandle = nullptr;
    aclrtBinaryLoadOptions loadOption;
    loadOption.numOpt = 1;
    aclrtBinaryLoadOption option;
    option.type = ACL_RT_BINARY_LOAD_OPT_MAGIC;
    option.value.magic = ACL_RT_BINARY_MAGIC_ELF_AICORE;
    loadOption.options = &option;
    ASCENDC_CHECK(aclrtBinaryLoadFromData(deviceELF.data(), binDataSizeRet, &loadOption, &binHandle));

    aclrtFuncHandle funcHandle = nullptr;
    const char *funcName = "hello_world";
    ASCENDC_CHECK(aclrtBinaryGetFunction(binHandle, funcName, &funcHandle));

    // Execute the Kernel.
    uint32_t numBlocks = 8;
    void *kernelArgs[] = {};
    ASCENDC_CHECK(aclrtLaunchKernelWithArgsArray(funcHandle, numBlocks, stream, nullptr, kernelArgs));
    ASCENDC_CHECK(aclrtSynchronizeStream(stream));

    ASCENDC_CHECK(aclrtBinaryUnLoad(binHandle));
    ASCENDC_CHECK(aclrtDestroyStream(stream));
    ASCENDC_CHECK(aclrtResetDevice(0));
    ASCENDC_CHECK(aclFinalize());
    // Compilation and running are complete. Destroy the program.
    ASCENDC_CHECK(aclrtcDestroyProg(&prog));
    return 0;
}
```

The compilation command is as follows. During compilation, set -I\$\{ASCEND_HOME_PATH\}/include to locate the aclrtc-related header files, and set -L\$\{ASCEND_HOME_PATH\}/lib64 to link the acl_rtc dynamic library.

```
g++ rtc_hello_world.cpp -I${ASCEND_HOME_PATH}/include -L${ASCEND_HOME_PATH}/lib64 -lascendcl -lacl_rtc -o main
```

For a non-template Kernel (such as `hello_world`), the compiler can automatically export the symbol, and no additional operation is required.<br>
When the Kernel is a template function, the compiler cannot automatically determine the specialization instances to be exported. You need to use `aclrtcAddNameExpr` to manually register the Kernel name (including template parameters) to be exported. After compilation, use `aclrtcGetLoweredName` to obtain the mangled name, which is used by `aclrtBinaryGetFunction` to find the handle.

```c++
// Register the Kernel name (including template parameters) to be exported.
const char* kernelNameExpr = "Kernel::add_custom<float>";
aclrtcAddNameExpr(prog, kernelNameExpr);
// ... Compilation process aclrtcCompileProg ...
// Obtain the compiled mangled name.
const char* manglingName = "";
aclrtcGetLoweredName(prog, kernelNameExpr, &manglingName);
```

For the complete sample, see [rtc_template_add](../../../../../../examples/01_simd_cpp_api/02_features/05_aclrtc/rtc_template_add/README_en.md).
