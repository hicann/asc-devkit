# Programming API Overview<a name="ZH-CN_TOPIC_0000001976190182"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-26T12:15:32.262Z pushedAt=2026-09-06T03:05:09.556Z -->

>[!NOTE] Note
>This chapter provides an overview of the programming APIs. For details about specific APIs, see [Ascend C API](../../../api/api_list.md).

Ascend C provides a set of library APIs. Developers use standard C++ syntax and library APIs for programming. The following figure shows the Ascend C programming library APIs, which are classified into:

-   **Language extension layer C API**: Opens up the complete chip programming capabilities, supports array memory allocation, is generally pointer-based, and provides a C language programming experience consistent with the industry.

-   **Basic API**: Abstracts hardware capabilities and opens up chip capabilities to ensure completeness and compatibility. APIs marked as Instruction Set Architecture Special Interface (ISASI) cannot guarantee cross-hardware-version compatibility.
-   **High-level API**: Implements some common computing algorithms to improve programming and development efficiency, and usually calls multiple basic APIs for implementation. High-level APIs include math libraries, Matmul, Softmax, and other APIs. High-level APIs guarantee compatibility.
-   **Utils API (public helper functions)**: A rich set of general-purpose utility classes covering standard libraries, platform information acquisition, runtime compilation, and log output, helping developers efficiently implement operator development and performance optimization.

![](../../figures/growth_map_9.png)

>[!NOTE] Note
>The header file directories of the Ascend C APIs are as follows:
>- Basic API: $\{INSTALL\_DIR\}/include/ascendc/basic\_api/interface
>- High-level API: (Note that if the interfaces contained in the header files in the following directories are not declared in the documentation, they are indirect call interfaces and developers do not need to pay attention to them.)
>    - $\{INSTALL\_DIR\}/include/ascendc/highlevel\_api/lib
>    - $\{INSTALL\_DIR\}/include/tiling
>Replace $\{INSTALL\_DIR\} with the file storage path after the CANN software is installed. For example, if the software is installed by the root user, the default file storage path is **/usr/local/Ascend/cann**.
>The library files that the Ascend C APIs depend on are described as follows:
>- Basic API: Not involved.
>- High-level API: Because the high-level API is paired with the Host Tiling interface, **libtiling\_api.a** needs to be linked.
