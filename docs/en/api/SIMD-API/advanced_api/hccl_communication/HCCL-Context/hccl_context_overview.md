# HCCL Context Overview

<!-- md-trans-meta sourceCommit=6024fbdeffe5f05ee90ea6f5e52eb8c784f03267 translatedAt=2026-09-10T02:10:24.953Z -->

The APIs in this chapter are used to set or obtain, on the kernel side, the context (message area) address corresponding to each communication domain of a communication-computation fusion operator. You must first call [HcclGroup](../../../../Utils-API/prototype_register_management/OpMC2Def/HcclGroup.md) on the host side to configure the communication domain name before you can call [GetHcclContext](GetHcclContext.md) to obtain the corresponding context address.

> [!NOTE]
> This API is experimental and may be adjusted or improved in later versions, with no guarantee of backward compatibility. Pay attention to updates in later versions when using it.

