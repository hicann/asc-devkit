# CHANGELOG

## v9.1.0-beta.2
Release Date: 2026/05/31
| CANN Version | Version Source Tag |
|--|--|
| [CANN 9.1.0-beta.2](https://www.hiascend.com/developer/download/community/result?module=cann&cann=9.1.0-beta.2) | [v9.1.0-beta.2](https://gitcode.com/cann/asc-devkit/tags/v9.1.0-beta.2) |

### 🚀 Key Features
- The AscendC framework basic API supports NPU Check ([PR#1557](https://gitcode.com/cann/asc-devkit/pull/1557) [PR#1467](https://gitcode.com/cann/asc-devkit/pull/1467)), enhancing operator runtime verification capability.
- SIMD VF supports printf and reg dump printing ([PR#1605](https://gitcode.com/cann/asc-devkit/pull/1605)), providing debug printing and register data dump capability.
- A5 supports DumpTensor for L1 Tensor data ([PR#2175](https://gitcode.com/cann/asc-devkit/pull/2175)), extending L1 layer data debugging support.
- The compilation project CMakeModule supports CMAKE<LANG> compilation options ([PR#2055](https://gitcode.com/cann/asc-devkit/pull/2055)); adds the optype_collector tool, supporting optype duplicate name checking ([PR#285](https://gitcode.com/cann/asc-tools/pull/285)).
- The basic API supports functional behavior in ctrl (saturation overflow management) ([PR#2077](https://gitcode.com/cann/asc-devkit/pull/2077)).
- SIMT programming adds ld/st interfaces ([PR#2058](https://gitcode.com/cann/asc-devkit/pull/2058)) and AddrSpace class interfaces ([PR#1597](https://gitcode.com/cann/asc-devkit/pull/1597)), enriching SIMT memory access programming capability.

### 🎯 Sample Updates
- Best practice sample development: matmul+gelu fusion, datacopy optimization, bank conflict optimization, group_matmul quantized group matrix multiplication, and simt&simd high-performance programming ([PR#1814](https://gitcode.com/cann/asc-devkit/pull/1814) [PR#2137](https://gitcode.com/cann/asc-devkit/pull/2137) [PR#2141](https://gitcode.com/cann/asc-devkit/pull/2141) [PR#2166](https://gitcode.com/cann/asc-devkit/pull/2166) [PR#2363](https://gitcode.com/cann/asc-devkit/pull/2363)).
- Ascend 950 new feature supplements and compatibility sample rectification: loopmode data movement, interleave vector computation, datacopy_gm2l1, loadmx (Load2DMX), mmad_mx, data_copy_pad, and so on ([PR#2336](https://gitcode.com/cann/asc-devkit/pull/2336) [PR#1899](https://gitcode.com/cann/asc-devkit/pull/1899) [PR#2124](https://gitcode.com/cann/asc-devkit/pull/2124)).
- RegBase adds basic samples: basic arithmetic, data type conversion, reduction, comparison, indexing, and other samples ([PR#1459](https://gitcode.com/cann/asc-devkit/pull/1459) [PR#1575](https://gitcode.com/cann/asc-devkit/pull/1575) [PR#2024](https://gitcode.com/cann/asc-devkit/pull/2024)).
- Added SIMD VF print samples and dump samples ([PR#2558](https://gitcode.com/cann/asc-devkit/pull/2558)).
- SIMT adds DCache access optimization samples ([PR#2453](https://gitcode.com/cann/asc-devkit/pull/2453)), samples demonstrating transpose-based memory coalescing and bank conflicts ([PR#1753](https://gitcode.com/cann/asc-devkit/pull/1753)), and best practice samples for improving data movement efficiency through type alignment ([PR#2297](https://gitcode.com/cann/asc-devkit/pull/2297)).
- SIMT adds functional feature samples: PyTorch custom operator registration ([PR#2769](https://gitcode.com/cann/asc-devkit/pull/2769)), compilation-related samples (dynamic, static, separate compilation, and so on) ([PR#2356](https://gitcode.com/cann/asc-devkit/pull/2356)), profiling samples ([PR#1989](https://gitcode.com/cann/asc-devkit/pull/1989)), memory barrier feature samples ([PR#1923](https://gitcode.com/cann/asc-devkit/pull/1923)), Warp class feature samples ([PR#2876](https://gitcode.com/cann/asc-devkit/pull/2876)), simulator samples ([PR#2692](https://gitcode.com/cann/asc-devkit/pull/2692)), and kernel log samples ([PR#2131](https://gitcode.com/cann/asc-devkit/pull/2131)).
- The SIMT getting-started sample is modified to gather ([PR#2405](https://gitcode.com/cann/asc-devkit/pull/2405)).
- Added Tensor API getting-started and best practice samples: Matmul getting started, data movement in/out, data movement out with on-path quantization, and MX FP4 best practices ([PR#2553](https://gitcode.com/cann/asc-devkit/pull/2553)).

### 📖 Documentation
- Added matrix computation overview and computation fractal introduction documentation ([PR#2533](https://gitcode.com/cann/asc-devkit/pull/2533)).
- Optimized vector computation API documentation, supplementing instruction constraints, and so on ([PR#2676](https://gitcode.com/cann/asc-devkit/pull/2676)).
- Set up a VitePress documentation site, providing AscendC documentation preview functionality ([PR#2547](https://gitcode.com/cann/asc-devkit/pull/2547)).
- Added SIMD and SIMT hybrid programming performance optimization overview ([PR#2736](https://gitcode.com/cann/asc-devkit/pull/2736)).

---

## v9.1.0-beta.1
Release Date: 2026/04/30
| CANN Version | Version Source Tag |
|--|--|
| [CANN 9.1.0-beta.1](https://www.hiascend.com/developer/download/community/result?module=cann&cann=9.1.0-beta.1) | [v9.1.0-beta.1](https://gitcode.com/cann/asc-devkit/tags/v9.1.0-beta.1) |

### 🚀 Key Features
- Aclrtc supports global functions as template parameters ([PR#1276](https://gitcode.com/cann/asc-devkit/pull/1276)), and fusion compilation supports msobjdump parsing ([PR#235](https://gitcode.com/cann/asc-tools/pull/235)), improving compilation project flexibility and debugging capability.
- Tensor API branch code is merged into master ([PR#1770](https://gitcode.com/cann/asc-devkit/pull/1770)), officially providing Tensor programming support.
- C-API adds support for moving int4x2_t type data between UB and registers ([PR#1404](https://gitcode.com/cann/asc-devkit/pull/1404) [PR#1504](https://gitcode.com/cann/asc-devkit/pull/1504)), extending low-bit data movement capability.
- SIMT API adds DCCI interfaces ([PR#1510](https://gitcode.com/cann/asc-devkit/pull/1510)), floating-point comparison interfaces ([PR#1693](https://gitcode.com/cann/asc-devkit/pull/1693)), and floating-point operation instructions ([PR#1774](https://gitcode.com/cann/asc-devkit/pull/1774)), enriching SIMT programming capability.

### 🎯 Samples
- Existing sample rectification: rectification of basic, high-level cube, vector, and data movement API samples ([PR#1200](https://gitcode.com/cann/asc-devkit/pull/1200) [PR#1429](https://gitcode.com/cann/asc-devkit/pull/1429) [PR#1454](https://gitcode.com/cann/asc-devkit/pull/1454) [PR#1311](https://gitcode.com/cann/asc-devkit/pull/1311) [PR#1563](https://gitcode.com/cann/asc-devkit/pull/1563)).
- Added static Tensor best practices: add high-performance sample ([PR#1262](https://gitcode.com/cann/asc-devkit/pull/1262)).
- Added low-bit cube best practices: matmul high-level and basic API samples, MXFP4 basic API samples ([PR#1535](https://gitcode.com/cann/asc-devkit/pull/1535) [PR#1648](https://gitcode.com/cann/asc-devkit/pull/1648) [PR#1788](https://gitcode.com/cann/asc-devkit/pull/1788)).
- Added custom operator project multi-vendors compilation sample ([PR#1466](https://gitcode.com/cann/asc-devkit/pull/1466)).
- Added atomic feature samples ([PR#1688](https://gitcode.com/cann/asc-devkit/pull/1688)).

### 📖 Documentation
- Added C-API manual content: supplemented UB and register data movement interface documentation ([PR#1404](https://gitcode.com/cann/asc-devkit/pull/1404) [PR#1504](https://gitcode.com/cann/asc-devkit/pull/1504)).
- Optimized getting-started tutorials and SIMT programming model ([PR#1487](https://gitcode.com/cann/asc-devkit/pull/1487)).
- Added CANN package installation and compatibility instructions ([PR#1451](https://gitcode.com/cann/asc-devkit/pull/1451) [PR#1514](https://gitcode.com/cann/asc-devkit/pull/1514)).
- Added AscendC getting-started tutorials, programming guides, compatibility migration guides, and operator practice reference documentation ([PR#1438](https://gitcode.com/cann/asc-devkit/pull/1438)).

---

## v9.0.0-beta.2
Release Date: 2026/03/31
| CANN Version | Version Source Tag |
|--|--|
| [CANN 9.0.0-beta.2](https://www.hiascend.com/developer/download/community/result?module=cann&cann=9.0.0-beta.2) | [v9.0.0-beta.2](https://gitcode.com/cann/asc-devkit/tags/v9.0.0-beta.2) |

### 🚀 Key Features
- Ascend 950PR supports SIMD programming mode, providing 200+ [API interfaces](https://gitcode.com/cann/asc-devkit/tree/9.0.0-beta.2/impl/basic_api/dav_c310) with cross-generation compatibility, enabling smooth operator migration from Atlas A2 series and Atlas A3 series products.
- Ascend 950PR adds Reg-based programming, providing 90+ [Reg programming interfaces](https://gitcode.com/cann/asc-devkit/tree/9.0.0-beta.2/impl/basic_api/reg_compute/dav_c310) including Reg data movement, basic arithmetic, reduction computation, and synchronization control.
- Atlas A2 series, Atlas A3 series, and Ascend 950PR support [language extension layer pure C interfaces](https://gitcode.com/cann/asc-devkit/tree/9.0.0-beta.2/include/c_api), providing array-style memory allocation and pointer-based computation interfaces for native pure C programming experience.
- Ascend 950PR supports SIMD and SIMT hybrid programming, providing approximately 700 [SIMT API interfaces](https://gitcode.com/cann/asc-devkit/tree/9.0.0-beta.2/include/simt_api), including warp, atomic, basic mathematical computation, type conversion, and other fundamental interfaces.
- Ascend 950PR supports communication high-level API CCU communication interfaces, providing [Allreduce, Allgather, Reducescatter, AlltoAll and other mainstream communication primitives](https://gitcode.com/cann/asc-devkit/tree/9.0.0-beta.2/impl/adv_api/detail/hccl/impl/platform_v310) based on CCU; Matmul high-level API adds support for [MXFP4/8 low-bit data type matrix operations](https://gitcode.com/cann/asc-devkit/blob/9.0.0-beta.2/impl/adv_api/detail/matmul/mx_matmul_impl.h), achieving half memory usage and doubled computing throughput.
- Ascend 950PR adds and compatibly supports approximately 260 samples, including SIMT samples, SIMD samples (framework, basic API, high-level API, best practices, and so on), and [sample directory structure adjustments](https://gitcode.com/cann/asc-devkit/pull/1223) to improve readability.
- Fusion compilation and <<<>>> invocation support [CPU mode](https://gitcode.com/cann/asc-tools/pull/138) and [SIM simulation mode](https://gitcode.com/cann/asc-devkit/blob/9.0.0-beta.2/cmake/asc/asc_modules/CMakeASCInformation.cmake).
### 📖 Documentation
- Added 90+ [Reg programming interface API](https://gitcode.com/cann/asc-devkit/blob/9.0.0-beta.2/docs/api/context/Reg%E7%9F%A2%E9%87%8F%E8%AE%A1%E7%AE%97.md) documentation. Reg vector computation API is developed for RegBase architecture, allowing users to directly operate registers involved in Vector computation on the chip for greater flexibility and better performance.
- Added SIMT [quick start](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_map_10_0022.html), [programming model](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_10_10064.html), and [operator implementation](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlasascendc_api_07_10293.html) introductions.
- Added SIMD and SIMT [hybrid programming model](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_10_10052.html), [operator implementation](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_10_10039.html), and [performance optimization](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_best_practices_10_10029.html) introductions.
- Added [SIMT API](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/API/ascendcopapi/atlasascendc_api_07_0427.html) documentation section.
- Added [compatibility migration guide](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_compatibility_10_00001.html) (220x architecture version migration to 351x architecture version).
- In Ascend community, Ascend C operator development added [visualization zone](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_map_10_0017.html), presenting Cube and Vector operator execution processes through videos.
---

## v9.0.0-beta.1
Release Date: 2026/02/25
| CANN Version | Version Source Tag |
|--|--|
| [CANN 9.0.0-beta.1](https://www.hiascend.com/developer/download/community/result?module=cann&cann=9.0.0-beta.1) | [v9.0.0-beta.1](https://gitcode.com/cann/asc-devkit/tags/v9.0.0-beta.1) |

### 🚀 Key Features

- Completed migration of [Gitee sample repository](https://gitee.com/ascend/samples/tree/master/operator/ascendc) AscendC samples and [AscendC high-level API repository](https://gitee.com/ascend/ascendc-api-adv/tree/master/examples) samples to [asc-devkit repository](https://gitcode.com/cann/asc-devkit/tree/9.0.0-beta.1/examples), and unified <<<>>> invocation method;
- Fusion compilation and <<<>>> invocation support [CPU mode](https://gitcode.com/cann/asc-tools/pull/138) and [SIM simulation mode](https://gitcode.com/cann/asc-devkit/blob/9.0.0-beta.1/cmake/asc/asc_modules/CMakeASCInformation.cmake).
- Ascend C project adds support for Kirin X90 and Kirin 9030 processors. Harmony developers can refer to [cann-recipes-harmony-infer](https://gitcode.com/cann/cann-recipes-harmony-infer) for Ascend C practice cases.
---

## v8.5.0
Release Date: 2026/01/23
| CANN Version | Version Source Tag |
|--|--|
| [CANN 8.5.0](https://www.hiascend.com/developer/download/community/result?module=cann&cann=8.5.0) | [v8.5.0](https://gitcode.com/cann/asc-devkit/tags/v8.5.0) |

### 🚀 Key Features

Based on Atlas A3 training series products/Atlas A3 inference series products, Atlas A2 training series products/Atlas A2 inference series products fully open source, including the following new features:
- Implemented repository separation and packaging, supporting independent installation and deployment of separate packages, including core repository asc-devkit for Ascend C operator development, debugging tool repository [asc-tools](https://gitcode.com/cann/asc-tools), Vector operator template library repository [atvc](https://gitcode.com/cann/atvc) and [atvoss](https://gitcode.com/cann/atvoss), Python frontend repository [pyasc](https://gitcode.com/cann/pyasc).
- Programming API capability expansion
  - Added language extension layer C API, providing industry-similar programming experience.
  - Basic API added LocalMemAllocator memory allocation interface.
- Full support for heterogeneous compilation and <<<>>> direct invocation, enabled through file suffix ".asc" or compilation option "-x asc".
- Standardized operator compilation CMake interfaces, providing Cmake module interfaces supporting different compilation scenarios.
- Supported CPU&NPU twin debugging capability consistency, one set of code supporting both CPU and NPU debugging.
- Comprehensive optimization of programming guide.
- Added operator samples.
