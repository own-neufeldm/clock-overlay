# clock-overlay

Clock overlay for desktops.

## Requirements

The following dependencies must already be installed on your system:

| Dependency                    | Version     |
| ----------------------------- | ----------- |
| [CMake](https://cmake.org/)   | ^3.29       |
| [vcpkg](https://vcpkg.io/en/) | ^2025.01.13 |

This application was built on and for Windows 11 using Visual Studio's C++ toolset. The following
guide assumes that you have this toolset installed and that you are running the below commands in
a Developer Command Prompt. Please refer to the official vendor documentation for more information.

## Setup

Build the app using CMake with the `vcpkg` preset:

```pwsh
$env:VCPKG_ROOT="<root directory of your local vcpkg repository>"
cmake --preset=vcpkg
cmake --build build
```

Run the app with `.\build\clock-overlay.exe`.

## Usage

You can move the window by holding the left mouse button and moving the mouse. To reset the window
to its default position, press the right mouse button. Press the middle mouse button to quit.

Additionally, you can change the window's opacity by holding CTRL and scrolling the mouse wheel.

## Attributions

The following resources have been authored by other people:

| Resource                    | Attribution                                    |
| --------------------------- | ---------------------------------------------- |
| [App icon](assets/icon.ico) | [Designed by Freepik](https://www.freepik.com) |

Thank you.
