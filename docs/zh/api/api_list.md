# Ascend C API列表

Ascend C提供了一套层次化的API体系，涵盖了从底层C扩展到高阶C++类库的完整能力。它支持开发者以标准C/C++语法为基础，在AI Core（SIMD/SIMT）及AI CPU等多种编程模型下，灵活实现精细化的内存管理与高效的矢量/矩阵运算。

## API分类总览

下表展示了Ascend C API的总体分类，帮助开发者根据编程模型和功能需求快速定位所需API。

**表 1**  Ascend C API分类

| API一级分类 | API二级分类 | 分类说明 |
| --- | --- | --- |
| [SIMD API](SIMD-API/basic_api/basic_api_list.md) | [基础API](SIMD-API/basic_api/basic_api.md) | 实现对硬件能力的抽象，开放芯片的能力，保证完备性和兼容性。标注为ISASI（Instruction Set Architecture Special Interface，硬件体系结构相关的接口）类别的API，不能保证跨硬件版本兼容。 |
| [SIMD API](SIMD-API/SIMD-API.md) | [Tensor API](SIMD-API/tensor_api/tensor_api_list.md) | 通过Tensor及其Layout描述数据，并提供当前支持的数据搬运与矩阵计算能力。未覆盖的硬件能力可配合C API使用。 |
| [SIMD API](SIMD-API/c_api/c_api.md) | [C API](SIMD-API/c_api/c_api.md) | 纯C接口，开放芯片完备编程能力，支持数组分配内存，一般基于指针编程，提供与业界一致的C语言编程体验。 |
| [SIMD API](SIMD-API/adv_api/adv_api_list.md) | [高阶API](SIMD-API/adv_api/adv_api.md) | 实现一些常用的计算算法，用于提高编程开发效率，通常会调用多种基础API实现。高阶API包括数学库、Matmul、Softmax等API。高阶API可以保证兼容性。 |
| [SIMT API](SIMT-API/overview.md) | - | 对标业界，提供单指令多线程API。以单条指令多个线程的形式来实现并行计算。SIMT编程主要用于向量计算，特别适合处理离散访问、复杂控制逻辑等场景。SIMT API支持两种编程模型：SIMT编程、SIMD与SIMT混合编程，具体支持的API请分别参见[SIMT编程API列表](SIMT-API/SIMT_programming_intro/api_list.md)、[SIMD与SIMT混合编程API列表](SIMT-API/SIMD_SIMT_hybrid_programming_intro/api_list.md)。 |
| [AI CPU API](AI-CPU-API/ai_cpu_api_list.md) | - | 通常作为上述API的补充，主要承担非矩阵类、逻辑比较复杂的分支密集型计算。 |
| [Utils API](Utils-API/utils_api_list.md) | - | 丰富的通用工具类，涵盖标准库（目前仅支持SIMD）、平台信息获取、运行时编译及日志输出等功能，支持开发者高效实现算子开发与性能优化。 |

## 调用接口依赖的头文件和库文件说明

安装固件、驱动及CANN软件包后，编译、运行应用程序时才能引用到Ascend C接口的头文件、库文件。

您可以根据实际使用的Ascend C接口来include依赖的文件，各头文件的用途如下表所示。

**表 2**  SIMT编程模型头文件列表

| 头文件 | 说明 | 对应的库文件 |
| --- | --- | --- |
| simt/asc_simt.h | 提供SIMT API对外接口，若计算只需要特定的数据类型如fp8，可只包含simt_api/asc_fp8.h。 | 不涉及 |

**关于SIMT编程头文件包含的更多细节请参考[SIMT-API](SIMT-API/overview.md)。**

**表 3**  SIMD基础编程能力头文件列表

| API能力层级| 开发场景 | 头文件 | 说明 | 对应的库文件 |
| --- | --- | --- | --- | --- |
| 基于C语言指针编程 | 开发任意算子（含融合算子） | c_api/asc_simd.h | 提供全量SIMD C API对外接口。 | 不涉及 |
| 基于C++ Tensor编程 | 开发数据搬运、矩阵计算及相关融合算子 | tensor_api/tensor.h | 提供当前支持的SIMD C++ Tensor API对外接口，未覆盖的能力可配合C API使用。 | 不涉及 |
| 基于TPipe/TQue框架编程 | 开发任意算子（含融合算子） | basic_api/kernel_basic_intf.h | 提供全量SIMD基于TPipe/TQue框架编程对外接口。 | 不涉及 |
| 基于TPipe/TQue框架编程 | 只开发矢量算子 | basic_api/kernel_vec_intf.h | 提供矢量计算依赖的SIMD基于TPipe/TQue框架编程对外接口。 | 不涉及 |
| 基于TPipe/TQue框架编程 | 只开发矩阵算子 | basic_api/kernel_cube_intf.h | 提供矩阵计算依赖的SIMD基于TPipe/TQue框架编程对外接口。 | 不涉及 |

**表 4**  SIMD高阶API头文件列表

| 头文件 | 说明 | 对应的库文件 |
| --- | --- | --- |
| adv_api/kernel_api.h | 提供SIMD高阶API Device侧对外接口。仅支持基于TPipe/TQue框架编程方式下调用。 | 不涉及 |
| adv_api/tiling_api.h | 提供SIMD高阶API Host侧对外接口。仅支持基于TPipe/TQue框架编程方式下调用。 | libtiling_api.a |
