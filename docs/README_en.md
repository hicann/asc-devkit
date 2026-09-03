# Project Documentation

English | [简体中文](./README.md)

## Directory Description
Key directory structure is as follows:
```
├── zh                             # Chinese documentation directory
│   ├── api                        # API documentation directory
│   ├── contributing               # Code contribution standards directory (code style, API design, directory structure, testing, etc.)
│   ├── figures                    # Image directory
│   ├── guide                      # Ascend C operator develop guide
│   ├── vitepress                      # docs build directory
│   ├── asc_adv_api_contributing.md    # Ascend C high-level API contribution guide
│   ├── asc_basic_api_contributing.md  # Ascend C basic API contribution guide
│   ├── asc_c_api_contributing.md      # Ascend C C API contribution guide
│   ├── asc_doc_contributing.md        # Ascend C documentation contribution guide
│   ├── asc_doc_design_rules.md        # Ascend C documentation design rules
│   ├── asc_how_to_choose_api.md       # Ascend C Multi-Level Programming Interface Selection Guide
│   └── quick_start.md                 # Quick start documentation
├── en                                 # English documentation directory
│   ├── contributing                   # Code contribution standards directory (code style, API design, directory structure, testing, etc.)
│   ├── asc_adv_api_contributing.md    # Ascend C high-level API contribution guide
│   ├── asc_basic_api_contributing.md  # Ascend C basic API contribution guide
│   ├── asc_c_api_contributing.md      # Ascend C C API contribution guide
│   ├── asc_doc_contributing.md        # Ascend C documentation contribution guide
│   ├── asc_doc_design_rules.md        # Ascend C documentation design rules
│   ├── asc_how_to_choose_api.md       # Ascend C Multi-Level Programming Interface Selection Guide
│   └── quick_start.md                 # Quick start documentation
├── README_en.md
└── README.md
```

## Documentation Description
To help developers quickly familiarize with this project, corresponding documentation can be obtained as needed. Documentation content includes:

| Document | Target Audience | Content Introduction |
|---|---|---|
| [API List](./zh/api/README.md) | Users developing customized APIs or operators based on Ascend C open source repository. | Introduces all APIs included in the project. |
| [High-level API Contribution Guide](./en/asc_adv_api_contributing.md) | Users developing customized APIs based on Ascend C open source repository. | Introduces how to extend or develop Ascend C high-level API. High-level API abstracts and encapsulates common algorithms based on single-core, implementing commonly used computational algorithms to improve operator development efficiency. |
| [Basic API Contribution Guide](./en/asc_basic_api_contributing.md) | Users developing customized APIs based on Ascend C open source repository. | Introduces how to extend or develop Ascend C basic API. Basic API implements abstraction of hardware capabilities, opening chip capabilities, ensuring completeness and compatibility. |
| [C API Contribution Guide](./en/asc_c_api_contributing.md) | Users developing customized APIs based on Ascend C open source repository. | Introduces how to extend or develop Ascend C C API. C API provides pure C style interfaces, conforming to C language operator development habits, opening complete chip programming capabilities. |
| [Documentation Contribution Guide](./en/asc_doc_contributing.md) | Developers improving existing Ascend C documentation. | Introduces the writing and contribution specifications for each chapter of Ascend C documentation. |
| [Documentation Design Rules](./en/asc_doc_design_rules.md) | Developers improving existing Ascend C documentation. | Introduces the Ascend C documentation architecture and overall design principles. |
| [Code Contribution Standards](./en/contributing/README.md) | Developers contributing code to Ascend C. | Index of contribution standards: code style, API design, directory structure, testing. |
| [Ascend C Getting Started Guide](./zh/guide/getting_started/ascend_c_overview_and_learning_path.md) | Beginners of Ascend C | Introduces basic concepts of operator programming and the Ascend C learning path, helping developers quickly get started with operator development. |
| [Ascend C Programming Guide](./zh/guide/programming_guide/document_structure.md) | Developers writing operator programs with Ascend C on Ascend AI hardware and developing custom operators. | Ascend C is a programming language launched by CANN for operator development scenarios. It natively supports C and C++ standards, offering both development efficiency and runtime performance. Write operator programs with Ascend C to run on Ascend AI processors and implement custom innovative algorithms. |
| [Ascend C Operator Practice Reference](./zh/guide/operator_practice/document_structure.md) | Developers who have completed Ascend C operator development and need to further optimize operator performance. | Characteristics of heterogeneous computing, debugging methods for operator functionality, and performance optimization strategies. By introducing debugging and optimization approaches in Ascend C programming, combined with various performance optimization techniques and specific cases, this guide aims to help developers achieve high-performance operator development. |
| [Ascend C Cross-Generation Migration Guide](./zh/guide/cross_gen_migration_guide/overview.md) | Developers who need to migrate existing operators across different generations of Ascend AI processors. | Guides how to achieve functional equivalence and optimal performance adaptation across chip generations. |


