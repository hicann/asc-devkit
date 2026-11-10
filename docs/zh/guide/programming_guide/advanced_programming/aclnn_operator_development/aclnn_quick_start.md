# Aclnn算子工程化开发快速入门

本文是基础内容，通过一个完整的AddCustom算子示例，引导您逐步创建工程目录、编写算子实现和CMakeLists.txt，再完成编译部署与单算子API调用验证。

<img src="../../../figures/end_to_end_operator_development_process.png" alt="" width="600px" height="225px">

## 前置环境准备

### 编译工具链

在开始之前，请确保系统中已安装以下工具：

| 工具 | 版本建议 | 说明 |
|------|-------------|------|
| CMake | ≥ 3.19 | 构建系统生成器。 |
| GCC / G++ | ≥ 7.5 | C++编译器。 |
| Python | 与所用CANN版本的环境要求一致 | CANN代码生成及算子编译依赖。 |
| Make | — | 构建执行工具。 |

可通过以下命令检查工具版本：

```bash
cmake --version
g++ --version
python3 --version
make --version
```

### CANN环境变量

编译和运行均依赖CANN提供的环境变量，在开始任何步骤前，先加载环境变量：

```bash
source /usr/local/Ascend/cann/set_env.sh
```

> 每次打开新终端时都需要重新执行该命令。后续步骤中不再重复提示。

## 快速入门流程

### 步骤概览

| 步骤 | 内容 |
|------|------|
| 步骤1 | 算子分析 |
| 步骤2 | 创建工程目录 |
| 步骤3 | 编写共享Tiling数据结构 |
| 步骤4 | 编写核函数（Kernel）侧代码 |
| 步骤5 | 编写Host侧原型、推导函数和Tiling实现 |
| 步骤6 | 编写三个CMakeLists.txt |
| 步骤7 | 编译与部署 |
| 步骤8 | 创建调用工程并验证算子 |

### 算子分析

本示例实现AddCustom算子，对两个形状相同、数据类型为float16的输入张量x和y进行逐元素相加，得到同形状、同数据类型的输出张量z，即z = x + y。

### 创建工程目录<a id="create-project"></a>

自定义算子工程的目录结构和CMakeLists.txt由开发者直接编写；CANN软件包以CMake modules的形式提供`npu_op_*`系列接口，用于声明算子包、生成aclnn单算子调用代码并编译Host侧库和核函数（Kernel）侧库。

