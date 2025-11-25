<!---
Copyright 2023 CARIAD SE.

This Source Code Form is subject to the terms of the Mozilla
Public License, v. 2.0. If a copy of the MPL was not distributed
with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
-->

# Viewer Interface

## Link with the Interface

As described in the section "Using the ReportGUI" of [Using the Checker
Framework](using_the_framework.md), it is possible to show issues in external
viewers, if they have valid location information. In general ASAM Quality Checker
Framework doesn't provide its own 3D viewers but a C-interface to connect external
3D and format-specific viewers to the ReportGUI. The ReportGUI loads viewer plugins
in the form of shared libraries from the folder `bin/plugin`. A plugin needs to
implement the interface `include/viewer/iConnector.h` and provide it via common
C-API `__declspec(dllexport)` mechanism. The ReportGUI will load the library during
startup and use the viewer plugin to show issues where applicable.

The C-interface declares the following functions, which are called by the
ReportGUI:

```c
bool CanSupportFormat(const char* inputPath);
bool StartViewer();
bool Initialize(const char* inputPath);
bool AddIssue(void * issueToAdd);
bool CanShowIssue(void * itemToShow, void* locationToShow);
bool ShowIssue(void * itemToShow, void* locationToShow);
const char* GetName();
bool CloseViewer();
const char* GetLastErrorMessage();
```

These functions should be implemented inside your viewer library to make your
viewer compatible with the Report GUI. If the shared library is not loadable
or one of the functions isn't defined, the ReportGUI will show an error and the
viewer won't be available.

It is not possible to start multiple instances of one viewer plugin: Restarting
a viewer automatically closes the currently active instance of that viewer. A
viewer is started either via the context menu **File -> Start Viewer** (manually),
or automatically on startup based on the input file and the viewers claim to
support it, via `CanSupportFormat`. Note that multiple viewers of different
plugins can be active at the same time.

If a viewer is started the following functions are called in this order:

1. `CanSupportFormat` (further calls only if this function returns true)
2. `StartViewer`
3. `Initialize`
4. `AddIssue` (will be called for each issue found)

If an error occurs during the startup process, the ReportGUI will call
GetLastErrorMessage to print out the error in the ReportGUI itself.

`CanShowIssue` is called to check whether an Issue is viewable in the viewer.
If it returns true, then the viewer claims that it can show information for
the given issue (and location).

`ShowIssue` is triggered if you click on an issue in the ReportGUI for which
`CanShowIssue` returned true. It will send the clicked issue and its location
to the viewer.

If the ReportGUI is closed all currently active viewers receive the `CloseViewer`
call.

## Viewer Examples

A demo viewer is provided as source code under `examples/viewer_example`. When
you open the GUI, it looks like in the image above and you can start the viewer
from the File menu. Click on issues that represent a 3D error and find the
function calls inside the console window. Your own viewer interface
implementation can use this information to show the error in 3D.

A fully functional viewer based on the [esmini simulator](https://esmini.github.io/) 
is also provided as part of the ASAM Quality Checker Framework. Please refer to 
the [esmini viewer plugin](esmini_viewer_plugin.md) for details.

A user-configurable viewer that is based on executing external commands
to show issues in external viewers is provided as part of the ASAM
Quality Checker Framework. Please refer to the
[command viewer plugin](command_viewer_plugin.md) for details.
