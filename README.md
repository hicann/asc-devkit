<div align="center">

# Ascend C

<h4>基于C/C++构建多层级API，开放芯片完备编程能力，支撑实现极致性能</h4>

[![community](https://img.shields.io/badge/docs-community-brightgreen.svg?style=flat)](https://www.hiascend.com/document/redirect/CannCommunityOpdevAscendC)
[![repo](https://img.shields.io/badge/docs-repo-blue.svg?style=flat)](docs)
[![examples](https://img.shields.io/badge/examples-repo-orange.svg?style=flat)](examples)
[![asc-tools](https://img.shields.io/badge/asc--tools-repo-6f42c1.svg?style=flat)](https://gitcode.com/cann/asc-tools)
[![license](https://img.shields.io/badge/license-CANN_Open_2.0-lightgrey.svg)](LICENSE)
[![contributing](https://img.shields.io/badge/CONTRIBUTING-teal)](CONTRIBUTING.md)
[![SIG](https://img.shields.io/badge/SIG-ascendc-yellow)](https://gitcode.com/cann/community/tree/master/CANN/sigs/ascendc)

</div>

## 🔥Latest News
[2026/05] v9.1.0-beta.2 版本关键特性
### 🚀 关键特性
- Ascend C框架基础API支持NPU Check（[PR#1557](https://gitcode.com/cann/asc-devkit/pull/1557) [PR#1467](https://gitcode.com/cann/asc-devkit/pull/1467)），增强算子运行时校验能力。
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
- SIMT新增DCache访问优化样例（[PR#2453](https://gitcode.com/cann/asc-devkit/pull/2453)）、基于transpose的仿存合并和bank冲突样例（[PR#1753](https://gitcode.com/cann/asc-devkit/pull/1753)）、最佳实践样例：通过类型对齐提升搬运效率（[PR#2297](https://gitcode.com/cann/asc-devkit/pull/2297)）。
- SIMT新增功能特性样例：pytorch注册自定义算子（[PR#2769](https://gitcode.com/cann/asc-devkit/pull/2769)）、编译相关样例（动态、静态、分离编译等）（[PR#2356](https://gitcode.com/cann/asc-devkit/pull/2356)）、profiling样例（[PR#1989](https://gitcode.com/cann/asc-devkit/pull/1989)）、内存屏障特性样例（[PR#1923](https://gitcode.com/cann/asc-devkit/pull/1923)）、Warp类特性样例（[PR#2876](https://gitcode.com/cann/asc-devkit/pull/2876)）、simulator样例（[PR#2692](https://gitcode.com/cann/asc-devkit/pull/2692)）、kernel log样例（[PR#2131](https://gitcode.com/cann/asc-devkit/pull/2131)）。
- SIMT入门样例修改为gather（[PR#2405](https://gitcode.com/cann/asc-devkit/pull/2405)）。
- 新增Tensor API入门及最佳实践样例：Matmul入门、数据搬入搬出、搬出随路量化、MX FP4最佳实践（[PR#2553](https://gitcode.com/cann/asc-devkit/pull/2553)）。
### 📖 资料文档
- 新增矩阵计算概述和计算分形介绍的文档（[PR#2533](https://gitcode.com/cann/asc-devkit/pull/2533)）。
- 优化矢量计算API文档，补充指令约束等（[PR#2676](https://gitcode.com/cann/asc-devkit/pull/2676)）。
- 搭建VitePress文档站点，提供AscendC资料预览功能（[PR#2547](https://gitcode.com/cann/asc-devkit/pull/2547)）。
- 增加SIMD与SIMT混合编程性能优化概述（[PR#2736](https://gitcode.com/cann/asc-devkit/pull/2736)）。

有关所有历史版本及更新的详细信息，请参阅[CHANGELOG.md](./CHANGELOG.md)。

## 🚀概述

[Ascend C](https://www.hiascend.com/cann/ascend-c)是CANN（Compute Architecture for Neural Networks）面向昇腾AI处理器打造的专用算子开发编程语言，原生兼容C/C++标准规范。

作为覆盖全场景算子开发需求的编程语言，Ascend C一方面完整开放芯片底层可编程能力，支撑极致性能调优；另一方面通过分层分级的API设计体系，开发者可根据业务场景、技术储备与性能目标灵活选择开发接口，在开发效率与运行性能之间找到最优平衡点。

### 设计目标

Ascend C以「**兼容C/C++标准 · 释放极致算力**」为核心设计理念：在严格遵循C/C++语言规范的基础上做最小化语法扩展，让具备C/C++开发基础的开发者可低门槛平滑迁移至昇腾平台，自主释放芯片全部算力潜能。语言同时兼容指针式原生C开发范式与Tensor+Layout现代C++编程模式，在深度支撑算子定制优化的同时，实现与现有C/C++开发生态的无缝衔接，保障跨平台开发体验的一致性。

我们坚持两大核心设计原则：
- **没有银弹**：不同业务场景对性能、开发效率的诉求存在差异，不存在适配所有场景的最优单一接口，分层设计是兼顾效率与性能的核心路径；
- **渐进式成长**：入门开发者可从高层易用接口快速上手，完成算法验证与原型落地；资深开发者可向下深入底层接口，通过精细化调优充分释放硬件潜能。

### API分层体系

Ascend C遵循「标准C/C++语法、最小化扩展」的核心原则，构建了轻量化高性能基座。

| API层级 | 语言范式 | 核心特性 | 目标用户 | 核心价值 |
|---------|----------|----------|----------|----------|
| **Basic API** | C++ | 基于SIMD编程模型，采用无Layout约束的Tensor抽象编程；依托TPipe/TQue框架实现内存调度与执行同步的统一托管。 | 通用算子库开发者 | 通过框架自动化完成内存与同步管理，屏蔽底层硬件实现细节，有效降低开发复杂度，提升编程易用性与工程交付效率。 |
| **Tensor API** | C++ | 基于SIMD编程模型，依托Layout代数体系提供携带**Layout**语义的Tensor抽象；采用**arch/atom/algorithm**三层解耦的API架构设计。 | 高性能算子优化开发者 | 将张量与布局作为一等公民，内置Layout代数运算能力，实现零成本编译抽象；三层解耦设计达成关注点分离，天然具备跨架构可移植性。 |
| **SIMD C API** | C | 基于SIMD编程模型与**原生指针**编程范式，提供完整C语言级底层可编程能力；支持数组下标式内存访问，内存生命周期与执行同步完全由开发者自主管控。 | 极致性能调优开发者 | 完全契合原生C语言开发习惯，支持深度定制内存排布与同步策略；指令级透明映射实现零封装开销，全面开放底层硬件能力，支撑精细化性能调优与极致算力释放。 |
| **SIMT API** | C | 基于业界通用SIMT编程模型，以单线程为基本编程单元，原生支持离散不规则并行计算逻辑。 | 不规则场景高性能算子开发者 | 天然适配离散、不规则的并行计算场景，支持业界主流异构开发范式，有效降低跨技术栈迁移成本与学习门槛。 |

在底层编程接口之上，Ascend C还提供了算子模板库与高阶API组件，沉淀通用开发能力，进一步降低开发门槛，加速算法原型落地。

| 组件层级 | 目标用户 | 核心价值 |
|----------|----------|----------|
| **算子模板库（BLAZE/ATVOSS等）** | 算法开发人员 | 提供典型算子的高性能模板实现，支持基于模板定制化扩展，快速适配业务场景的高性能算力需求 |
| **高阶API** | 算法开发人员 | 封装通用单核计算算法原语，屏蔽底层硬件实现细节，支撑开发者快速搭建算法逻辑，高效完成功能验证与方案原型落地 |

此外，联合生态正持续建设Kernel编程C++标准库**asc-stl**、设备侧线性代数库**asc-mathdx**等基础组件，不断丰富Ascend C算子编程生态。

### 整体架构

Ascend C采用分层架构设计，自下而上构建完整的算子开发技术栈，整体逻辑架构如下：

<img src="docs/zh/guide/figures/architecture_ascendc.png" alt="Ascend C整体架构图"  width="1000px">

各层级能力定义与说明如下：
- **语言扩展层（SIMT API）**：面向多线程并行场景的原生C语言编程接口，支持业界通用的SIMT编程模型，保障跨技术栈开发体验的一致性，支撑离散、不规则计算场景下的高性能算子开发。
- **语言扩展层（SIMD C API）**：基于SIMD向量化编程模型、面向极致性能调优的C语言级底层编程接口，原生支持数组式内存分配与指针式计算原语，具备指令级零封装开销的硬件访问能力。Ascend 950PR/Ascend 950DT架构新增SIMD/SIMT混合编程模式支持，可覆盖多形态并行计算场景。
- **Tensor API**：面向高性能算子开发的C++ Tensor级核心编程接口。将内存布局（Layout）作为Tensor抽象的一等公民，原生支持携带Layout语义的Tensor对象；内置Layout代数运算能力，可自动化完成内存布局索引推导与布局变换管理，显著降低复杂内存排布场景的开发复杂度，提升代码可维护性与跨架构可移植性。
- **基础API（Basic API）**：基于单指令抽象的C++类库接口，以无Layout约束的Tensor对象为核心提供基础编程能力；依托TPipe/TQue框架统一托管内存调度与执行同步，屏蔽底层硬件实现细节。
- **高阶API（Adv API)**：对单核通用计算算法进行标准化抽象封装，提供开箱即用的公共算法实现，屏蔽底层硬件指令细节，支撑算法快速验证与工程原型落地。
- **算子模板库**：面向典型计算场景的高性能算子参考实现框架，基于模板化设计提供算子完整工程实现参考，沉淀Tiling调优与性能优化最佳实践，简化算子开发流程，支持用户自定义扩展。
- **生态扩展组件（开发中）**：
  - **asc-comm**：提供通信算子自主开发能力，支撑分布式场景下的算子定制开发；
  - **asc-stl**：提供设备侧常用C++标准库能力，深度适配Kernel编程范式与运行环境；
  - **asc-mathdx**：集成blasdx等设备侧线性代数计算库，支撑高性能数学运算与科学计算场景。
- **Python前端（PyAsc）**：基于Python语言封装芯片底层完备可编程能力，持续完善Layout体系下的Tensor编程能力，新增SIMT编程模型支持，实现通过Python接口开发高性能算子。

### 如何选择多层级API进行算子开发
- **基于C/C++语言开发**：详细请参考[Ascend C多级API选择指南](./docs/zh/asc_how_to_choose_api.md)
- **基于Python语言开发，支撑完备编程能力，实现极致性能**：推荐选用Ascend C Python前端[PyAsc](https://gitcode.com/cann/pyasc)
- **基于Python语言开发，快速开发验证，易用性优先**：推荐选用 [PyPTO](https://gitcode.com/cann/pypto)

## 🔍目录结构说明
本仓主要包含Ascend C编程API和必要的cmake编译脚本，是算子开发所需的核心模块，其目录结构如下：

```
├── cmake                               # Ascend C 构建源代码
├── docs                                # 项目文档介绍
├── examples                            # Ascend C API样例工程
├── impl                                # Ascend C API接口实现源代码
│   ├── adv_api                         # Ascend C 高阶API实现源代码
│   ├── aicpu_api                       # Ascend C AI CPU API实现源代码
│   ├── basic_api                       # Ascend C 基础API实现源代码
│   ├── c_api                           # Ascend C 语言扩展层C API实现源代码
│   ├── simt_api                        # Ascend C SIMT API实现源代码
│   ├── tensor_api                      # Ascend C TENSOR API实现源代码
│   └── utils                           # Ascend C 工具类实现源代码
├── include                             # Ascend C API接口声明源代码
│   ├── adv_api                         # Ascend C 高阶API声明源代码
│   ├── aicpu_api                       # Ascend C AI CPU API声明源代码
│   ├── basic_api                       # Ascend C 基础API声明源代码
│   ├── c_api                           # Ascend C 语言扩展层C API声明源代码
│   ├── simt_api                        # Ascend C SIMT API声明源代码
│   ├── tensor_api                      # Ascend C TENSOR API声明源代码
│   └── utils                           # Ascend C 工具类声明源代码
├── scripts                             # 打包相关脚本
├── tests                               # Ascend C API的UT用例
└── tools                               # Ascend C 工具源代码
```

## ⚡️快速入门

若您希望快速体验项目的构建和算子样例的执行，请访问如下文档获取简易教程。

- [编译构建](docs/zh/quick_start.md)：介绍搭建环境、编译执行、本地验证等操作。
- [样例执行](examples/README.md)：提供算子开发样例，介绍端到端执行样例的方式。

## 🧰clangd/IDE 支持

- 安装 clangd（推荐 15+，以Ubuntu操作系统为例）以及VSCode插件clangd

  ```bash
  sudo apt install -y clangd-15
  sudo update-alternatives --install /usr/bin/clangd clangd /usr/bin/clangd-15 100
  ```

- 配置本地VSCode的`settings.json`（示例）

  ```json
  {
    "clangd.path": "/usr/bin/clangd",
    "clangd.arguments": [
        "--background-index=0",
        "--clang-tidy=0"
    ],
    "C_Cpp.intelliSenseEngine": "disabled"
  }
  ```

- 在项目根目录下配置 `.clangd`（示例）完整 `.clangd`文件在本目录下给出，其中涉及 CANN 头文件目录需对应实际安装位置，`.clangd`中默认为`/usr/local/Ascend`.

  如果 CANN 安装在非默认路径，或需要切换 NPU 架构宏，可先 source CANN `set_env.sh`，再通过脚本基于 `.clangd.in` 生成本地配置：

  ```bash
  source /path/to/cann/set_env.sh
  python3 scripts/setup_clangd.py --npu-arch 2201 --output .clangd.local
  ```

  `ASCEND_HOME_PATH` 由 `set_env.sh` 设置，脚本会使用该环境变量生成真实 CANN 路径；如果未设置，脚本会提示 source `set_env.sh` 后重试。生成文件默认为 `.clangd.local`，不会覆盖仓库内置 `.clangd`；如需让项目级 clangd 配置生效，可按需复制为 `.clangd` 后重启 clangd。

  ```yaml
  CompileFlags:
    Add:
      - "-std=c++17"
      - "-stdlib=libstdc++"
      - "-D__NPU_ARCH__=2201"
      - "-DASCENDC_CPU_DEBUG=1"
      ...

  ---
  If:
    PathMatch: ".*\\.(asc|aicpu)$"
  CompileFlags:
    CompilationDatabase: None
    Add:
      - "-x"
      - "c++"
  Diagnostics:
    Suppress:
      - "attributes_not_allowed"
      - "decomp_decl_template"
      - "ignored-attributes"
      - "unknown_typename"
      - "undeclared_var_use"
      - "invalid_token_after_toplevel_declarator"
      - "missing_type_specifier"
      - "typename_nested_not_found"
      - "redefinition"
  ```

- 重启clangd（VS Code: Command Palette -> "Clangd: Restart language server"）

- 推荐安装VS Code扩展Ascend C Toolkit，用于在VS Code中完成Ascend C算子工程的编辑、编译、运行、调试、异常检测与性能调优等流程。打开本仓工作区时，VS Code会根据`.vscode/extensions.json`提示安装推荐扩展。

- Ascend C Toolkit近期更新重点增强Atlas A2系列产品和Atlas A3系列产品的环境解析、标准自定义算子创建、SoC设置、仿真分析、NPU调试、设置迁移与稳定性；完整使用说明和问题反馈请参考[CANN讨论区使用指导](https://gitcode.com/org/cann/discussions/54)。

- 💡 关于 ASC 语言语法高亮、代码跳转的支持，如有任何建议或改进意见，欢迎社区开发者积极反馈！

## 📖相关资源

- **编程指南**
  | 文档  |  说明   |
  |---------|--------|
  |[Ascend C 编程指南](https://hiascend.com/document/redirect/CannCommunityOpdevAscendC)|基于昇腾AI硬件，使用Ascend C编写算子程序，开发自定义算子。|
  |[Ascend C 实践参考](https://hiascend.com/document/redirect/CannCommunityAscendCBestPractice) | 基于已完成开发的Ascend C算子，介绍如何进一步优化算子性能。 |
  |[Ascend C API列表](https://hiascend.com/document/redirect/CannCommunityAscendCApi)| Ascend C SIMD&SIMT API，包括语言扩展层C API、C++类库基础API和高阶API|
  |[Ascend C 样例](./examples)| Ascend C API关键特性介绍样例，包括AICore SIMD&SIMT、AICPU等|
  |[Ascend C 编程指南（鸿蒙）](https://gitcode.com/cann/cann-recipes-harmony-infer/blob/master/docs/ascendc_develop_guide.md)|基于麒麟AI硬件，使用Ascend C编写算子程序，开发自定义算子。|

- **贡献指南**
  | 文档  |  说明   |
  |---------|--------|
  |[CANN 社区贡献指南](https://gitcode.com/cann/community)| CANN社区Issue、PR等通用处理流程|
  |[ASC-DevKit贡献指南](./CONTRIBUTING.md) | Ascend C API、资料与样例等贡献指南|

- **其他**
  | 文档  |  说明   |
  |---------|--------|
  |[Ascend C Meetup材料](https://gitcode.com/cann/community/tree/master/events/meetup/slides/sig-ascendc)|Ascend C对外宣传的PPT材料，包括950新增特性等|
  |[Ascend C Wiki](https://gitcode.com/cann/asc-devkit/wiki)|Ascend C技术宣传文章等|
  |[CANN-Learning-Hub](https://gitcode.com/cann/cann-learning-hub/tree/master/tutorials/ascendc_operator_development)|Ascend C算子开发在线全流程教程|
  |[Ascend C Ops Samples](https://gitcode.com/cann/cann-samples)|Ascend C 算子如何逐步实现高性能样例仓|

## 📌相关规划

- [Ascend C Development Roadmap (2026 Q2)](https://gitcode.com/cann/asc-devkit/issues/316)；
- [Ascend C Development Roadmap (2026 Q3)](https://gitcode.com/cann/asc-devkit/issues/938)；

## 📝相关信息

- [贡献指南](CONTRIBUTING.md)
- [安全声明](SECURITY.md)
- [许可证](LICENSE)
