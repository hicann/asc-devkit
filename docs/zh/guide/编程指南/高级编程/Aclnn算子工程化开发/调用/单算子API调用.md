# 单算子API调用

本文是基础内容，介绍单算子API调用——即通过C/C++调用aclnn接口完成异构计算的过程。aclnn接口和实现已经随自定义算子包交付，因此调用前需要先完成自定义算子包部署。运行时需要可用NPU环境；计算过程中使用的输入输出Tensor、Device内存、workspace、stream等资源由调用方负责申请和释放。

## 调用前检查

调用单算子API前，先确认以下条件已经满足：

-   调用算子的运行环境有NPU设备。
-   完成自定义算子工程的创建。
-   完成算子原型定义、Kernel侧实现和Host侧Tiling实现。
-   对于算子包编译场景，参考[算子工程编译与部署](../编译与部署/基本流程.md)完成算子包的编译部署。算子二进制编译开关`ENABLE_BINARY_PACKAGE`默认开启；如果工程中显式设置为`False`，需要改为`True`后重新编译部署。

    算子编译部署后，会在算子包安装目录下的`op_api`目录生成单算子调用的头文件`aclnn_<op>.h`和动态库`libcust_opapi.so`。

    以默认安装场景为例，单算子调用的头文件和动态库所在的目录结构如下所示：

    ```
    ├── opp    //算子库目录
    │   ├── vendors     //自定义算子所在目录
    │       ├── config.ini
    │       └── vendor_name   // 存储对应厂商部署的自定义算子，此名字为编译自定义算子安装包时配置的vendor_name，若未配置，默认值为customize
    │           ├── op_api
    │           │   ├── include
    │           │   │  └── aclnn_xx.h
    │           │   └── lib
    │           │       └── libcust_opapi.so
    ...
    ```
-   对于算子动态库或静态库编译场景，参考[算子动态库和静态库编译](../编译与部署/算子动态库和静态库编译.md)完成算子的编译安装。其中`CMAKE_INSTALL_PREFIX`为开发者在cmake文件中配置的安装路径。
    -   动态库路径：`${CMAKE_INSTALL_PREFIX}/lib/libcust_opapi.so`
    -   静态库路径：`${CMAKE_INSTALL_PREFIX}/lib/lib${vendor_name}.a`
    -   头文件路径：`${CMAKE_INSTALL_PREFIX}/include`

## 基本原理

完成自定义算子编译后，算子工程会基于算子原型定义自动生成单算子API头文件`aclnn_<op>.h`，并编译生成单算子API动态库`libcust_opapi.so`。调用时导入该头文件、链接该动态库后，就可以直接调用自定义算子。

单算子API一般是“两段式接口”：

```c
aclnnStatus aclnnXxxGetWorkspaceSize(const aclTensor *src, ..., aclTensor *out, uint64_t *workspaceSize, aclOpExecutor **executor);
aclnnStatus aclnnXxx(void *workspace, uint64_t workspaceSize, aclOpExecutor *executor, aclrtStream stream);
```

其中：

- `aclnnXxxGetWorkspaceSize`是第一段接口，用于计算本次调用需要的workspace大小，按照workspaceSize申请NPU内存。
- `aclnnXxx`是第二段接口，使用第一段接口得到的`workspaceSize`、`executor`以及调用方申请的workspace，在指定stream上执行实际计算。
- `Xxx`来自算子原型注册时传入的算子类型。例如AddCustom算子生成的接口名为`aclnnAddCustomGetWorkspaceSize`和`aclnnAddCustom`。

## 准备调用工程

本节以[AddCustom自定义算子](../Aclnn算子工程化开发快速入门.md)调用为例，介绍如何编写单算子调用的代码逻辑。其他算子的调用逻辑与AddCustom算子类似，请根据实际情况自行修改代码。该工程包含两个文件：

```text
├── CMakeLists.txt    // 编译规则文件
├── main.cpp          // 单算子调用主体流程实现文件
```

以下是关键步骤的代码示例，不可以直接拷贝编译运行，仅供参考，调用接口后，需增加异常处理的分支，并记录报错日志、提示日志，此处不一一列举。

