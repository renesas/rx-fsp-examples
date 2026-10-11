# Introduction #

The example project demonstrates typical use of the IIC slave HAL module APIs. The project initializes IIC slave and IIC master module with standard rate and is made interfaced with loop-back mechanism. It performs Slave read and write operation continuously once initialization is successful. On successful I2C transaction (6 bytes), the transmitted and received data compared. Led blinks on data match else it is turned ON as sign of failure. Output message for both corresponding slave operations is displayed on the Terminal Application. Any API/event failure message is also displayed.

**Note**: 
- To display information, the user can select between Debug Virtual Console and the Serial Terminal (UART) with USB Serial COM. It is important to note that the user should only operate a single terminal application at a time to avoid conflicts or data inconsistencies. For instructions on how to switch between these options, please refer to the section [`Special Topics`](#special-topics)/[`Changing Terminal Application`](#changing-terminal-application).

- By default, EP information is printed to the host PC using the Serial Terminal (UART) for boards that support USB Serial COM. LED3 is turned ON to indicate the UART initialized unsuccessfully.

- RX boards that support USB Serial COM: EK-RX74M, EK-RX74N. 

## Required Resources ##
To build and run the IIC_Slave example project, the following resources are needed.

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
* 2 x Jumper wires.

### Hardware Connection ###

- Connect the RX board USB debug port (J8) to the PC via a Type-C USB cable for EP debugging (If E2 Lite on-board is used).
- Connect the RX board USB Serial port (J27) to the PC using a Type-C USB cable to verify logs through the Terminal Application. 

Channel 0 has been used by IIC master and channel 2 has been used by IIC slave:

For EK-RX74M:
   1) Slave IIC pins
      * IIC2 P206 (J1:21)  ----> SCL2
      * IIC2 P207 (J1:22)  ----> SDA2
   2) Master IIC pins
      * IIC0 P102 (J2:5)   ----> SCL0
      * IIC0 P103 (J2:6)   ----> SDA0

For EK-RX74N:
   1) Slave IIC pins
      * IIC2 P206 (J1:49)  ----> SCL2
      * IIC2 P207 (J1:52)  ----> SDA2
   2) Master IIC pins
      * IIC0 P202 (J2:4)   ----> SCL0
      * IIC0 P203 (J2:3)   ----> SDA0

   * Note: External pull up resistors of value 3.9 or 4.7k ohms are required to be connected on I2C (SDA/SCL) lines.

