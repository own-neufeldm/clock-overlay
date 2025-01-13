# clock-overlay

Clock overlay for desktops.

## Requirements

tbc

## Setup

Build the project using `cmake` (example with MSYS2 MinGW64 on Windows 11):

```pwsh
mkdir .\build
cmake `
  -DCMAKE_BUILD_TYPE:STRING=Debug `
  -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE `
  -DCMAKE_C_COMPILER:FILEPATH=C:\msys64\ucrt64\bin\gcc.exe `
  -DCMAKE_CXX_COMPILER:FILEPATH=C:\msys64\ucrt64\bin\g++.exe `
  -S. `
  -B.\build `
  -G "MinGW Makefiles" `
  --no-warn-unused-cli

# repeat this step every time you change source code
cmake --build build
```

Run the project with `.\build\clock-overlay.exe`.

## Usage

You can move the window by holding the left mouse button and moving the mouse. To reset the window
to its origin position, press the right mouse button. Press the middle mouse button to quit.

## Attributions

The following resources have been authored by other people:

| Resource                    | Attribution                                    |
| --------------------------- | ---------------------------------------------- |
| [App icon](assets/icon.ico) | [Designed by Freepik](https://www.freepik.com) |

Thank you.
