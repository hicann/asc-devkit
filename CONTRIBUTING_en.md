# Contribution Guide

This project welcomes developers to experience and participate in contributions. Before participating in community contributions, see [cann-community](https://gitcode.com/cann/community) to understand the code of conduct, sign the CLA agreement, and learn about the contribution process for source code repositories.

When preparing local code and submitting PRs, developers need to pay attention to the following points:

1. When submitting a PR, carefully fill in the business background, purpose, and solution according to the PR template.
2. If your modification is not a simple bug fix but involves adding new features, new interfaces, new configuration parameters, or modifying code flows, discuss the solution through an Issue first to avoid your code being rejected. If you are unsure whether your modification can be classified as a simple bug fix, you can also submit an Issue for discussion.

## API Development Guide

This project supports three types of API development, each with dedicated contribution guides:

### 1. High-level API

High-level API abstracts and encapsulates common algorithms based on single-core, implementing commonly used computational algorithms to improve programming development efficiency. High-level API generally implements through calling multiple basic APIs. High-level API includes mathematical computation, matrix computation, activation function, and other APIs.

- **Applicable Scenarios**: Abstraction and encapsulation of common algorithms, improving operator development efficiency
- **Contribution Guide**: [docs/en/asc_adv_api_contributing.md](./docs/en/asc_adv_api_contributing.md)

### 2. Basic API

Basic API implements abstraction of hardware capabilities, opening chip capabilities, ensuring completeness and compatibility.

- **Applicable Scenarios**: C++ language programming style scenarios
- **Contribution Guide**: [docs/en/asc_basic_api_contributing.md](./docs/en/asc_basic_api_contributing.md)

### 3. C API

C API provides pure C style interfaces, conforming to C language operator development habits, opening complete chip programming capabilities, supporting array memory allocation, generally based on pointer programming. Provides industry-similar programming experience.

- **Applicable Scenarios**: C language programming style scenarios
- **Contribution Guide**: [docs/en/asc_c_api_contributing.md](./docs/en/asc_c_api_contributing.md)


## Documentation Contribution Guide

This project includes five core Ascend C documents (Getting Started Tutorial, Programming Guide, API Reference Manual, Operator Implementation Reference, and Cross-Generation Migration Compatibility Guide); developers are welcome to correct, supplement, and contribute to the content.

- **Documentation Writing Guide**: [docs/en/asc_doc_contributing.md](./docs/en/asc_doc_contributing.md) — Contribution scenarios, writing conventions, PR checklist
- **Documentation Design Principles**: [docs/en/asc_doc_design_rules.md](./docs/en/asc_doc_design_rules.md) — Discoverability, readability, completeness design specification

## Developer Contribution Scenarios

- Bug Fix

  If you discover certain bugs in this project and want to fix them, you are welcome to create a new Issue for feedback and tracking.

  Follow the [Submit Issue/Process Issue Task](https://gitcode.com/cann/community#提交Issue处理Issue任务) guide to create a `Bug-Report|缺陷反馈` type Issue to describe the bug, then enter "/assign" or "/assign @yourself" in the comment box to assign the Issue to yourself for processing.
  
- Code Optimization

  If you have generalization enhancement/performance optimization ideas for certain API implementations in this project and want to implement these optimization points, you are welcome to contribute API optimizations.

  Follow the [Submit Issue/Process Issue Task](https://gitcode.com/cann/community#提交Issue处理Issue任务) guide to create a `Requirement|需求建议` type Issue to explain the optimization points and provide your design solution, then enter "/assign" or "/assign @yourself" in the comment box to assign the Issue to yourself for tracking optimization.

- Documentation Correction

  If you discover certain documentation description errors in this project, you are welcome to create a new Issue for feedback and correction.

  Follow the [Submit Issue/Process Issue Task](https://gitcode.com/cann/community#提交Issue处理Issue任务) guide to create a `Documentation|文档反馈` type Issue to point out the corresponding documentation problems, then enter "/assign" or "/assign @yourself" in the comment box to assign the Issue to yourself to correct the corresponding documentation description.
  
- Help Resolve Others' Issues

  If you have appropriate solutions for problems encountered by others in the community, you are welcome to comment and communicate in the Issue to help others solve problems and pain points, jointly improving usability.

  If the corresponding Issue requires code modification, you can enter "/assign" or "/assign @yourself" in the Issue comment box to assign the Issue to yourself, tracking assistance in solving the problem.

## Code Standards

This project has a complete code contribution standards system. Please read the relevant guides before development. For the complete index, see [docs/en/contributing/README.md](./docs/en/contributing/README.md).

| Document | Content |
|----------|---------|
| [code-style.md](./docs/en/contributing/code-style.md) | C++ naming, include, header guard, namespace, macros, constexpr, formatting, code comment standards, deprecation annotation |
| [api-design.md](./docs/en/contributing/api-design.md) | API interface design, template parameters, `__aicore__` usage, Tiling interface |
| [directory-structure.md](./docs/en/contributing/directory-structure.md) | Directory structure, file naming suffixes, architecture directories, new API operation checklist |
| [testing.md](./docs/en/contributing/testing.md) | Testing framework, file naming, UT writing, Header Checker, coverage requirements |
