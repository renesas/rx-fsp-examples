# Introduction 
This example project demonstrates the basic use of SDHI on Renesas RX MCUs. The project uses the SDHI driver to perform data writing and reading operations directly on the SD card. The example also includes functionality for clearing the buffer (erasing) from the SD card when needed. Unlike full file system operations, this example focuses solely on low-level block media operations such as write, read, and erase using the SDHI interface.

Note: 
* To display information, the user can select between the Debug Virtual Console and the Serial Terminal (UART) with USB Serial COM. It is important to note that the user should only operate a single Terminal Application at a time to avoid conflicts or data inconsistencies. For instructions on how to switch between these options, please refer to the section [`Special Topics`](#special-topics)/[`Changing Terminal Application`](#changing-terminal-application).

* By default, EP information is printed to the host PC using the Serial Terminal (UART) for boards that support USB Serial COM. LED3 is turned ON to indicate the UART initialized unsuccessfully.

* RX boards that support USB Serial COM: EK-RX74M.

## Required Resources  
To build and run the SDHI example project, the following resources are needed.

### Software 
* Renesas Flexible Software Package (FSP): Version 1.0.0.
* e2 studio: Version 2026-07.
* Renesas GCC Compiler for RX: Version 14.2.0.202607.
* Renesas CC-RX Compiler: Version v3.08.00.
* Terminal Console Application: Tera Term or a similar application.

### Hardware 
* 1 x Renesas RX board.
* 2 x Type-C USB cables for programming and debugging.
* 1 x Micro SD Card.

## Hardware Connections  
* Connect the RX board USB debug port (J8) to the PC via a Type-C USB cable for EP debugging (If E2 Lite on-board is used).
* Connect the RX board USB Serial port (J27) to the PC using a Type-C USB cable to verify logs through the Terminal Application.

- **SDHI Connection:**

  For EK-RX74M:
    - DAT0: 
        - E70: Close.
        - E50: Open.
    - DAT1: 
        - E71: Close.
        - E51: Open.
    - J48: Jumper on pins 2-3.
    - Insert the SD card directly into the SD slot on the RX board.

- **SDHI Power Control IC:**

  For EK-RX74M:
    - E69: Close.
    - E58: Open.

## Related Collateral References 
The following documents can be referred to for enhancing your understanding of the operation of this example project:
- [FSP User Manual on GitHub (Reference link)](https://renesas.github.io/fsp/)
- [FSP Known Issues (Reference link)](https://github.com/renesas/fsp/issues)

# Project Notes 

## System Level Block Diagram 
High level block diagram of the system is as shown below:
 
![SDHI](images/SDHI_hld.png "High Level Block Diagram")

## FSP Modules Used 
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage | Searchable Keyword  |
|-------------|-----------------------------------------------|-----------------------------------------------|
| SDHI | SDHI driver to perform data writing and reading operations directly on the SD card. | r_sdhi |

## Module Configuration Notes 
This section describes FSP Configurator properties which are important or different than those selected by default. 

**Configuration Properties for using SDHI**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
|-----------------------------------------|-------------------|----------------|------------|
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Add Transfer Driver (DMAC or DTC) > New > Transfer (r_dtc) | None | None | Required driver for transfer operations. |
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Settings > Property > Module g_sdmmc SD/MMC (r_sdhi) > Interrupts > Callback | NULL | sdmmc_callback | Callbacks are used to handle events such as read, write, or erase. |
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Settings > Property > Module g_sdmmc SD/MMC (r_sdhi) > Interrupts > Card Interrupt Priority | Disabled | Priority 2 | Select the Card Interrupt Priority. |
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Settings > Property > Module g_sdmmc SD/MMC (r_sdhi) > Interrupts > DTC Interrupt Priority	|	Disabled | Priority 2 | Select the DTC Interrupt Priority. |
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Settings > Property > Pins > SD0CD | None | P205 | Specifies the pin for Card Detect. |
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Settings > Property > Pins > SD0CLK | None | P201 | Specifies the pin for Clock. |
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Settings > Property > Pins > SD0CMD | None | P200 | Specifies the pin for Command. |
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Settings > Property > Pins > SD0DAT0 | None | P202 | Specifies the pin for Data 0. |
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Settings > Property > Pins > SD0DAT1 | None | P203 | Specifies the pin for Data 1. |
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Settings > Property > Pins > SD0DAT2 | None | P106 | Specifies the pin for Data 2. |
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Settings > Property > Pins > SD0DAT3 | None | P107 | Specifies the pin for Data 3. |
| configuration.xml > g_sdmmc SD/MMC (r_sdhi) > Settings > Property > Pins > SD0WP | None | P204 | Specifies the pin for Write Protect. |

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

## API Usage 
The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| R_SDHI_Open | This API is used to initialize the SDHI module. |
| R_SDHI_Write | This API is performs SD Card write operation. |
| R_SDHI_Read | This API is performs SD Card read operation. |
| R_SDHI_MediaInit | This API is used initializes the SD or eMMC device. |
| R_SDHI_Close | This API is used to de-initialize the SDHI module. |
| R_SDHI_Erase | This API is performs SD Card Erase operation. |
| R_SDHI_StatusGet | This API is Provides driver status. |
| R_BSP_SoftwareDelay | This API is used to delay a specified period of time. |

**For using the Serial Terminal (UART)**

| API Name           | Usage    |
|--------------------|----------|
| R_SCI_B_UART_Open  | This API is used to configures the UART driver based on the input configurations. |
| R_SCI_B_UART_Write | This API is used to transmit user specified number of bytes from the source buffer pointer. |
| R_SCI_B_UART_Close | This API is used to abort any in progress transfers, disables interrupts, receiver, and transmitter, close lower level transfer drivers if used and remove power. |

## Verifying operation 

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

6. Users can perform the menu operations and check the corresponding results on the terminal console (USB Serial COM).

The below images showcase the output on the Serial terminal application (Tera Term):

  **EP information**

  ![SDHI Info](images/SDHI_info_tera_term.png)

  **Write data message**

  ![Write data](images/Write_data_tera_term.png)

  **Read data message**

  ![Read data](images/Read_data_tera_term.png)

  **Erase data message**

  ![Erase data](images/Erase_data_tera_term.png)

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

  **Note:** Connect the RX board debugger port (J9) to the host PC via E2 Emulator Lite (with USB cable and 20-20 pin cable).

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
