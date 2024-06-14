# clock-overlay

Clock overlay for desktops.

## Requirements

This application requires Python v3.12 (or higher) to be installed. It was developed on and for
Windows (x64), though other common operating systems are probably supported too (not tested).

## Setup

Assuming your environment meets the requirements stated above, install like so on Windows:

1. clone this repository and `cd` into it
2. create a virtual environment (venv): `python -m venv .\.venv`
3. activate the virtual environment: `.\.venv\Scripts\Activate.ps1`
4. install clock in the virtual environment: `pip install -e .`

The executable is at `.\.venv\Scripts\clock-overlay.exe`.

## Controls

You can move the window by holding the left mouse button and moving the mouse. To reset the window to
its origin position, press the right mouse button. Press the middle mouse button to quit.

## Attributions

The following resources have been authored by other people:

| Resource                          | Attribution                                    |
| --------------------------------- | ---------------------------------------------- |
| [App icon](clock/icons/clock.ico) | [Designed by Freepik](https://www.freepik.com) |

Thank you.
