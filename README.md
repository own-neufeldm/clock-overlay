# clock-overlay

Clock overlay for desktops.

## Requirements

This application requires [Python v3.12](https://www.python.org/downloads/) and
[Poetry](https://python-poetry.org/) with its
[Dynamic Versioning plugin](https://github.com/mtkennerly/poetry-dynamic-versioning) to be
installed. To follow the [setup](#Setup) guide step by step, you will also need
[PowerShell 7](https://github.com/PowerShell/PowerShell).

The app was developed on and for Windows 10 (x64), though other common operating systems are
probably supported too (not tested).

## Setup

Assuming your environment meets the requirements stated above, you can install the app like so:

```
$ poetry install

Installing dependencies from lock file
Installing the current project: clock (1.0.2)
```

The app is now installed in a virtual environment and can be run using `poetry run clock-overlay`.
To make `clock-overlay` available outside the virtual environment, you can create a function in your
user profile (`$Profile`), e.g.:

```
function clock-overlay() {
    $Scripts = "POETRY_PATH\Scripts"

    & "$Scripts\pythonw.exe" "$Scripts\clock-overlay" $args
}
```

Replace `POETRY_PATH` with the actual path of your poetry environment (`poetry env info -p`). You
have to reload your shell or source the user profile for the function to be recognized, e.g.:

```
$ . $Profile
```

You can now run the app simply by running `clock-overlay`.

Furthermore, you can add a shortcut to your Start Menu so the app can be run from there. Do so by
creating a directory, e.g. `Clock Overlay`, in your Start Menu path
`$Home\AppData\Roaming\Microsoft\Windows\Start Menu\Programs`. Within your new directory, create a
shortcut, e.g. `Clock Overlay`, to the following target:
`"C:\Program Files\PowerShell\7\pwsh.exe" -c clock-overlay`. You can set the
[icon from this repository](./clock/assets/icon.ico) as the shortcut icon.

## Usage

You can move the window by holding the left mouse button and moving the mouse. To reset the window to
its origin position, press the right mouse button. Press the middle mouse button to quit.

## Attributions

The following resources have been authored by other people:

| Resource                          | Attribution                                    |
| --------------------------------- | ---------------------------------------------- |
| [App icon](clock/icons/clock.ico) | [Designed by Freepik](https://www.freepik.com) |

Thank you.
