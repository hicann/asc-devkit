# CHANGELOG

## v9.1.0-beta.2
发布日期：2026/05/31
| CANN版本 | 版本源码标签   |
|--|--|
| [CANN 9.1.0-beta.2](https://www.hiascend.com/developer/download/community/result?module=cann&cann=9.1.0-beta.2) | [v9.1.0-beta.2](https://gitcode.com/cann/asc-devkit/tags/v9.1.0-beta.2) |

### 🚀 关键特性
- AscendC框架基础API支持NPU Check（[PR#1557](https://gitcode.com/cann/asc-devkit/pull/1557) [PR#1467](https://gitcode.com/cann/asc-devkit/pull/1467)），增强算子运行时校验能力。
- SIMD VF内支持printf和reg dump打印（[PR#1605](https://gitcode.com/cann/asc-devkit/pull/1605)），提供调试打印和寄存器数据dump能力。
- A5支持L1 Tensor数据的DumpTensor（[PR#2175](https://gitcode.com/cann/asc-devkit/pull/2175)），扩展L1层数据调试支持。
- 编译工程CMakeModule支持CMAKE<LANG>编译选项（[PR#2055](https://gitcode.com/cann/asc-devkit/pull/2055)）；新增optype_collector工具，支持检查optype重名（[PR#285](https://gitcode.com/cann/asc-tools/pull/285)）。
- 基础API支持ctrl中的功能行为（饱和溢出管理）（[PR#2077](https://gitcode.com/cann/asc-devkit/pull/2077)）。
- SIMT编程新增ld/st接口（[PR#2058](https://gitcode.com/cann/asc-devkit/pull/2058)）和AddrSpace类接口（[PR#1597](https://gitcode.com/cann/asc-devkit/pull/1597)），丰富SIMT内存访问编程能力。

### 🎯 样例更新
- 最佳实践样例开发：matmul+gelu融合、datacopy优化、bank冲突优化、group_matmul量化组矩阵乘、simt&simd高性能编程（[PR#1814](https://gitcode.com/cann/asc-devkit/pull/1814) [PR#2137](https://gitcode.com/cann/asc-devkit/pull/2137) [PR#2141](https://gitcode.com/cann/asc-devkit/pull/2141) [PR#2166](https://gitcode.com/cann/asc-devkit/pull/2166) [PR#2363](https://gitcode.com/cann/asc-devkit/pull/2363)）。
- Ascend 950新特性补充及兼容性样例整改：loopmode数据搬运、interleave矢量计算、datacopy_gm2l1、loadmx（Load2DMX）、mmad_mx、data_copy_pad等（[PR#2336](https://gitcode.com/cann/asc-devkit/pull/2336) [PR#1899](https://gitcode.com/cann/asc-devkit/pull/1899) [PR#2124](https://gitcode.com/cann/asc-devkit/pull/2124)）。
- RegBase新增基础样例：基础算术、数据类型转换、归约、比较、索引等样例（[PR#1459](https://gitcode.com/cann/asc-devkit/pull/1459) [PR#1575](https://gitcode.com/cann/asc-devkit/pull/1575) [PR#2024](https://gitcode.com/cann/asc-devkit/pull/2024)）。
- 新增SIMD VF print样例和dump样例（[PR#2558](https://gitcode.com/cann/asc-devkit/pull/2558)）。
- SIMT新增DCache访问优化样例（[PR#2453](https://gitcode.com/cann/asc-devkit/pull/2453)）、基于transpose的访存合并和bank冲突样例（[PR#1753](https://gitcode.com/cann/asc-devkit/pull/1753)）、最佳实践样例：通过类型对齐提升搬运效率（[PR#2297](https://gitcode.com/cann/asc-devkit/pull/2297)）。
- SIMT新增功能特性样例：pytorch注册自定义算子（[PR#2769](https://gitcode.com/cann/asc-devkit/pull/2769)）、编译相关样例（动态、静态、分离编译等）（[PR#2356](https://gitcode.com/cann/asc-devkit/pull/2356)）、profiling样例（[PR#1989](https://gitcode.com/cann/asc-devkit/pull/1989)）、内存屏障特性样例（[PR#1923](https://gitcode.com/cann/asc-devkit/pull/1923)）、Warp类特性样例（[PR#2876](https://gitcode.com/cann/asc-devkit/pull/2876)）、simulator样例（[PR#2692](https://gitcode.com/cann/asc-devkit/pull/2692)）、kernel log样例（[PR#2131](https://gitcode.com/cann/asc-devkit/pull/2131)）。
- SIMT入门样例修改为gather（[PR#2405](https://gitcode.com/cann/asc-devkit/pull/2405)）。
- 新增Tensor API入门及最佳实践样例：Matmul入门、数据搬入搬出、搬出随路量化、MX FP4最佳实践（[PR#2553](https://gitcode.com/cann/asc-devkit/pull/2553)）。

### 📖 资料文档
- 新增矩阵计算概述和计算分形介绍的文档（[PR#2533](https://gitcode.com/cann/asc-devkit/pull/2533)）。
- 优化矢量计算API文档，补充指令约束等（[PR#2676](https://gitcode.com/cann/asc-devkit/pull/2676)）。
- 搭建VitePress文档站点，提供AscendC资料预览功能（[PR#2547](https://gitcode.com/cann/asc-devkit/pull/2547)）。
- 增加SIMD与SIMT混合编程性能优化概述（[PR#2736](https://gitcode.com/cann/asc-devkit/pull/2736)）。

---

## v9.1.0-beta.1
发布日期：2026/04/30
| CANN版本 | 版本源码标签   |
|--|--|
| [CANN 9.1.0-beta.1](https://www.hiascend.com/developer/download/community/result?module=cann&cann=9.1.0-beta.1) | [v9.1.0-beta.1](https://gitcode.com/cann/asc-devkit/tags/v9.1.0-beta.1) |

### 🚀 关键特性
- Aclrtc支持global函数为模板参数（[PR#1276](https://gitcode.com/cann/asc-devkit/pull/1276)），融合编译支持msobjdump解析（[PR#235](https://gitcode.com/cann/asc-tools/pull/235)），提升编译工程灵活性和调试能力。
- Tensor API分支代码合入主线（[PR#1770](https://gitcode.com/cann/asc-devkit/pull/1770)），正式提供Tensor编程支持。
- C-API新增支持在UB和寄存器之间搬运int4x2_t类型的数据（[PR#1404](https://gitcode.com/cann/asc-devkit/pull/1404) [PR#1504](https://gitcode.com/cann/asc-devkit/pull/1504)），扩展低比特数据搬运能力。
- SIMT API新增DCCI接口（[PR#1510](https://gitcode.com/cann/asc-devkit/pull/1510)）、浮点比较接口（[PR#1693](https://gitcode.com/cann/asc-devkit/pull/1693)）及浮点运算指令（[PR#1774](https://gitcode.com/cann/asc-devkit/pull/1774)），丰富SIMT编程能力。

### 🎯 样例更新
- 存量样例整改：涉及基础、高阶的cube、vector、数据搬运等API样例整改（[PR#1200](https://gitcode.com/cann/asc-devkit/pull/1200) [PR#1429](https://gitcode.com/cann/asc-devkit/pull/1429) [PR#1454](https://gitcode.com/cann/asc-devkit/pull/1454) [PR#1311](https://gitcode.com/cann/asc-devkit/pull/1311) [PR#1563](https://gitcode.com/cann/asc-devkit/pull/1563)）。
- 新增静态Tensor最佳实践：add高性能样例（[PR#1262](https://gitcode.com/cann/asc-devkit/pull/1262)）。
- 新增低比特cube最佳实践：matmul的高阶、基础API样例，MXFP4基础API样例（[PR#1535](https://gitcode.com/cann/asc-devkit/pull/1535) [PR#1648](https://gitcode.com/cann/asc-devkit/pull/1648) [PR#1788](https://gitcode.com/cann/asc-devkit/pull/1788)）。
- 新增自定义算子工程多vendors编译样例（[PR#1466](https://gitcode.com/cann/asc-devkit/pull/1466)）。
- 新增atomic类特性样例（[PR#1688](https://gitcode.com/cann/asc-devkit/pull/1688)）。

### 📖 资料文档
- 新增C-API API手册内容：补充UB与寄存器的搬运接口资料（[PR#1404](https://gitcode.com/cann/asc-devkit/pull/1404) [PR#1504](https://gitcode.com/cann/asc-devkit/pull/1504)）。
- 优化入门教程及SIMT编程模型（[PR#1487](https://gitcode.com/cann/asc-devkit/pull/1487)）。
- 增加CANN包安装及配套说明（[PR#1451](https://gitcode.com/cann/asc-devkit/pull/1451) [PR#1514](https://gitcode.com/cann/asc-devkit/pull/1514)）。
- 新增AscendC入门教程、编程指南、兼容性迁移指南、算子实践参考文档（[PR#1438](https://gitcode.com/cann/asc-devkit/pull/1438)）。

---

## v9.0.0-beta.2
发布日期：2026/03/31
| CANN版本 | 版本源码标签   |
|--|--|
| [CANN 9.0.0-beta.2](https://www.hiascend.com/developer/download/community/result?module=cann&cann=9.0.0-beta.2) | [v9.0.0-beta.2](https://gitcode.com/cann/asc-devkit/tags/v9.0.0-beta.2) |

### 🚀 关键特性
- Ascend 950PR支持SIMD编程模式，提供200+ [API接口](https://gitcode.com/cann/asc-devkit/tree/9.0.0-beta.2/impl/basic_api/dav_c310)跨代兼容能力，可实现Atlas A2系列产品和Atlas A3系列产品算子平滑迁移。
- Ascend 950PR新增基于Reg的编程方式，提供Reg数据搬运、基础算术、规约计算、同步控制等90+ [Reg编程接口](https://gitcode.com/cann/asc-devkit/tree/9.0.0-beta.2/impl/basic_api/reg_compute/dav_c310)。
- Atlas A2系列产品、Atlas A3系列产品、Ascend 950PR支持[语言扩展层纯C接口](https://gitcode.com/cann/asc-devkit/tree/9.0.0-beta.2/include/c_api)，支持数组式内存分配与指针型计算接口，提供原生纯 C 编程体验。
- Ascend 950PR支持SIMD与SIMT混合编程，提供约700个[SIMT API接口](https://gitcode.com/cann/asc-devkit/tree/9.0.0-beta.2/include/simt_api)，包含warp、atomic、基本数学计算、类型转换等基础接口。
- Ascend 950PR支持通信高阶API的CCU通信接口，提供基于CCU的[Allreduce，Allgather，Reducescatter，AlltoAll等主流通信原语](https://gitcode.com/cann/asc-devkit/tree/9.0.0-beta.2/impl/adv_api/detail/hccl/impl/platform_v310)；Matmul高阶API新增支持[MXFP4/8低比特数据类型的矩阵运算](https://gitcode.com/cann/asc-devkit/blob/9.0.0-beta.2/impl/adv_api/detail/matmul/mx_matmul_impl.h)，实现内存占用减半、算力吞吐倍增。
- Ascend 950PR新增及兼容支持样例共计约260个，包含SIMT样例、SIMD样例（框架类、基础API、高阶API、最佳实践等），并按照编程模型和样例类别对[样例目录结构进行调整](https://gitcode.com/cann/asc-devkit/pull/1223)，提升样例目录结构的易读性。
- 融合编译与<<<>>>调用方式支持[CPU模式](https://gitcode.com/cann/asc-tools/pull/138)以及[SIM仿真模式](https://gitcode.com/cann/asc-devkit/blob/9.0.0-beta.2/cmake/asc/asc_modules/CMakeASCInformation.cmake)。
### 📖 资料文档
- 新增90+ [Reg编程接口API](https://gitcode.com/cann/asc-devkit/blob/9.0.0-beta.2/docs/api/context/Reg%E7%9F%A2%E9%87%8F%E8%AE%A1%E7%AE%97.md)资料，Reg矢量计算API是面向RegBase架构开发的API，用户可通过该API直接对芯片中涉及Vector计算的寄存器进行操作，实现更大的灵活性和更好的性能。
- 新增SIMT[快速入门](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_map_10_0022.html)、[编程模型](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_10_10064.html)和[算子实现](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlasascendc_api_07_10293.html)介绍。
- 新增SIMD与SIMT[混合编程模型](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_10_10052.html)、[算子实现](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_10_10039.html)、[性能优化](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_best_practices_10_10029.html)介绍。
- 新增[SIMT API](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/API/ascendcopapi/atlasascendc_api_07_0427.html)资料章节。
- 新增[兼容性迁移指南](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_compatibility_10_00001.html)（220x架构版本迁移到351x架构版本）。
- 昇腾社区中，Ascend C算子开发新增[可视化专区](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_map_10_0017.html)，通过视频呈现Cube和Vector算子的执行过程。
---

## v9.0.0-beta.1
发布日期：2026/02/25
| CANN版本 | 版本源码标签   |
|--|--|
| [CANN 9.0.0-beta.1](https://www.hiascend.com/developer/download/community/result?module=cann&cann=9.0.0-beta.1) | [v9.0.0-beta.1](https://gitcode.com/cann/asc-devkit/tags/v9.0.0-beta.1) |

### 🚀 关键特性

- 完成[Gitee样例仓](https://gitee.com/ascend/samples/tree/master/operator/ascendc)AscendC样例及[AscendC高阶API仓](https://gitee.com/ascend/ascendc-api-adv/tree/master/examples)样例到[asc-devkit仓](https://gitcode.com/cann/asc-devkit/tree/9.0.0-beta.1/examples)的迁移，并统一使用<<<>>>调用方式；
- 融合编译与<<<>>>调用方式支持[CPU模式](https://gitcode.com/cann/asc-tools/pull/138)以及[SIM仿真模式](https://gitcode.com/cann/asc-devkit/blob/9.0.0-beta.1/cmake/asc/asc_modules/CMakeASCInformation.cmake)。
- Ascend C项目新增对Kirin X90和Kirin 9030处理器的支持。鸿蒙开发者基于Ascend C的实践案例可参考：[cann-recipes-harmony-infer](https://gitcode.com/cann/cann-recipes-harmony-infer)。
---

## v8.5.0
发布日期：2026/01/23
| CANN版本 | 版本源码标签   |
|--|--|
| [CANN 8.5.0](https://www.hiascend.com/developer/download/community/result?module=cann&cann=8.5.0) | [v8.5.0](https://gitcode.com/cann/asc-devkit/tags/v8.5.0) |

### 🚀 关键特性

基于Atlas A3 训练系列产品/Atlas A3推理系列产品、Atlas A2训练系列产品/Atlas A2推理系列产品全面开源开放，包含以下新特性：
- 实现分仓分包，支持分包独立安装部署，包括Ascend C算子开发所需的核心仓asc-devkit、调试工具仓[asc-tools](https://gitcode.com/cann/asc-tools)、Vector算子模板库仓[atvc](https://gitcode.com/cann/atvc)和[atvoss](https://gitcode.com/cann/atvoss)、Python前端仓[pyasc](https://gitcode.com/cann/pyasc)。
- 编程API能力扩展
  - 新增语言扩展层C API，提供与业界相似的编程体验。
  - 基础API新增LocalMemAllocator内存分配接口。
- 全面支持异构编译与<<<>>> 直调，通过文件后缀名“.asc”或编译选项“-x asc”使能异构编译。
- 算子编译CMake接口标准化，提供Cmake module接口，支持不同编译场景。
- 支持CPU&NPU孪生调试的能力一致性，一套代码同时支持CPU和NPU调试。
- 编程指南全面优化。
- 新增算子样例。