## Related Collateral References ##
The following documents can be referred to for enhancing your understanding of the operation of this example project:
- [FSP User Manual on GitHub (Reference link)](https://renesas.github.io/fsp/)
- [FSP Known Issues (Reference link)](https://github.com/renesas/fsp/issues)

# Project Notes #

## System Level Block Diagram ##
![IIc_Slave High Level Block Diagram](images/High_Level_Design.png "IIC_Slave Interface Block Diagram")

## FSP Modules Used ##
List all the various modules that are used in this example project. Refer to the FSP User Manual for further details on each module listed below.

| Module Name | Usage  | Searchable Keyword (using New Stack > Search) |
| :---------: | :---------------: | :------------: |
| iic_b_master | This module is used acts as an I2C master to communicate with slave devices. | r_iic_b_master |
| iic_b_slave | This module is used acts as an I2C slave to receive data from master, return data to master.  | r_iic_b_slave |

## Module Configuration Notes ##
This section describes FSP Configurator properties that are important or different from those selected by default. 

**Common Configuration Properties**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|   configuration.xml > BSP > Properties > Settings > Property > RX Common > Heap size (bytes)  |  0   | 0x400  |  Heap size is required for standard library functions to be used. |

**Common Configuration Properties (r_iic_b_master)**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|   configuration.xml > Stacks > g_i2c_master I2C Master (r_iic_b_master) > Properties > Settings > Property > Module g_i2c_master I2C Master (r_iic_b_master) > Name | - | g_i2c_master | Assigned name for iic_master.  |
|   configuration.xml > Stacks > g_i2c_master I2C Master (r_iic_b_master) > Properties > Settings > Property > Module g_i2c_master I2C Master (r_iic_b_master) > Channel | 0 | 0 | Specify the IIC channel.  |
|   configuration.xml > Stacks > g_i2c_master I2C Master (r_iic_b_master) > Properties > Settings > Property > Module g_i2c_master I2C Master (r_iic_b_master) > Slave Address | 0x00 | 0x4A | Specify the master address.  |
|   configuration.xml > Stacks > g_i2c_master I2C Master (r_iic_b_master) > Properties > Settings > Property > Module g_i2c_master I2C Master (r_iic_b_master) > Callback | NULL | iic_b_master_callback | Assigned callback function for i2c master.  |
|   configuration.xml > Stacks > g_i2c_master I2C Master (r_iic_b_master) > Properties > Settings > Property > Module g_i2c_master I2C Master (r_iic_b_master) > Interrupt Priority Level | Disabled | Priority 3 | Assigned priority for interrupt.  |
|   configuration.xml > Stacks > g_i2c_master I2C Master (r_iic_b_master) > Properties > Settings > Property > Pins > SCL0 | None | P202 | Select and set the pin for SCL.  |
|   configuration.xml > Stacks > g_i2c_master I2C Master (r_iic_b_master) > Properties > Settings > Property > Pins > SDA0 | None | P203 | Select and set the pin for SDA.  |

**Common Configuration Properties (r_iic_b_slave)**

|   Module Property Path and Identifier   |   Default Value   |   Used Value   |   Reason   |
| :-------------------------------------: | :---------------: | :------------: | :--------: |
|   configuration.xml > Stacks > g_i2c_slave I2C Slave (r_iic_b_slave) > Properties > Settings > Property > Module g_i2c_slave I2C Slave (r_iic_b_slave) > Name | - | g_i2c_slave | Assigned name for iic_slave.  |
|   configuration.xml > Stacks > g_i2c_slave I2C Slave (r_iic_b_slave) > Properties > Settings > Property > Module g_i2c_slave I2C Slave (r_iic_b_slave) > Channel | 0 | 2 | Specify the IIC channel.  |
|   configuration.xml > Stacks > g_i2c_slave I2C Slave (r_iic_b_slave) > Properties > Settings > Property > Module g_i2c_slave I2C Slave (r_iic_b_slave) > Slave Address | 0x00 | 0x4A | Specify the slave address.  |
|   configuration.xml > Stacks > g_i2c_slave I2C Slave (r_iic_b_slave) > Properties > Settings > Property > Module g_i2c_slave I2C Slave (r_iic_b_slave) > Callback | NULL | iic_b_slave_callback | Assigned callback function for i2c slave.  |
|   configuration.xml > Stacks > g_i2c_slave I2C Slave (r_iic_b_slave) > Properties > Settings > Property > Pins > SCL2 | None | P206 | Select and set the pin for SCL.  |
|   configuration.xml > Stacks > g_i2c_slave I2C Slave (r_iic_b_slave) > Properties > Settings > Property > Pins > SDA2 | None | P207 | Select and set the pin for SDA.  |

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

The table below lists the FSP provided API used at the application layer by this example project.

| API Name    | Usage                                                                          |
|-------------|--------------------------------------------------------------------------------|
| R_IIC_B_MASTER_Open | This API is used to open IIC_Master. |
| R_IIC_B_MASTER_Write | This API is used to write to the I2C device. |
| R_IIC_B_MASTER_Read | This API is used to read from the I2C device. |
| R_IIC_B_MASTER_Close | This API is used to close IIC_Master. |
| R_IIC_B_SLAVE_Open | This API is used to open IIC_Slave. |
| R_IIC_B_SLAVE_Write | This API is used to write to the I2C master device. |
| R_IIC_B_SLAVE_Read | This API is used to read from the I2C master device. |
| R_IIC_B_SLAVE_Close | This API is used to close IIC_Slave. |
| R_IOPORT_Open | This API is used to open ioport. |

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

### The below images showcase the output on the Terminal Application (Tera Term) ###
	
![iic_slave_output](images/iic_slave_Output.png "read_write_slave_operation")
	
## Special Topics ##
For the functioning of IIC Slave on EK-RX74M board, no external pull-up resistors of value 3.9 kΩ or 4.7 kΩ are required to be connected on the I2C (SDA/SCL) lines.

### LED Behavior ###
   * Board LED will be turned ON when an error occurs.
   * Toggle the onboard LED when communication is successful.

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
    