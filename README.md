# defendnot

An even funnier way to disable windows defender.

Defendnot is a successor of [no-defender](https://github.com/es3n1n/no-defender).

![](https://i.imgur.com/epX8wkd.jpeg)

> [!CAUTION]
> **Permitted Use Notice**: 
> 
> Using this tool to facilitate malware distribution, cybercrime, unauthorized access, evading detection, or any illegal activity is strictly prohibited.
> 
> Users assume all legal responsibility for how they use this tool and any consequences thereof. You must comply with all applicable local, state, federal, and international laws when using this tool.
> 
> By downloading, installing, or using this tool, you acknowledge that you have read, understood, and agree to these terms.

## Installation

> [!TIP]
> You may need to temporarily disable realtime and tamper protection before proceeding, otherwise defender will block `defendnot` binaries due to the `VirTool:Win64/Defnot.A` detection. 

### One-liner

Open the powershell as administrator and execute any of these:

```powershell
# Example 1: Basic installation
irm https://dnot.sh/ | iex

# Example 2: With custom AV name
& ([ScriptBlock]::Create((irm https://dnot.sh/))) --name "Custom AV name"

# Example 3: Without allocating console
& ([ScriptBlock]::Create((irm https://dnot.sh/))) --silent
```

> [!NOTE]
> As seen in examples 2 and 3, you can pass the commandline arguments to the installer script and it will forward them to `defendnot-loader`. For reference what commandline arguments are allowed, see the `Usage` section below.

> [!NOTE]
> You can also directly use the 'longer' version of installer script url, which is `https://raw.githubusercontent.com/es3n1n/defendnot/refs/heads/master/install.ps1`

### Manual

Download the [latest](https://github.com/es3n1n/defendnot/releases/latest) release, extract it somewhere and launch `defendnot-loader`.

## Usage

```commandline
Usage: defendnot-loader [--help] [--version] [--name VAR] [--disable] [--verbose] [--silent] [--autorun-as-user] [--disable-autorun]

Optional arguments:
  -h, --help         prints help message and exits
  --version          shows version and exits
  -n, --name         av display name [default: "dnot.sh"]
  -d, --disable      disable defendnot
  -v, --verbose      verbose logging
  --silent           do not allocate console
  --autorun-as-user  create autorun task as currently logged in user
  --disable-autorun  disable autorun task creation
```

## How it works

There's a WSC (Windows Security Center) service in Windows which is used by antiviruses to let Windows know that there's some other antivirus in the hood and it should disable Windows Defender.  
This WSC API is undocumented and furthermore requires people to sign an NDA with Microsoft to get its documentation.

The initial implementation of [no-defender](https://github.com/es3n1n/no-defender) used thirdparty code provided by other AVs to register itself in the WSC, while `defendnot` interacts with WSC directly.

## Limitations

- **Needs to stay on disk:**  
  To keep the AV registration persistent after reboot, `defendnot` adds itself to autorun. That means the binaries have to remain on your system for the Defender "disable" to stick. (Yeah, I wish it were more elegant too.)
  
- **No support for Windows Server:**  
  The Windows Security Center (WSC) service doesn’t exist on Windows Server editions, so `defendnot` *won’t* work there. See [#17](https://github.com/es3n1n/defendnot/issues/17).

- **Defender Detection:**  
  Not surprisingly, Windows Defender really doesn’t like `defendnot` and will flag or remove it as `VirTool:Win64/Defnot.A`. You’ll need to (temporarily) disable Defender’s real-time and tamper protection to install.

## Legitimate Use Cases

- Reducing resource consumption in development environments
- Testing system performance under different security configurations
- Educational research on Windows security mechanisms
- Home lab experimentation and learning

> [!IMPORTANT]
> If your intended usage falls outside these legitimate use cases, support in issues/DMs might be denied without any further explanations.

## Writeup

[How I ruined my vacation by reverse engineering WSC](https://blog.es3n1n.eu/posts/how-i-ruined-my-vacation/)

## Special thanks

* [mrbruh](https://mrbruh.com) for poking me to research this topic
* [pindos](https://github.com/pind0s) for providing their machine for WSC service debugging

## License

Apache-2.0
