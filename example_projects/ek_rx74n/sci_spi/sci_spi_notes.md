# Introduction

This Example Project demonstrates the basic usage of SCI SPI driver. The project writes commands to the temperature module MAX31723PMB1 and reads the temperature value using SCI SPI bus which is configured as master. The temperature values are continuously printed on terminal console.

Note:

* To display information, the user can select between the Debug Virtual Console and the Serial Terminal (UART) with USB Serial COM. It is important to note that the user should only operate a single Terminal Application at a time to avoid conflicts or data inconsistencies. For instructions on how to switch between these options, please refer to the section [`Special Topics`](#special-topics)/[`Changing Terminal Application`](#changing-terminal-application).
* By default, EP information is printed to the host PC using the Serial Terminal (UART) for boards that support USB Serial COM. LED3 is turned ON to indicate the UART initialized unsuccessfully.
* RX boards that support USB Serial COM: EK-RX74M, EK-RX74N.

## Required Resources

To build and run the SPI example project, the following resources are needed.

### Software

* Renesas Flexible Software Package (FSP): Version 1.0.0.
* e2 studio: Version 2026-07.
* Renesas GCC Compiler for RX: Version 14.2.0.202607.
* Renesas CC-RX Compiler: Version v3.08.00.
* Terminal Console Application: Tera Term or a similar application.

### Hardware

Supported RX boards: EK-RX74M, EK-RX74N.

* 1 x Renesas RX board.
* 2 x Type-C USB cables for programming and debugging.
* 1 x Temperature sensor MAX31723 as a slave device.
  * Link product: [PMOD MAX31723PMB1](https://www.mouser.com/ProductDetail/Analog-Devices-Maxim-Integrated/MAX31723PMB1?qs=UmMSjoC1xtH8e742i4OoUA%3D%3D).
  * Manufacturer Product Number: MAX31723PMB1#

**Select SPI communication protocol on the PMOD MAX31723PMB1 by configuring the following jumper configurations on JP1:**

![PMOD MAX31723PMB1 configuration](images/sau_spi_pmod_configuration.png "PMOD MAX31723PMB1 configuration")

## Hardware Connections

Supported RX boards: EK-RX74M, EK-RX74N.

- Connect the RX board USB debug port (J8) to the PC via a Type-C USB cable for EP debugging (If E2 Lite on-board is used).
- Connect the RX board USB SERIAL port (J27) to the PC using a Type-C USB cable to verify logs through the terminal console.
- Connect Temperature sensor (J1) to PMOD2. Please carefully align pin 1 on the Temperature sensor with pin 1 on the PMOD2.
- Jumpers configuration:
   - J17: Jumper on pins 2-3.
   - J18: Jumper on pins 2-3.
   - J19: Jumper on pins 2-3.

## Related Collateral References

The following documents can be referred to for enhancing your understanding of the operation of this example project:

- [FSP User Manual on GitHub (Reference link)](https://renesas.github.io/fsp/)
- [FSP Known Issues (Reference link)](https://github.com/renesas/fsp/issues)

# Project Notes

## System Level Block Diagram

![SCI_SPI](images/SCI_SPI_hld.png "High Level Block Diagram")

## FSP Modules Used

List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.


| Module Name | Usage                                                      | Searchable Keyword (using New Stack > Search) |
| ----------- | ---------------------------------------------------------- | --------------------------------------------- |
| SCI SPI     | SCI SPI is used to communicate with the Temperature sensor | r_sci_b_spi                                   |

## Module Configuration Notes

This section describes FSP Configurator properties which are important or different than those selected by default.

**Common Configuration Properties**


|                      Module Property Path and Identifier                      | Default Value | Used Value |                              Reason                              |
| :---------------------------------------------------------------------------: | :-----------: | :--------: | :--------------------------------------------------------------: |
| configuration.xml > BSP > Settings > Property > RX Common > Heap size (bytes) |       0       |   0x900   | Heap size is required for standard library functions to be used. |

**Configuration Properties for using SCI SPI**


| Module Property Path and Identifier                                                                                                     | Default Value                                          | Used Value                                             | Reason                                                                                                                  |
| --------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------ | ------------------------------------------------------ | ----------------------------------------------------------------------------------------------------------------------- |
| configuration.xml > Stacks > g_spi SPI (r_sci_b_spi) > Properties >Settings > Property > Module g_spi SPI  (r_sci_b_spi) > Channel      | 0                                                      | 6                                                      | Use SCI SPI Channel 6 to communicate with the sensor.                                                                  |
| configuration.xml > Stacks > g_spi SPI (r_sci_b_spi) > Properties > Settings > Property > Module g_spi SPI  (r_sci_b_spi) > Clock Phase | Data sampling on odd edge, data variation on even edge | Data sampling on even edge, data variation on odd edge | Select Clock Phase.                                                                                                     |
| configuration.xml > Stacks > g_spi SPI (r_sci_b_spi) > Properties > Settings > Property > Module g_spi SPI  (r_sci_b_spi) > Callback    | NULL                                                   | sci_spi_callback                                       | It is called from the interrupt service routine (ISR) upon SCI transaction completion reporting the transaction status. |
| configuration.xml > Stacks > g_spi SPI (r_sci_b_spi) > Properties > Settings > Property > Module g_spi SPI  (r_sci_b_spi) > Bitrate     | 8000000                                                | 5000000                                                | Select the bitrate.                                                                                                     |
| configuration.xml > Stacks > g_spi SPI (r_sci_b_spi) > Properties > Settings > Pins > RXD6                                              | unavailable                                            | P905                                                   | Select the RXD pin.                                                                                                     |
| configuration.xml > Stacks > g_spi SPI (r_sci_b_spi) > Properties > Settings > Pins > SCK6                                              | unavailable                                            | P906                                                   | Select the SCK pin.                                                                                                     |
| configuration.xml > Stacks > g_spi SPI (r_sci_b_spi) > Properties > Settings > Pins > TXD6                                              | unavailable                                            | P904                                                   | Select the TXD pin.                                                                                                     |

**Configuration Properties for using the Serial Terminal (UART)**   


| Configure Clock Path                                               | Default Value  | Used Value       | Reason                                                      |
| ------------------------------------------------------------------ | -------------- | ---------------- | ----------------------------------------------------------- |
| configuration.xml > Clocks > Clocks Configuration > SCICLK Disable | SCICLK Disable | SCICLK Src: HOCO | Enable operating clock for SCI module by HOCO clock source. |


| Configure Interrupt Event Path                                                                                                         | Default Value | Used Value         | Reason                                                                            |
| -------------------------------------------------------------------------------------------------------------------------------------- | ------------- | ------------------ | --------------------------------------------------------------------------------- |
| configuration.xml > Interrupts > Interrupts Configuration > New User Event > RSCI > RSCI12 > RSCI12 RXI (Receive data full)            | empty         | sci_b_uart_rxi_isr | Assign the UART receive ISR (Receive data full) to the interrupt vector table.    |
| configuration.xml > Interrupts > Interrupts Configuration > New User Event > RSCI > RSCI12 > RSCI12 TXI (Transmit data empty)          | empty         | sci_b_uart_txi_isr | Assign the UART transfer ISR (Transfer data empty) to the interrupt vector table. |
| configuration.xml > Interrupts > Interrupts Configuration > New User Event > RSCI > RSCI12 > 105-6 GROUPAL1 RSCI12 TEI (Transmit end)  | empty         | sci_b_uart_tei_isr | Assign the UART transfer ISR (Transfer end) to the interrupt vector table.        |
| configuration.xml > Interrupts > Interrupts Configuration > New User Event > RSCI > RSCI12 > 105-7 GROUPAL1 RSCI12 ERI (Receive error) | empty         | sci_b_uart_eri_isr | Assign the UART receive ISR (Receive error) to the interrupt vector table.        |

## API Usage

The table below lists the FSP provided APIs used at the application layer by this example project.


| API Name              | Usage                                                                                            |
| --------------------- | ------------------------------------------------------------------------------------------------ |
| R_SCI_B_SPI_Open      | This API is used to initialize the SCI SPI module.                                               |
| R_SCI_B_SPI_Write     | This API is used transmit data to a SPI  device.                                                 |
| R_SCI_B_SPI_WriteRead | This API is used simultaneously transmit data to SPI device while receiving data from SPI device |
| R_SCI_B_SPI_Read      | This API is used receive data from an SPI device.                                                |
| R_SCI_B_SPI_Close     | This API is used to de-initialize the SCI SPI module.                                            |
| R_BSP_SoftwareDelay   | This API is used to delay a specified period of time.                                            |

**For using the Serial Terminal (UART)**


| API Name           | Usage                                                                                                                                                             |
| ------------------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| R_SCI_B_UART_Open  | This API is used to configures the UART driver based on the input configurations.                                                                                 |
| R_SCI_B_UART_Write | This API is used to transmit user specified number of bytes from the source buffer pointer.                                                                       |
| R_SCI_B_UART_Close | This API is used to abort any in progress transfers, disables interrupts, receiver, and transmitter, close lower level transfer drivers if used and remove power. |

## Verifying Operation

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

The below image showcases the output on the Serial terminal application (Tera Term):

![teraterm_log](images/SCI_SPI_log_tera_term.png "Tera Term Console Output")

## Special Topics

### Debugger Setting

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

### Changing Terminal Application

By default, the EP supports Serial Terminal for RX boards that support USB Serial COM**For use CCRX Compiler:**

* Define USE_VIRTUAL_COM=1 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings -> Compiler -> Source -> Macro definition (-define)

To use Debug Virtual Console, please follow the instructions below:

* Define USE_VIRTUAL_COM=0 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings -> Compiler -> Source -> Macro definition (-define)
* Define BSP_CFG_IO_LIB_ENABLE=1 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings -> Compiler -> Source -> Macro definition (-define)

![USE_VIRTUAL_COM settings for CCRX](images/use_virtual_com_setting_ccrx.png "USE_VIRTUAL_COM settings CCRX")

**For use GCCRX Compiler:**

* Define USE_VIRTUAL_COM=1 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings -> Compiler -> Includes -> Macro definition (-define)

To use Debug Virtual Console, please follow the instructions below:

* Define USE_VIRTUAL_COM=0 macro in Project Properties -> C/C++ Build -> Settings -> Tool Settings -> Compiler -> Includes -> Macro definition (-define)

![USE_VIRTUAL_COM settings for GCCRX](images/use_virtual_com_setting_gccrx.png "USE_VIRTUAL_COM settings GCCRX")
