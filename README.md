# defendnot

<img src="https://i.imgur.com/F9gWA92.png" align="right" width="50%"/>

An even funnier way to disable windows defender.

> [!CAUTION]
> **Permitted Use Notice**: 
> 
> Using this tool to facilitate malware distribution, or any illegal activity is strictly prohibited.
> 
> Users assume all legal responsibility for how they use this tool and any consequences thereof. You must comply with all applicable laws when using this tool.
> 
> By downloading, installing, or using this tool, you acknowledge that you agree to these terms.

## Installation

> [!WARNING]
> You may need to temporarily disable realtime and tamper protection before proceeding, otherwise defender will block defendnot binaries from running.
>
> On newer Windows 11 builds you may also have to turn off Smart App Control.
>
> - Real-time protection: `windowsdefender://threatsettings/`
> - Smart App Control: `windowsdefender://appbrowser/`

### One-liner

Open the powershell as administrator and execute any of these:

```powershell
# Example 1: Basic installation
irm https://dnot.sh/ | iex

# Example 2: With custom AV name and firewall
& ([ScriptBlock]::Create((irm https://dnot.sh/))) --name "Custom AV name" --firewall

# Example 3: Without allocating console
& ([ScriptBlock]::Create((irm https://dnot.sh/))) --silent

# Example 4: Run once, without allocating console
& ([ScriptBlock]::Create((irm https://dnot.sh/))) --silent --disable-autorun

# Example 5: Uninstall
& ([ScriptBlock]::Create((irm https://dnot.sh/))) --disable
```

### Manual

Download the [latest](https://github.com/es3n1n/defendnot/releases/latest) release, extract it somewhere and launch `defendnot-loader`.

## Usage

```commandline
Usage: defendnot-loader [--help] [--version] [--name VAR] [--disable] [--verbose] [--firewall] [--silent] [--autorun-as-user] [--disable-autorun]

Optional arguments:
  -h, --help         prints help message and exits
  --version          shows version and exits
  -n, --name         av display name [default: "dnot.sh"]
  -d, --disable      disable defendnot
  -v, --verbose      verbose logging
  --firewall         also register a fake firewall
  --silent           do not allocate console
  --autorun-as-user  create autorun task as currently logged in user
  --disable-autorun  disable autorun task creation
```

## Stripping Defender out further (optional)

defendnot only registers a fake AV through WSC, but if you want to strip more Defender stuff out, run the optional `extra-strip.bat` as admin after defendnot is active. It disables a bunch more Defender policies in registry: real-time monitoring, behavior monitoring, cloud reporting, signature updates, etc.

It's a separate script because the keys are finicky and undoing them on `--disable` would mean saving every value first somewhere and writing all of them back, which is _waaay_ more bookkeeping than I feel like implementing.

## Limitations

- **Needs to stay on disk:**  
  To keep the AV registration after reboot, defendnot adds itself to autorun.

## Writeup

[How I ruined my vacation by reverse engineering WSC](https://blog.es3n1n.eu/posts/how-i-ruined-my-vacation/)

## Special thanks

* [mrbruh](https://mrbruh.com) for poking me to research this topic
* [pindos](https://github.com/pind0s) for providing their machine for WSC service debugging

## License

Apache-2.0
