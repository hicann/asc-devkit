# Error "Failed to Open config.ini" Is Reported When Calling an Operator<a name="ZH-CN_TOPIC_0000002129648797"></a>

<!-- md-trans-meta sourceCommit=e2a2709368f65375f9b4d904990c7884199bd22e translatedAt=2026-10-23T14:58:14.800Z pushedAt=2026-11-03T07:05:13.447Z nl=2 -->

## Symptoms<a name="section151611254194612"></a>

After a custom operator package is installed and deployed, the following error message indicating a failure to obtain the json file is displayed when a deployed operator is called:

```
[INFO] Start get path and read binary_info_config.json.
[WARNING] Get jsonfile path for */binary_info_config.json failed, errmsg:No such file or directory.
[ERROR] Get path and read binary_info_config.json failed, please check if the opp_kernel package is installed!
```

Based on the preceding error message, the failure to obtain the json file is caused by the inability to open **config.ini** in the preceding process. The prompt message is as follows:

```
[INFO] Start to get opp kernel base path, default custom opp kernel is in ASCEND_OPP_PATH.
[INFO] The real path of config.ini is */opp/vendors/config.ini.
[WARNING]  Can not open file: */opp/vendors/config.ini.
```

## Root Cause<a name="section417961104715"></a>

The root cause is that the user who calls the operator lacks read permission on the **config.ini** file (\*/opp/vendors/config.ini) in the operator package deployment directory. The default permission of the **config.ini** file is 640, which allows access only to the deployment user and users in the same group. The current execution user is not in the same group as the installation user and therefore lacks read permission, causing the operator call to fail.

For example, the following scenario causes an execution error when the operator is called: the root user installs and deploys a custom operator package, and a user in the HwHiAiUser group calls the deployed custom operator.

## Handling Procedure<a name="section166318242419"></a>

Contact the user who installed the custom operator package to change the permission of **config.ini** to 644:

```
chmod 644 config.ini
```
