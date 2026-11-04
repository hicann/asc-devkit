# Aclnn Operator Engineering Development Quick Start

<!-- md-trans-meta sourceCommit=c891f0c5d5b2ac25c5683ff40810fa381d654ece translatedAt=2026-10-23T13:29:55.858Z pushedAt=2026-11-03T07:05:13.390Z nl=2 -->

This document covers the basics. Using a complete AddCustom operator example, it walks you through the end-to-end process of creating a project, implementing the operator, compiling and deploying it, and verifying it through a single-operator API call, all within 5 minutes.

![](../../../figures/end_to_end_operator_development_process.png)

## Prerequisites

### Compilation Toolchain

Before you begin, ensure that the following tools are installed on your system:

| Tool | Recommended Version | Description |
|------|-------------|------|
| CMake | ≥ 3.19 | Build system generator. |
| GCC / G++ | ≥ 7.5 | C++ compiler. |
| Python | ≥ 3.9 | Runtime dependency of the msOpGen tool. |
| Make | — | Build execution tool. |

msOpGen is an operator project generation tool provided in the CANN software package. After loading the CANN environment variables, you can run `msopgen --help` to verify that the tool is available. If the command is not in `PATH`, you can also invoke it using `${INSTALL_DIR}/python/site-packages/bin/msopgen`, where `${INSTALL_DIR}` is the CANN software installation path.

You can check the tool versions by running the following commands:

```bash
cmake --version
g++ --version
python3 --version
make --version
```

### CANN Environment Variables

Both compilation and running depend on the environment variables provided by CANN. Before starting any step, load the environment variables first:

```bash
source /usr/local/Ascend/cann/set_env.sh
```

> You need to run this command again each time you open a new terminal. This is not repeated in the subsequent steps.

## Quick Start Process

### Step Overview

| Step | Content | Estimated Time |
|------|------|----------|
| Step 1 | Create a project | 1 minute |
| Step 2 | View the directory structure | — |
| Step 3 | Modify the kernel function side code | 1 minute |
| Step 4 | Modify the Host-side Tiling | 1 minute |
| Step 5 | Compile | 1 minute |
| Step 6 | Deploy | — |
| Step 7 | Call the operator | 1 minute |

### Creating a Project<a id="create-project"></a>

Write the operator prototype definition file `add_custom.json` (it is recommended to place it in a path containing only English characters), and then use the msOpGen tool to generate the project skeleton. In the JSON, the `format` and `type` lists of each input/output must have the same length and maintain a one-to-one correspondence.

```json
[
    {
        "op": "AddCustom",
        "input_desc": [
            {
                "name": "x",
                "param_type": "required",
                "format": ["ND", "ND", "ND"],
                "type": ["float16", "float", "int32"]
            },
            {
                "name": "y",
                "param_type": "required",
                "format": ["ND", "ND", "ND"],
                "type": ["float16", "float", "int32"]
            }
        ],
        "output_desc": [
            {
                "name": "z",
                "param_type": "required",
                "format": ["ND", "ND", "ND"],
                "type": ["float16", "float", "int32"]
            }
        ]
    }
]
```

```bash
# Save the JSON content above as add_custom.json and then run:
source /usr/local/Ascend/cann/set_env.sh
msopgen gen -i add_custom.json -c ai_core-ascendxxyy -lan cpp -out AddCustom
```

-   **-i**: specifies the path of the operator prototype definition file add\_custom_.json. Modify it based on the actual situation.
-   **-c**: `ai_core-ascendxxyy` indicates that the operator runs on the AI Core, where ascendxxyy is the `soc_version` of the Ascend AI processor model. The type/format lists in the JSON define the data type and format combinations supported by the operator. msOpGen generates the corresponding kernel function compilation configuration based on this information.
-   -lan: the parameter cpp indicates that the operator is developed using the C/C++ programming language based on the Ascend C programming framework.
-   -out: path for storing the generated files. It can be configured as an absolute path or a relative path, and the user executing the tool must have read and write permissions on the path. If it is not configured, the files are generated in the current path where the command is executed by default.

Replace `ascendxxyy` with the `soc_version` of your AI processor model. msOpGen automatically maps the chip model to the unified framework identifier and writes it into `ASCEND_COMPUTE_UNIT` in `CMakePresets.json` and `AddConfig` in the prototype definition.

