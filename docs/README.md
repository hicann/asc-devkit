# 项目文档

## 目录说明
关键目录结构如下：
```
├── zh                             # 中文文档目录
│   ├── api                        # API文档目录
│   ├── contributing               # 代码贡献规范目录（代码风格、接口设计、目录结构、测试等）
│   ├── figures                    # 图片目录
│   ├── guide                      # Ascend C 算子开发指南
│   ├── vitepress                      # 文档构建脚本目录
│   ├── asc_adv_api_contributing.md    # Ascend C 高阶API贡献指南
│   ├── asc_basic_api_contributing.md  # Ascend C 基础API贡献指南
│   ├── asc_c_api_contributing.md      # Ascend C C API贡献指南
│   ├── asc_doc_contributing.md        # Ascend C 资料贡献指南
│   ├── asc_doc_design_rules.md        # Ascend C 资料设计规范
│   ├── asc_how_to_choose_api.md       # Ascend C多层级编程接口选择参考文档
│   └── quick_start.md                 # 快速入门文档
├── en                                 # 英文文档目录
│   ├── contributing                   # 代码贡献规范目录（代码风格、接口设计、目录结构、测试等）
│   ├── asc_adv_api_contributing.md    # Ascend C 高阶API贡献指南
│   ├── asc_basic_api_contributing.md  # Ascend C 基础API贡献指南
│   ├── asc_c_api_contributing.md      # Ascend C C API贡献指南
│   ├── asc_doc_contributing.md        # Ascend C 资料贡献指南
│   ├── asc_doc_design_rules.md        # Ascend C 资料设计规范
│   ├── asc_how_to_choose_api.md       # Ascend C多层级编程接口选择参考文档
│   └── quick_start.md                 # 快速入门文档
├── README_en.md
└── README.md
```


## 文档说明
为方便开发者快速熟悉本项目，可按需获取对应文档，文档内容包括：

| 文档  | 面向对象  | 内容介绍  |
|---|---|---|
| [API列表](./zh/api/README.md) | 基于Ascend C开源仓，开发定制化API或算子的开发者。 | 介绍项目包含的所有API。  |
| [高阶API贡献指南](./zh/asc_adv_api_contributing.md) | 基于Ascend C开源仓，开发定制化API的开发者。 | 介绍如何扩展或开发Ascend C高阶API。高阶API基于单核对常见算法进行抽象和封装，实现常用的计算算法，旨在提高算子开发效率。  |
| [基础API贡献指南](./zh/asc_basic_api_contributing.md) | 基于Ascend C开源仓，开发定制化API的开发者。 | 介绍如何扩展或开发Ascend C基础API。基础API实现对硬件能力的抽象，开放芯片的能力，保证完备性和兼容性。  |
| [C API贡献指南](./zh/asc_c_api_contributing.md) | 基于Ascend C开源仓，开发定制化API的开发者。 | 介绍如何扩展或开发Ascend C C API。C API提供纯C风格的接口，符合C语言算子开发习惯，开放芯片完备编程能力。  |
| [资料贡献指南](./zh/asc_doc_contributing.md) | 完善Ascend C现有资料的开发者。 |  介绍Ascend C资料各大章节的编写与贡献规范。|
| [资料设计规范](./zh/asc_doc_design_rules.md) | 完善Ascend C现有资料的开发者。 |  介绍Ascend C资料体系架构与总体设计原则。|
| [代码贡献规范](./zh/contributing/README.md) | 参与Ascend C代码贡献的开发者。 | 代码风格、接口设计、目录结构、测试等贡献规范索引。 |
| [Ascend C入门教程](./zh/guide/getting_started/ascend_c_overview_and_learning_path.md) | Ascend C初学者 | 介绍算子编程基本概念与Ascend C学习路径，旨在帮助开发者快速上手算子开发。 |
| [Ascend C编程指南](./zh/guide/programming_guide/document_structure.md) | 基于昇腾AI硬件，使用Ascend C编写算子程序，开发自定义算子的开发者。 | Ascend C是CANN针对算子开发场景推出的编程语言，原生支持C和C++标准规范，兼具开发效率和运行性能。使用Ascend C编写算子程序，运行在昇腾AI处理器上，实现自定义的创新算法。 |
| [Ascend C算子实践参考](./zh/guide/operator_practice/document_structure.md) | 基于已完成开发的Ascend C算子，需要进一步优化算子性能的开发者。 | 异构计算的特点、算子功能的调试方法以及算子性能的优化策略。通过介绍Ascend C编程中的调试与优化思路，结合多种性能优化手段，并辅以具体案例，旨在帮助开发者实现高性能算子的开发。|
| [Ascend C兼容性迁移指南](./zh/guide/cross_gen_migration_guide/overview.md) | 需将已有算子跨代迁移至不同昇腾AI处理器的开发者。 | 指导如何实现跨代芯片间的功能对等与性能最优适配。|


## 附录
除了上述系统性介绍的相关开发文档外，您还可以根据实际场景和开发阶段，有选择性地快速了解相应的专题内容。
- 技术文章  
  - 基础入门
    - [Ascend C编程介绍](https://www.hiascend.com/zh/developer/techArticles/20230830-1)
    - [Ascend C快速入门](https://www.hiascend.com/zh/developer/techArticles/20230830-2)
    - [Ascend C孪生调试](https://www.hiascend.com/zh/developer/techArticles/20231215-2)
    - [Ascend C算子的调用方式](https://www.hiascend.com/zh/developer/techArticles/20240523-1)
  - 概念原理
    - [Ascend C非对齐数据的处理方案](https://www.hiascend.com/zh/developer/techArticles/20250627-1)
    - [深度理解多核并行/流水计算/double buffer技术](https://www.hiascend.com/zh/developer/techArticles/20230807-1)
  - 问题案例
    - [Ascend C算子开发常见问题案例](https://www.hiascend.com/zh/developer/techArticles/20240106-1) 
    - [含有Matmul高阶API的算子精度问题定位](https://www.hiascend.com/zh/developer/techArticles/20250107-1)
  - 性能优化
    - [Ascend C算子性能优化实用技巧01——流水优化](https://www.hiascend.com/zh/developer/techArticles/20240819-1)
    - [Ascend C算子性能优化实用技巧02——内存优化](https://www.hiascend.com/zh/developer/techArticles/20240823-1)
    - [Ascend C算子性能优化实用技巧03——搬运优化](https://www.hiascend.com/zh/developer/techArticles/20240906-1)
    - [Ascend C算子性能优化实用技巧04——Tiling优化](https://www.hiascend.com/zh/developer/techArticles/20240920-1)
    - [Ascend C算子性能优化实用技巧05——API使用优化](https://www.hiascend.com/zh/developer/techArticles/20241107-1)
  - 优秀实践
    - [基于Ascend C的Matmul算子性能优化最佳实践](https://www.hiascend.com/zh/developer/techArticles/20240816-1)
    - [基于Ascend C的FlashAttention算子性能优化最佳实践](https://www.hiascend.com/zh/developer/techArticles/20240607-1)


- 培训视频
  - [Ascend C系列教程（入门）](https://www.hiascend.com/developer/courses/detail/1691696509765107713)
  - [Ascend C系列教程（进阶）](https://www.hiascend.com/zh/developer/courses/detail/1696414606799486977)
  - [Ascend C系列教程（高级）](https://www.hiascend.com/zh/developer/courses/detail/1696690858236694530)
