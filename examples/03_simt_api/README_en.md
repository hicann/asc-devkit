# Example List
|  Directory Name                                                   |  Description                                              |
| ------------------------------------- | ------------------------------------- |
| [00_introduction](./00_introduction) | Simple examples based on Ascend C SIMT programming, implementing custom operators through Ascend C programming language, providing corresponding <<<>>> direct invocation implementations, suitable for beginners. |
| [01_utilities](./01_utilities) | Debugging and tuning examples based on Ascend C SIMT, introducing system tool usage methods such as on-board printing, timing, and CPU twin debugging through printf(), assert(), clock() and other APIs, suitable for debugging phase. |
| [02_features](./02_features) | Examples demonstrating key features of Ascend C SIMT programming, including basic programming, memory resource management, thread synchronization, atomic operations, and other features. |
| [03_best_practices](./03_best_practices) | Demonstrating best practices for Ascend C SIMT programming. |
| [05_troubleshooting](./05_troubleshooting) | Examples demonstrating fault analysis and issue localization for Ascend C SIMT programming. |

## npu-arch Build Option Description

Developers need to modify the `--npu-arch` build option in the CMakeLists.txt file in the specific example directory according to the actual execution environment. Refer to the corresponding relationship in the table below to modify to the npu-arch parameter value corresponding to the environment.
| Product Model |  npu-arch Parameter |
| ---- | ---- |
| Ascend 950PR/Ascend 950DT | --npu-arch=dav-3510 |
