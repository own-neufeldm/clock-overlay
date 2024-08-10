# clock-overlay

Clock overlay for desktops.

## Requirements

The following dependencies must already be installed on your system:

| Dependency                                  | Version |
| ------------------------------------------- | ------- |
| [python](https://www.python.org/downloads/) | ^3.12   |
| [pipx](https://pipx.pypa.io/stable/)        | ^1.6    |

This app was written on and for Windows 10 (x64). It may work on other operating systems but it is
not officially supported.

## Setup

Install the app using `pipx`, e.g. directly from GitHub using SSH:

```
$ pipx install git+ssh://git@github.com/own-neufeldm/clock-overlay.git

  installed package clock 1.0.3, installed using Python 3.12.5
  These apps are now globally available
    - clock-overlay.exe
done! ✨ 🌟 ✨
```

You can now run the app using `clock-overlay`.

Additionally, you can configure a shortcut to run the app from your Start Menu without opening a
Terminal first. To do so, create a shortcut named `Clock Overlay` in the
`C:\ProgramData\Microsoft\Windows\Start Menu\Programs` directory. Point it to the following target:
`pwsh.exe -w hidden -c clock-overlay`. You can use [this asset](./clock/assets/icon.ico) as icon.

> [!NOTE]
> If you do not have PowerShell 7 (`pwsh`) installed, use `powershell.exe` instead.

You can now run the app from your Start Menu using `Clock Overlay`.

## Usage

You can move the window by holding the left mouse button and moving the mouse. To reset the window to
its origin position, press the right mouse button. Press the middle mouse button to quit.

## Attributions

The following resources have been authored by other people:

| Resource                          | Attribution                                    |
| --------------------------------- | ---------------------------------------------- |
| [App icon](clock/icons/clock.ico) | [Designed by Freepik](https://www.freepik.com) |

Thank you.
