# Organizing Multiple Operator Packages

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T13:46:19.071Z pushedAt=2026-11-03T07:05:13.401Z nl=2 -->

This document is supplementary content that describes how to decide the release and deployment method for multiple custom operators—whether to place all of them in a single operator package or split them into multiple operator packages.

In this document, "operator package" refers to the custom operator installation package `custom_opp_*.run` generated after compilation. An operator package can contain one or more OpTypes and, after deployment, is usually located in the `vendors/<vendor_name>` directory. OpType is the operator type used when registering the operator prototype, for example, `AddCustom` in `OP_ADD(AddCustom)`.

This document mainly discusses "how to split multiple operators into one or more operator packages." To learn how to compile multiple operators into a single operator package and how to organize the Host, Kernel, and Tiling files of multiple operators, see "Directory Structure by Operator" in [Basic Compilation and Deployment Process](./basic_process.md#compilation-organization).

## Default Recommendation

By default, it is recommended to place them in the same operator package. Only when operators have different update frequencies, need to be compiled and deployed independently, or are maintained by different business teams should you consider splitting them into multiple operator packages.

| Scenario | Recommended Strategy | Description |
|------|---------|------|
| A small number of operators maintained by the same team | Single operator package | The compilation, deployment, and verification process is the simplest. |
| Multiple operators belonging to the same business, released and regressed together | Single operator package | Reduces the number of operator packages and avoids additional priority configuration. |
| Basic operators and experimental operators have different update frequencies | Multiple operator packages | The stable package and the experimental package can be independently compiled, deployed, and rolled back. |
| Maintained separately by different business teams | Multiple operator packages | Each team maintains an independent package, integrated through priority and deployment paths. |

## Differences Between a Single Operator Package and Multiple Operator Packages

| Comparison Item | Single Operator Package | Multiple Operator Packages |
|--------|------------|------------|
| Number of artifacts | One `custom_opp_*.run` | Multiple `custom_opp_*.run` files |
| `vendor_name` | Usually only one. | Each package can use a different `vendor_name`. |
| Build and Deployment | Compiled once and deployed once. | Each package is compiled and deployed independently. |
| Priority configuration | Usually not required. | Required when the same OpType conflicts. |
| Applicable scenario | A batch of operators released together. | Operators with different lifecycles, priorities, or maintenance boundaries. |

A single operator package does not mean that it can contain only one operator. It means that these operators are packaged into the same installation package, deployed to the same vendor directory, and released as a whole.


## Scenarios Suitable for Splitting into Multiple Operator Packages

### Different Update Frequencies

Basic operators are rarely updated once they become stable, whereas experimental operators or business-customized operators may be iterated frequently. Splitting them into different operator packages reduces the impact scope of each modification.

Example:

- Basic operator package: contains stable general-purpose operators with a low update frequency;
- Experimental operator package: contains operators that are still being tuned or verified, with a high update frequency;
- Business-customized operator package: serves only a specific business scenario and is released and rolled back independently.

### Different Maintenance Boundaries

Operators maintained by different business teams are generally not recommended to be mixed in the same operator package. After splitting them into multiple operator packages, each team can independently manage its own source code, versions, deployment pace, and issue rollback.

## Priority Configuration

When multiple custom operator packages coexist, only operators that contain the same OpType involve priority selection. Operators with different OpTypes do not overwrite each other due to `load_priority`.

It is not recommended to keep multiple versions of the same OpType coexisting for a long time as a regular deployment method. If temporary coexistence is indeed required and versions are selected by priority, ensure that the artifacts of each version, such as the host-side library, kernel binary, and operator information, are complete and consistent, and complete regression verification in the target scenario. Otherwise, when artifacts such as `.o` of a high-priority version are incomplete, the runtime may mix in artifacts of a low-priority version, causing version misuse or abnormal behavior.

In the default installation scenario, configure the priority of each vendor through `load_priority` in `opp/vendors/config.ini`. In the specified-directory installation scenario, control the priority through the execution order of the `set_env.bash` script in the installation directory of each operator package.

For details about the configuration method, see [Configuring Custom Operator Priority](./basic_process.md#configuring-custom-operator-priority).

## Related Documents

- [Basic Compilation and Deployment Process](./basic_process.md) — Compilation configuration, deployment process, and directory organization within the package.
- [Compiling Operator Dynamic and Static Libraries](./dynamic_static_lib_compilation.md) — Output operators as dynamic or static libraries.
