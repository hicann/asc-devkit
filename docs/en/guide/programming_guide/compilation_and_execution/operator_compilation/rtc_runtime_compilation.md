# RTC<a name="ZH-CN_TOPIC_0000002462620725"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T10:53:55.641Z pushedAt=2026-08-31T03:56:19.739Z -->

RTC is the Ascend C runtime compilation library. Through the [aclrtc](../../../../api/Utils-API/RTC/RTC.md) APIs, it dynamically compiles intermediate code into target machine code at program runtime, improving program execution performance.

The runtime compilation library provides the following core APIs:
-   aclrtcCreateProg: Creates an aclrtcProg program instance based on input parameters (such as Ascend C source code expressed as a string).
-   aclrtcAddNameExpr (optional): Registers the name expression of the kernel function to be exported. It supports template parameters (such as "Kernel::add_custom\<float\>"). This step can be skipped for non-template kernel functions.
-   aclrtcCompileProg: Compiles the given program and supports user-defined compilation options, such as specifying the [NPU architecture version number](../../language_extension/SIMD-BuiltIn_keyword.md#table65291052154114): --npu-arch=dav-2201. For supported compilation options, see [*BiSheng Compiler User Guide*](https://www.hiascend.com/document/redirect/CannCommunityBiSheng).
-   aclrtcGetBinDataSize: Obtains the size of the compiled device-side binary data.
-   aclrtcGetBinData: Obtains the compiled device-side binary data.
-   aclrtcGetLoweredName (optional): Obtains the mangled name of the compiled kernel function, which is used to find the kernel function handle through aclrtBinaryGetFunction. This step can be skipped for non-template kernel functions.
-   aclrtcDestroyProg: Destroys the given program after the compilation and execution process ends.

After compilation, call the following APIs (only the core APIs are listed) to complete kernel loading and execution. For the complete process and detailed API descriptions, see the "Kernel Loading and Execution" section in [*Runtime API*](https://hiascend.com/document/redirect/CannCommunityRuntimeApi).
1.  Call `aclrtBinaryLoadFromData` to parse the operator binary data obtained by `aclrtcGetBinData`.
2.  Obtain the kernel function handle and operate its parameter list based on the handle. The related APIs include `aclrtBinaryGetFunction` (obtain the kernel function handle), `aclrtKernelArgsInit` (initialize the parameter list), and `aclrtKernelArgsAppend` (append and copy the parameter values set by the user, such as xDevice, yDevice, and zDevice).
3.  Call `aclrtLaunchKernelWithConfig` to start the computation task of the corresponding operator.

The following sample demonstrates how to use the aclrtc APIs to compile and run a kernel function that calls `printf` for printing. For the complete sample, click [here](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/02_features/05_aclrtc/rtc_hello_world/README.md).


```c++
#include <cstdio>
#include <vector>
#include "acl/acl.h"
#include "acl/acl_rt_compile.h" // Header file required for using the aclrtc APIs

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
    #include "debug/asc_printf.h"
    extern "C" __global__ __vector__ void hello_world()
    {
        printf("Hello World!!!\n");
    }
)"""";

int main(int argc, char *argv[])
{
    aclrtcProg prog;
    ASCENDC_CHECK(aclrtcCreateProg(&prog, src, "hello_world.asc", 0, nullptr, nullptr));

    // In the aclrtc process, pass the compilation options of the BiSheng compiler and call aclrtcCompileProg for compilation
    const char *options[] = {
        "--npu-arch=" ACL_RTC_NPU_ARCH,
    };
    int numOptions = sizeof(options) / sizeof(options[0]);
    aclError ret = aclrtcCompileProg(prog, 1, options);
    if (ret != ACL_SUCCESS) { // Print the error information when a compilation error occurs
        size_t size = 0;
        (void)aclrtcGetCompileLogSize(prog, &size);
        char log[size] = {0};
        (void)aclrtcGetCompileLog(prog, log);
        printf("Compile Error Log : %s", log);
    }
    // In the aclrtc process, obtain the binary content and size on the device side.
    size_t binDataSizeRet;
    ASCENDC_CHECK(aclrtcGetBinDataSize(prog, &binDataSizeRet));
    std::vector<char> deviceELF(binDataSizeRet);
    ASCENDC_CHECK(aclrtcGetBinData(prog, deviceELF.data()));

    // ----------------------------------------------- aclrt part ------------------------------------------------
    aclrtBinHandle binHandle = nullptr;
    aclrtBinaryLoadOptions loadOption;
    loadOption.numOpt = 1;
    aclrtBinaryLoadOption option;
    option.type = ACL_RT_BINARY_LOAD_OPT_LAZY_MAGIC;
    option.value.magic = ACL_RT_BINARY_MAGIC_ELF_AICORE;
    loadOption.options = &option;
    ASCENDC_CHECK(aclrtSetDevice(0));
    ASCENDC_CHECK(aclrtBinaryLoadFromData(deviceELF.data(), binDataSizeRet, &loadOption, &binHandle));


    aclrtFuncHandle funcHandle = nullptr;
    const char *funcName = "hello_world";
    ASCENDC_CHECK(aclrtBinaryGetFunction(binHandle, funcName, &funcHandle));

    aclrtArgsHandle argsHandle = nullptr;
    ASCENDC_CHECK(aclrtKernelArgsInit(funcHandle, &argsHandle));
    ASCENDC_CHECK(aclrtKernelArgsFinalize(argsHandle));
    // Execute the kernel function.
    uint32_t numBlocks = 8;
    ASCENDC_CHECK(aclrtLaunchKernelWithConfig(funcHandle, numBlocks, nullptr, nullptr, argsHandle, nullptr));
    ASCENDC_CHECK(aclrtSynchronizeDevice());
    ASCENDC_CHECK(aclrtBinaryUnLoad(binHandle));
    ASCENDC_CHECK(aclrtResetDevice(0));

    // Compilation and execution are complete. Destroy the program.
    ASCENDC_CHECK(aclrtcDestroyProg(&prog));
    return 0;
}
```

The compilation command is as follows. During compilation, set **-I\$\{ASCEND_HOME_PATH\}/include** to locate the aclrtc-related header files, and set **-L\$\{ASCEND_HOME_PATH\}/lib64** to link the alc_rtc dynamic library.

```
g++ rtc_hello_world.cpp -I${ASCEND_HOME_PATH}/include -L${ASCEND_HOME_PATH}/lib64 -lascendcl -lacl_rtc -o main
```

For a non-template kernel function (such as `hello_world`), the compiler can automatically export the symbol, and no additional operation is required.  
When the kernel function is a template function, the compiler cannot automatically determine the specialization instances to be exported. You need to use `aclrtcAddNameExpr` to manually register the kernel function name (including template parameters) to be exported. After compilation, use `aclrtcGetLoweredName` to obtain the mangled name, which is used by `aclrtBinaryGetFunction` to find the handle.

```c++
// Register the kernel function name to be exported (including template parameters).
const char* kernelNameExpr = "Kernel::add_custom<float>";
aclrtcAddNameExpr(prog, kernelNameExpr);
// ... Compilation process aclrtcCompileProg ...
// Obtain the mangled name after compilation.
const char* manglingName = "";
aclrtcGetLoweredName(prog, kernelNameExpr, &manglingName);
```

For the complete sample, see [rtc_template_add](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/02_features/05_aclrtc/rtc_template_add/README.md).
