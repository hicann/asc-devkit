# 图编译和图执行<a name="ZH-CN_TOPIC_0000001920158120"></a>

本节通过单算子模型执行的样例来介绍图模式下图编译和图执行流程。单算子模型执行是指基于图IR执行算子，先编译算子（例如，使用ATC工具将Ascend IR定义的单算子描述文件编译成算子om模型文件），再调用acl接口加载算子模型，最后调用acl接口执行算子。

## 环境要求<a name="zh-cn_topic_0000001541959061_section19582183344920"></a>

-   已参考[环境准备](../../../getting_started/environment_setup.md)，完成CANN驱动和软件的安装，配置CANN软件所需基本环境变量。

    安装CANN软件后，使用CANN运行用户进行编译、运行时，需要以CANN运行用户登录环境，执行`source ${INSTALL_DIR}/set_env.sh`命令设置环境变量。`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。以root用户安装为例，安装后文件默认存储路径为：/usr/local/Ascend/cann。

-   已参考[Aclnn算子工程化开发](../aclnn_operator_development/overview.md)完成算子的开发和部署。

## 准备验证代码工程<a name="zh-cn_topic_0000001541959061_section2021523012501"></a>

代码工程目录结构如下，您可以参考[单算子模型调用样例](../../../../../../examples/01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclop_invocation)，获取完整的样例工程：

```
├── aclop_invocation
│   ├── add_custom.json                   // 算子描述文件，用于构造单算子模型文件
│   ├── CMakeLists.txt
│   ├── main.cpp                          // 加载并执行单算子om模型，校验计算结果
│   └── scripts
│       └── gen_data.py                   // 生成输入数据和精度校验所需的预期结果
```

## 生成单算子离线模型文件<a name="section17164152011141"></a>

1.  构造静态shape单算子描述文件add\_custom.json，描述算子的输入、输出及属性等信息。

    AddCustom静态shape算子的描述文件示例如下：

    ```
    [
        {
            "op": "AddCustom",
            "input_desc": [
                {
                    "name": "x",
                    "param_type": "required",
                    "format": "ND",
                    "shape": [8, 2048],
                    "type": "float16"
                },
                {
                    "name": "y",
                    "param_type": "required",
                    "format":"ND",
                    "shape": [8, 2048],
                    "type": "float16"
                }
            ],
            "output_desc": [
                {
                    "name": "z",
                    "param_type": "required",
                    "format":  "ND",
                    "shape": [8, 2048],
                    "type": "float16"
                }
            ]
        }
    ]
    ```

2.  使用ATC工具，将该算子描述文件编译成单算子模型文件（\*.om文件）。在样例工程根目录执行以下命令，进入build目录后生成数据和om文件，使其与后续编译出的可执行文件位于同一目录。`scripts/gen_data.py`依赖NumPy，执行前请确认Python环境中已安装该依赖。

    ```
    mkdir -p build && cd build
    python3 ../scripts/gen_data.py
    atc --singleop=../add_custom.json --output=. --soc_version=<soc_version>
    ```

    `gen_data.py`在当前build目录下生成`input/input0.bin`、`input/input1.bin`和`output/golden.bin`。前两个文件为算子输入，`golden.bin`为预期的相加结果，验证程序会读取这三个文件进行计算和精度对比。

    关键参数解释如下：

    -   --singleop：单算子描述文件（json格式）的路径。
    -   --output：存放om模型文件的目录。
    -   --soc\_version：配置为AI处理器的型号，请根据实际环境进行替换。

    以上命令执行后，会在output参数指定路径下生成\*.om后缀的离线模型文件。

## 编写验证代码<a name="zh-cn_topic_0000001541959061_section1862016464513"></a>

您可以参考如下样例编写单算子加载、执行的代码逻辑。

以下是关键步骤的代码示例，不可以直接拷贝编译运行，仅供参考，调用接口后，需增加异常处理的分支，并记录报错日志、提示日志，此处不一一列举。

```
// 1.初始化
CHECK_ACL(aclInit(nullptr));

// 2.运行管理资源申请
const int32_t deviceId = 0;
CHECK_ACL(aclrtSetDevice(deviceId));

