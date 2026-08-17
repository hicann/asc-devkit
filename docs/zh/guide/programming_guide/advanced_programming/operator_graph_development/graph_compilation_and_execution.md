# 图编译和图执行<a name="ZH-CN_TOPIC_0000001920158120"></a>

本节通过单算子模型执行的样例来介绍图模式下图编译和图执行流程。单算子模型执行是指基于图IR执行算子，先编译算子（例如，使用ATC工具将Ascend IR定义的单算子描述文件编译成算子om模型文件），再调用acl接口加载算子模型，最后调用acl接口执行算子。

## 环境要求<a name="zh-cn_topic_0000001541959061_section19582183344920"></a>

-   已参考[环境准备](../../../getting_started/environment_setup.md)，完成CANN驱动和软件的安装，配置CANN软件所需基本环境变量。

    安装CANN软件后，使用CANN运行用户进行编译、运行时，需要以CANN运行用户登录环境，执行`source ${INSTALL_DIR}/set_env.sh`命令设置环境变量。`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。以root用户安装为例，安装后文件默认存储路径为：/usr/local/Ascend/cann。

-   已参考[Aclnn算子工程化开发](../aclnn_operator_development/overview.md)完成算子的开发和部署。

## 准备验证代码工程<a name="zh-cn_topic_0000001541959061_section2021523012501"></a>

代码工程目录结构如下，您可以单击[LINK](../../../../../../examples/01_simd_cpp_api/02_features/99_acl_based/01_acl_invocation/aclop_invocation)，获取样例工程的完整样例：

```
├── aclop_invocation
│   ├── add_custom.json                   // 算子描述文件，用于构造单算子模型文件
│   ├── CMakeLists.txt
│   └── main.cpp                          // 将单算子编译为om文件并加载om文件执行
```

## 生成单算子离线模型文件<a name="section17164152011141"></a>

1.  构造静态shape单算子描述文件add\_custom\_static\_shape.json，描述算子的输入、输出及属性等信息。

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

2.  使用ATC工具，将该算子描述文件编译成单算子模型文件（\*.om文件）

    ATC工具的命令示例如下：

    ```
    atc --singleop=op_verify/run/out/test_data/config/add_custom_static_shape.json --output=. --soc_version=<soc_version>
    ```

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

    2.  进入build目录，执行cmake编译命令，生成编译文件

        命令示例如下所示：

        **cd build**

        **cmake ../src -DCMAKE\_SKIP\_RPATH=TRUE**

    3.  执行如下命令，生成可执行文件。

        **make**

        会在工程目录的output目录下生成可执行文件**execute\_add\_op**。

3.  执行单算子
    1.  以运行用户（例如HwHiAiUser）拷贝开发环境中样例工程output下的**execute\_add\_op**到运行环境任一目录。

        说明：若您的开发环境即为运行环境，此拷贝操作可跳过。

    2.  在运行环境中，执行**execute\_add\_op**文件，验证单算子模型文件。

        **chmod +x execute\_add\_op**

        **./execute\_add\_op**

        如果有test pass，表明执行成功。
