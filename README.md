# defendnot

An even funnier way to disable windows defender.

Defendnot is a successor of [no-defender](https://github.com/es3n1n/no-defender).

![](https://i.imgur.com/VGE8g6a.jpeg)

## Installation

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

The initial implementation of [no-defender](https://github.com/es3n1n/no-defender) used thirdparty code provided by other AVs to register itself in the WSC, while defendnot interacts with WSC directly.

## Limitations

Sadly, to keep this WSC stuff even after reboot, defendnot adds itself to the autorun. Thus, you would need to keep the defendnot binaries on your disk :(

## Writeup

[How I ruined my vacation by reverse engineering WSC](https://blog.es3n1n.eu/posts/how-i-ruined-my-vacation/)

## Special thanks

* [mrbruh](https://mrbruh.com) for poking me to research this topic
* [pindos](https://github.com/pind0s) for providing their machine for WSC service debugging

## License

Apache-2.0
