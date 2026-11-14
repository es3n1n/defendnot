# defendnot 适配说明（Windows 10.0.26300 / 26H2 + VS 2026）

本文件记录为「让本仓库能在当前这台电脑上编译并运行」所做的适配与验证结果。

## 1. 本机环境

| 项目 | 值 |
| --- | --- |
| 系统版本 | Windows `10.0.26300`（注册表 `DisplayVersion = 26H2`，ProductType=1 工作站）|
| 架构 | x64 (AMD64) |
| 编译器 | Visual Studio Community 2026（18.11），MSVC `14.51` → 工具集 `v145` |
| Windows SDK | `10.0.26100.0` |
| WSC 服务 | `wscsvc` 正在运行；`WinDefend` 正在运行，篡改保护(Tamper Protection)=开启 |

## 2. 改动内容

1. **工具集适配（关键）**
   三个工程原本硬编码 `PlatformToolset = v143`（VS 2022 的工具集），而 VS 2026 只提供了 `v145`，
   直接构建会报 “无法找到 v143 生成工具”。
   已将下列文件中的 `v143` 全部改为 `$(DefaultPlatformToolset)`，从而自动跟随所安装的 VS：

   - `defendnot/defendnot.vcxproj`
   - `defendnot-loader/defendnot-loader.vcxproj`
   - `wsc-binary-check/wsc-binary-check.vcxproj`

   > 这样在 VS 2022（v143）和 VS 2026（v145）上都能编译。

2. **补齐子模块**：`vendor/argparse`（header-only，加载器解析命令行需要）。

3. **新增 `build.ps1`**：一键定位 MSBuild 并构建（默认 Release x64，`/m:1` 串行构建）。

源码逻辑未做任何功能性修改——原因见下一节。

## 3. 兼容性验证（为什么不需要改 WSC 代码）

defendnot 的核心是调用 WSC 的 ISV COM 接口注册一个“假杀毒软件”，从而让 Defender 自动让位。
新版 Windows 曾经担心过接口变化会导致 `CoCreateInstance` / `QueryInterface` 失败，因此做了实测：

- 本机注册表 `HKCR\Interface` 中仍存在 `IWscAVStatus`、`IWscAVStatus2/3/4`、`IWscASStatus`、`IWscFWStatus`、`IWscFWStatus2`；
  **最新的 AV 接口仍是 `IWscAVStatus4`**，与代码里使用的完全一致。
- 通过 `CoCreateInstance(CLSID_WscIsv)` + `QueryInterface` 在本机实测（未调用任何注册方法）：

  ```
  CoCreateInstance(CLSID_WscIsv, IUnknown) = 0x00000000 (S_OK)
  IWscAVStatus4  : OK (interface present)
  IWscAVStatus   : OK (interface present)
  IWscASStatus   : OK (interface present)
  IWscFWStatus2  : OK (interface present)
  ```

- 注入目标 `C:\Windows\System32\Taskmgr.exe`：签名有效（Microsoft Windows），
  且 PE `DllCharacteristics` 含 `ForceIntegrity (0x80)`——这正是 WSC 校验通过的必要条件。

**结论：本机上 WSC 的 ABI/接口与 v1.6.0 源码完全兼容，无需修改 `core/com.hpp` 里的接口定义。**
真正妨碍在本机跑起来的，只是编译工具集（已修复）以及运行前提（见第 5 节）。

## 4. 构建

任选其一：

```powershell
# 方式 A：使用附带脚本（推荐）
.\build.ps1                 # Release x64
.\build.ps1 -Platform ARM64 # 其它平台

# 方式 B：手动调用 MSBuild
& "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\amd64\MSBuild.exe" `
    defendnot.sln /p:Configuration=Release /p:Platform=x64 -t:rebuild /m:1 /verbosity:minimal
```

> 注意：在受限沙箱里 `/m`（并行）可能因并发编译器进程被限制而失败，请用 `/m:1` 串行构建；
> 在普通 PowerShell 里 `/m` 正常。也可以直接用 Visual Studio 打开 `defendnot.sln` 按 F7 生成。

构建产物（`out\x64\`）：

- `defendnot.dll` —— 注入到目标进程里的核心模块
- `defendnot-loader.exe` —— 加载器（带 `requireAdministrator` 清单）
- `wsc-binary-check.exe` —— 辅助诊断工具（扫描 System32 中满足 ForceIntegrity+签名 的进程）

## 5. 运行前提（重要，否则一定失败）

本机 Defender 的实时保护与篡改保护均处于开启状态，**会直接拦截/查杀未经签名的 defendnot 二进制**。
实测本机编译出的未签名 exe 会被 Defender 以“包含病毒或潜在垃圾软件”为由阻止运行。
因此运行前必须按顺序：

1. **关闭篡改保护（Tamper Protection）**：Windows 安全中心 → 病毒和威胁防护 → 管理设置 → 篡改保护 → 关。
2. **关闭实时保护（Real-time protection）**。
3. 若开启了 **Smart App Control**（智能应用控制），也需要关闭：
   `windowsdefender://appbrowser/`。
4. **以管理员身份**运行：

   ```powershell
   # 以管理员身份打开 PowerShell，进入产物目录：
   cd .\out\x64
   .\defendnot-loader.exe            # 默认 AV 名称 dnot.sh
   .\defendnot-loader.exe --name "MyAV" --firewall   # 自定义名称 + 假防火墙
   .\defendnot-loader.exe --silent --disable-autorun # 静默、不写自启
   .\defendnot-loader.exe --disable  # 卸载 / 恢复
   .\defendnot-loader.exe --verbose  # 详细日志（便于排错）
   ```

> `defendnot-loader.exe` 必须和 `defendnot.dll` 放在同一目录。

可选：defendnot 生效后，可再以管理员运行 `extra-strip.bat` 进一步关闭更多 Defender 策略项。

## 6. 风险与许可

该工具会通过 WSC 注册假杀软来使 Windows Defender 停止工作，属于修改系统安全状态的高风险操作。
请仅在**你拥有且被授权**的机器上使用，并自行承担一切后果；禁止用于传播恶意软件或任何非法用途
（见仓库 `LICENSE` 与 README 的 Permitted Use Notice）。