## 单算子调用流程

单算子API执行流程如下：

**图1** 单算子API执行接口调用流程。
![](../../../../figures/single_operator_api_execution_interface_call_flow.png "单算子API执行接口调用流程")

### 包含头文件并准备检查宏

调用程序需要包含CANN运行时头文件、aclnn基础头文件和算子API头文件：

```cpp
#include "acl/acl.h"
#include "aclnn/aclnn_base.h"
#include "aclnn_add_custom.h"
```

`acl/acl.h`提供`aclrtMalloc`、`aclrtMemcpy`、`aclrtSetDevice`等运行时接口；`aclnn/aclnn_base.h`提供`aclnnInit`、`aclnnFinalize`、`aclOpExecutor`等aclnn基础类型和接口；`aclnn_add_custom.h`是AddCustom算子工程生成的单算子API头文件。

### 初始化运行环境

```cpp
CHECK_ACL(aclnnInit(nullptr));

const int32_t deviceId = 0;
CHECK_RTL(aclrtSetDevice(deviceId));

aclrtStream stream = nullptr;
CHECK_RTL(aclrtCreateStream(&stream));
```

这一步完成ACL初始化、指定Device，并创建后续算子下发使用的stream。

### 准备输入输出数据并创建Tensor

```cpp
// 申请内存存放算子的输入输出
CHECK_RTL(aclrtMalloc(&input0DeviceMem, bufferSize, ACL_MEM_MALLOC_HUGE_FIRST));
CHECK_RTL(aclrtMalloc(&input1DeviceMem, bufferSize, ACL_MEM_MALLOC_HUGE_FIRST));
CHECK_RTL(aclrtMalloc(&outputDeviceMem, bufferSize, ACL_MEM_MALLOC_HUGE_FIRST));
// 传输数据
CHECK_RTL(aclrtMemcpy(input0DeviceMem, bufferSize, input0Host.data(),
                      bufferSize, ACL_MEMCPY_HOST_TO_DEVICE));
CHECK_RTL(aclrtMemcpy(input1DeviceMem, bufferSize, input1Host.data(),
                      bufferSize, ACL_MEMCPY_HOST_TO_DEVICE));

int64_t shape[] = {count};
uint64_t shapeNum = 1;
// 创建tensor
aclTensor *input0 = aclCreateTensor(shape, shapeNum, ACL_FLOAT, nullptr, 0,
                                    ACL_FORMAT_ND, shape, shapeNum, input0DeviceMem);
aclTensor *input1 = aclCreateTensor(shape, shapeNum, ACL_FLOAT, nullptr, 0,
                                    ACL_FORMAT_ND, shape, shapeNum, input1DeviceMem);
aclTensor *output = aclCreateTensor(shape, shapeNum, ACL_FLOAT, nullptr, 0,
                                    ACL_FORMAT_ND, shape, shapeNum, outputDeviceMem);
```

### 调用两段式接口

```cpp
// 计算workspace大小并申请内存
uint64_t workspaceSize = 0;
aclOpExecutor *executor = nullptr;
CHECK_ACL(aclnnAddCustomGetWorkspaceSize(input0, input1, output,
                                         &workspaceSize, &executor));

void *workspaceDeviceMem = nullptr;
if (workspaceSize > 0) {
    CHECK_RTL(aclrtMalloc(&workspaceDeviceMem, workspaceSize,
                          ACL_MEM_MALLOC_HUGE_FIRST));
}
// 执行算子
CHECK_ACL(aclnnAddCustom(workspaceDeviceMem, workspaceSize, executor, stream));
// 同步等待
CHECK_RTL(aclrtSynchronizeStream(stream));
```

第一段接口返回`workspaceSize`后，调用程序需要按需申请Device侧workspace内存。第二段接口负责下发算子计算，随后通过`aclrtSynchronizeStream`等待计算完成。

### 拷贝结果并释放资源

```cpp
CHECK_RTL(aclrtMemcpy(outputHost.data(), bufferSize, outputDeviceMem,
                      bufferSize, ACL_MEMCPY_DEVICE_TO_HOST));

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
```

