# Introduction 
This example project demonstrates the standard usage of the Extended Direct Memory Access (EXDMAC) HAL module APIs on an RX MCU. It employs the General PWM Timer (GPT) and Software Trigger as activation sources for EXDMAC transfer to control LEDs. The LEDs reflect the behavior of different EXDMAC transfer modes, including Normal Mode, Repeat Mode, Block Mode, and Repeat-Block Mode.   

**Note**: 
- To display information, the user can select between the Debug Virtual Console and the Serial Terminal (UART) with USB Serial COM. It is important to note that the user should only operate a single Terminal Application at a time to avoid conflicts or data inconsistencies. For instructions on how to switch between these options, please refer to the section [`Special Topics`](#special-topics)/[`Changing Terminal Application`](#changing-terminal-application).

- By default, EP information is printed to the host PC using the Serial Terminal (UART) for boards that support USB Serial COM. LED3 is turned ON to indicate the UART initialized unsuccessfully.

- RX boards that support USB Serial COM: EK-RX74M, EK-RX74N.

## Required Resources ##
To build and run the EXDMAC example project, the following resources are needed.

### Software ###

* Renesas Flexible Software Package (FSP): Version 1.0.0.
* e2 studio: Version 2026-07.
* Renesas GCC Compiler for RX: Version 14.2.0.202607.
* Renesas CC-RX Compiler: Version v3.08.00.
* Terminal Console Application: Tera Term or a similar application.

### Hardware ###

* 1 x Renesas RX board.
* 2 x Type-C USB cables for programming and debugging.

### Hardware Connections ###

* Connect the RX board USB debug port (J8) to the PC via a Type-C USB cable for EP debugging (If E2 Lite on-board is used).
* Connect the RX board USB Serial port (J27) to the PC using a Type-C USB cable to verify logs through the Terminal Application.

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of the operation of this example project:
- [FSP User Manual on GitHub (Reference link)](https://renesas.github.io/fsp/)
- [FSP Known Issues (Reference link)](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
  
![EXDMAC](images/EXDMAC_hld.png "High Level Block Diagram")


## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
| EXDMAC | Extended Direct Memory Access (EXDMAC) is a peripheral used for efficient data transfer. | r_exdmac|
| GPT | The GPT module is used to generate trigger events for EXDMAC transfers | r_gpt|

## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different than those selected by default.


**Configuration Properties for using GPT (g_timer)**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stacks > g_timer Timer, General PWM (r_gpt) > Properties > Settings > Property > Module g_timer Timer, General PWM (r_gpt) > General > Mode	| Periodic | One-Shot | Select mode. |
| configuration.xml > Stacks > g_timer Timer, General PWM (r_gpt) > Properties > Settings > Property > Module g_timer Timer, General PWM (r_gpt) > General > Period	| 0x10000 | 100 | Select Period. |
| configuration.xml > Stacks > g_timer Timer, General PWM (r_gpt) > Properties > Settings > Property > Module g_timer Timer, General PWM (r_gpt) > General > Period Unit	| Raw Counts | Milliseconds | Period is defined as a raw count instead of a time unit such as milliseconds or seconds. |
| configuration.xml > Stacks > g_timer Timer, General PWM (r_gpt) > Properties > Settings > Property > Module g_timer Timer, General PWM (r_gpt) > Interrupts > Callback	| NULL | g_timer_callback | A user callback function can be specified here. If this callback function is specified, it will be called from the interrupt service routine (ISR) each time the corresponding interrupt occurs. | 
| configuration.xml > Stacks > g_timer Timer, General PWM (r_gpt) > Properties > Settings > Property > Module g_timer Timer, General PWM (r_gpt) > Interrupts > Overflow/Crest Interrupt Priority | Disable | Priority 20 | Select the overflow interrupt priority. This is the crest interrupt for triangle-wave PWM. | 

**Configuration Properties for using EXDMAC (g_transfer_normal)**
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stack > g_transfer_normal Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_normal Transfer (r_exdmac) > Channel | 0 | 0 |	Specify the hardware channel. |
| configuration.xml > Stack > g_transfer_normal Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_normal Transfer (r_exdmac) > Mode | Normal | Normal | Select the transfer mode. |
| configuration.xml > Stack > g_transfer_normal Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_normal Transfer (r_exdmac) > Transfer Size | 2 Bytes | 4 Bytes | Select the transfer size. |
| configuration.xml > Stack > g_transfer_normal Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_normal Transfer (r_exdmac) > Destination Address Mode | Fixed | Incremented | Select the address mode for the destination.|
| configuration.xml > Stack > g_transfer_normal Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_normal Transfer (r_exdmac) > Source Address Mode | Fixed | Fixed | 	Select the address mode for the source. |
| configuration.xml > Stack > g_transfer_normal Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_normal Transfer (r_exdmac) > Number of Transfers | 1 | 1 | Specify the number of transfers for repeat and normal mode or block size for repeat-block transfer mode. |
| configuration.xml > Stack > g_transfer_normal Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_normal Transfer (r_exdmac) > Activation Source | Software Trigger | ID:X GPT0 COUNTER OVERFLOW (Overflow interrupt) | Select the EXDMAC transfer start event. |
| configuration.xml > Stack > g_transfer_normal Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_normal Transfer (r_exdmac) > Interrupt Frequency | Interrupt after all transfers have completed | Interrupt after all transfers have completed | Select to have interrupt after each transfer or after last transfer.|

**Configuration Properties for using EXDMAC (g_transfer_repeat)**
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stack > g_transfer_repeat Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_repeat Transfer (r_exdmac) > Channel | 0 | 1 |	Specify the hardware channel. |
| configuration.xml > Stack > g_transfer_repeat Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_repeat Transfer (r_exdmac) > Mode | Normal | Repeat | Select the transfer mode. |
| configuration.xml > Stack > g_transfer_repeat Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_repeat Transfer (r_exdmac) > Transfer Size | 2 Bytes | 4 Bytes | Select the transfer size. |
| configuration.xml > Stack > g_transfer_repeat Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_repeat Transfer (r_exdmac) > Destination Address Mode | Fixed | Fixed | Select the address mode for the destination.|
| configuration.xml > Stack > g_transfer_repeat Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_repeat Transfer (r_exdmac) > Source Address Mode | Fixed | Incremented | Select the address mode for the source. |
| configuration.xml > Stack > g_transfer_normal Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_repeat Transfer (r_exdmac) > Number of Transfers | 1 | 2 | Specify the number of transfers for repeat and normal mode or block size for repeat-block transfer mode. |
| configuration.xml > Stack > g_transfer_repeat Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_repeat Transfer (r_exdmac) > Activation Source | Software Trigger | Software Trigger | Select the EXDMAC transfer start event. |
| configuration.xml > Stack > g_transfer_repeat Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_repeat Transfer (r_exdmac) > Transfer End Interrupt Priority | Disabled | Priority 28 | Select the transfer end interrupt priority. |
| configuration.xml > Stack > g_transfer_repeat Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_repeat Transfer (r_exdmac) > Interrupt Frequency | Interrupt after all transfers have completed | Interrupt after all transfers have completed | Select to have interrupt after each transfer or after last transfer.|

**Configuration Properties for using EXDMAC (g_transfer_block)**
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stack > g_transfer_block Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_block Transfer (r_exdmac) > Channel | 0 | 0 |	Specify the hardware channel. |
| configuration.xml > Stack > g_transfer_block Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_block Transfer (r_exdmac) > Mode | Normal | Block | Select the transfer mode. |
| configuration.xml > Stack > g_transfer_block Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_block Transfer (r_exdmac) > Transfer Size | 2 Bytes | 4 Bytes | Select the transfer size. |
| configuration.xml > Stack > g_transfer_block Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_block Transfer (r_exdmac) > Destination Address Mode | Fixed | Incremented | Select the address mode for the destination.|
| configuration.xml > Stack > g_transfer_block Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_block Transfer (r_exdmac) > Source Address Mode | Fixed | Incremented | Select the address mode for the source. |
| configuration.xml > Stack > g_transfer_block Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_normal Transfer (r_exdmac) > Number of Transfers | 1 | 60 | Specify the number of transfers for repeat and normal mode or block size for repeat-block transfer mode. |
| configuration.xml > Stack > g_transfer_block Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_normal Transfer (r_exdmac) > Number of Blocks | 1 | 15 | Specify the number of blocks to transfer in Repeat, Block or Repeat-Block mode. |
| configuration.xml > Stack > g_transfer_block Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_block Transfer (r_exdmac) > Activation Source | Software Trigger | ID:X GPT0 COUNTER OVERFLOW (Overflow interrupt) | Select the EXDMAC transfer start event. |
| configuration.xml > Stack > g_transfer_block Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_block Transfer (r_exdmac) > Interrupt Frequency | Interrupt after all transfers have completed | Interrupt after each block, or repeat size is transferred | Select to have interrupt after each transfer or after last transfer.|

**Configuration Properties for using EXDMAC (g_transfer_repeat_block)**
|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > Stack > g_transfer_repeat_block Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_repeat_block Transfer (r_exdmac) > Channel | 0 | 0 |	Specify the hardware channel. |
| configuration.xml > Stack > g_transfer_repeat_block Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_repeat_block Transfer (r_exdmac) > Mode | Normal | Repeat-Block | Select the transfer mode. |
| configuration.xml > Stack > g_transfer_repeat_block Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_repeat_block Transfer (r_exdmac) > Transfer Size | 2 Bytes | 4 Bytes | Select the transfer size. |
| configuration.xml > Stack > g_transfer_repeat_block Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_repeat_block Transfer (r_exdmac) > Destination Address Mode | Fixed | Incremented | Select the address mode for the destination.|
| configuration.xml > Stack > g_transfer_repeat_block Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_repeat_block Transfer (r_exdmac) > Source Address Mode | Fixed | Incremented | 	Select the address mode for the source. |
| configuration.xml > Stack > g_transfer_repeat_block Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_repeat_block Transfer (r_exdmac) > Activation Source | Software Trigger | ID:X GPT0 COUNTER OVERFLOW (Overflow interrupt) | Select the EXDMAC transfer start event. |
| configuration.xml > Stack > g_transfer_repeat_block Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_repeat_block Transfer (r_exdmac) > Interrupt Frequency | Interrupt after all transfers have completed | Interrupt after all transfers have completed | Select to have interrupt after each transfer or after last transfer.|
| configuration.xml > Stack > g_transfer_repeat_block Transfer (r_exdmac) > Properties > Settings > Property > Module g_transfer_repeat_block Transfer (r_exdmac) > Source Buffer Size | 1 | 10 | Specify the size of whole source buffer (valid only for Repeat-Block transfer mode with source address update mode other than offset addition). |

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
| R_EXDMAC_Open | This API is used to initialize the EXDMAC module. |
| R_EXDMAC_Enable | This API is used to enable the EXDMAC transfers on the specified activation source. |
| R_EXDMAC_Close | This API is used to de-initialize the EXDMAC module. |
| R_EXDMAC_SoftwareStart | This API is used to trigger a transfer by software. |
| R_GPT_Open |	This API is used to initialize the GPT module. |
| R_GPT_Start | This API is used to start the GPT module. |
| R_GPT_Stop | This API is used to stop the GPT module. |
| R_GPT_Close | This API is used to de-initialize the GPT module. |
| R_IOPORT_B_PinWrite |	This API is used to set pin's output high or low. |

**For using the Serial Terminal (UART)**
| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| R_SCI_B_UART_Open | This API is used to configure the UART driver based on the input configurations. |
| R_SCI_B_UART_Write | This API is used to transmit a user-specified number of bytes from the source buffer pointer. |
| R_SCI_B_UART_Close | This API is used to abort any in progress transfers, disables interrupts, receiver, and transmitter, close lower level transfer drivers if used and remove power. |

## Verifying operation ## 

1. Import the example project.

2. Generate, and build the example project.

3. Before running the example project, make sure hardware connections are completed.

4. Connect the RX MCU to the host PC via debugger. By default, EP will use E2lite debugger.

5. Open a Terminal Application (e.g., Tera Term or Debug Virtual Console) on the host PC and connect to the RX board’s USB Serial port.  
   * Note: For using the Serial Terminal Application:
        * To echo back typed content in Tera Term, go to [Setup] -> [Terminal...] and check [Local echo].
        * The configuration parameters of the serial port are:
            - COM port: Provided by the USB Serial COM.
            - Baud rate: 115200 bps
            - Data length: 8 bits
            - Parity: None
            - Stop bit: 1 bit
            - Flow control: None 

6. User can perform Menu option operations and check corresponding results on the Terminal Application.

### Below images showcase the output log on the Terminal Application (Tera Term):
  **Display information of the EP**  

  ![EXDMAC info](images/EXDMAC_info.png "Display information of the EP")  

  **Below is output when running with the Normal mode option**

  ![EXDMAC normal mode](images/EXDMAC_normal_mode.png "Normal mode")
  
  **Below is output when running with the Repeat mode option**

  ![EXDMAC repeat mode](images/EXDMAC_repeat_mode.png "Repeat mode")

  **Below is output when running with the Block mode option**

  ![EXDMAC block mode](images/EXDMAC_block_mode.png "Block mode")

  **Below is output when running with the Repeat-Block mode option**

  ![EXDMAC repeat-block mode](images/EXDMAC_repeat_block_mode.png "Repeat-Block mode")

## Special Topics ##

### LED Behavior ###

- Option 1 (Normal Mode), 3 (Block Mode), 4 (Repeat-Block Mode): LED2 will turn ON for 2 seconds if the data is verified successfully and then turn OFF.

- Option 2 (Repeat Mode): LED1 will turn ON/OFF alternately when user selects option 2.

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

  **Note:** Connect the RX board debugger port (J9) to the host PC via E2 Emulator Lite (with USB cable and 20-20 pin cable).

### Adding lowsrc Folder

Before changing Terminal Application settings, you need to add the `lowsrc` folder from another Example Project:

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

5. **Changing the Terminal Application to use the Debug Virtual Console:**
   
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