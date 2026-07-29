# 贡献指南

本项目欢迎广大开发者体验并参与贡献，在参与社区贡献之前。请参见[cann-community](https://gitcode.com/cann/community)了解行为准则，进行CLA协议签署，了解源码仓的贡献流程。

开发者准备本地代码与提交PR时需要重点关注如下几点：

1. 提交PR时，请按照PR模板仔细填写本次PR的业务背景、目的、方案等信息。
2. 若您的修改不是简单的bug修复，而是涉及到新增特性、新增接口、新增配置参数或者修改代码流程等，请务必先通过Issue进行方案讨论，以避免您的代码被拒绝合入。若您不确定本次修改是否可被归为“简单的bug修复”，亦可通过提交Issue进行方案讨论。

## API开发指南

本项目支持三种类型的API开发，每类API都有专门的贡献指南：

### 1. 高阶API

高阶API基于单核对常见算法进行抽象和封装，实现了一些常用的计算算法，旨在提高编程开发效率。高阶API一般通过调用多种基础API实现。高阶API包括数学计算、矩阵计算、激活函数等API。

- **适用场景**：常见算法的抽象和封装，提高算子开发效率
- **贡献指南**：[docs/asc_adv_api_contributing.md](./docs/zh/asc_adv_api_contributing.md)

### 2. 基础API

基础API实现对硬件能力的抽象，开放芯片的能力，保证完备性和兼容性。

- **适用场景**：使用C++语言编程风格的场景
- **贡献指南**：[docs/asc_basic_api_contributing.md](./docs/zh/asc_basic_api_contributing.md)

### 3. C API

C API提供纯C风格的接口，符合C语言算子开发习惯，开放芯片完备编程能力，支持数组分配内存，一般基于指针编程。提供与业界类似的编程体验。

- **适用场景**：使用C语言编程风格的场景
- **贡献指南**：[docs/asc_c_api_contributing.md](./docs/zh/asc_c_api_contributing.md)


## 资料贡献指南

本项目包含Ascend C的五份核心资料文档（入门教程、编程指南、API参考手册、算子实践参考、跨代迁移兼容性指南），开发者可以对资料内容进行纠错、补充和贡献。

- **资料编写规范**：[docs/asc_doc_contributing.md](./docs/zh/asc_doc_contributing.md) — 资料贡献场景、各文档编写规范、PR自检清单
- **资料设计原则**：[docs/asc_doc_design_rules.md](./docs/zh/asc_doc_design_rules.md) — 可获取性、可读性、完备性三维度设计规范

## 开发者贡献场景主要包括

- Bug修复

  如果您在本项目中发现了某些Bug，希望对其进行修复，欢迎您新建Issue进行反馈和跟踪处理。

  您可以按照[提交Issue/处理Issue任务](https://gitcode.com/cann/community#提交Issue处理Issue任务)指引新建 `Bug-Report|缺陷反馈` 类Issue对Bug进行描述，然后在评论框中输入“/assign”或“/assign @yourself”，将该Issue分配给您进行处理。
  
- 代码优化

  如果您对本项目中某些API实现有泛化性增强/性能优化思路，希望着手实现这些优化点，欢迎您对API进行优化贡献。

  您可以按照[提交Issue/处理Issue任务](https://gitcode.com/cann/community#提交Issue处理Issue任务)指引新建 `Requirement|需求建议` 类Issue对优化点进行说明，并提供您的设计方案，
  然后在评论框中输入“/assign”或“/assign @yourself”，将该Issue分配给您进行跟踪优化。

- 文档纠错

  如果您在本项目中发现某些文档描述错误，欢迎您新建Issue进行反馈和修复。

  您可以按照[提交Issue/处理Issue任务](https://gitcode.com/cann/community#提交Issue处理Issue任务)指引新建 `Documentation|文档反馈` 类Issue指出对应文档的问题，然后在评论框中输入“/assign”或“/assign @yourself”，将该Issue分配给您纠正对应文档描述。
  
- 帮助解决他人Issue

  如果社区中他人遇到的问题您有合适的解决方法，欢迎您在Issue中发表评论交流，帮助他人解决问题和痛点，共同优化易用性。

  如果对应Issue需要进行代码修改，您可以在Issue评论框中输入“/assign”或“/assign @yourself”，将该Issue分配给您，跟踪协助解决问题。

## 代码规范

本项目有完整的代码贡献规范体系，请在开发前阅读相关指南。完整索引见 [docs/zh/contributing/README.md](./docs/zh/contributing/README.md)。

| 文档 | 内容 |
|------|------|
| [code-style.md](./docs/zh/contributing/code-style.md) | C++ 命名、include、header guard、namespace、宏、constexpr、格式化、代码注释、弃用标注 |
| [api-design.md](./docs/zh/contributing/api-design.md) | API 接口设计、模板参数、`__aicore__` 使用、Tiling 接口 |
| [directory-structure.md](./docs/zh/contributing/directory-structure.md) | 目录结构、文件命名后缀、架构目录、新增 API 操作清单 |
| [testing.md](./docs/zh/contributing/testing.md) | 测试框架、文件命名、UT 编写、Header Checker、覆盖率要求 |