使用`aclCreateTensor`创建的Tensor需要调用`aclDestroyTensor`释放；第一段接口返回的`executor`需要调用`aclDestroyAclOpExecutor`释放；Device内存和stream也需要在程序结束前释放。

## 编译与运行

### CMakeLists.txt关键配置

算子编译部署后，会生成单算子调用的头文件`aclnn_<op>.h`和算子API库。算子包部署场景通常链接`libcust_opapi.so`；动态库或静态库编译场景可参考[算子动态库和静态库编译](../编译与部署/算子动态库和静态库编译.md)，通过`find_package`导入算子库或按实际路径直接链接。

编译算子调用程序时，需要在头文件的搜索路径include\_directories中增加单算子调用的头文件目录，便于找到该头文件；同时需要链接cust\_opapi动态库并在库文件的搜索路径link\_directories中增加libcust\_opapi.so所在目录。

-   在头文件的搜索路径include\_directories中增加单算子调用的头文件目录。以下样例仅为参考，请根据头文件的实际目录位置进行设置。

    ```
    target_include_directories(execute_add_op PRIVATE
        $ENV{ASCEND_HOME_PATH}/include
        $ENV{ASCEND_OPP_PATH}/vendors/${VENDOR_NAME}/op_api/include
    )
    ```

-   链接cust\_opapi链接库。

    ```
    target_link_libraries(execute_add_op PRIVATE
        cust_opapi
        nnopbase
        acl_rt
    )
    ```

-   在库文件的搜索路径link\_directories中增加libcust\_opapi.so所在目录。以下样例仅为参考，请根据库文件的实际目录位置进行设置。

    ```
    target_link_directories(execute_add_op PRIVATE
        $ENV{ASCEND_HOME_PATH}/lib64
        $ENV{ASCEND_OPP_PATH}/vendors/${VENDOR_NAME}/op_api/lib
    )
    ```

### 编译运行步骤

1.  在运行环境上设置环境变量，配置单算子验证程序编译依赖的头文件与库文件路径：

    ```bash
    source /usr/local/Ascend/cann/set_env.sh
    # <vendor_name>需替换为算子工程CMakePresets.json中的vendor_name
    export VENDOR_NAME=<vendor_name>
    export LD_LIBRARY_PATH=${ASCEND_OPP_PATH}/vendors/${VENDOR_NAME}/op_api/lib/:${LD_LIBRARY_PATH}
    ```

2.  编译样例工程，生成单算子验证可执行文件。
    1.  切换到样例工程根目录，然后在样例工程根目录下执行如下命令创建目录用于存放编译文件，例如`build`。

        ```
        mkdir -p build
        ```

    2.  进入build目录，执行cmake编译命令，生成编译文件。

        命令示例如下所示：

        ```
        cd build
        cmake ../ -DCMAKE_SKIP_RPATH=TRUE
        ```

    3.  执行如下命令，生成可执行文件。

        ```
        make
        ```

        可执行文件生成位置由调用工程的CMakeLists.txt决定。未设置输出目录时，通常生成在`build`目录下；如果调用工程设置了`RUNTIME_OUTPUT_DIRECTORY`，请按实际配置路径查找。

3.  在运行环境中配置运行依赖，并执行`execute_add_op`文件：

        ```
        # 以下路径仅为示例，请替换为实际生成的可执行文件路径
        ./build/execute_add_op
        ```

        如果有`test pass`，表明执行成功。

## 常见问题

**找不到`aclnn_add_custom.h`**

先确认`VENDOR_NAME`是否与算子工程`CMakePresets.json`中的`vendor_name`一致，再确认算子包是否已经安装：

```bash
ls $ASCEND_OPP_PATH/vendors/${VENDOR_NAME}/op_api/include/aclnn_add_custom.h
```

如果使用指定目录安装，需要检查`<path>/vendors/${VENDOR_NAME}/op_api/include/aclnn_add_custom.h`。

**运行时报`libcust_opapi.so: cannot open shared object file`**