// 3.加载单算子模型文件（*.om文件）
CHECK_ACL(aclopSetModelDir("."));

// 4.设置算子的输入，申请内存，然后读取输入数据保存至申请的内存中
// ......

// 5.创建Stream流
aclrtStream stream = nullptr;
aclrtCreateStream(&stream);

// 6.执行算子
// opType表示算子类型名称，例如AddCustom
// inputDesc.size()表示算子输入个数，例如AddCustom算子是2个输入
// inputDesc.data()表示算子输入tensor描述的数组，描述每个输入的format、shape、数据类型
// inputBuffers.data()表示算子输入tensor数据
// outputDesc.size()表示算子输出个数，例如AddCustom算子是1个输出
// outputDesc.data()表示算子输出tensor描述的数组，描述每个输出的format、shape、数据类型
// outputBuffers.data()表示算子输出tensor数据
// opAttr表示算子属性，如果算子没有属性，也需要调用aclopCreateAttr接口创建aclopAttr类型的数据
// stream用于维护一些异步操作的执行顺序

CHECK_ACL(aclopExecuteV2(opType, inputDesc.size(), inputDesc.data(), inputBuffers.data(),
                             outputDesc.size(), outputDesc.data(), outputBuffers.data(), opAttr, stream));


// 7.阻塞应用运行，直到指定Stream中的所有任务都完成
aclrtSynchronizeStream(stream);

// 8.处理执行算子后的输出数据，例如在屏幕上显示、写入文件等，由用户根据实际情况自行实现
// ......

// 9.释放stream流
aclrtDestroyStream(stream);

// 10.释放运行管理资源
aclRet = aclrtResetDevice(deviceId);
aclRet = aclFinalize();

// ....
```

## 运行和验证<a name="zh-cn_topic_0000001541959061_section236513711532"></a>

1.  开发环境上，设置环境变量，配置单算子验证程序编译依赖的头文件与库文件路径，如下为设置环境变量的示例。`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。以root用户安装为例，安装后文件默认存储路径为：/usr/local/Ascend/cann。{arch-os}为运行环境的架构和操作系统，arch表示操作系统架构，os表示操作系统，例如x86\_64-linux。

    ```
    export DDK_PATH=${INSTALL_DIR}
    export NPU_HOST_LIB=${INSTALL_DIR}/{arch-os}/devlib
    ```

2.  编译样例工程，生成单算子验证可执行文件。
    1.  切换到样例工程根目录，然后在样例工程根目录下执行如下命令创建目录用于存放编译文件，例如，创建的目录为“build“。

        **mkdir -p build**

    2.  进入build目录，执行CMake编译命令，生成编译文件

        命令示例如下所示：

        **cd build**

        **cmake ..**

    3.  执行如下命令，生成可执行文件。

        **make**

        会在build目录下生成可执行文件**execute\_add\_op**。

3.  执行单算子
    1.  将开发环境中样例工程build目录下的以下文件和目录一起拷贝到运行环境的同一目录：

        ```text
        ├── execute_add_op
        ├── *.om
        ├── input
        │   ├── input0.bin
        │   └── input1.bin
        └── output
            └── golden.bin
        ```

        说明：若您的开发环境即为运行环境，此拷贝操作可跳过。

    2.  以运行用户登录运行环境，配置CANN和自定义算子包的环境变量。以下以CANN默认安装路径和样例的`customize`算子包为例；请根据运行环境修改`INSTALL_DIR`。算子包安装在自定义路径时，将`OPP_INSTALL_DIR`改为安装时`--install-path`指定的目录。

        ```bash
        INSTALL_DIR=/usr/local/Ascend/cann
        OPP_INSTALL_DIR=${INSTALL_DIR}/opp
        source ${INSTALL_DIR}/set_env.sh
        source ${OPP_INSTALL_DIR}/vendors/customize/bin/set_env.bash
        ```

    3.  切换到存放上述文件和目录的位置，执行验证程序。程序从当前工作目录查找om模型、输入数据和预期结果文件。

        ```bash
        chmod +x execute_add_op
        ./execute_add_op
        ```

        如果有test pass，表明执行成功。
