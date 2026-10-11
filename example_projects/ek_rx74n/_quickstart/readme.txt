/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates some of the capabilities of the EK and FSP by using multiple peripherals.
	The project initializes the OSPI, UART various timers and GPIO modules along with various supporting stacks
	to create an interactive user demo.

	There is a serial console which can be activated by connecting USB cable to USB SERIAL (J27),
	connection settings are Speed 115200, Data 8-bit, Parity None, Stop 1-bit, Flow Control None.
	Once application is running the user can control the board by button presses on the board and see regularly
	updated board configuration on the kit information screen using the serial console, please read the following
	r20qs0094eg Evaluation Kit for RX74N Microcontroller Group EK-RX74N v1 Quick Start Guide.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 1.0.0
	e2 studio: Version 2026-07
	Terminal Console Application: Tera Term or a similar application
	USB Serial Drivers (included in Windows 11)
	Windows® 11 operating system
  
3. Hardware settings for the project:
	Link to access Quick Start Guide - https://www.renesas.com/us/en/document/qsg/ek-rx74n-quick-start-guide

4. Configuration Settings:
	Please explore the configuration settings of each module in the RX configurator.
 