# SafeBeat-Infant-Monitor
 This embedded software project utilizes CCS Theia and the TI CC2651P3 MCU to develop an advanced monitoring system that tracks and analyzes ECG (electrocardiogram) and respiratory data in infants, with the goal of preventing infant sleep apnea.  

# CCS Theia
In order to build and load the software onto the CC2651P3 MCU, the CCTUDIO-THEIA IDE was used for this project.  

The download link for the IDE can be found on TI's website at the link below. Once CCS Theia is installed, the project can be added to the active workspace.  

CCS Theia -> https://www.ti.com/tool/download/CCSTUDIO-THEIA  

# Setting up Version Control in CCS Theia (Devs Only)
Alternative ways to setup version control are likely possible but the following instructions for this project use the GitHub desktop client.  
1. Download GitHub Desktop here -> https://desktop.github.com/download/
2. Connect/Create GitHub account if prompted.
3. Once downloaded, open and click "Clone repository from the internet" to clone the project repo.
4. Change the local path of the clone to the CCS Theia workspace, the directory will default to the following filepath -> C:\Users\[username]\workspace_ccstheia
5. Click "clone" and the cloned repo should be insid eyour CCS Theia workspace ready to use. The source control tab is on the left similar to VS Code. This can be used for the various version control commands.  

# SIMPLELINK SDK Download
The software development kit (SDK) for the CC2651P3 MCU was downloaded from TI's website in order to utilize the SDK's software support. If the SDK is not installed some header files will not be included in the main.c source file. Do not change installation directory of the SDK during the installation proccess.  
CC2651P3 on TI website -> https://www.ti.com/product/CC2651P3#software-development  

# TI Drivers
The drivers for the TI microcontroller are located in the directory \SafeBeat-Infant-Monitor\Debug\sysconfig. The header files from the SDK are located in the install directory \ti\simplelink_cc13xx_cc26xx_sdk_7_41_00_17  
ti_drivers_config.c contains the GPIO pin configurations and names.  
GPIO.h contains GPIO functions included with the SDK.  
SPI.h contains SPI functions included with the SDK.  
