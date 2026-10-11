# Introduction #

The example project demonstrates the typical use of the CAC HAL module APIs. On any key press from Terminal Application, CAC module starts measuring the accuracy of selected target clock against the selected reference clock. Result of the measurement is displayed on Terminal Application.

**Note**: 
- To display information, the user can select between Debug Virtual Console and the Serial Terminal (UART) with USB Serial COM. It is important to note that the user should only operate a single terminal application at a time to avoid conflicts or data inconsistencies. For instructions on how to switch between these options, please refer to the section [`Special Topics`](#special-topics)/[`Changing Terminal Application`](#changing-terminal-application).

- By default, EP information is printed to the host PC using the Serial Terminal (UART) for boards that support USB Serial COM. LED3 is turned ON to indicate the UART initialized unsuccessfully.

- RX boards that support USB Serial COM: EK-RX74M, EK-RX74N. 
## Required Resources ##

To build and run the CAC example project, the following resources are needed.

### Software ###
* Renesas Flexible Software Package (FSP): Version 1.0.0.
* e2 studio: Version 2026-07.
* Renesas GCC Compiler for RX: Version 14.2.0.202607.
* Renesas CC-RX Compiler: Version v3.08.00.
* Terminal Console Application: Tera Term or a similar application.

### Hardware ###
Supported RX boards: EK-RX74M, EK-RX74N.

* 1 x Renesas RX board.
* 2 x Type-C USB cables for programming and debugging.

### Hardware Connections ###

- Connect the RX board USB debug port (J8) to the PC via a Type-C USB cable for EP debugging.
- Connect the RX board USB SERIAL port (J27) to the PC using a Type-C USB cable to verify logs through the terminal console.

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of the operation of this example project:
- [FSP User Manual on GitHub (Reference link)](https://renesas.github.io/fsp/)
- [FSP Known Issues (Reference link)](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##

![cac](images/cac_hld.png "High Level Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
| CAC | This module is used to measure and calibrate internal clock accuracy by comparing it to an external reference clock source. | r_cac |
| CGC | This module is used to create and manage clock sources for the entire MCU system. | r_cgc |

## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different than those selected by default.

**Common Configuration Properties**
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|   configuration.xml > BSP > Properties > Settings > Property > RX Common > Heap size (bytes)  |  0   | 0x400  |  Heap size is required for standard library functions to be used. |

**Configuration Properties for CAC**
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stacks > g_cac Clock Accuracy Circuit (r_cac) > Properties > Settings > Property > Module g_cac Clock Accuracy Circuit (r_cac) > Reference clock source | Main Oscillator | MOCO | Select the reference clock source. |
| configuration.xml > Stacks > g_cac Clock Accuracy Circuit (r_cac) > Properties > Settings > Property > Module g_cac Clock Accuracy Circuit (r_cac) > Upper Limit Threshold | 0 | 211 | Top end of allowable range for measurement completion. |
| configuration.xml > Stacks > g_cac Clock Accuracy Circuit (r_cac) > Properties > Settings > Property > Module g_cac Clock Accuracy Circuit (r_cac) > Lower Limit Threshold | 0 | 173 | Bottom end of allowable range for measurement completion. |
| configuration.xml > Stacks > g_cac Clock Accuracy Circuit (r_cac) > Properties > Settings > Property > Module g_cac Clock Accuracy Circuit (r_cac) > Frequency Error Interrupt Priority | Disabled | Priority 28 | Select CAC frequency error interrupt priority. |
| configuration.xml > Stacks > g_cac Clock Accuracy Circuit (r_cac) > Properties > Settings > Property > Module g_cac Clock Accuracy Circuit (r_cac) > Measurement End Interrupt Priority | Disabled | Priority 29 | Select CAC measurement end interrupt priority. |
| configuration.xml > Stacks > g_cac Clock Accuracy Circuit (r_cac) > Properties > Settings > Property > Module g_cac Clock Accuracy Circuit (r_cac) > Overflow Interrupt Priority | Disabled | Priority 28 | Select CAC overflow interrupt priority. |
| configuration.xml > Stacks > g_cac Clock Accuracy Circuit (r_cac) > Properties > Settings > Property > Module g_cac Clock Accuracy Circuit (r_cac) > Callback | NULL | cac_callback | Assign function name for using callback. |

**Configuration Properties for using the Serial Terminal (UART)**   
|   Configure Clock Path                  |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Clocks > Clocks Configuration > SCICLK Disable | SCICLK Disable | SCICLK Src: HOCO | Enable operating clock for SCI module by HOCO clock source. |

|   Configure Interrupt Event Path        |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Interrupts > Interrupts Configuration > New User Event > RSCI > RSCI12 > RSCI12 RXI (Receive data full) | empty | sci_b_uart_rxi_isr | Assign the UART receive ISR (Receive data full) to the interrupt vector table. |
| configuration.xml > Interrupts > Interrupts Configuration > New User Event > RSCI > RSCI12 > RSCI12 TXI (Transmit data empty) | empty | sci_b_uart_txi_isr | Assign the UART transfer ISR (Transfer data empty) to the interrupt vector table. |
| configuration.xml > Interrupts > Interrupts Configuration > New User Event > RSCI > RSCI12 > 105-6 GROUPAL1 RSCI12 TEI (Transmit end) | empty | sci_b_uart_tei_isr | Assign the UART transfer ISR (Transfer end) to the interrupt vector table. |
| configuration.xml > Interrupts > Interrupts Configuration > New User Event > RSCI > RSCI12 > 105-7 GROUPAL1 RSCI12 ERI (Receive error) | empty | sci_b_uart_eri_isr | Assign the UART receive ISR (Receive error) to the interrupt vector table. |

## API Usage ##

The table below lists the APIs used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| R_CAC_Open | This API is used to initialize the CAC module. |
| R_CAC_StartMeasurement | This API is used to start the CAC measurement process. |
| R_CAC_Read | This API is used to read and return the CAC status and counter registers. |
| R_CAC_StopMeasurement | This API is used to stop the CAC measurement process. |
| R_CAC_Close | This API is used to de-initialize the CAC module. |
| R_CGC_Open | This API is used to initialize the CGC module. |
| R_CGC_ClockStart | This API is used to start the specified clock. |
| R_CGC_ClockCheck | This API is used to stop the specified clock. |
| R_CGC_Close | This API is used to de-initialize the CGC module. |

**For using the Serial Terminal (UART)**
| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| R_SCI_B_UART_Open | This API is used to configures the UART driver based on the input configurations. |
| R_SCI_B_UART_Write | This API is used to transmit user specified number of bytes from the source buffer pointer. |
| R_SCI_B_UART_Close | This API is used to abort any in progress transfers, disables interrupts, receiver, and transmitter, close lower level transfer drivers if used and remove power. |

## Verifying Operation ##
1. Import the example project.

2. Generate, and build the example project.

3. Before running the example project, make sure hardware connections are completed.

4. Connect the RX MCU to the host PC via debugger. By default, EP will use E2lite debugger.
5. Open a terminal application (e.g., Tera Term or Debug Virtual Console) on the host PC and connect to the RX board’s USB Serial port.  
   * Note: For using the Serial Terminal application:
        * To echo back typed content in Tera Term, go to [Setup] -> [Terminal...] and check [Local echo].
        * The configuration parameters of the serial port are:
            - COM port: Provided by the USB Serial COM.
            - Baud rate: 115200 bps
            - Data length: 8 bits
            - Parity: None
            - Stop bit: 1 bit
            - Flow control: None 

### Below images showcases the output log on the Terminal Application (Tera Term):

![cac ouput](images/cac_output.png "EP cac")

## Special Topics ##

### Note ###

* The user may select any reference clock and clock divider.
* The user may select any measurement clock and clock divider.
* The user needs to ensure that the Main clock/Sub-clock oscillator is available on the board before selecting it.
* The user must set upper limit and lower limit in configurator based on the selected reference and measurement clocks.

### Calculate Threshold (Upper/Lower) limits ###

* Get Measurement clock frequency with clock divider i.e. say 'M'.
  - For ex. select HOCO (48MHZ on EK-RX74M) and clock divider as '1', then M = 48MHZ.

* Get Reference clock frequency with clock divider i.e. say 'R'.
  - For ex. select MOCO (8MHZ on EK-RX74M) and clock divider as '32', then R = (8MHZ/32).

* Calculate count value, say 'C'.
  - i.e. C = (M/R), so C = 192.

* Take a tolerance of 10%, say 'T'.
  - i.e. T = (C * (10/100)) = C/10 = 19.2.

* Upper limit Threshold = (count value + tolerance) = (C + T) = 211.2.

* Lower limit Threshold = (count value - tolerance) = (C - T) = 172.8.

### Debugger Setting ###

* To use E2 Lite on-board or E2 Emulator Lite, please follow the instructions below:

1. **Common step**: Click [**Run**] → [**Debug Configurations…**] to open the “**Debug Configurations**” window.  
  ![Debug Configuration](images/Debug_configuration_common.png)

2. **Select file application**  
  a. **Used for projects built with GCC**: --> Select file **.elf**.  
  ![Debug Configuration](images/Debug_configuration_GCC_select_file_application.png)  
  b. **Used for projects built with CCRX**: --> Select file **.x**.  
  ![Debug Configuration](images/Debug_configuration_CCRX_select_file_application.png)

3. **Setting debugger configurations**  
  a. **Used for E2 Lite on-board**.    
  ![Debug Configuration](images/Debug_configuration_E2lite_onboard_setting_debugger.png)  
  b. **Used for external E2 Emulator Lite**.  
  ![Debug Configuration](images/Debug_configuration_E2lite_emulator_setting_debugger.png)  

  **Note:** Connect the RX board debugger port (J9) to the host PC via E2 Emulator Lite (With USB cable and 20-20 pin cable).

### Adding lowsrc Folder

Before changing terminal application settings, you need to add the `lowsrc` folder from another Example Project:

1. **Locate the lowsrc folder:**
   
   - Navigate to Example Project `sci_uart` or `usb_pcdc_with_freertos`  (`example_projects/<board>/sci_uart/e2studio/gccrx_sci_uart_<board>_ep/src`)
   - Find the `lowsrc` folder containing low-level source files.

2. **Copy `lowsrc` folder to this Example Project:**
   
   - Copy the entire `lowsrc` folder.
   - Paste it into this your target Example Project directory: `example_projects/ek_rx74m/<EP>/e2studio/`.

3. **Add lowsrc to e2studio project:**
   
   - In e2studio, right-click on the project in Project Explorer.
   - Select **Refresh** (or press F5) to detect the new folder.

4. **Verify the folder is included:**
   
   - Expand the project tree in Project Explorer.
   - Confirm that `lowsrc` folder and its contents are visible.
   - The folder should be included in the build path with source files ready to compile.

5. **Changing the terminal Application to use the Debug Virtual Console:**
   
   - For instructions on how to switch between these options, please refer to the section [`Special Topics`](#special-topics)/[`Changing Terminal Application`](#changing-terminal-application).

6. **Re-build the project:**
   
   - In e2studio, right-click on the project and select **Build Project**.

**Note:** To use Debug Virtual Console, the `lowsrc` folder must be added to the project, as it contains the source files required for Debug Virtual Console operation.

### Changing Terminal Application ###
By default, the EP supports Serial Terminal for RX boards that support USB Serial COM   
**For use CCRX Compiler:**
* Define USE_VIRTUAL_COM=1 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings -> Compiler -> Source -> Macro definition (-define)

To use Debug Virtual Console, please follow the instructions below:
* Define USE_VIRTUAL_COM=0 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings -> Compiler -> Source -> Macro definition (-define)
* Define BSP_CFG_IO_LIB_ENABLE=1 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings -> Compiler -> Source -> Macro definition (-define)

![USE_VIRTUAL_COM settings for CCRX](images/use_virtual_com_setting_ccrx.png "USE_VIRTUAL_COM settings CCRX")

**For use GCCRX Compiler:**  
* Define USE_VIRTUAL_COM=1 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings -> Compiler -> Includes -> Macro Defines (-D)
  
To use Debug Virtual Console, please follow the instructions below:
* Define USE_VIRTUAL_COM=0 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings -> Compiler -> Includes -> Macro Defines (-D)

![USE_VIRTUAL_COM settings for GCCRX](images/use_virtual_com_setting_gccrx.png "USE_VIRTUAL_COM settings GCCRX")
