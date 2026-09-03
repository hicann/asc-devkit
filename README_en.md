<div align="center">

# Ascend C

English | [简体中文](./README.md)

<h4>Multi-layer APIs built on C/C++, unlocking full chip programmability for ultimate performance</h4>

[![community](https://img.shields.io/badge/docs-community-brightgreen.svg?style=flat)](https://www.hiascend.com/document/redirect/CannCommunityOpdevAscendC)
[![repo](https://img.shields.io/badge/docs-repo-blue.svg?style=flat)](docs)
[![examples](https://img.shields.io/badge/examples-repo-orange.svg?style=flat)](examples)
[![asc-tools](https://img.shields.io/badge/asc--tools-repo-6f42c1.svg?style=flat)](https://gitcode.com/cann/asc-tools)
[![license](https://img.shields.io/badge/license-CANN_Open_2.0-lightgrey.svg)](LICENSE)
[![contributing](https://img.shields.io/badge/CONTRIBUTING-teal)](CONTRIBUTING.md)
[![SIG](https://img.shields.io/badge/SIG-ascendc-yellow)](https://gitcode.com/cann/community/tree/master/CANN/sigs/ascendc)

</div>

## 🔥 Latest News
[2026/05] Key Features of v9.1.0-beta.2

### 🚀 Key Features
- NPU Check support added to Ascend C framework basic APIs ([PR#1557](https://gitcode.com/cann/asc-devkit/pull/1557), [PR#1467](https://gitcode.com/cann/asc-devkit/pull/1467)), enhancing runtime verification capability for operators.
- printf and register dump support added within SIMD VF ([PR#1605](https://gitcode.com/cann/asc-devkit/pull/1605)), enabling debug printing and register data dumping.
- DumpTensor for L1 Tensor data supported on A5 ([PR#2175](https://gitcode.com/cann/asc-devkit/pull/2175)), extending debug support for L1-level data.
- CMAKE<LANG> compilation options supported in the CMakeModule of build projects ([PR#2055](https://gitcode.com/cann/asc-devkit/pull/2055)); `optype_collector` tool added to check for duplicate optype names ([PR#285](https://gitcode.com/cann/asc-tools/pull/285)).
- Functional behaviors in ctrl (saturation overflow management) supported by basic APIs ([PR#2077](https://gitcode.com/cann/asc-devkit/pull/2077)).
- ld/st interfaces ([PR#2058](https://gitcode.com/cann/asc-devkit/pull/2058)) and AddrSpace class interfaces ([PR#1597](https://gitcode.com/cann/asc-devkit/pull/1597)) added to SIMT programming, enriching SIMT memory access capabilities.

### 🎯 Sample Updates
- Best practice sample development: matmul+gelu fusion, datacopy optimization, bank conflict optimization, grouped quantized matmul, and high-performance SIMD & SIMT programming ([PR#1814](https://gitcode.com/cann/asc-devkit/pull/1814), [PR#2137](https://gitcode.com/cann/asc-devkit/pull/2137), [PR#2141](https://gitcode.com/cann/asc-devkit/pull/2141), [PR#2166](https://gitcode.com/cann/asc-devkit/pull/2166), [PR#2363](https://gitcode.com/cann/asc-devkit/pull/2363)).
- New feature supplements and compatibility sample rectification for Ascend 950: loopmode data transfer, interleave vector computation, datacopy_gm2l1, loadmx (Load2DMX), mmad_mx, data_copy_pad, etc. ([PR#2336](https://gitcode.com/cann/asc-devkit/pull/2336), [PR#1899](https://gitcode.com/cann/asc-devkit/pull/1899), [PR#2124](https://gitcode.com/cann/asc-devkit/pull/2124)).
- Basic samples added to RegBase: basic arithmetic, data type conversion, reduction, comparison, indexing, etc. ([PR#1459](https://gitcode.com/cann/asc-devkit/pull/1459), [PR#1575](https://gitcode.com/cann/asc-devkit/pull/1575), [PR#2024](https://gitcode.com/cann/asc-devkit/pull/2024)).
- SIMD VF print samples and dump samples added ([PR#2558](https://gitcode.com/cann/asc-devkit/pull/2558)).
- DCache access optimization samples added to SIMT ([PR#2453](https://gitcode.com/cann/asc-devkit/pull/2453)); transpose-based memory coalescing and bank conflict samples added ([PR#1753](https://gitcode.com/cann/asc-devkit/pull/1753)); best practice sample: improving transfer efficiency through type alignment ([PR#2297](https://gitcode.com/cann/asc-devkit/pull/2297)).
- Functional feature samples added to SIMT: PyTorch custom operator registration ([PR#2769](https://gitcode.com/cann/asc-devkit/pull/2769)), compilation-related samples (dynamic, static, separate compilation, etc.) ([PR#2356](https://gitcode.com/cann/asc-devkit/pull/2356)), profiling samples ([PR#1989](https://gitcode.com/cann/asc-devkit/pull/1989)), memory barrier feature samples ([PR#1923](https://gitcode.com/cann/asc-devkit/pull/1923)), Warp-class feature samples ([PR#2876](https://gitcode.com/cann/asc-devkit/pull/2876)), simulator samples ([PR#2692](https://gitcode.com/cann/asc-devkit/pull/2692)), kernel log samples ([PR#2131](https://gitcode.com/cann/asc-devkit/pull/2131)).
- SIMT introductory sample revised to gather ([PR#2405](https://gitcode.com/cann/asc-devkit/pull/2405)).
- Tensor API introductory and best practice samples added: Matmul introduction, data load/store, output-side quantization, MX FP4 best practices ([PR#2553](https://gitcode.com/cann/asc-devkit/pull/2553)).

### 📖 Documentation
- Documentation on matrix computation overview and computation fractal introduction added ([PR#2533](https://gitcode.com/cann/asc-devkit/pull/2533)).
- Vector computation API documentation optimized, with instruction constraints supplemented ([PR#2676](https://gitcode.com/cann/asc-devkit/pull/2676)).
- VitePress documentation site built to provide AscendC document preview capability ([PR#2547](https://gitcode.com/cann/asc-devkit/pull/2547)).
- Overview of performance optimization for hybrid SIMD and SIMT programming added ([PR#2736](https://gitcode.com/cann/asc-devkit/pull/2736)).

For detailed information on all historical releases and updates, please refer to [CHANGELOG.md](./CHANGELOG.md).

## 🚀 Overview

[Ascend C](https://www.hiascend.com/cann/ascend-c) is a dedicated operator development programming language built by CANN (Compute Architecture for Neural Networks) for Ascend AI processors, natively compatible with C/C++ standard specifications.

As a programming language covering full-scenario operator development needs, Ascend C fully unlocks the underlying programmability of the chip to enable ultimate performance tuning. Meanwhile, through a hierarchical API design system, developers can flexibly select development interfaces according to business scenarios, technical capabilities and performance goals, to achieve an optimal balance between development efficiency and runtime performance.

### Design Goals

Ascend C takes **"Compatible with C/C++ standards · Unleash ultimate computing power"** as its core design philosophy: on the basis of strictly complying with C/C++ language specifications, it makes minimal syntax extensions, enabling developers with C/C++ foundation to migrate smoothly to the Ascend platform with low threshold, and independently unleash the full computing potential of the chip. The language is simultaneously compatible with pointer-based native C development paradigm and Tensor+Layout modern C++ programming mode, deeply supporting operator customization and optimization while realizing seamless integration with existing C/C++ development ecosystems and ensuring consistent cross-platform development experience.

We adhere to two core design principles:
- **No silver bullet**: Different business scenarios have different demands for performance and development efficiency. There is no single optimal interface suitable for all scenarios. Layered design is the core path to balance efficiency and performance.
- **Progressive growth**: Entry-level developers can quickly get started with high-level easy-to-use interfaces to complete algorithm verification and prototype implementation; senior developers can dive into low-level interfaces to fully unleash hardware potential through fine-grained tuning.

### Hierarchical API System

Following the core principle of "standard C/C++ syntax, minimal extensions", Ascend C builds a lightweight and high-performance foundation.

| API Layer | Language Paradigm | Core Features | Target Users | Core Value |
|-----------|-------------------|---------------|--------------|------------|
| **Basic API** | C++ | Based on SIMD programming model, adopts Tensor abstraction without Layout constraints; realizes unified management of memory scheduling and execution synchronization via the TPipe/TQue framework. | General operator library developers | Automatically handles memory and synchronization management through the framework, shields underlying hardware details, effectively reduces development complexity, and improves programming usability and engineering delivery efficiency. |
| **Tensor API** | C++ | Based on SIMD programming model, provides Tensor abstraction with **Layout** semantics via the Layout algebra system; adopts **arch/atom/algorithm** three-layer decoupled API architecture. | High-performance operator optimization developers | Treats tensors and layouts as first-class citizens. Built-in Layout algebra enables zero-cost compilation abstraction; three-layer decoupling achieves separation of concerns, with native cross-architecture portability. |
| **SIMD C API** | C | Based on SIMD programming model and **native pointer** paradigm, provides complete C-level underlying programmability; supports array subscript memory access, with memory lifecycle and execution synchronization fully controlled by developers. | Ultimate performance tuning developers | Fully conforms to native C development habits, supports in-depth customization of memory layout and synchronization strategies; instruction-level transparent mapping delivers zero encapsulation overhead, fully opens underlying hardware capabilities, and enables fine-grained performance tuning and maximum hardware utilization. |
| **SIMT API** | C | Based on industry-standard SIMT programming model, takes single thread as the basic programming unit, natively supports discrete and irregular parallel computing logic. | Developers of high-performance operators for irregular scenarios | Naturally adapts to discrete and irregular parallel computing scenarios, aligns with mainstream heterogeneous development paradigms in the industry, and effectively reduces cross-technology migration cost and learning threshold. |

On top of the underlying programming interfaces, Ascend C also provides operator template libraries and high-level API components, which precipitate general development capabilities, further lower the development threshold, and accelerate algorithm prototype delivery.

| Component Layer | Target Users | Core Value |
|-----------------|--------------|------------|
| **Operator Template Library (BLAZE/ATVOSS, etc.)** | Algorithm developers | Provides high-performance template implementations of typical operators, supports customized expansion based on templates, and quickly adapts to high-performance computing requirements of business scenarios. |
| **High-level API** | Algorithm developers | Encapsulates general single-core computing algorithm primitives, shields underlying hardware implementation details, enables developers to quickly build algorithm logic, and efficiently completes function verification and scheme prototyping. |

In addition, the joint ecosystem is continuously building basic components such as **asc-stl** (C++ standard library for kernel programming) and **asc-mathdx**(on-device linear algebra library), to continuously enrich the Ascend C operator programming ecosystem.

### Overall Architecture

Ascend C adopts a layered architecture design, building a complete operator development technology stack from bottom to top. The overall logical architecture is as follows:

<img src="docs/zh/guide/figures/architecture_ascendc.png" alt="Ascend C Overall Architecture Diagram" width="1000px">

The capability definition and description of each layer are as follows:
- **Language Extension Layer (SIMT API)**: Native C programming interface for multi-threaded parallel scenarios, supports industry-standard SIMT programming model, ensures consistent cross-technology development experience, and enables high-performance operator development in discrete and irregular computing scenarios.
- **Language Extension Layer (SIMD C API)**: C-level underlying programming interface based on SIMD vectorized programming model for ultimate performance tuning, natively supports array-style memory allocation and pointer-based computing primitives, with instruction-level zero-encapsulation-overhead hardware access capability. Ascend 950PR/Ascend 950DT architecture adds support for SIMD/SIMT hybrid programming mode, covering multi-form parallel computing scenarios.
- **Tensor API**: Core C++ Tensor-level programming interface for high-performance operator development. Takes memory layout (Layout) as a first-class citizen of Tensor abstraction, natively supports Tensor objects carrying Layout semantics; built-in Layout algebra can automatically complete memory layout index derivation and layout transformation management, significantly reducing development complexity in complex memory layout scenarios, and improving code maintainability and cross-architecture portability.
- **Basic API**: C++ class library interface based on single-instruction abstraction, provides basic programming capability centered on Tensor objects without Layout constraints; relies on TPipe/TQue framework to uniformly manage memory scheduling and execution synchronization, shielding underlying hardware implementation details.
- **High-level API (Adv API)**: Standardized abstract encapsulation of single-core general computing algorithms, provides out-of-the-box public algorithm implementations, shields underlying hardware instruction details, and supports rapid algorithm verification and engineering prototype delivery.
- **Operator Template Library**: High-performance operator reference implementation framework for typical computing scenarios, provides complete engineering implementation references of operators based on template design, precipitates best practices of Tiling optimization and performance tuning, simplifies operator development process, and supports user-defined expansion.
- **Ecosystem Extension Components (Under Development)**:
  - **asc-comm**: Provides independent development capability for communication operators, supports customized operator development in distributed scenarios;
  - **asc-stl**: Provides common C++ standard library capabilities on the device side, deeply adapted to Kernel programming paradigm and runtime environment;
  - **asc-mathdx**: Integrates on-device linear algebra computing libraries such as blasdx, supports high-performance mathematical computing and scientific computing scenarios.
- **Python Frontend (PyAsc)**: Encapsulates the full underlying programmability of the chip based on Python language, continuously improves Tensor programming capability under the Layout system, adds support for SIMT programming model, and enables development of high-performance operators through Python interfaces.

### How to Select Multi-layer APIs for Operator Development
- **Development in C/C++**: Please refer to [Ascend C Multi-level API Selection Guide](./docs/zh/asc_how_to_choose_api.md) for details
- **Python-based development for full programmability and ultimate performance**: Ascend C Python frontend [PyAsc](https://gitcode.com/cann/pyasc) is recommended
- **Python-based development for rapid verification and ease of use**: [PyPTO](https://gitcode.com/cann/pypto) is recommended

## 🔍 Directory Structure

This repository mainly contains Ascend C programming APIs and necessary cmake build scripts, which are the core modules required for operator development. Its directory structure is as follows:

```
├── cmake                               # Ascend C build source code
├── docs                                # Project documentation
├── examples                            # Ascend C API sample projects
├── impl                                # Ascend C API implementation source code
│   ├── adv_api                         # Ascend C high-level API implementation
│   ├── aicpu_api                       # Ascend C AI CPU API implementation
│   ├── basic_api                       # Ascend C basic API implementation
│   ├── c_api                           # Ascend C language extension layer C API implementation
│   ├── simt_api                        # Ascend C SIMT API implementation
│   ├── tensor_api                      # Ascend C Tensor API implementation
│   └── utils                           # Ascend C utility class implementation
├── include                             # Ascend C API header files
│   ├── adv_api                         # Ascend C high-level API headers
│   ├── aicpu_api                       # Ascend C AI CPU API headers
│   ├── basic_api                       # Ascend C basic API headers
│   ├── c_api                           # Ascend C language extension layer C API headers
│   ├── simt_api                        # Ascend C SIMT API headers
│   ├── tensor_api                      # Ascend C Tensor API headers
│   └── utils                           # Ascend C utility class headers
├── scripts                             # Packaging scripts
├── tests                               # UT test cases for Ascend C APIs
└── tools                               # Ascend C tool source code
```

## ⚡️ Quick Start

If you want to quickly experience project building and operator sample execution, please refer to the following documents for tutorials.

- [Compilation and Building](docs/zh/quick_start.md): Introduces environment setup, compilation & execution, and local verification.
- [Sample Execution](examples/README.md): Provides operator development samples and introduces the end-to-end execution method.

## 🧰 clangd/IDE Support

- Install clangd (version 15+ recommended, taking Ubuntu as an example) and the VSCode clangd plugin

  ```bash
  sudo apt install -y clangd-15
  sudo update-alternatives --install /usr/bin/clangd clangd /usr/bin/clangd-15 100
  ```

- Configure `settings.json` for local VSCode (example)

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

- Configure `.clangd` in the project root directory (example). The complete `.clangd` file is provided in this directory, where the CANN header file path should match your actual installation location; the default path in `.clangd` is `/usr/local/Ascend`.

  If CANN is installed in a non-default path, or you need to switch the NPU architecture macro, you can first source CANN `set_env.sh`, then generate the local configuration from `.clangd.in` via the script:

  ```bash
  source /path/to/cann/set_env.sh
  python3 scripts/setup_clangd.py --npu-arch 2201 --output .clangd.local
  ```

  `ASCEND_HOME_PATH` is set by `set_env.sh`, and the script will use this environment variable to resolve the actual CANN path; if not set, the script will prompt you to source `set_env.sh` first. The generated file defaults to `.clangd.local`, which will not overwrite the built-in `.clangd` of the repository; to enable the project-level clangd configuration, you can copy it to `.clangd` as needed and then restart clangd.

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

- Restart clangd (VS Code: Command Palette -> "Clangd: Restart language server")

- We recommend downloading and installing the Ascend C Toolkit VS Code extension from the [releases page](https://gitcode.com/opdevtools/plugin_release/releases). It supports editing, building, running, debugging, exception detection, and performance tuning for Ascend C operator projects in VS Code.

- Recent updates of Ascend C Toolkit focus on enhancing environment resolution, standard custom operator creation, SoC settings, simulation analysis, NPU debugging, setting migration and stability for Atlas A2 series and Atlas A3 series products. For complete usage instructions and feedback, please refer to [CANN Discussion Area Usage Guide](https://gitcode.com/org/cann/discussions/54).

- 💡 Community developers are welcome to provide feedback on ASC language syntax highlighting and code navigation support.

## 📖 Related Resources

- **Programming Guides**

  | Document | Description |
  |----------|-------------|
  |[Ascend C Programming Guide](https://hiascend.com/document/redirect/CannCommunityOpdevAscendC)|Write operator programs with Ascend C and develop custom operators based on Ascend AI hardware.|
  |[Ascend C Practice Reference](https://hiascend.com/document/redirect/CannCommunityAscendCBestPractice) | Introduces how to further optimize operator performance based on developed Ascend C operators. |
  |[Ascend C API List](https://hiascend.com/document/redirect/CannCommunityAscendCApi)| Ascend C SIMD & SIMT APIs, including language extension layer C API, C++ class library basic API and high-level API |
  |[Ascend C Samples](./examples)| Samples introducing key features of Ascend C APIs, including AICore SIMD & SIMT, AICPU, etc. |
  |[Ascend C Developer Guide for Kylin](https://gitcode.com/cann/cann-recipes-harmony-infer/blob/master/docs/ascendc_develop_guide.md)|Write operator programs with Ascend C and develop custom operators based on Kylin AI hardware.|

- **Contribution Guides**

  | Document | Description |
  |----------|-------------|
  |[CANN Community Contribution Guide](https://gitcode.com/cann/community)| General processing flow of CANN community Issues, PRs, etc. |
  |[ASC-DevKit Contribution Guide](./CONTRIBUTING.md) | Contribution guide for Ascend C APIs, documents and samples. |

- **Others**

  | Document | Description |
  |----------|-------------|
  |[SIG-ascendc Meetup Slides](https://gitcode.com/cann/community/tree/master/events/meetup/slides/sig-ascendc)|Publicity materials of Ascend C, including new features of Ascend 950.|
  |[Ascend C Wiki](https://gitcode.com/cann/asc-devkit/wiki)|Ascend C technical articles and announcements.|
  |[CANN-Learning-Hub](https://gitcode.com/cann/cann-learning-hub/tree/master/tutorials/ascendc_operator_development)|Full-process online tutorial for Ascend C operator development|
  |[Ascend C Ops Samples](https://gitcode.com/cann/cann-samples)|Repository of step-by-step high-performance implementation samples for Ascend C operators|

## 📌 Roadmap

- [Ascend C Development Roadmap (2026 Q2)](https://gitcode.com/cann/asc-devkit/issues/316)
- [Ascend C Development Roadmap (2026 Q3)](https://gitcode.com/cann/asc-devkit/issues/938)

## 📝 Related Information

- [Contribution Guide](CONTRIBUTING_en.md)
- [Security Statement](SECURITY_en.md)
- [License](LICENSE)
