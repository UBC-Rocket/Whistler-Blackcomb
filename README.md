# README

## Prerequisites

To compile the fimware, you will need CMake and Ninja installed and available in the system path. GCC for the ARM microcontroller and OpenOCD for uploading or debugging are also used, but downloaded automatically by a CMake script.

### Windows

Scoop is a Windows package manager that makes it easy to install command-line tools. Follow the instructions at the bottom of [this page](https://scoop.sh) to install it.

If you have never used PowerShell scripts before, you will need to open up a new PowerShell terminal and copy and paste in the following command to enable scripts, which will ask you for a confirmation:

    Set-ExecutionPolicy RemoteSigned -scope CurrentUser

Next, install CMake and Ninja:

    scoop install cmake
    scoop install ninja

### Linux

Install CMake and Ninja using your system package manager.

## Drivers

In most cases, driver installation for the STLink is not needed. However, if you run into any trouble:

### Windows

First, try different USB ports. Some USB3 ports seem to not work properly. If you are still having trouble the correct drivers might not be selected. Download and run (Zadig)[https://zadig.akeo.ie/], select the STLink device, and change its driver to libusb-win32.

### Linux

If you have libusb installed, the STLink should work. However, you may need to add udev rules to allow your user account access to the device. Some rules can be found [here](https://github.com/texane/stlink/tree/master/etc/udev/rules.d), and you will need to figure out where they go depending on your distribution.

## Building

This project uses CMake to build code, allowing the same build system to support Windows, Linux, and other operating systems.
The build runs in two stages, configuration, where CMake gathers information about tools and files, and building, in which a build tool such as Ninja actually invokes the compiler. It is important to reconfigure when source files are added or removed. This also includes when pulling or switching branches on git.

### Available Targets:

* blink: Builds blink code
* upload-blink: Builds and uploads blink code to microcontroller

### Command line

From the repository directory, create a build folder:

    mkdir build
    cd build

From the build folder run this to configure the project:

    cmake .. -G Ninja

From the build folder, b uild a target (replace $TARGET with the target name):
    ninja $TARGET

### Visual Studio Code

If you do not already have a preferred text editor, Visual Studio code works well and provides CMake build support, C++ autocompletion, and debugging with a few extensions you should install:

 * The [Microsoft C/C++ extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) provides autocompletion and error detection.

 * The [CMake Tools extension](https://marketplace.visualstudio.com/items?itemName=vector-of-bool.cmake-tools) provides an interface to build CMake projects.

 * The [Cortex-Debug extension](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug) provides graphical debugging support for ARM microcontrollers.

It is important to open up the top-level directory of this repository so that settings get loaded correctly.

Clicking *CMake: ___: Ready* in the bottom toolbar will run configuration, and ask you to choose between debug or release settings. The first time, you will also be asked to choose a compiler, but for that select [Unspecified], as the correct one should later be selected automatically.

After CMake configuration runs, you can select which board to build for. In the bottom toolbar is some buttons. Clicking *⚙ Build* will build code, and just to the right is where you can select which target to build. (See above for target descriptions) Clicking *CMake: ___: Ready* will allow you to change build types by reconfiguring.

Need to reimplement:

~~After CMake runs, there will also be debug launch configurations automatically generated. Click on the debugger icon in the left side of the screen, choose which file to debug, and hit the ▶️ button to start debugging. (Make sure to build and upload the correct firmware first! If you try to debug code that is different from what is on the microcontroller, you will end up very confused.)~~

### CLion

Supposedly supports CMake projects natively but untested.