## Appendix
Besides the systematic development documentation introduced above, you can also selectively learn about corresponding specialized content based on actual scenarios and development stages.
- Technical Articles  
  - Basics Introduction
    - [Ascend C Programming Introduction](https://www.hiascend.com/zh/developer/techArticles/20230830-1)
    - [Ascend C Quick Start](https://www.hiascend.com/zh/developer/techArticles/20230830-2)
    - [Ascend C Twin Debugging](https://www.hiascend.com/zh/developer/techArticles/20231215-2)
    - [Ascend C Operator Invocation Methods](https://www.hiascend.com/zh/developer/techArticles/20240523-1)
  - Concept Principles
    - [Ascend C Non-aligned Data Processing Solutions](https://www.hiascend.com/zh/developer/techArticles/20250627-1)
    - [Deep Understanding of Multi-core Parallel/Pipeline Computing/Double Buffer Technology](https://www.hiascend.com/zh/developer/techArticles/20230807-1)
  - Problem Cases
    - [Ascend C Operator Development Common Problem Cases](https://www.hiascend.com/zh/developer/techArticles/20240106-1) 
    - [Locating Precision Issues in Operators Containing Matmul High-level API](https://www.hiascend.com/zh/developer/techArticles/20250107-1)
  - Performance Optimization
    - [Ascend C Operator Performance Optimization Practical Tips 01 - Pipeline Optimization](https://www.hiascend.com/zh/developer/techArticles/20240819-1)
    - [Ascend C Operator Performance Optimization Practical Tips 02 - Memory Optimization](https://www.hiascend.com/zh/developer/techArticles/20240823-1)
    - [Ascend C Operator Performance Optimization Practical Tips 03 - Data Movement Optimization](https://www.hiascend.com/zh/developer/techArticles/20240906-1)
    - [Ascend C Operator Performance Optimization Practical Tips 04 - Tiling Optimization](https://www.hiascend.com/zh/developer/techArticles/20240920-1)
    - [Ascend C Operator Performance Optimization Practical Tips 05 - API Usage Optimization](https://www.hiascend.com/zh/developer/techArticles/20241107-1)
  - Best Practices
    - [Matmul Operator Performance Optimization Best Practice Based on Ascend C](https://www.hiascend.com/zh/developer/techArticles/20240816-1)
    - [FlashAttention Operator Performance Optimization Best Practice Based on Ascend C](https://www.hiascend.com/zh/developer/techArticles/20240607-1)


- Training Videos
  - [Ascend C Series Tutorial (Introductory)](https://www.hiascend.com/developer/courses/detail/1691696509765107713)
  - [Ascend C Series Tutorial (Advanced)](https://www.hiascend.com/zh/developer/courses/detail/1696414606799486977)
  - [Ascend C Series Tutorial (Expert)](https://www.hiascend.com/zh/developer/courses/detail/1696690858236694530)