>[!NOTE] Note
>Obtain the `soc_version` of the AI processor model as follows:
><!-- npu="910b,910,310p,310b" id1 -->
>- For the following products, run the `npu-smi info` command on the server where the AI processor is installed to query and obtain the Name information. The actual configuration value is AscendName. For example, if the Name value is xxxyy, the actual configuration value is Ascendxxxyy.<br><br>
>    <!-- npu="910b" id2 -->
>    Atlas A2 products  
>    <!-- end id2 -->
>
>    <!-- npu="310b" id3 -->
>    Atlas 200I/500 A2 inference products  
>    <!-- end id3 -->
>
>    <!-- npu="310p" id4 -->
>    Atlas inference products  
>    <!-- end id4 -->
>
>    <!-- npu="910" id5 -->
>    Atlas training products<br><br> 
>    <!-- end id5 -->
><!-- end id1 -->
><!-- npu="A3" id6 -->
>- For <term>Atlas A3 products</term>, run the `npu-smi info -t board -i <id> -c <chip_id>` command on the server where the AI processor is installed to query and obtain the Chip Name and NPU Name information. The actual configuration value is `Chip Name_NPU Name`. For example, if the Chip Name value is Ascendxxx and the NPU Name value is 1234, the actual configuration value is Ascendxxx_1234. Where:<br>`id`: device ID. The NPU ID obtained by running the `npu-smi info -l` command is the device ID, represented as `<id>` in the command.<br>`chip_id`: chip ID. The Chip ID obtained by running the `npu-smi info -m` command is the chip ID, represented as `<chip_id>` in the command.<br><br>
><!-- end id6 -->
><!-- npu="950" id7 -->
>- For Ascend 950PR&950DT products, run the `npu-smi info -t board -i <id>` command on the server where the AI processor is installed to query and obtain the Chip Name and NPU Name information. The actual configuration value is `Chip Name_NPU Name`. For example, if the Chip Name value is Ascendxxx and the NPU Name value is 1234, the actual configuration value is Ascendxxx_1234.<br>Where: `id` is the device ID. The NPU ID obtained by running the `npu-smi info -l` command is the device ID, represented as `<id>` in the command.
><!-- end id7 -->

### Viewing the Directory Structure

The project directory structure generated by msOpGen is as follows:

```
AddCustom
├── build.sh                  // Build entry script.
├── CMakeLists.txt            // Project CMakeLists.txt.
├── CMakePresets.json         // Compilation preset configuration such as the chip model (automatically generated by msOpGen).
├── framework                 // AI framework adaptation plugin directory (used in graph integration scenarios; can be ignored for beginners).
├── op_host                   // Host-side implementation files.
│   ├── add_custom.cpp        // [To be modified] Operator prototype registration and Tiling implementation.
│   └── CMakeLists.txt
└── op_kernel                 // Kernel function (Kernel) side implementation files.
    ├── add_custom.cpp        // [To be modified] Operator code implementation.
    ├── add_custom_tiling.h   // [To be modified] Tiling data structure definition
    └── CMakeLists.txt
```

The files to be modified are marked with `[To be modified]`. These files will be modified one by one in the subsequent steps.

The `op_host/add_custom.cpp` generated by msOpGen already contains the framework code of `namespace ops` (operator prototype registration) and `namespace ge` (InferShape/InferDataType), as well as an empty implementation of the `TilingFunc` function. In the subsequent steps, you only need to modify the implementation of `TilingFunc` and keep the framework code unchanged.

### Modifying the Kernel Function Side Code<a name="modifying-kernel-side-code"></a>

Edit `op_kernel/add_custom.cpp` and replace the content in the auto-generated template with the complete operator computation implementation:

