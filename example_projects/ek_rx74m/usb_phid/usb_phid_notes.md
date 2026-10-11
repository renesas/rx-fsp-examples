# Introduction #

This project demonstrates the basic functionalities of USB PHID driver on Renesas RX MCUs based on Renesas FSP. The project configures RX board as a Keyboard. On pressing any key from terminal, MCU sends characters from a - z & numbers 0 - 9 to the host PC.

Note: 
* To display information, the user can select between the Debug Virtual Console and the Serial Terminal (UART) with USB Serial COM. It is important to note that the user should only operate a single Terminal Application at a time to avoid conflicts or data inconsistencies. For instructions on how to switch between these options, please refer to the section [`Special Topics`](#special-topics)/[`Changing Terminal Application`](#changing-terminal-application).

* By default, EP information is printed to the host PC using the Serial Terminal (UART) for boards that support USB Serial COM. LED3 is turned ON to indicate the UART initialized unsuccessfully.

* RX boards that support USB Serial COM: EK-RX74M.

## Required Resources ##

To build and run the USB PHID example project, the following resources are needed.

### Software ###

* Renesas Flexible Software Package (FSP): Version 1.0.0.
* e2 studio: Version 2026-07.
* Renesas GCC Compiler for RX: Version 14.2.0.202607.
* Renesas CC-RX Compiler: Version v3.08.00.
* Terminal Console Application: Tera Term or a similar application.

### Hardware ###

* 1 x Renesas RX board.
* 3 x Type-C USB cables.
* 1 x PC with at least 3 USB ports (1 for debugging, 1 for communication, and 1 for logging output to terminal application).

### Hardware Connections ###

- Connect the RX board USB debug port (J8) to the PC via a Type-C USB cable for EP debugging.
- Connect the RX board USB FS port (J31) to the PC via a Type-C USB cable.
- Connect the RX board USB Serial port (J27) to the PC using a Type-C USB cable to verify logs through the Terminal Application.
- Configure SW6 as follows to use OB Debugger: SW6-Pin1 OFF, SW6-Pin2 OFF. 

- **VBUS supply enable signal:**

  For EK-RX74M:
    - E64: Close
    - E65: Open

- **Self-powered Mode:**
  - J28: Open (No jumper).
  - J30: Jumper on pins 2-3.
  - SW4: SW4-Pin6, 8 ON, SW4-Pin5, 7, 11, 12 OFF.

  **Note:** The RX board can operate in different power modes (**Bus-powered**) based on jumper configuration. Refer to the "USB PHID Function Modes" section in Special Topics for detailed Bus-powered mode configuration.

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of the operation of this example project:
- [FSP User Manual on GitHub (Reference link)](https://renesas.github.io/fsp/)
- [FSP Known Issues (Reference link)](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##

![usb_phid](images/usb_phid_hld.png "High Level Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage  | Searchable Keyword (using New Stack > Search) |
|-------------|-----------------------------------------------|-----------------------------------------------|
| USB PHID | USB PHID is used to send character data from the device to the host machine over the USB HID interface. | r_usb_phid |

## Module Configuration Notes ##
This section describes FSP Configurator properties which are important or different than those selected by default.

**Common Configuration Properties**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|   configuration.xml > BSP > Properties > Settings > Property > RX Common > Heap size (bytes)  |  0   | 0x1000  |  Heap size is required for standard library functions to be used. |

**USB PHID Configuration Properties**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|  configuration.xml > Stacks > Threads > Phid Thread > g_basic0 USB (r_usb_basic) > Properties > Settings > Property > Common > PLL Frequency | 24MHz | 24MHz | The PLL frequency is set to 24MHz to match the clock settings of the RX74M board. |
|  configuration.xml > Stacks > Threads > Phid Thread > g_basic0 USB (r_usb_basic) > Properties > Settings > Property > Module g_basic0 USB (r_usb_basic) > USBFS Interrupt Priority | Priority 3 | Priority 4 | The USBFS interrupt priority is set to 12 to ensure that USB events are handled promptly. |
|  configuration.xml > Stacks > Threads > Phid Thread > g_basic0 USB (r_usb_basic) > Properties > Settings > Property > Module g_basic0 USB (r_usb_basic) > USBFS Resume Priority | Not Supported | Priority 3 | The USBFS resume priority is set to 11 to ensure that the USB device can resume operation quickly after a suspend event. |
|  configuration.xml > Stacks > Threads > Phid Thread > g_basic0 USB (r_usb_basic) > Properties > Settings > Property > Pins > USBFS_OVRCURA_A(DS) | None | P104 | The USBFS_OVRCURA_A(DS) pin is configured to P104 to match the RX74M board's pinout. |
|  configuration.xml > Stacks > Threads > Phid Thread > g_basic0 USB (r_usb_basic) > Properties > Settings > Property > Pins > USBFS_VBUSEN | None | P304 | The USBFS_VBUSEN pin is configured to P304 to match the RX74M board's pinout. |
|  configuration.xml > Stacks > Threads > Phid Thread > g_basic0 USB (r_usb_basic) > Properties > Settings > Property > Pins > USBFS_VBUS_A(DS) | None | P101 | The USBFS_VBUS_A(DS) pin is configured to P101 to match the RX74M board's pinout. |

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

| API Name                    | Usage                                                      |
|-----------------------------|------------------------------------------------------------|
| R_USB_Open                  | This API is used to open an USB instance.                  |
| R_USB_Write                 | This API is used to write data to the USB interface.       |
| R_USB_PeriControlDataGet    | This API is used to receive data sent by control transfer. |
| R_USB_PeriControlDataSet    | This API is used to set a class request.                   |
| R_USB_PeriControlStatusSet  | This API is used to set the USB status response.           |
| R_USB_Close                 | This API is used to close the USB instance.                |

**For using the Serial Terminal (UART)**  

| API Name           | Usage                                                                          |
|--------------------|--------------------------------------------------------------------------------|
| R_SCI_B_UART_Open  | This API is used to configures the UART driver based on the input configurations. |
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

6. Users can perform the menu operations and check the corresponding results on the terminal console (USB Serial COM).

**Note:**
- Lower case or upper case a - z and numbers 0 - 9 are sent from the MCU to the host PC

Below images showcases the output log on the Serial terminal application (Tera Term):

Keyboard keys were printed on the Notepad.

![dvc_info](images/notepad_page_tera_term.png "Keyboard keys on Notepad")  

The device enumeration in Device Manager. The user can update the Vendor ID and Product ID of their own devices in description file.

![dvc_info](images/descriptor_code.png "Descriptor")

## Special Topics ##

### Developing Descriptor ###

A USB device provides information about itself in data structures called USB descriptors.

The host obtains descriptors from an attached device by sending various standard control requests to the default endpoint. These requests specify the type of descriptor to retrieve. In response to such requests, the device sends descriptors that include information about the device, its configurations, interfaces and the related endpoints.

* A template for PHID descriptors can be found in rx/fsp/src/r_usb_phid_descriptor.c.template. Also, please be sure to use your vendor ID.

### USB PHID Function Modes ###

**1. Bus-powered Mode**: The USB device is powered by the USB host through the USB cable. The device draws power from the USB bus. The following jumper settings are used for this mode:
  - J28: Jumper on pins 1-2.
  - J30: Jumper on pins 2-3.
  - SW4: SW4-Pin6, 8 ON, SW4-Pin5, 7, 11, 12 OFF.

**2. Self-powered Mode**: The USB device has its own power supply and does not rely on the USB host for power. The device operates independently of the USB bus power.

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
