# clock-overlay

Clock overlay for desktops.

## Working with submodules

* add a submodule in current directory: `git submodule add <url>`
* initialize all submodules recursively: `git submodule update --init --recursive --remote --merge`
* update all submodules recursively: `git submodule update --recursive --remote --merge`
* remove a submodule: follow [instructions](https://stackoverflow.com/questions/1260748/how-do-i-remove-a-submodule) as befits needs

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