```cpp
#include "kernel_operator.h"
#include "add_custom_tiling.h"

using namespace AscendC;

class KernelAdd {
public:
    __aicore__ inline KernelAdd() {}
    __aicore__ inline void Init(GM_ADDR x, GM_ADDR y, GM_ADDR z,
                                uint32_t totalLength, uint32_t tileNum)
    {
        this->totalLength = totalLength;
        this->tileNum = tileNum;
        xGlobal.SetGlobalBuffer((__gm__ float*)x);
        yGlobal.SetGlobalBuffer((__gm__ float*)y);
        zGlobal.SetGlobalBuffer((__gm__ float*)z);

        pipe.InitBuffer(xQueue, 1, totalLength / tileNum * sizeof(float));
        pipe.InitBuffer(yQueue, 1, totalLength / tileNum * sizeof(float));
        pipe.InitBuffer(zQueue, 1, totalLength / tileNum * sizeof(float));
    }

    __aicore__ inline void Process()
    {
        uint32_t lengthPerBlock = totalLength / tileNum;
        uint32_t lengthPerCore = totalLength / GetBlockNum();
        uint32_t coreOffset = lengthPerCore * GetBlockIdx();

        for (uint32_t i = 0; i < lengthPerCore / lengthPerBlock; i++) {
            uint32_t offset = coreOffset + i * lengthPerBlock;
            CopyIn(offset, lengthPerBlock);
            Compute(lengthPerBlock);
            CopyOut(offset, lengthPerBlock);
        }
    }

private:
    __aicore__ inline void CopyIn(uint32_t offset, uint32_t length)
    {
        LocalTensor<float> xLocal = xQueue.AllocTensor<float>();
        LocalTensor<float> yLocal = yQueue.AllocTensor<float>();
        DataCopy(xLocal, xGlobal[offset], length);
        DataCopy(yLocal, yGlobal[offset], length);
        xQueue.EnQue(xLocal);
        yQueue.EnQue(yLocal);
    }

    __aicore__ inline void Compute(uint32_t length)
    {
        LocalTensor<float> xLocal = xQueue.DeQue<float>();
        LocalTensor<float> yLocal = yQueue.DeQue<float>();
        LocalTensor<float> zLocal = zQueue.AllocTensor<float>();

        Add(zLocal, xLocal, yLocal, length);

        zQueue.EnQue(zLocal);
        xQueue.FreeTensor(xLocal);
        yQueue.FreeTensor(yLocal);
    }

    __aicore__ inline void CopyOut(uint32_t offset, uint32_t length)
    {
        LocalTensor<float> zLocal = zQueue.DeQue<float>();
        DataCopy(zGlobal[offset], zLocal, length);
        zQueue.FreeTensor(zLocal);
    }

    TPipe pipe;
    TQue<TPosition::VECIN, 1> xQueue;
    TQue<TPosition::VECIN, 1> yQueue;
    TQue<TPosition::VECOUT, 1> zQueue;
    GlobalTensor<float> xGlobal;
    GlobalTensor<float> yGlobal;
    GlobalTensor<float> zGlobal;
    uint32_t totalLength;
    uint32_t tileNum;
};

extern "C" __global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z,
                                                   GM_ADDR workspace, GM_ADDR tiling)
{
    REGISTER_TILING_DEFAULT(AddCustomTilingData);  // Register the TilingData structure type.
    GET_TILING_DATA(tilingData, tiling);           // Deserialize the tiling parameters into a structure instance.
    KernelAdd op;
    op.Init(x, y, z, tilingData.totalLength, tilingData.tileNum);
    op.Process();
}
```

> The code above shows the basic pattern of [kernel function side operator implementation](./design_and_implementation/kernel_operator_implementation.md).

### Modifying Tiling and Host-Side Implementation

Tiling is responsible for passing the scheduling information (such as the total data volume and the number of blocks) from the Host side to the kernel function for execution. The Tiling data structure is defined in `op_kernel/add_custom_tiling.h` and is referenced by both the Host and the kernel function.

#### Modifying the Tiling Data Structure Definition

Edit `op_kernel/add_custom_tiling.h` and use standard C++ syntax to define the TilingData structure:

```cpp
#ifndef ADD_CUSTOM_TILING_H
#define ADD_CUSTOM_TILING_H
#include <cstdint>

struct AddCustomTilingData {
    uint32_t totalLength;  // Total amount of data to compute
    uint32_t tileNum;      // Number of data blocks
};
#endif // ADD_CUSTOM_TILING_H
```

#### Modifying the Host-Side Implementation

