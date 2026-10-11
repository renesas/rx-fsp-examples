# Introduction 
This example demonstrates typical usage of the CMT HAL module APIs.  
Users can input values via the Terminal console menu to select different CMT-supported modes (One-Shot, Periodic) and input type (Period (ms) or Frequency (Hz)).  
The example project operates in:  
- Demo 1: One-Shot mode - Compare Match: LED1 turns on for a user-defined period or frequency, then turns off automatically.  
- Demo 2: Periodic mode - Compare Match: LED2 blinks based on the user-defined period or frequency.   

**Note**: 
- To display information, the user can select between Debug Virtual Console and the Serial Terminal (UART) with USB Serial COM. It is important to note that the user should only operate a single terminal application at a time to avoid conflicts or data inconsistencies. For instructions on how to switch between these options, please refer to the section [`Special Topics`](#special-topics)/[`Changing Terminal Application`](#changing-terminal-application).

- By default, EP information is printed to the host PC using the Serial Terminal (UART) for boards that support USB Serial COM. LED3 is turned ON to indicate the UART initialized unsuccessfully.

- RX boards that support USB Serial COM: EK-RX74M, EK-RX74N.

## Required Resources ##
To build and run the CMT example project, the following resources are needed.

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
- Configure SW6 as follow to use OB Debugger: SW6-Pin1 OFF, SW6-Pin2 OFF.
- For EK-RX74M/EK-RX74N: No additional wires need to be connected.

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of the operation of this example project:
- [FSP User Manual on GitHub (Reference link)](https://renesas.github.io/fsp/)
- [FSP Known Issues (Reference link)](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
  
![CMT](images/CMT_hld.png "High Level Block Diagram")


## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
|CMT | CMT (Compare Match Timer) is a timer peripheral used for generating periodic interrupts or measuring time intervals. | r_cmt|


## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different than those selected by default.

**Configuration Properties for using CMT (g_timer_one_shot_compare_match)**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stack > g_timer_one_shot_compare_match Timer Driver on r_cmt > Properties > Settings > Property > Module g_timer_one_shot_compare_match Timer Driver on r_cmt > General > Channel | 0 | 0 | Specify the hardware channel. |
| configuration.xml > Stack > g_timer_one_shot_compare_match Timer Driver on r_cmt > Properties > Settings > Property > Module g_timer_one_shot_compare_match Timer Driver on r_cmt > General > Mode | Periodic | One-shot | Mode selection. |
| configuration.xml > Stack > g_timer_one_shot_compare_match Timer Driver on r_cmt > Properties > Settings > Property > Module g_timer_one_shot_compare_match Timer Driver on r_cmt > General > Period | 0x10000 | 10 | Specified the timer period based on the selected unit. |
| configuration.xml > Stack > g_timer_one_shot_compare_match Timer Driver on r_cmt > Properties > Settings > Property > Module g_timer_one_shot_compare_match Timer Driver on r_cmt > General > Period Unit | Raw Counts | Seconds | Unit of the period specified. |
| configuration.xml > Stack > g_timer_one_shot_compare_match Timer Driver on r_cmt > Properties > Settings > Property > Module g_timer_one_shot_compare_match Timer Driver on r_cmt > Interrupts > Callback | NULL | one_shot_timer_callback | A user call back function. |
| configuration.xml > Stack > g_timer_one_shot_compare_match Timer Driver on r_cmt > Properties > Settings > Property > Module g_timer_one_shot_compare_match Timer Driver on r_cmt > Interrupts > Compare Match Interrupt Priority | Disabled | Priority 15 (highest) | Select the interrupt priority. |

**Configuration Properties for using CMT (g_timer_periodic_compare_match)**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stack > g_timer_periodic_compare_match Timer Driver on r_cmt > Properties > Settings > Property > Module g_timer_periodic_compare_match Timer Driver on r_cmt > General > Channel | 0 | 1 | Specify the hardware channel. |
| configuration.xml > Stack > g_timer_periodic_compare_match Timer Driver on r_cmt > Properties > Settings > Property > Module g_timer_periodic_compare_match Timer Driver on r_cmt > General > Mode | Periodic | Periodic | Mode selection. |
| configuration.xml > Stack > g_timer_periodic_compare_match Timer Driver on r_cmt > Properties > Settings > Property > Module g_timer_periodic_compare_match Timer Driver on r_cmt > General > Period | 0x10000 | 10 | Specified the timer period based on the selected unit. |
| configuration.xml > Stack > g_timer_periodic_compare_match Timer Driver on r_cmt > Properties > Settings > Property > Module g_timer_periodic_compare_match Timer Driver on r_cmt > General > Period Unit | Raw Counts | Seconds | unit of the period specified. |
| configuration.xml > Stack > g_timer_periodic_compare_match Timer Driver on r_cmt > Properties > Settings > Property > Module g_timer_periodic_compare_match Timer Driver on r_cmt > Interrupts > Callback | NULL | periodic_timer_callback | A user call back function. |
| configuration.xml > Stack > g_timer_periodic_compare_match Timer Driver on r_cmt > Properties > Settings > Property > Module g_timer_periodic_compare_match Timer Driver on r_cmt > Interrupts > Compare Match Interrupt Priority | Disabled | Priority 15 (highest) | Select the interrupt priority. |

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

The table below lists the CMT APIs used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| R_CMT_Open | This API is used to initialize the CMT module. |
| R_CMT_Start  |This API is used to start CMT module. |
| R_CMT_Close |This API is used to de-initialize the CMT module. |
| R_CMT_StatusGet | This API is used get current timer status and store it in provided pointer p_status. |
| R_CMT_InfoGet |This API is used get timer information and store it in provided pointer p_info. |
| R_CMT_PeriodSet |This API is used sets period value provided. |
| R_IOPORT_PinWrite | This API is used sets a pin's output either high or low. |

**For using the Serial Terminal (UART)**
| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| R_SCI_B_UART_Open | This API is used to configures the UART driver based on the input configurations. |
| R_SCI_B_UART_Write | This API is used to transmit user specified number of bytes from the source buffer pointer. |
| R_SCI_B_UART_Close | This API is used to abort any in progress transfers, disables interrupts, receiver, and transmitter, close lower level transfer drivers if used and remove power. |
## Verifying operation ## 
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

6. User can perform Menu option operations and check corresponding results on the Terminal Application.

   Please refer section [`Limitation`](#limitation). 

### Below images showcases the output log on the Terminal Application (Tera Term):
   **Display information of the EP**  
   ![CMT_log_print_EP_info.png](images/CMT_log_print_EP_info.png "Display information of the EP")  

   **Run the EP with One Shot - Compare Match, input type is ***Period(ms)*****  
   ![CMT_log_oneshot_input_ms.png](images/CMT_log_oneshot_input_ms.png "Run the EP")  

   **Run the EP with One Shot - Compare Match, input type is ***Frequency(Hz)*****  
   ![CMT_log_oneshot_input_hz.png](images/CMT_log_oneshot_input_hz.png "Run the EP")  

   **Run the EP with Periodic - Compare Match, input type is ***Period(ms)*****  
   ![CMT_log_periodic_input_ms.png](images/CMT_log_periodic_input_ms.png "Run the EP")  

   **Run the EP with Periodic - Compare Match, input type is ***Frequency(Hz)*****  
   ![CMT_log_periodic_input_hz.png](images/CMT_log_periodic_input_hz.png "Run the EP")  

## Special Topics ##
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

## Limitation ##
   * The ***input type*** (millisecond or Hz) must be an integer within 15 bytes.  
   * Operation is not guaranteed for non-integer inputs (e.g., float, char, special characters) via the terminal.  
   * The input ***period value (ms)*** should be in the range of **1 to 400 ms** for easier observation of LED behavior.  
   * The input ***frequency value (Hz)*** should be in the range of **3 to 50 Hz** to facilitate LED behavior observation and pulse width display on the terminal.  
