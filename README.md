# SafeBeat-Infant-Monitor
 This embedded software project utilizes CCS Theia and the TI CC2651P3 MCU to develop an advanced monitoring system that tracks and analyzes ECG (electrocardiogram) and respiratory data in infants, with the goal of preventing infant sleep apnea.  

# CCS Theia
In order to build and load the software onto the CC2651P3 MCU, the CCTUDIO-THEIA IDE was used for this project.  

The download link for CCS Theia can be found on [TI's Website](https://www.ti.com/tool/download/CCSTUDIO-THEIA). Once CCS Theia is installed, the project can be added to the active workspace.  

# Kernel & Compiler
When creating a project in CCS Theia you will be prompoted to select what microcontroller is being used, and then a kernel and compiler must be chosen for the project. For this project the kernel used is (TODO: FreeRTOS or TI-RTOS7) and the compolier used is the CCS - TI Arm Clang Compiler

# Setting up Version Control in CCS Theia (Devs Only)
Alternative ways to setup version control are likely possible but the following instructions for this project use the GitHub desktop client.
1. Make sure the workspace in CCS Theia is setup with the correct name you want.
2. Download GitHub Desktop [here](https://desktop.github.com/download/).
3. Connect/Create GitHub account if prompted.
4. Once downloaded, open and click "Clone repository from the internet" to clone the project repo. Make sure when a workspace and repository are selected, the local path looks like the picture below with the workspace name being the last item in the directory (When you select a repo it will try to make folder in the workspace with the repo name, but it messes up version control).

<p align="center">
<img src=https://i.imgur.com/EB93kvP.png height="40%" width="40%" alt="Login Screen"/>
<br />
<br />

 5. Click "clone" and the cloned repo should be inside your CCS Theia workspace ready to use. The source control tab is on the left similar to VS Code. This can be used for the various version control commands.  
 6. Once the repo is cloned to the workspace the project should appear like in the picture below. From here you can try to build the project, if everything is installed correctly it should build successfully.

<p align="center">
<img src=https://i.imgur.com/viacCcD.png height="40%" width="40%" alt="Login Screen"/>
<br />
<br />

# SIMPLELINK SDK Download
The software development kit (SDK) for the CC2651P3 MCU was downloaded from TI's website in order to utilize the SDK's software support. If the SDK is not installed some header files will not be included in the main.c source file. Do not change installation directory of the SDK during the installation proccess.  
1. Download the [SDK](https://www.ti.com/product/CC2651P3#software-development) from the TI website. 
2. The default directory for the SDK should be C:\ti\simplelink_cc13xx_cc26xx_sdk_7_41_00_17
3. This directory will be added to the project in CCS Theie by navigating to the workspace with the cloned repo (Open the workspace from File -> Open Folder... -> Selecting the Workspace folder)
4. Once you are in the workspace add the SDK by navigating to File -> Preferences -> Code Composer Studio Settings... -> General (On left) -> Products
5. Once in the products window click the + in the top right and navigate to the SDK install location and click it... mine is C:\ti\simplelink_cc13xx_cc26xx_sdk_7_41_00_17
7. The SDK should show up in the Product Discovery Path Window like in the image below

<p align="center">
<img src=https://i.imgur.com/Sq5JZnz.png height="40%" width="40%" alt="Login Screen"/>
<br />
<br />
 
# Project Setup
All source files that can be edited are included in each projects src folder. The GitHub repo is set up so both projects can be compiled separately, since each will be loaded onto a separate MCU. Within the src folder, common.h contains any used libraries, header files, and macros used by the main project. Some of the header files in common.h include SDK driver inclusions that are required, while other header files are custom ones that can be changed. Within common.h the required SDK headers are shown in the first image below.
<br />
<br />
FOR ANY INCLUSIONS FROM THE SDK, THE DRIVER FOR IT MUST BE CONFIGURED USING "main.sysconfig" FOR THAT PROJECT. This is shown in the second image below, and any drivers that need to be configured can be done using the sysconfig GUI.
For example, in the first image we include the GPIO.h driver and SPI.h driver from the SDK, so when you go into the main.sysconfig GUI, both GPIO and SPI should have a green check by it, if they do not, the project will not compile.

<p align="center">
<img src=https://i.imgur.com/zj9DDPJ.png height="40%" width="40%" alt="Login Screen"/>
<br />
<br />

<p align="center">
<img src=https://i.imgur.com/wvmYZUw.png height="100%" width="100%" alt="Login Screen"/>
<br />
<br />

# Naming Standard
![image](https://github.com/user-attachments/assets/c1a33758-17f2-4f18-b4ec-6e7b8ec317fd)

# Helpful Links
[TI's SimpleLink Low Power F2 SDK Repository](https://github.com/TexasInstruments/simplelink-lowpower-f2-sdk)
<br />
[BLE5 Stack Example Repository](https://github.com/TexasInstruments/simplelink-ble5stack-examples)