Edit `op_host/add_custom.cpp`. The auto-generated file already contains the operator prototype registration and the TilingFunc framework. Locate the `TilingFunc` function and modify it to the following implementation:

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
```

Keep the auto-generated `namespace ge` (InferShape/InferDataType) and `namespace ops` ([operator prototype registration](./design_and_implementation/operator_prototype_definition.md)) code at the end of the file unchanged.

> The code above shows the basic usage of the Tiling function and the TilingData data structure in the [host-side Tiling implementation](./design_and_implementation/host_tiling_implementation.md).

### Compiling

Run the compilation script in the root directory of the operator project:

```bash
cd AddCustom
./build.sh
```

After successful compilation, the compilation outputs are generated in the `build_out` directory, including the operator installation package in `.run` format.

> To learn about compilation configuration options, dynamic/static library compilation, and more options, see [Operator Project Compilation](./compilation_and_deployment/basic_process.md).

### Deploying

In the root directory of the operator project, install the compiled artifacts to the CANN directory:

```bash
./build_out/custom_opp_*.run
```

After the installation is complete, the operator library and header files are deployed to `$ASCEND_OPP_PATH/vendors/customize/`:

```
customize
├── framework     // AI framework adaptation plugin
├── op_api        // Single-operator API header files and dynamic library (aclnn_*.h, libcust_opapi.so)
├── op_impl       // Kernel function (Kernel) binary (.o) and Tiling dynamic library
├── op_proto      // Operator prototype dynamic library
├── scripts       // Operator package maintenance scripts
└── version.info  // Version information
```

Set the path of the single-operator API library deployed from the custom operator package:

```bash
export LD_LIBRARY_PATH=/usr/local/Ascend/cann/opp/vendors/customize/op_api/lib/:${LD_LIBRARY_PATH}
```

> `LD_LIBRARY_PATH` is used to set the search path list for loading dynamic libraries. For more options such as the operator package deployment path and multi-version management, see [Operator Package Deployment](./compilation_and_deployment/basic_process.md).

### Calling the Operator

Create an independent verification project to call the deployed operator. The directory structure is as follows:

```
AddCustomTest
├── CMakeLists.txt          // Compilation configuration
└── main.cpp                // Call verification code
```

#### Writing main.cpp

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
    // 1. Initialize ACL
    CHECK_ACL(aclnnInit(nullptr));

    const int32_t deviceId = 0;
    CHECK_RTL(aclrtSetDevice(deviceId));

    aclrtStream stream = nullptr;
    CHECK_RTL(aclrtCreateStream(&stream));

    // 2. Prepare the input data
    const int count = 1024;
    size_t bufferSize = count * sizeof(float);

    std::vector<float> input0Host(count);
    std::vector<float> input1Host(count);
    std::vector<float> outputHost(count, 0.0f);

    for (int i = 0; i < count; i++) {
        input0Host[i] = static_cast<float>(i);
        input1Host[i] = static_cast<float>(i);
    }

    // 3. Allocate device memory
    void *input0DeviceMem = nullptr;
    void *input1DeviceMem = nullptr;
    void *outputDeviceMem = nullptr;

    CHECK_RTL(aclrtMalloc(&input0DeviceMem, bufferSize, ACL_MEM_MALLOC_HUGE_FIRST));
    CHECK_RTL(aclrtMalloc(&input1DeviceMem, bufferSize, ACL_MEM_MALLOC_HUGE_FIRST));
    CHECK_RTL(aclrtMalloc(&outputDeviceMem, bufferSize, ACL_MEM_MALLOC_HUGE_FIRST));

    // 4. Copy the input data from Host to Device
    CHECK_RTL(aclrtMemcpy(input0DeviceMem, bufferSize, input0Host.data(),
                          bufferSize, ACL_MEMCPY_HOST_TO_DEVICE));
    CHECK_RTL(aclrtMemcpy(input1DeviceMem, bufferSize, input1Host.data(),
                          bufferSize, ACL_MEMCPY_HOST_TO_DEVICE));

    // 5. Create a tensor description
    int64_t shape[] = {count};
    uint64_t shapeNum = 1;

    // aclCreateTensor parameters: viewDims, viewDimsNum, dataType, stride (nullptr indicates the default contiguous layout),
    //     offset, format, storageDims, storageDimsNum, tensorData
    aclTensor *input0 = aclCreateTensor(shape, shapeNum, ACL_FLOAT, nullptr, 0,
                                        ACL_FORMAT_ND, shape, shapeNum, input0DeviceMem);
    aclTensor *input1 = aclCreateTensor(shape, shapeNum, ACL_FLOAT, nullptr, 0,
                                        ACL_FORMAT_ND, shape, shapeNum, input1DeviceMem);
    aclTensor *output = aclCreateTensor(shape, shapeNum, ACL_FLOAT, nullptr, 0,
                                        ACL_FORMAT_ND, shape, shapeNum, outputDeviceMem);

    // 6. Calculate the workspace size and allocate memory
    uint64_t workspaceSize = 0;
    aclOpExecutor *executor = nullptr;
    CHECK_ACL(aclnnAddCustomGetWorkspaceSize(input0, input1, output,
                                             &workspaceSize, &executor));

    void *workspaceDeviceMem = nullptr;
    if (workspaceSize > 0) {
        CHECK_RTL(aclrtMalloc(&workspaceDeviceMem, workspaceSize,
                              ACL_MEM_MALLOC_HUGE_FIRST));
    }

    // 7. Execute the operator
    CHECK_ACL(aclnnAddCustom(workspaceDeviceMem, workspaceSize, executor, stream));

    // 8. Wait synchronously for execution to complete
    CHECK_RTL(aclrtSynchronizeStream(stream));

    // 9. Copy the output data from Device back to Host.
    CHECK_RTL(aclrtMemcpy(outputHost.data(), bufferSize, outputDeviceMem,
                          bufferSize, ACL_MEMCPY_DEVICE_TO_HOST));

    // 10. Verify the result.
    bool pass = true;
    for (int i = 0; i < count; i++) {
        float expected = input0Host[i] + input1Host[i];
        if (outputHost[i] != expected) {
            printf("Error at index %d: expected %f, got %f\n",
                   i, expected, outputHost[i]);
            pass = false;
            break;
        }
    }

    if (pass) {
        printf("test pass\n");
    } else {
        printf("test failed\n");
    }

    // 11. Release resources.
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

#### Writing CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.19)
project(AddCustomTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Skip RPATH settings to avoid failing to find dynamic libraries at runtime
set(CMAKE_SKIP_RPATH TRUE)

add_executable(execute_add_op main.cpp)

set(CANN_INSTALL_PREFIX "/usr/local/Ascend/cann" CACHE PATH "CANN installation path")

# Header file search path: includes CANN base header files and operator API header files
target_include_directories(execute_add_op PRIVATE
    ${CANN_INSTALL_PREFIX}/include
    $ENV{ASCEND_OPP_PATH}/vendors/customize/op_api/include
)

# Library file search path
target_link_directories(execute_add_op PRIVATE
    ${CANN_INSTALL_PREFIX}/lib64
    $ENV{ASCEND_OPP_PATH}/vendors/customize/op_api/lib
)

# Link dependent libraries
target_link_libraries(execute_add_op PRIVATE
    cust_opapi
    nnopbase
    acl_rt
)

# Output directory
set_target_properties(execute_add_op PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output
)
```

#### Compilation and Running

```bash
# Create the build directory and compile
mkdir -p build && cd build
cmake ../ -DCMAKE_SKIP_RPATH=TRUE
make

# Run
cd ..
./output/execute_add_op
```

## Verification Success Criteria and Failure Troubleshooting

If the program outputs the following information, the end-to-end process runs successfully:

```
test pass
```

If the output contains `test failed`, an ACL Error, or a `not found` error for a dynamic library, check the following:
1. Whether the CANN environment variables are loaded (`source /usr/local/Ascend/cann/set_env.sh`).
2. Whether the operator is correctly deployed by following the steps in [Deploying](#deploying).
3. Whether the kernel function side and Host side code are correctly modified as described in [Kernel Function Code](#modifying-the-kernel-function-side-code) and [Host Code](#modifying-tiling-and-host-side-implementation).

## Next Steps

Congratulations! You have completed the end-to-end development of your first operator. Next, you are advised to continue learning by following the path in [Overview](./overview.md).