说明运行时动态库搜索路径中没有自定义算子API库。设置：

```bash
export LD_LIBRARY_PATH=${ASCEND_OPP_PATH}/vendors/${VENDOR_NAME}/op_api/lib/:${LD_LIBRARY_PATH}
```

## 进阶：API参数命名规则

首次跑通AddCustom调用时，可以先跳过本节。aclnn接口入参由算子原型注册信息转换生成，输入、输出、属性等原型定义会映射为C接口中的参数；具体参数类型和名称以实际生成的`aclnn_<op>.h`为准。开发包含可选输入、多输出、属性或数据依赖输入的算子时，可参考以下规则确认生成接口的参数名称。

**aclnn_Xxx_GetWorkspaceSize**接口的输入输出参数生成规则如下：

-   可选输入的命名增加Optional后缀。如下样例中，x是可选输入。

    ```
    aclnnStatus aclnnXxxGetWorkspaceSize(const aclTensor *xOptional, ..., aclTensor *out, uint64_t *workspaceSize, aclOpExecutor **executor);
    ```

-   输入输出同名、使用同一个Tensor承载的情况下，生成的aclnn接口中只保留input参数，同时去掉input的const修饰，并以Ref作为后缀。如下样例中，原型定义input、output都定义为x，xRef既作为输入，又作为输出。

    ```
    aclnnStatus aclnnXxxGetWorkspaceSize(aclTensor *xRef, ..., uint64_t *workspaceSize, aclOpExecutor **executor);
    ```

-   如果仅有一个输出，输出参数命名为out；如果存在多个输出，每个输出后面都以Out作为后缀。

    ```
    // 仅有一个输出
    aclnnStatus aclnnXxxGetWorkspaceSize(const aclTensor *src, ..., aclTensor *out, uint64_t *workspaceSize, aclOpExecutor **executor);
    // 存在多个输出
    aclnnStatus aclnnXxxGetWorkspaceSize(const aclTensor *src, ..., aclTensor *yOut, aclTensor *y1Out, ..., uint64_t *workspaceSize, aclOpExecutor **executor);
    ```

-   如果算子包含属性，则属性参数的位置介于输入和输出之间。如下示例中，x是算子输入，negativeSlope是算子属性，out是算子输出。

    ```
    aclnnStatus aclnnXxxGetWorkspaceSize(const aclTensor *x, double negativeSlope, aclTensor *out, uint64_t *workspaceSize, aclOpExecutor **executor);
    ```

当算子原型注册时使用ValueDepend接口标识输入为数据依赖输入时，会额外生成一个API，该API支持值依赖场景输入数据为空的一阶段计算。

```
aclnnStatus aclnnXxxTensorGetWorkspaceSize(const aclTensor *src, ..., aclTensor *out, uint64_t *workspaceSize, aclOpExecutor **executor);
```

**在aclnn_Xxx_TensorGetWorkspaceSize**中，**aclnn_Xxx_GetWorkspaceSize**参数的数据类型（aclIntArray、aclFloatArray和aclBoolArray）将被转换为aclTensor数据类型，其他输入输出参数生成规则与**aclnn_Xxx_GetWorkspaceSize**一致。如下示例中，x0、x1、x2是算子声明为数据依赖的输入，数据类型分别为DT\_INT64、DT\_BOOL、DT\_FLOAT，out是算子输出。

```
aclnnStatus aclnnXxxGetWorkspaceSize(const aclIntArray *x0, const aclBoolArray *x1, const aclFloatArray *x2, aclTensor *out, uint64_t *workspaceSize, aclOpExecutor **executor);
aclnnStatus aclnnXxxTensorGetWorkspaceSize(const aclTensor *x0, const aclTensor *x1, const aclTensor *x2, aclTensor *out, uint64_t *workspaceSize, aclOpExecutor **executor);
```
## 相关文档

- [快速入门](../Aclnn算子工程化开发快速入门.md) — 创建并跑通AddCustom算子工程。
- [算子工程编译](../编译与部署/基本流程.md) — 部署算子包并查看部署目录。