>[!NOTE]说明
>除了手写CMakeLists.txt外，也可以使用msOpGen工具生成自定义算子工程骨架，具体用法请参考[msOpGen快速入门](https://www.hiascend.com/document/detail/zh/mindstudio/latest/msOT/Operatordevelopmenttools/docs/zh/quick_start/msopgen_quick_start.md)。本文后续步骤均以手写CMakeLists.txt为例。

先创建工程目录：

```bash
mkdir -p AddCustom/op_host AddCustom/op_kernel
cd AddCustom
```

此时`op_host`和`op_kernel`均为空目录，下文将逐一创建并编写以下文件，完成后工程结构如下：

```
AddCustom
├── CMakeLists.txt            // [待创建] 工程CMakeLists.txt
├── op_host                   // Host侧实现文件
│   ├── add_custom.cpp        // [待创建] 算子原型注册、Tiling实现
│   └── CMakeLists.txt        // [待创建]
└── op_kernel                 // 核函数（Kernel）侧实现文件
    ├── add_custom.cpp        // [待创建] 算子代码实现
    ├── add_custom_tiling.h   // [待创建] Tiling数据结构定义
    └── CMakeLists.txt        // [待创建]
```

### 编写共享Tiling数据结构<a id="修改tiling与host侧实现"></a>

Tiling负责将Host侧的调度信息（如数据总量、分块数）传递到核函数（Kernel）侧执行。Tiling数据结构定义放在`op_kernel/add_custom_tiling.h`，供Host和核函数（Kernel）共同引用。

创建`op_kernel/add_custom_tiling.h`，使用标准C++语法定义TilingData结构体：

```cpp
#ifndef ADD_CUSTOM_TILING_H
#define ADD_CUSTOM_TILING_H
#include <cstdint>

struct AddCustomTilingData {
    uint32_t totalLength;  // 总计算数据量
    uint32_t tileNum;      // 总数据分块数量
};
#endif // ADD_CUSTOM_TILING_H
```

### 编写核函数（Kernel）侧代码<a id="修改kernel侧代码"></a>

创建`op_kernel/add_custom.cpp`，编写完整的算子计算实现：

```cpp
#include "kernel_operator.h"
#include "add_custom_tiling.h"

constexpr int32_t BUFFER_NUM = 2;
constexpr uint32_t STATIC_TILE_LENGTH = 128;

template <uint32_t tileLength>
class KernelAdd {
public:
    __aicore__ inline KernelAdd() {}
    __aicore__ inline void Init(
        __gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z, uint32_t totalLength, uint32_t tileNum)
    {
        ascendc_assert(tileNum != 0, "tileNum can not be zero.\n");
        this->blockLength = totalLength / AscendC::GetBlockNum();
        this->tileNum = tileNum;
        uint32_t runtimeTileLength = this->blockLength / tileNum / BUFFER_NUM;
        ascendc_assert(runtimeTileLength == tileLength, "runtime tileLength must match template tileLength.\n");

        xGm.SetGlobalBuffer((__gm__ DTYPE_X*)x + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
        yGm.SetGlobalBuffer((__gm__ DTYPE_Y*)y + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
        zGm.SetGlobalBuffer((__gm__ DTYPE_Z*)z + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
    }
    __aicore__ inline void Process()
    {
        int32_t loopCount = this->tileNum * BUFFER_NUM;

        AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
        AscendC::LocalTensor<DTYPE_X> xLocal = ubAllocator.Alloc<DTYPE_X, tileLength>();
        AscendC::LocalTensor<DTYPE_Y> yLocal = ubAllocator.Alloc<DTYPE_Y, tileLength>();
        AscendC::LocalTensor<DTYPE_Z> zLocal = ubAllocator.Alloc<DTYPE_Z, tileLength>();

        AscendC::SetFlag<AscendC::HardEvent::V_MTE2>(EVENT_ID0);
        AscendC::SetFlag<AscendC::HardEvent::MTE3_V>(EVENT_ID0);

        for (int32_t i = 0; i < loopCount; i++) {
            AscendC::WaitFlag<AscendC::HardEvent::V_MTE2>(EVENT_ID0);

            AscendC::DataCopy(xLocal, xGm[i * tileLength], tileLength);
            AscendC::DataCopy(yLocal, yGm[i * tileLength], tileLength);

            AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
            AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

            AscendC::WaitFlag<AscendC::HardEvent::MTE3_V>(EVENT_ID0);

            AscendC::Add(zLocal, xLocal, yLocal, tileLength);

            AscendC::SetFlag<AscendC::HardEvent::V_MTE2>(EVENT_ID0);
            AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
            AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);

            AscendC::DataCopy(zGm[i * tileLength], zLocal, tileLength);

            AscendC::SetFlag<AscendC::HardEvent::MTE3_V>(EVENT_ID0);
        }

        AscendC::WaitFlag<AscendC::HardEvent::V_MTE2>(EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE3_V>(EVENT_ID0);
    }

private:
    AscendC::GlobalTensor<DTYPE_X> xGm;
    AscendC::GlobalTensor<DTYPE_Y> yGm;
    AscendC::GlobalTensor<DTYPE_Z> zGm;
    uint32_t blockLength;
    uint32_t tileNum;
};

extern "C" __global__ __aicore__ void add_custom(
    __gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z, __gm__ uint8_t* workspace, __gm__ uint8_t* tiling)
{
    AscendC::InitSocState();

    REGISTER_TILING_DEFAULT(AddCustomTilingData);  // 注册TilingData结构体类型
    GET_TILING_DATA(tilingData, tiling);           // 反序列化tiling参数为结构体实例

    KernelAdd<STATIC_TILE_LENGTH> op;
    op.Init(x, y, z, tilingData.totalLength, tilingData.tileNum);
    op.Process();

    AscendC::PipeBarrier<PIPE_ALL>();
}
```

以上代码按`BUFFER_NUM`（双缓冲数）和模板参数`STATIC_TILE_LENGTH`（编译期分块长度）手动管理UB内存和搬运/计算之间的流水同步（`SetFlag`/`WaitFlag`），`DTYPE_X`/`DTYPE_Y`/`DTYPE_Z`由构建时根据下文Host原型注册的输入输出数据类型生成。`Init`中的断言要求`totalLength`满足`totalLength / GetBlockNum() / tileNum / BUFFER_NUM == STATIC_TILE_LENGTH`，即数据总量需与Host侧Tiling的`NUM_BLOCKS`、`TILE_NUM`和此处的`BUFFER_NUM`、`STATIC_TILE_LENGTH`配套，调用算子时需按此关系设置数据规模，具体可参考[核函数（Kernel）侧算子实现](./design_and_implementation/kernel_operator_implementation.md)。

### 编写Host侧实现

本示例以Atlas A2系列产品为例；请按[构建目标配置](#target-configuration)将此处的`AddConfig`和CMake配置中的`ascendxxx`替换为实际构建目标。

创建`op_host/add_custom.cpp`，包含三部分内容：`namespace optiling`中的Tiling函数、`namespace ge`中的InferShape/InferDataType实现，以及`namespace ops`中的算子原型注册。

```cpp
#include "../op_kernel/add_custom_tiling.h"
#include "register/op_def_registry.h"

namespace optiling {
const uint32_t NUM_BLOCKS = 8;
const uint32_t TILE_NUM = 8;

static ge::graphStatus TilingFunc(gert::TilingContext *context)
{
    uint32_t totalLength = context->GetInputShape(0)->GetOriginShape().GetShapeSize();
    context->SetSimdNumBlocks(NUM_BLOCKS);

    AddCustomTilingData *tiling = context->GetTilingData<AddCustomTilingData>();
    tiling->totalLength = totalLength;
    tiling->tileNum = TILE_NUM;

    size_t *currentWorkspace = context->GetWorkspaceSizes(1);
    currentWorkspace[0] = 0;

    return ge::GRAPH_SUCCESS;
}
} // namespace optiling

namespace ge {
static graphStatus InferShape(gert::InferShapeContext *context)
{
    const gert::Shape *xShape = context->GetInputShape(0);
    gert::Shape *zShape = context->GetOutputShape(0);
    *zShape = *xShape;
    return GRAPH_SUCCESS;
}

static graphStatus InferDataType(gert::InferDataTypeContext *context)
{
    const auto inputDataType = context->GetInputDataType(0);
    context->SetOutputDataType(0, inputDataType);
    return ge::GRAPH_SUCCESS;
}
} // namespace ge

namespace ops {
class AddCustom : public OpDef {
public:
    explicit AddCustom(const char *name) : OpDef(name)
    {
        this->Input("x").ParamType(REQUIRED).DataType({ge::DT_FLOAT16}).Format({ge::FORMAT_ND});
        this->Input("y").ParamType(REQUIRED).DataType({ge::DT_FLOAT16}).Format({ge::FORMAT_ND});
        this->Output("z").ParamType(REQUIRED).DataType({ge::DT_FLOAT16}).Format({ge::FORMAT_ND});

        this->SetInferShape(ge::InferShape).SetInferDataType(ge::InferDataType);
        this->AICore()
            .SetTiling(optiling::TilingFunc)
            .AddConfig("ascendxxx");  // ascendxxx请修改为对应的AI处理器系列，与CMake的ASCEND_SOC_SERIES保持一致
    }
};
OP_ADD(AddCustom);
} // namespace ops
```

> 以上代码分别展示了[Host侧Tiling实现](./design_and_implementation/host_tiling_implementation.md)中Tiling函数的基本写法，以及[算子原型定义](./design_and_implementation/operator_prototype_definition.md)中`OpDef`的注册方法。

### 编写CMakeLists.txt

#### 配置构建目标<a id="target-configuration"></a>

本示例的CMake和Host原型均使用`ascendxxx`作为构建目标，使用前需将两处替换为实际构建目标；配置多个目标时，Host原型应为各目标分别调用`AddConfig`。具体支持范围以所安装CANN版本为准，各产品对应的取值请参考[产品形态说明](https://www.hiascend.com/document/detail/zh/AscendFAQ/ProduTech/productform/hardwaredesc_0001.html)中的表1。

`ASCEND_SOC_SERIES`用于选择构建目标，支持单个或多个取值，输入不区分大小写。更多配置方式及存量工程使用`ASCEND_COMPUTE_UNIT`的说明见[编译配置](./compilation_and_deployment/basic_process.md#compile-configuration)。

#### 编写顶层文件

在工程根目录创建`CMakeLists.txt`，保存以下完整内容，用于声明算子包并添加`op_host`、`op_kernel`子目录：

```cmake
cmake_minimum_required(VERSION 3.19.0)
project(AddCustom)

# 在加载ASC前配置构建目标。CACHE默认值允许用cmake -D或preset覆盖。
if(NOT DEFINED ASCEND_COMPUTE_UNIT)
    set(ASCEND_SOC_SERIES "ascendxxx" CACHE STRING "AI processor series")
endif()
set(vendor_name "customize" CACHE STRING "Operator package vendor")

# 查找CANN软件包提供的ASC CMake modules
find_package(ASC REQUIRED)

npu_op_package(${vendor_name}
    TYPE RUN                           # 编译产物形态：RUN/STATIC/SHARED
    CONFIG
        INSTALL_PATH ${CMAKE_BINARY_DIR}/
)

add_subdirectory(op_host)
add_subdirectory(op_kernel)
```

#### 编写Host侧构建文件

在`op_host`目录下创建`CMakeLists.txt`，保存以下完整内容，用于生成aclnn调用代码并编译Host侧库：

```cmake
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} ops_srcs)

npu_op_code_gen(
    SRC ${ops_srcs}
    PACKAGE ${vendor_name}
    OUT_DIR ${ASCEND_AUTOGEN_PATH}
)

file(GLOB autogen_aclnn_src ${ASCEND_AUTOGEN_PATH}/aclnn_*.cpp)
set_source_files_properties(${autogen_aclnn_src} PROPERTIES GENERATED TRUE)
npu_op_library(cust_opapi ACLNN
    ${autogen_aclnn_src}
)

file(GLOB fallback_src ${ASCEND_AUTOGEN_PATH}/fallback_*.cpp)
set_source_files_properties(${fallback_src} PROPERTIES GENERATED TRUE)
npu_op_library(cust_optiling TILING
    ${ops_srcs}
    ${fallback_src}
)

npu_op_package_add(${vendor_name}
    LIBRARY
        cust_optiling
        cust_opapi
)
```

#### 编写核函数侧构建文件

在`op_kernel`目录下创建`CMakeLists.txt`，保存以下完整内容，用于指定核函数源码并配置编译：

```cmake
npu_op_kernel_sources(ascendc_kernels
    OP_TYPE AddCustom
    KERNEL_DIR ./                                  #核函数（Kernel）源码目录，该目录下文件（含头文件）整体打包进算子包
    KERNEL_FILE add_custom.cpp
)

npu_op_kernel_library(ascendc_kernels
    SRC_BASE ${CMAKE_CURRENT_SOURCE_DIR}
    TILING_LIBRARY cust_optiling
)

npu_op_package_add(${vendor_name}
    LIBRARY ascendc_kernels
)
```

> 以上三个CMakeLists.txt涉及的`npu_op_*`接口完整参数说明请参考[CMake函数参考](./appendix/cmake_function_reference.md)。

### 编译

当前终端仍位于前面创建的`AddCustom`工程根目录。确认所有文件已保存后，执行：

```bash
cmake -S . -B build
cmake --build build --target binary package --parallel 8
```

编译成功后，编译产物输出在`build`目录下，其中包含`.run`格式的算子安装包。

> 如需了解编译配置选项、动态库/静态库编译等更多选项，请参考[算子工程编译](./compilation_and_deployment/basic_process.md)。

### 部署

在算子工程根目录，将编译产物安装到CANN目录：

```bash
./build/custom_opp_*.run
```

安装完成后，算子库和头文件会被部署到`$ASCEND_OPP_PATH/vendors/customize/`下：

```
customize
├── op_api        // 单算子API头文件和动态库（aclnn_*.h、libcust_opapi.so）
├── op_impl       // 核函数（Kernel）二进制（.o）和Tiling动态库
├── scripts       // 算子包维护脚本
└── version.info  // 版本信息
```

设置自定义算子包部署出来的单算子API库路径：

```bash
export LD_LIBRARY_PATH=/usr/local/Ascend/cann/opp/vendors/customize/op_api/lib/:${LD_LIBRARY_PATH}
```

> `LD_LIBRARY_PATH`用于设置加载动态库时的搜寻路径列表，如需了解算子包部署路径、多版本管理等更多选项，请参考[算子包部署](./compilation_and_deployment/basic_process.md)。

### 调用算子

在算子工程的同级目录创建独立的验证工程；下面的命令从`AddCustom`根目录执行：

```bash
mkdir -p ../AddCustomTest
cd ../AddCustomTest
```

在该目录保存后续的`main.cpp`和`CMakeLists.txt`，目录结构如下：

```
AddCustomTest
├── CMakeLists.txt          // 编译配置
└── main.cpp                // 调用验证代码
```

#### 编写main.cpp

```cpp
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "acl/acl.h"
#include "aclnn/aclnn_base.h"
#include "aclnn_add_custom.h"

#define CHECK_ACL(call) \
    do { \
        aclnnStatus status = (call); \
        if (status != ACL_SUCCESS) { \
            printf("ACL Error: %d at %s:%d\n", status, __FILE__, __LINE__); \
            return -1; \
        } \
    } while (0)

#define CHECK_RTL(call) \
    do { \
        aclError err = (call); \
        if (err != ACL_SUCCESS) { \
            printf("ACL RT Error: %d at %s:%d\n", err, __FILE__, __LINE__); \
            return -1; \
        } \
    } while (0)

int main()
{
    // 1. 初始化ACL
    CHECK_ACL(aclnnInit(nullptr));

    const int32_t deviceId = 0;
    CHECK_RTL(aclrtSetDevice(deviceId));

    aclrtStream stream = nullptr;
    CHECK_RTL(aclrtCreateStream(&stream));

    // 2. 准备输入数据。shape需与Kernel侧的NUM_BLOCKS、TILE_NUM、BUFFER_NUM、
    //    STATIC_TILE_LENGTH配套（本示例为8*8*2*128=16384），数据类型需与算子原型注册的
    //    DT_FLOAT16一致
    const std::vector<int64_t> shape = {8, 2048};
    const int64_t count = shape[0] * shape[1];
    size_t bufferSize = count * sizeof(aclFloat16);

    std::vector<aclFloat16> input0Host(count);
    std::vector<aclFloat16> input1Host(count);
    std::vector<aclFloat16> outputHost(count, aclFloatToFloat16(0.0f));

    for (int64_t i = 0; i < count; i++) {
        input0Host[i] = aclFloatToFloat16(static_cast<float>(i % 100));
        input1Host[i] = aclFloatToFloat16(static_cast<float>(i % 100));
    }

    // 3. 申请设备内存
    void *input0DeviceMem = nullptr;
    void *input1DeviceMem = nullptr;
    void *outputDeviceMem = nullptr;

    CHECK_RTL(aclrtMalloc(&input0DeviceMem, bufferSize, ACL_MEM_MALLOC_HUGE_FIRST));
    CHECK_RTL(aclrtMalloc(&input1DeviceMem, bufferSize, ACL_MEM_MALLOC_HUGE_FIRST));
    CHECK_RTL(aclrtMalloc(&outputDeviceMem, bufferSize, ACL_MEM_MALLOC_HUGE_FIRST));

    // 4. 将输入数据从Host拷贝到Device
    CHECK_RTL(aclrtMemcpy(input0DeviceMem, bufferSize, input0Host.data(),
                          bufferSize, ACL_MEMCPY_HOST_TO_DEVICE));
    CHECK_RTL(aclrtMemcpy(input1DeviceMem, bufferSize, input1Host.data(),
                          bufferSize, ACL_MEMCPY_HOST_TO_DEVICE));

    // 5. 创建Tensor描述
    // aclCreateTensor参数：viewDims, viewDimsNum, dataType, stride(nullptr表示默认连续排布),
    //     offset, format, storageDims, storageDimsNum, tensorData
    aclTensor *input0 = aclCreateTensor(shape.data(), shape.size(), ACL_FLOAT16, nullptr, 0,
                                        ACL_FORMAT_ND, shape.data(), shape.size(), input0DeviceMem);
    aclTensor *input1 = aclCreateTensor(shape.data(), shape.size(), ACL_FLOAT16, nullptr, 0,
                                        ACL_FORMAT_ND, shape.data(), shape.size(), input1DeviceMem);
    aclTensor *output = aclCreateTensor(shape.data(), shape.size(), ACL_FLOAT16, nullptr, 0,
                                        ACL_FORMAT_ND, shape.data(), shape.size(), outputDeviceMem);

    // 6. 计算workspace大小并申请内存
    uint64_t workspaceSize = 0;
    aclOpExecutor *executor = nullptr;
    CHECK_ACL(aclnnAddCustomGetWorkspaceSize(input0, input1, output,
                                             &workspaceSize, &executor));

    void *workspaceDeviceMem = nullptr;
    if (workspaceSize > 0) {
        CHECK_RTL(aclrtMalloc(&workspaceDeviceMem, workspaceSize,
                              ACL_MEM_MALLOC_HUGE_FIRST));
    }

    // 7. 执行算子
    CHECK_ACL(aclnnAddCustom(workspaceDeviceMem, workspaceSize, executor, stream));

    // 8. 同步等待执行完成
    CHECK_RTL(aclrtSynchronizeStream(stream));

    // 9. 将输出数据从Device拷贝回Host
    CHECK_RTL(aclrtMemcpy(outputHost.data(), bufferSize, outputDeviceMem,
                          bufferSize, ACL_MEMCPY_DEVICE_TO_HOST));

    // 10. 验证结果
    bool pass = true;
    for (int64_t i = 0; i < count; i++) {
        float expected = aclFloat16ToFloat(input0Host[i]) + aclFloat16ToFloat(input1Host[i]);
        float actual = aclFloat16ToFloat(outputHost[i]);
        if (actual != expected) {
            printf("Error at index %lld: expected %f, got %f\n",
                   static_cast<long long>(i), expected, actual);
            pass = false;
            break;
        }
    }

    if (pass) {
        printf("test pass\n");
    } else {
        printf("test failed\n");
    }

    // 11. 释放资源
    aclDestroyTensor(input0);
    aclDestroyTensor(input1);
    aclDestroyTensor(output);
    aclDestroyAclOpExecutor(executor);

    if (workspaceDeviceMem) {
        aclrtFree(workspaceDeviceMem);
    }
    aclrtFree(input0DeviceMem);
    aclrtFree(input1DeviceMem);
    aclrtFree(outputDeviceMem);

    CHECK_RTL(aclrtDestroyStream(stream));
    CHECK_RTL(aclrtResetDevice(deviceId));
    aclnnFinalize();

    return 0;
}
```

#### 编写CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.19)
project(AddCustomTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 跳过RPATH设置，避免运行时找不到动态库
set(CMAKE_SKIP_RPATH TRUE)

add_executable(execute_add_op main.cpp)

set(CANN_INSTALL_PREFIX "/usr/local/Ascend/cann" CACHE PATH "CANN安装路径")

# 头文件搜索路径：包含CANN基础头文件和算子API头文件
target_include_directories(execute_add_op PRIVATE
    ${CANN_INSTALL_PREFIX}/include
    $ENV{ASCEND_OPP_PATH}/vendors/customize/op_api/include
)

# 库文件搜索路径
target_link_directories(execute_add_op PRIVATE
    ${CANN_INSTALL_PREFIX}/lib64
    $ENV{ASCEND_OPP_PATH}/vendors/customize/op_api/lib
)

# 链接依赖库
target_link_libraries(execute_add_op PRIVATE
    cust_opapi
    nnopbase
    acl_rt
)

# 输出目录
set_target_properties(execute_add_op PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output
)
```

#### 编译与运行

```bash
# 创建build目录并编译
mkdir -p build && cd build
cmake ../ -DCMAKE_SKIP_RPATH=TRUE
make

# 运行
cd ..
./output/execute_add_op
```

## 验证成功标志及运行失败检查

程序输出以下信息，表示端到端流程跑通成功：

```
test pass
```

如果输出`test failed`、ACL Error或动态库`not found`类错误，请检查：
1. CANN环境变量是否已加载（`source /usr/local/Ascend/cann/set_env.sh`）。
2. 算子是否已按照[部署](#部署)步骤正确部署。
3. 核函数（Kernel）侧和Host侧代码是否按[核函数（Kernel）代码](#修改kernel侧代码)、[Host代码](#编写host侧实现)正确编写。

## 下一步指引

恭喜您完成了第一个算子的端到端开发！接下来建议按[概述](./overview.md)里的路径继续学习。
