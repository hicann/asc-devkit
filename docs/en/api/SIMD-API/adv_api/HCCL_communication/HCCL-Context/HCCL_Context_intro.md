# HCCL Context Introduction

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-02T06:00:19.161Z pushedAt=2026-11-18T04:58:29.402Z nl=2 -->

The APIs in this chapter are used to set/obtain the context (message area) address corresponding to each communicator of the communication-computation fusion operator on the kernel side. You must first call the [HcclGroup](../../../../Utils-API/prototype_register_management/OpMC2Def/HcclGroup.md) API on the host side to configure the communicator name before calling [GetHcclContext](GetHcclContext.md) to obtain the corresponding context address.

> [!NOTE] Note
>This API is an experimental API and may be adjusted or improved in later versions, without guaranteeing backward compatibility. Developers are advised to keep track of updates in later versions during use.

