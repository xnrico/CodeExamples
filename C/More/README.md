# PSoC&trade; 6 MCU: Low-power CAPSENSE&trade; design (FreeRTOS)

This code example demonstrates how to create a low-power CAPSENSE&trade; design using PSoC&trade; 6 MCU. This example features a 5-segment CAPSENSE&trade; slider and a ganged sensor, and displays the detected touch position over the serial terminal. This example uses the [CAPSENSE&trade; middleware library](https://github.com/Infineon/capsense).

[View this README on GitHub.](https://github.com/Infineon/mtb-example-psoc6-low-power-capsense-freertos)

[Provide feedback on this code example.](https://cypress.co1.qualtrics.com/jfe/form/SV_1NTns53sK2yiljn?Q_EED=eyJVbmlxdWUgRG9jIElkIjoiQ0UyMzA5NjgiLCJTcGVjIE51bWJlciI6IjAwMi0zMDk2OCIsIkRvYyBUaXRsZSI6IlBTb0MmdHJhZGU7IDYgTUNVOiBMb3ctcG93ZXIgQ0FQU0VOU0UmdHJhZGU7IGRlc2lnbiAoRnJlZVJUT1MpIiwicmlkIjoidmFpciIsIkRvYyB2ZXJzaW9uIjoiMi4xLjAiLCJEb2MgTGFuZ3VhZ2UiOiJFbmdsaXNoIiwiRG9jIERpdmlzaW9uIjoiTUNEIiwiRG9jIEJVIjoiSUNXIiwiRG9jIEZhbWlseSI6IlBTT0MifQ==)


## Requirements

- [ModusToolbox&trade; software](https://www.infineon.com/modustoolbox) v3.1 or later (tested with v3.1)
- Board support package (BSP) minimum required version: 4.0.0.
- Programming language: C
- Associated parts: All [PSoC&trade; 6 MCU](https://www.infineon.com/cms/en/product/microcontroller/32-bit-psoc-arm-cortex-microcontroller/psoc-6-32-bit-arm-cortex-m4-mcu/) parts, [AIROC&trade; CYW43012 Wi-Fi & Bluetooth&reg; combo chip](https://www.infineon.com/cms/en/product/wireless-connectivity/airoc-wi-fi-plus-bluetooth-combos/wi-fi-4-802.11n/cyw43012), [AIROC&trade; CYW4343W Wi-Fi & Bluetooth&reg; combo chip](https://www.infineon.com/cms/en/product/wireless-connectivity/airoc-wi-fi-plus-bluetooth-combos/wi-fi-4-802.11n/cyw4343w), [AIROC&trade; CYW4373 Wi-Fi & Bluetooth&reg; combo chip](https://www.infineon.com/cms/en/product/wireless-connectivity/airoc-wi-fi-plus-bluetooth-combos/wi-fi-5-802.11ac/cyw4373),


## Supported toolchains (make variable 'TOOLCHAIN')

- GNU Arm&reg; embedded compiler v11.3.1 (`GCC_ARM`) - Default value of `TOOLCHAIN`
- Arm&reg; compiler v6.16 (`ARM`)
- IAR C/C++ compiler v9.30.1 (`IAR`)


## Supported kits (make variable 'TARGET')

- [PSoC&trade; 6 Wi-Fi Bluetooth&reg; prototyping kit](https://www.infineon.com/CY8CPROTO-062-4343W) (`CY8CPROTO-062-4343W`) - Default value of `TARGET`
- [PSoC&trade; 6 Wi-Fi Bluetooth&reg; pioneer kit](https://www.infineon.com/CY8CKIT-062-WIFI-BT) (`CY8CKIT-062-WiFi-BT`)
- [PSoC&trade; 6 Bluetooth&reg; LE pioneer kit](https://www.infineon.com/CY8CKIT-062-BLE) (`CY8CKIT-062-BLE`)
- [PSoC&trade; 62S2 Wi-Fi Bluetooth&reg; pioneer kit](https://www.infineon.com/CY8CKIT-062S2-43012) (`CY8CKIT-062S2-43012`)
- [PSoC&trade; 62S1 Wi-Fi Bluetooth&reg; pioneer kit](https://www.infineon.com/CYW9P62S1-43438EVB-01) (`CYW9P62S1-43438EVB-01`)
- [PSoC&trade; 62S1 Wi-Fi Bluetooth&reg; pioneer kit](https://www.infineon.com/CYW9P62S1-43012EVB-01) (`CYW9P62S1-43012EVB-01`)
- [PSoC&trade; 62S3 Wi-Fi Bluetooth&reg; prototyping kit](https://www.infineon.com/CY8CPROTO-062S3-4343W) (`CY8CPROTO-062S3-4343W`)
- [PSoC&trade; 64 "Secure Boot" Wi-Fi Bluetooth&reg; pioneer kit](https://www.infineon.com/CY8CKIT-064B0S2-4343W) (`CY8CKIT-064B0S2-4343W`)
- [PSoC&trade; 64 Standard Secure AWS Wi-Fi Bluetooth&reg; pioneer kit](https://www.infineon.com/CY8CKIT-064S0S2-4343W) (`CY8CKIT-064S0S2-4343W`)
- [PSoC&trade; 62S4 pioneer kit](https://www.infineon.com/CY8CKIT-062S4) (`CY8CKIT-062S4`)
- [PSoC&trade; 62S2 evaluation kit](https://www.infineon.com/CY8CEVAL-062S2) (`CY8CEVAL-062S2`, `CY8CEVAL-062S2-LAI-4373M2`, `CY8CEVAL-062S2-MUR-43439M2`)


## Hardware setup

This example uses the board's default configuration. See the kit user guide to ensure that the board is configured correctly.

**Note:** The PSoC&trade; 6 Bluetooth&reg; LE pioneer kit (CY8CKIT-062-BLE) and the PSoC&trade; 6 Wi-Fi Bluetooth&reg; pioneer kit (CY8CKIT-062-WIFI-BT) ship with KitProg2 installed. The ModusToolbox&trade; software requires KitProg3. Before using this code example, make sure that the board is upgraded to KitProg3. The tool and instructions are available in the [Firmware Loader](https://github.com/Infineon/Firmware-loader) GitHub repository. If you do not upgrade, you will see an error like "unable to find CMSIS-DAP device" or "KitProg firmware is out of date".


## Software setup

Install a terminal emulator if you don't have one. Instructions in this document use [Tera Term](https://ttssh2.osdn.jp/index.html.en).

This example requires no additional software or tools.


## Using the code example

Create the project and open it using one of the following:

<details><summary><b>In Eclipse IDE for ModusToolbox&trade; software</b></summary>

1. Click the **New Application** link in the **Quick Panel** (or, use **File** > **New** > **ModusToolbox&trade; Application**). This launches the [Project Creator](https://www.infineon.com/ModusToolboxProjectCreator) tool.

2. Pick a kit supported by the code example from the list shown in the **Project Creator - Choose Board Support Package (BSP)** dialog.

   When you select a supported kit, the example is reconfigured automatically to work with the kit. To work with a different supported kit later, use the [Library Manager](https://www.infineon.com/ModusToolboxLibraryManager) to choose the BSP for the supported kit. You can use the Library Manager to select or update the BSP and firmware libraries used in this application. To access the Library Manager, click the link from the **Quick Panel**.

   You can also just start the application creation process again and select a different kit.

   If you want to use the application for a kit not listed here, you may need to update the source files. If the kit does not have the required resources, the application may not work.

3. In the **Project Creator - Select Application** dialog, choose the example by enabling the checkbox.

4. (Optional) Change the suggested **New Application Name**.

5. The **Application(s) Root Path** defaults to the Eclipse workspace which is usually the desired location for the application. If you want to store the application in a different location, you can change the *Application(s) Root Path* value. Applications that share libraries should be in the same root path.

6. Click **Create** to complete the application creation process.

For more details, see the [Eclipse IDE for ModusToolbox&trade; software user guide](https://www.infineon.com/MTBEclipseIDEUserGuide) (locally available at *{ModusToolbox&trade; software install directory}/docs_{version}/mt_ide_user_guide.pdf*).

</details>

<details><summary><b>In command-line interface (CLI)</b></summary>

ModusToolbox&trade; software provides the Project Creator as both a GUI tool and the command line tool, "project-creator-cli". The CLI tool can be used to create applications from a CLI terminal or from within batch files or shell scripts. This tool is available in the *{ModusToolbox&trade; software install directory}/tools_{version}/project-creator/* directory.

Use a CLI terminal to invoke the "project-creator-cli" tool. On Windows, use the command line "modus-shell" program provided in the ModusToolbox&trade; software installation instead of a standard Windows command-line application. This shell provides access to all ModusToolbox&trade; software tools. You can access it by typing `modus-shell` in the search box in the Windows menu. In Linux and macOS, you can use any terminal application.

The "project-creator-cli" tool has the following arguments:

Argument | Description | Required/optional
---------|-------------|-----------
`--board-id` | Defined in the `<id>` field of the [BSP](https://github.com/Infineon?q=bsp-manifest&type=&language=&sort=) manifest | Required
`--app-id`   | Defined in the `<id>` field of the [CE](https://github.com/Infineon?q=ce-manifest&type=&language=&sort=) manifest | Required
`--target-dir`| Specify the directory in which the application is to be created if you prefer not to use the default current working directory | Optional
`--user-app-name`| Specify the name of the application if you prefer to have a name other than the example's default name | Optional

<br />

The following example clones the "[mtb-example-psoc6-low-power-capsense-freertos](https://github.com/Infineon/mtb-example-psoc6-low-power-capsense-freertos)" application with the desired name "CapsenseFreetos" configured for the *CY8CKIT-062-WIFI-BT* BSP into the specified working directory, *C:/mtb_projects*:

   ```
   project-creator-cli --board-id CY8CKIT-062-WIFI-BT --app-id mtb-example-psoc6-low-power-capsense-freertos --user-app-name CapsenseFreetos --target-dir "C:/mtb_projects"
   ```

**Note:** The project-creator-cli tool uses the `git clone` and `make getlibs` commands to fetch the repository and import the required libraries. For details, see the "Project creator tools" section of the [ModusToolbox&trade; software user guide](https://www.infineon.com/ModusToolboxUserGuide) (locally available at *{ModusToolbox&trade; software install directory}/docs_{version}/mtb_user_guide.pdf*).

To work with a different supported kit later, use the [Library Manager](https://www.infineon.com/ModusToolboxLibraryManager) to choose the BSP for the supported kit. You can invoke the Library Manager GUI tool from the terminal using `make modlibs` command or use the Library Manager CLI tool "library-manager-cli" to change the BSP.

The "library-manager-cli" tool has the following arguments:

Argument | Description | Required/optional
---------|-------------|-----------
`--add-bsp-name` | Name of the BSP that should be added to the application | Required
`--set-active-bsp` | Name of the BSP that should be as active BSP for the application | Required
`--add-bsp-version`| Specify the version of the BSP that should be added to the application if you do not wish to use the latest from manifest | Optional
`--add-bsp-location`| Specify the location of the BSP (local/shared) if you prefer to add the BSP in a shared path | Optional

<br />

Following example adds the CY8CPROTO-062-4343W BSP to the already created application and makes it the active BSP for the app:

   ```
   ~/ModusToolbox/tools_{version}/library-manager/library-manager-cli --project "C:/mtb_projects/CapsenseFreetos" --add-bsp-name CY8CPROTO-062-4343W --add-bsp-version "latest-v4.X" --add-bsp-location "local"

   ~/ModusToolbox/tools_{version}/library-manager/library-manager-cli --project "C:/mtb_projects/CapsenseFreetos" --set-active-bsp APP_CY8CPROTO-062-4343W
   ```

</details>

<details><summary><b>In third-party IDEs</b></summary>

Use one of the following options:

- **Use the standalone [Project Creator](https://www.infineon.com/ModusToolboxProjectCreator) tool:**

   1. Launch Project Creator from the Windows Start menu or from *{ModusToolbox&trade; software install directory}/tools_{version}/project-creator/project-creator.exe*.

   2. In the initial **Choose Board Support Package** screen, select the BSP, and click **Next**.

   3. In the **Select Application** screen, select the appropriate IDE from the **Target IDE** drop-down menu.

   4. Click **Create** and follow the instructions printed in the bottom pane to import or open the exported project in the respective IDE.

<br />

- **Use command-line interface (CLI):**

   1. Follow the instructions from the **In command-line interface (CLI)** section to create the application.

   2. Export the application to a supported IDE using the `make <ide>` command.

   3. Follow the instructions displayed in the terminal to create or import the application as an IDE project.

For a list of supported IDEs and more details, see the "Exporting to IDEs" section of the [ModusToolbox&trade; software user guide](https://www.infineon.com/ModusToolboxUserGuide) (locally available at *{ModusToolbox&trade; software install directory}/docs_{version}/mtb_user_guide.pdf*).

</details>


## Operation

If using a PSoC&trade; 64 "Secure" MCU kit (like CY8CKIT-064B0S2-4343W), the PSoC&trade; 64 device must be provisioned with keys and policies before being programmed. Follow the instructions in the ["Secure Boot" SDK user guide](https://www.infineon.com/dgdlac/Infineon-PSoC_64_Secure_MCU_Secure_Boot_SDK_User_Guide-Software-v07_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f8c361a7666) to provision the device. If the kit is already provisioned, copy-paste the keys and policy folder to the application folder.

1. Connect the board to your PC using the provided USB cable through the KitProg3 USB connector.

2. Open a terminal program and select the KitProg3 COM port. Set the serial port parameters to 8N1 and 115200 baud.

3. Program the board using one of the following:

   <details><summary><b>Using Eclipse IDE for ModusToolbox&trade; software</b></summary>

      1. Select the application project in the Project Explorer.

      2. In the **Quick Panel**, scroll down, and click **\<Application Name> Program (KitProg3_MiniProg4)**.
   </details>

   <details><summary><b>Using CLI</b></summary>

     From the terminal, execute the `make program` command to build and program the application using the default toolchain to the default target. The default toolchain and target are specified in the application's Makefile but you can override those values manually:
      ```
      make program TOOLCHAIN=<toolchain>
      ```

      Example:
      ```
      make program TOOLCHAIN=GCC_ARM
      ```
   </details>

4. After programming, the application starts automatically. Confirm that "\<CE Title>" is displayed on the UART terminal.

    **Figure 1. Terminal output on program startup**

    ![](images/figure1.png)

5. Slide your finger along the slider to view the touch position detected on the serial terminal as shown below:

    **Figure 2. Terminal output on touching the slider**

    ![](images/figure2.png)

6. The example switches to slow scan if touch is not detected for `MAX_CAPSENSE_FAST_SCAN_COUNT` fast scans. The example indicates the transition to slow scan on the serial terminal as shown below:

    **Figure 3. Terminal output when shifting to slow scan**

    ![](images/figure3.png)

7. Touch the slider again to shift to fast scan. The example indicates the transition to fast scan on the serial terminal as shown below:

    **Figure 4. Terminal output when shifting to fast scan**

    ![](images/figure4.png)

8. Measure the current as instructed in the [Making current measurements for PSoC&trade; 6 MCU device](#making-current-measurements-for-psoc&trade;-6-mcu-device) section.


## Debugging

You can debug the example to step through the code. In the IDE, use the **\<Application Name> Debug (KitProg3_MiniProg4)** configuration in the **Quick Panel**. For details, see the "Program and debug" section in the [Eclipse IDE for ModusToolbox&trade; software user guide](https://www.infineon.com/MTBEclipseIDEUserGuide).

**Note:** **(Only while debugging)** On the CM4 CPU, some code in `main()` may execute before the debugger halts at the beginning of `main()`. This means that some code executes twice – once before the debugger stops execution, and again after the debugger resets the program counter to the beginning of `main()`. See [KBA231071](https://community.infineon.com/docs/DOC-21143) to learn about this and for the workaround.


## Design and implementation

The example performs CAPSENSE&trade; scans at two different rates. It scans at `CAPSENSE_FAST_SCAN_INTERVAL_MS` intervals (fast scan) when there is a touch detected on the slider, and at `CAPSENSE_SLOW_SCAN_INTERVAL_MS` interval (slow scan) when there is no touch detected for `MAX_CAPSENSE_FAST_SCAN_COUNT` fast scans. `CAPSENSE_FAST_SCAN_INTERVAL_MS`, `CAPSENSE_SLOW_SCAN_INTERVAL_MS`, and `MAX_CAPSENSE_FAST_SCAN_COUNT` can be configured in *source/capsense.c*.

The example uses two CAPSENSE&trade; widgets; a linear slider, which is set up and scanned during the fast scan, and a GangedSensor (button widget), which is set up and scanned during the slow scan. The linear slider widget has five sensors, whereas the GangedSensor widget has only one sensor which corresponds to a ganged connection of the five sensors used in the linear slider widget.

The main function initializes the UART and creates `capsense_task` before starting the FreeRTOS scheduler. The example disables support for CAPSENSE&trade; tuner by default. You can enable the tuner by defining the `CAPSENSE_TUNER_ENABLE` variable in the *Makefile* under `DEFINES`.

The `capsense_task` is responsible for initializing the CAPSENSE&trade; hardware block, tuner communication if enabled, and scanning and processing the touch information. It also creates and starts a FreeRTOS timer instance which is used to signal the start of a new scan at the end of every timer period. The task implements an FSM to scan, process touch, and schedule sleep/deepsleep.

   **Figure 5. FSM state diagram**

   ![](images/figure5.png)

The FSM comprises the following states:

1. `INITIATE_SCAN`: In this state, the example checks if the CAPSENSE&trade; HW block is busy. If not, the example initiates a CAPSENSE&trade; scan and changes the state to `WAIT_IN_SLEEP`. If tuner is enabled, `Cy_CapSense_ScanAllWidgets` is called to scan the widgets. It is not called if tuner communication is disabled because it sets up and scans both the widgets used in this example which results in longer scan times. Instead, `Cy_CapSense_Scan` is called to scan the widget. The widget that is scanned is the one that was last set up using `Cy_CapSense_SetupWidget`.

2. `WAIT_IN_SLEEP`: In this state, the example locks the deep sleep state and waits for task notification from `capsense_callback`, which signals that the CAPSENSE&trade; scan has completed. The task notification updates the state variable to `PROCESS_TOUCH`.

3. `PROCESS_TOUCH`: In this state, the device processes the scan data. The widget that is processed depends on the type of scan performed i.e., fast scan or slow scan.

   In fast scan, if a new touch is detected, the value of `capsense_fast_scan_count` is reset to `RESET_CAPSENSE_FAST_SCAN_COUNT`, and the slider position is displayed on the serial terminal. If not, `capsense_fast_scan_count` is incremented until `MAX_CAPSENSE_FAST_SCAN_COUNT` after which the timer period is changed to `CAPSENSE_SLOW_SCAN_INTERVAL_MS` and the GangedSensor widget is set up for the next scan.

   In slow scan, if a touch is detected for the GangedSensor widget, the example switches to fast scan mode by setting up the linear slider widget, resetting the value of `capsense_fast_scan_count` to `RESET_CAPSENSE_FAST_SCAN_COUNT`, and changing the timer period to `CAPSENSE_FAST_SCAN_INTERVAL_MS`.

   After processing the touch data, the state variable is changed to `WAIT_IN_DEEP_SLEEP`.

4. `WAIT_IN_DEEP_SLEEP`: In this state, the example unlocks the deep sleep state and waits for task notification from `scan_timer_callback` which signals that the timer period has elapsed and a new scan must be issued. The task notification updates the state variable to `INITIATE_SCAN`.


### Low-power design considerations

The common techniques used for the low-power operation of PSoC&trade; 6 MCU are:

1. Reducing the operating frequency of the CPU

2. Selecting system ULP mode over system LP

3. Selecting a buck regulator over LDO

4. Retaining as less RAM as possible by disabling SRAM controllers and unused pages within a SRAM controller. Note that disabling portion of SRAM requires update to linker script capturing the used length of SRAM. 

In addition to these, the following firmware changes are required to achieve a low-power CAPSENSE&trade; design:

1. Select manual tuning against SmartSense auto-tuning - start from a lower resolution and higher modulator clock for the fastest scan possible until you achieve the desired performance. See [AN85951 - PSoC&trade; 4 and PSoC&trade; 6 MCU CAPSENSE&trade; design guide](https://www.infineon.com/AN85951) for details on manual tuning.

2. Reduce the scan resolution to reduce the scan time.

3. Perform scans at a lower frequency when no touch has been detected. This enables the MCU to stay in deepsleep longer when no touch is detected. Increase the frequency when touch is detected to improve performance but at the cost of increase in current consumption.

4. Use a widget in slow scan where the sensors of the widget used in fast scan are ganged together to constitute a single sensor for shorter scan times. Note that doing so increases the parasitic capacitance of the sensor.

Some of these configurations can be made using the device configurator and CAPSENSE&trade; configurator which are packaged with ModusToolbox&trade; software. See the [Creating a custom device configuration for low-power operation](#creating-a-custom-device-configuration-for-low-power-operation) section.


### Resources and settings

**Table 1. Application resources**

| Resource  |  Alias/object     |    Purpose     |
| :------- | :------------    | :------------ |
| UART (HAL)|cy_retarget_io_uart_obj| UART HAL object used by retarget-io for debug UART port  |
| I2C (HAL)|sEzI2C| Slave EZI2C used for CAPSENSE&trade; tuner communication. This object is not enabled by default. |
| CAPSENSE&trade; (Middleware)| CSD0 | CAPSENSE&trade; hardware used for scanning the position of touch on the linear slider.|

<br />

### Creating a custom device configuration for low-power operation

*In ModusToolbox&trade; software v3.0, the BSPs will be placed locally in the project directory itself. This means that the Application owns the BSP and it is done so that the user can easily customize the BSP. To allow customization, the BSPs are delinked from the git repository. Tools delete the .git folder after creating the BSP. The libs/assets inside mtb_shared folder are not meant to be modified by user and they still carry the link to the git repo.*   

The custom configuration disables the phase-locked loop (PLL), selects system ULP power mode, selects **Minimum Current Buck** as the regulator, and provides a CAPSENSE&trade; configuration with a linear slider and a GangedSensor widget.

In this code example, we will use the Device Configurator (v4.0) to create a custom device configuration for low-power operation. The device configurator is bundled with the ModusToolbox&trade; software installation and can be found at **\<ModusToolbox_installed_location>/tools_\*.\*/device-configurator**.

The example ships with the device configuration in which the CM4 CPU is clocked by FLL at 48 MHz, CLK_PERI at 24 MHz, CAPSENSE&trade; Mod clock at 24 MHz, and PSoC&trade; 6 MCU in ULP power mode.

Perform the following steps using the ModusToolbox&trade; tools like Device Configurator, CAPSENSE&trade; tuner and CAPSENSE&trade; Configurator to create a custom configuration for a new kit. The screenshots provided with the instructions use **CY8CPROTO-062-4343W** as an example.

Launch the [Device Configurator](https://www.infineon.com/ModusToolboxDeviceConfig) tool (v4.0) using the Quick Panel link in the IDE. This opens the *design.modus* file from the *bsps/TARGET_APP_\<kit>/config* folder.

1. Under the **Power** resource, change the **Power mode** to **ULP**, and change the **Core Regulator** under **General** to **Minimum Current Buck** as shown below:

    **Figure 6. Change power mode settings**

    ![](images/figure6-change-power-mode-settings.png)

2. Switch to the **System** tab and expand the **System Clocks** resource. Disable all instances of the PLL by unchecking the box in the **FLL+PLL** section as shown below:

    **Figure 7. Disable PLL**

    ![](images/figure7-disable-pll.png)

   PLL is disabled because it consumes higher current even though the noise performance is better than FLL.

   **2.1. If you are using the FLL to source the clock to CPU:**

    2.1.1. Enable the FLL in the **FLL+PLL** section if disabled. The  maximum operating frequency is 50 MHz in ULP mode; the FLL supports a range of 24-100 MHz. Therefore, provide the value of frequency within 24-50 MHz. This example uses a frequency of 48 MHz as shown below:

    **Figure 8. Enable FLL**

    ![](images/figure8-enable-fll.png)

    2.1.2. The CM4 CPU is clocked by **CLK_FAST**, which is derived from **CLK_HF0**. Ensure that **CLK_HF0** is allotted **CLK_PATH0** to connect **CLK_HF0** to FLL as shown below:

    **Figure 9. Change clock path**

    ![](images/figure9-change-clock-path.png)

    2.1.3. **CLK_PERI** in ULP mode cannot exceed 25 MHz; therefore, ensure that **CLK_PERI** is not greater than 25 MHz by changing the divider appropriately as shown below. Optionally, if the FLL frequency is 24 MHz, you can change the divider for **CLK_PERI** to 1.

    **Figure 10. Change divider for CLK_PERI**

    ![](images/figure10-change-divider-for-clk-peri.png)

   **2.2. If you are using the IMO to source the clock to CPU:**

   2.2.1. You can also use the IMO to clock the CPU. Disable the FLL in the **FLL+PLL** section, if enabled as shown below:

    **Figure 11. Disable FLL**

    ![](images/figure11-disable-fll.png)

   2.2.2. In the **High Frequency** section under **System Clocks**, select **CLK_PERI** and decrease the divider to '1' as shown below:

    **Figure 12. Change divider for CLK_PERI**

    ![](images/figure12-change-divider-for-clk-peri.png)

    Because the source clock is IMO, which is at 8 MHZ, CLK_PERI can also be configured to run at 8 MHZ. This helps in a better performance of the peripherals than when the divider is set as '2'.

3. Save the file, switch to the **Peripherals** tab, and select **CSD (CAPSENSE&trade;)** under **System**. Select **Launch CAPSENSE&trade; configurator** in the panel on the right as shown below:

    **Figure 13. Launch CAPSENSE&trade; configurator**

    ![](images/figure13-launch-capsense-configurator.png)

4. In the **Basic** tab of CAPSENSE&trade; configurator, create a new self-capacitance button widget called "GangedSensor", delete the Button0 and Button1 widgets, and select **Manual Tuning** as shown below:

    **Figure 14. Configure widgets**

    ![](images/figure14-configure-widgets.png)

5. Open the **Advanced** tab.

   **5.1. If you are using FLL to source the clock to CPU:**

   5.1.1. Open the **CSD settings** tab and set the modulator clock divider such that the modulator clock frequency is lower than 50 MHz. This example uses the modulator clock of 24 MHz for a FLL frequency of 48 MHz as shown below:

    **Figure 15. Configure modulator clock**

    ![](images/figure15-configure-modulator-clock.png)

   5.1.2. Open the **Widget Details** tab, select **LinearSlider0**, and change the **Widget hardware parameters** and **Widget threshold parameters** as shown below:

    **Figure 16. Configure linear slider widget parameters**

    ![](images/figure16-configure-linear-slider-widget-parameters.png)

   5.1.3. Select **GangedSensor** and change the **Widget hardware parameters** and **Widget threshold parameters** as shown below:

    **Figure 17. Configure GangedSensor widget parameters**

    ![](images/figure17-configure-ganged-sensor-widget-parameters.png)

    **5.2 If you are using IMO to source the clock to CPU:**

    5.2.1.  Open the **CSD settings** tab and set the modulator clock divider to 1 as shown below. Increasing the modulator clock divider will result in reduction in performance.

    **Figure 18. Configure modulator clock**

    ![](images/figure18-configure-modulator-clock.png)

    5.2.2. Open the **Widget Details** tab, select **LinearSlider0**, and change the **Widget hardware parameters** and **Widget threshold parameters** as shown below:

    **Figure 19. Configure linear slider widget parameters**

    ![](images/figure19-configure-linear-slider-widget-parameters.png)

    5.2.3. Select **GangedSensor** and change the **Widget hardware parameters** and **Widget threshold parameters** as shown below:

    **Figure 20. Configure GangedSensor widget parameters**

    ![](images/figure20-configure-ganged-sensor-widget-parameters.png)

6. Select **GangedSensor_Sns0** under **Widget Details** of the **Advanced** tab and do the following:

    6.1. In the **Sensor connections** option on the panel on the right, unselect **dedicated pin**.

    6.2. Select the remaining ones as shown below to to create a ganged connection of the sensors of the linear slider:

    **Figure 21. Configure GangedSensor connection parameters**

    ![](images/figure21-configure-ganged-sensor-connection-parameters.png)

7. Save and close CAPSENSE&trade; configurator. Return to device configurator and save the file to generate the source files.


### Considerations for widget parameters in CAPSENSE&trade; configurator

1. The example reduces the scan resolution to 9 bits as shown in **Figure 18-19** and **Figure 21-22**. Reducing the scan resolution reduces the scan time at the cost of accuracy.

2. Because the accuracy of capacitance measurements at the sensors is reduced, the widget threshold parameters such as finger threshold and noise threshold need to be updated according to the observed values of raw counts.

   Enable tuner communication by defining the `CAPSENSE_TUNER_ENABLE` variable in the *Makefile* under `DEFINES` and program the example onto the device.

   **Using CAPSENSE&trade; tuner to monitor data:**

   1. Open CAPSENSE&trade; tuner from the IDE Quick Panel.

       You can also run the CAPSENSE&trade; tuner application standalone from *{ModusToolbox&trade; software install directory}/ModusToolbox/tools_{version}/capsense-configurator/capsense-tuner*. In this case, after opening the application, select **File** > **Open** and open the *design.cycapsense* file for the respective kit, which is present in the *bsps/TARGET_APP_\<BSP-NAME>/COMPONENT_BSP_DESIGN_MODUS*.

    2. Ensure that the kit is in KitProg3 mode. See [Firmware-loader](https://github.com/Infineon/Firmware-loader) to learn on how to update the firmware and switch to KitProg3 mode.

    3. In the tuner application, click **Tuner Communication Setup** or select **Tools** > **Tuner Communication Setup**. In the window that appears, select the I2C checkbox under KitProg3 and configure as follows:

       - I2C Address: 8
       - Sub-address: 2-Bytes
       - Speed (kHz): 400

    4. Click **Connect** or select **Communication** > **Connect**.

    5. Click **Start** or select **Communication** > **Start**.

    Under the **Widget View** tab, you can see the corresponding widgets highlighted in blue color when you touch the button or slider. You can also view the sensor data in the **Graph View** tab. For example, to view the sensor data for Button 0, select **Button0_Rx0** under **Button0**.

    **Figure 24** shows the CAPSENSE&trade; tuner displaying the status of CAPSENSE&trade; touch on LinearSlider 0 and GangedSensor.

    **Figure 22. CAPSENSE&trade; tuner showing touch data**

    ![](images/figure22-capsense-tuner-showing-touch-data.png)

    The CAPSENSE&trade; tuner can also be used for CAPSENSE&trade; parameter tuning, and measuring signal-to-noise ratio (SNR). See the [ModusToolbox&trade; CAPSENSE&trade; tuner guide](https://www.infineon.com/ModusToolboxCapSenseTuner) (**Help** > **View Help**) and [AN85951 – PSoC&trade; 4 and PSoC&trade; 6 MCU CAPSENSE&trade; design guide](https://www.infineon.com/AN85951) for more details on selecting the tuning parameters.

3. In the widget threshold parameters, the ON debounce is set as '1', which can result in false positives for touch detection. Increase the value to '3' to reduce false positives; however, this will mean a poor reaction time in slow scan mode.

4. The sensor clock divider for GangedSensor is increased to '8' when CAPSENSE&trade; modulator clock frequency of 24 MHz. This is because its sensor, which comprises a ganged connection of five sensors, has a high parasitic capacitance that requires more time for making measurements. Reducing the sensor clock divider can result in failure to initialize CAPSENSE&trade; due to a calibration error for the sensor.

5. The sensor clock source for the GangedSensor is chosen as direct instead of PRS* or SS* clock. This is because SS* and PRS* modulate the clock, which results in varying pulse widths. This can cause errors when making measurements of the GangedSensor sensor due to its high parasitic capacitance.


### Operation at a custom power supply voltage

The application is configured to work with the default operating voltage of the kit.

**Table 1. Operating voltages supported by the kits**

| Kit                   | Supported operating voltages | Default operating voltage |
| :-------------------- | ---------------------------- | ------------------------- |
| CY8CPROTO-062-4343W   | 3.3 V / 1.8 V                | 3.3 V                     |
| CY8CKIT-062-BLE       | 3.3 V / 1.8 V                | 3.3 V                     |
| CY8CKIT-062-WiFi-BT   | 3.3 V / 1.8 V                | 3.3 V                     |
| CY8CKIT-062S2-43012   | 3.3 V / 1.8 V                | 3.3 V                     |
| CYW9P62S1-43438EVB-01 | 3.3 V only                   | 3.3 V                     |
| CYW9P62S1-43012EVB-01 | 1.8 V only                   | 1.8 V                     |
| CY8CPROTO-062S3-4343W | 3.3 V / 1.8 V                | 3.3 V                     |

<br />

For kits that support multiple operating voltages, follow the instructions to use the example at a custom power supply, such as 1.8 V:

1. Launch the [Device configurator](https://www.infineon.com/ModusToolboxDeviceConfig) tool using the Quick Panel link in the IDE. This opens the *design.modus* file from the *bsps/TARGET_APP_\<kit>/config* folder.

2. Update the **Operating Conditions** parameters in Power settings with the desired voltage and select **File** > **Save**.

   **Figure 23. Power settings to work with 1.8 V**

   ![](images/figure4.png)

3. Change the jumper/switch setting as follows:

   **Table 2. Jumper/switch position for 1.8 V operation**

   | Kit                   | Jumper/switch position |
   | :-------------------- | ---------------------- |
   | CY8CPROTO-062-4343W   | J3 (1-2)               |
   | CY8CKIT-062-BLE       | SW5 (1-2)              |
   | CY8CKIT-062-WIFI-BT   | SW5 (1-2)              |
   | CY8CKIT-062S2-43012   | J14 (1-2)              |
   | CY8CPROTO-062S3-4343W | J3 (1-2)               |
   | CY8CKIT-064B0S2-4343W | J14 (1-2)              |

<br />

4. Rebuild and program the application to evaluate the application at the new power setting.


### Making current measurements for PSoC&trade; 6 MCU device

- **CY8CKIT_062S2_43012, CYW9P62S1_43438EVB_01, and CYW9P62S1_43012EVB_01**

   Measure the current at J15 across VTARG and P6_VDD. Ensure that J25 is removed to eliminate leakage currents across potentiometer R1.


- **CY8CPROTO_062_4343W**

   Measure the current by removing R65 on the right of the board close to the USB connector of the PSoC&trade; 6 MCU USB (device) and connecting an ammeter between VTARG (J2.32) and P6_VDD (J2.24).

   The PSoC&trade; 6 MCU deepsleep current is approximately 350 uA because of the pull-up resistor (R24) attached to the WL_HOST_WAKE pin P0_4, which leaks around 330 uA. This is because P0_4 is driven LOW when there is no network activity. Therefore, remove R24 (at the back of the board, below J1.9).

- **CY8CPROTO_062S3_4343W**

   Measure the current by removing R59 on the back of the board, towards the right and above J2.31, and connecting an ammeter between VTARG (J2.32) and P6_VDD (J2.31).

- **CY8CKIT_062_WIFI_BT**

   Measure the current by connecting an ammeter to the PWR MON jumper J8.

- **CY8CKIT_062_BLE**

   Measure the current by connecting an ammeter to the PWR MON jumper J8.


### Current measurements

**Table 3** provides the typical current measurement values where PSoC&trade; 6 MCU is operated with the Arm&reg; Cortex&reg;-M4 CPU (CM4) running at 48 MHz in ultra-low-power (ULP) mode with **Minimum Current Buck** selected as the regulator. The amount of SRAM retained is provided in the table below. **CLK_PERI** and CAPSENSE&trade; modulator clock run at 48 MHz.

**Table 3. Typical current values when CPU clock is sourced by FLL**

<table>
<tr><th>Target</th><th>Amount of SRAM retained</th><th>Slow scan (Scan GangedSensor at 5 Hz. The widget has only one sensor which is a ganged connection of five sensors fom the linear slider widget.)</th><th>Fast scan (Scan LinearSlider widget at 50 Hz. The widget has five sensors.)</th></tr>
        <tr>
            <td>CY8CKIT-062-BLE</td>
            <td>64 KB (Block 0 and Block 9 of SRAM0 are retained</td>
            <td>26 µA</td>
            <td>255 µA</td>
        </tr>
        <tr>
            <td>CY8CKIT-062-WIFI-BT</td>
            <td>64 KB (Block 0 and Block 9 of SRAM0 are retained</td>
            <td>26 µA</td>
            <td>300 µA</td>
        </tr>
        <tr>
            <td>CY8CKIT-062S2-43012</td>
            <td>288 KB (Block 0 of SRAM0 and 256 KB of SRAM2 are retained)</td>
            <td>26 µA</td>
            <td>315 µA</td>
        </tr>
        <tr>
            <td>CY8CPROTO-062-4343W</td>
            <td>288 KB (Block 0 of SRAM0 and 256 KB of SRAM2 are retained)</td>
            <td>26.6 µA</td>
            <td>350 µA</td>
        </tr>
        <tr>
            <td>CY8CPROTO-062S3-4343W</td>
            <td>64 KB (Block 0 and Block 8 of SRAM0 are retained</td>
            <td>20.7 µA</td>
            <td>230 µA</td>
        </tr>
        <tr>
            <td>CYW9P62S1-43012EVB-01</td>
            <td>64 KB (Block 0 and Block 9 of SRAM0 are retained</td>
            <td>25.2 µA</td>
            <td>360 µA</td>
        </tr>
        <tr>
            <td>CYW9P62S1-43438EVB-01</td>
            <td>64 KB (Block 0 and Block 9 of SRAM0 are retained</td>
            <td>23 µA</td>
            <td>260 µA</td>
        </tr>
        <tr>
            <td>CY8CKIT-064B0S2-4343W</td>
            <td>288 KB (Block 0 of SRAM0 and 256 KB of SRAM2 are retained)</td>
            <td>30.6 µA</td>
            <td>365 µA</td>
        </tr>
 </table>

<br />

 **Table 4** provides the typical current measurement values where PSoC&trade; 6 MCU is operated with CM4 running at 8 MHz in ULP mode with **Minimum Current Buck** selected as the regulator. The amount of SRAM retained is provided in the table below. **CLK_PERI** and CAPSENSE&trade; modulator clock run at 8 MHz.


**Table 4. Typical current values when CPU clock is sourced by IMO**

<table>
<tr><th>Target</th><th>Amount of SRAM retained</th><th>Slow scan (Scan GangedSensor at 5 Hz. The widget has only one sensor which is a ganged connection of five sensors fom the linear slider widget.)</th><th>Fast scan (Scan LinearSlider widget at 50 Hz. The widget has five sensors.)</th></tr>
        <tr>
            <td>CY8CKIT-062-BLE</td>
            <td>64 KB (Block 0 and Block 9 of SRAM0 are retained</td>
            <td>23 µA</td>
            <td>165 µA</td>
        </tr>
        <tr>
            <td>CY8CKIT-062-WIFI-BT</td>
            <td>64 KB (Block 0 and Block 9 of SRAM0 are retained</td>
            <td>23 µA</td>
            <td>170 µA</td>
        </tr>
        <tr>
            <td>CY8CKIT-062S2-43012</td>
            <td>288 KB (Block 0 of SRAM0 and 256 KB of SRAM2 are retained)</td>
            <td>23 µA</td>
            <td>165 µA</td>
        </tr>
        <tr>
            <td>CY8CPROTO-062-4343W</td>
            <td>288 KB (Block 0 of SRAM0 and 256 KB of SRAM2 are retained)</td>
            <td>22.3 µA</td>
            <td>190 µA</td>
        </tr>
        <tr>
            <td>CY8CPROTO-062S3-4343W</td>
            <td>64 KB (Block 0 and Block 8 of SRAM0 are retained</td>
            <td>18.6 µA</td>
            <td>146.6 µA</td>
        </tr>
        <tr>
            <td>CYW9P62S1-43012EVB-01</td>
            <td>64 KB (Block 0 and Block 9 of SRAM0 are retained</td>
            <td>19.0 µA</td>
            <td>185 µA</td>
        </tr>
        <tr>
            <td>CYW9P62S1-43438EVB-01</td>
            <td>64 KB (Block 0 and Block 9 of SRAM0 are retained</td>
            <td>20 µA</td>
            <td>170 µA</td>
        </tr>
        <tr>
            <td>CY8CKIT-064B0S2-4343W</td>
            <td>288 KB (Block 0 of SRAM0 and 256 KB of SRAM2 are retained)</td>
            <td>27.6 µA</td>
            <td>210.5 µA</td>
        </tr>
 </table>

<br />


## Related resources

Resources  | Links
-----------|----------------------------------
Application notes  | [AN228571](https://www.infineon.com/AN228571) – Getting started with PSoC&trade; 6 MCU on ModusToolbox&trade; software <br /> [AN215656](https://www.infineon.com/AN215656) – PSoC&trade; 6 MCU: Dual-CPU system design
Code examples  | [Using ModusToolbox&trade; software](https://github.com/Infineon/Code-Examples-for-ModusToolbox-Software) on GitHub
Device documentation | [PSoC&trade; 6 MCU datasheets](https://documentation.infineon.com/html/psoc6/bnm1651211483724.html) <br /> [PSoC&trade; 6 technical reference manuals](https://documentation.infineon.com/html/psoc6/zrs1651212645947.html)
Development kits | Select your kits from the [evaluation board finder](https://www.infineon.com/cms/en/design-support/finder-selection-tools/product-finder/evaluation-board)
Libraries on GitHub  | [mtb-pdl-cat1](https://github.com/Infineon/mtb-pdl-cat1) – PSoC&trade; 6 peripheral driver library (PDL)  <br /> [mtb-hal-cat1](https://github.com/Infineon/mtb-hal-cat1) – Hardware abstraction layer (HAL) library <br /> [retarget-io](https://github.com/Infineon/retarget-io) – Utility library to retarget STDIO messages to a UART port
Middleware on GitHub  | [capsense](https://github.com/Infineon/capsense) – CAPSENSE&trade; library and documents <br /> [psoc6-middleware](https://github.com/Infineon/modustoolbox-software#psoc-6-middleware-libraries) – Links to all PSoC&trade; 6 MCU middleware
Tools  | [Eclipse IDE for ModusToolbox&trade; software](https://www.infineon.com/modustoolbox) – ModusToolbox&trade; software is a collection of easy-to-use software and tools enabling rapid development with Infineon MCUs, covering applications from embedded sense and control to wireless and cloud-connected systems using AIROC&trade; Wi-Fi and Bluetooth&reg; connectivity devices.

## Other resources


Infineon provides a wealth of data at www.infineon.com to help you select the right device, and quickly and effectively integrate it into your design.

For PSoC&trade; 6 MCU devices, see [How to design with PSoC&trade; 6 MCU - KBA223067](https://community.infineon.com/docs/DOC-14644) in the Infineon Developer community.


## Document history

Document title: *CE230968* - *PSoC&trade; 6 MCU: Low-power CAPSENSE&trade; design (FreeRTOS)*

| Version | Description of change |
| ------- | --------------------- |
| 1.0.0   | New code example.      |
| 1.1.0   | Updated to support ModusToolbox&trade; software v2.3.<br /> Added support for CY8CKIT-062S4. |
| 1.2.0   | Added support for CY8CEVAL-062S2 and CY8CEVAL-062S2-LAI-4373M2. |
| 2.0.0   | Major update to support ModusToolbox&trade; software v3.0. <br /> This version is not backward compatible with previous versions of ModusToolbox&trade;. <br /> Added support for CY8CEVAL-062S2-MUR-43439M2 and CY8CKIT-064S0S2-4343W |
| 2.1.0   | Update to support ModusToolbox&trade; v3.1 and CAPSENSE&trade; middleware v4.X
------


© Cypress Semiconductor Corporation, 2020-2022. This document is the property of Cypress Semiconductor Corporation, an Infineon Technologies company, and its affiliates ("Cypress").  This document, including any software or firmware included or referenced in this document ("Software"), is owned by Cypress under the intellectual property laws and treaties of the United States and other countries worldwide.  Cypress reserves all rights under such laws and treaties and does not, except as specifically stated in this paragraph, grant any license under its patents, copyrights, trademarks, or other intellectual property rights.  If the Software is not accompanied by a license agreement and you do not otherwise have a written agreement with Cypress governing the use of the Software, then Cypress hereby grants you a personal, non-exclusive, nontransferable license (without the right to sublicense) (1) under its copyright rights in the Software (a) for Software provided in source code form, to modify and reproduce the Software solely for use with Cypress hardware products, only internally within your organization, and (b) to distribute the Software in binary code form externally to end users (either directly or indirectly through resellers and distributors), solely for use on Cypress hardware product units, and (2) under those claims of Cypress’s patents that are infringed by the Software (as provided by Cypress, unmodified) to make, use, distribute, and import the Software solely for use with Cypress hardware products.  Any other use, reproduction, modification, translation, or compilation of the Software is prohibited.
<br />
TO THE EXTENT PERMITTED BY APPLICABLE LAW, CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS DOCUMENT OR ANY SOFTWARE OR ACCOMPANYING HARDWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  No computing device can be absolutely secure.  Therefore, despite security measures implemented in Cypress hardware or software products, Cypress shall have no liability arising out of any security breach, such as unauthorized access to or use of a Cypress product. CYPRESS DOES NOT REPRESENT, WARRANT, OR GUARANTEE THAT CYPRESS PRODUCTS, OR SYSTEMS CREATED USING CYPRESS PRODUCTS, WILL BE FREE FROM CORRUPTION, ATTACK, VIRUSES, INTERFERENCE, HACKING, DATA LOSS OR THEFT, OR OTHER SECURITY INTRUSION (collectively, "Security Breach").  Cypress disclaims any liability relating to any Security Breach, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any Security Breach.  In addition, the products described in these materials may contain design defects or errors known as errata which may cause the product to deviate from published specifications. To the extent permitted by applicable law, Cypress reserves the right to make changes to this document without further notice. Cypress does not assume any liability arising out of the application or use of any product or circuit described in this document. Any information provided in this document, including any sample design information or programming code, is provided only for reference purposes.  It is the responsibility of the user of this document to properly design, program, and test the functionality and safety of any application made of this information and any resulting product.  "High-Risk Device" means any device or system whose failure could cause personal injury, death, or property damage.  Examples of High-Risk Devices are weapons, nuclear installations, surgical implants, and other medical devices.  "Critical Component" means any component of a High-Risk Device whose failure to perform can be reasonably expected to cause, directly or indirectly, the failure of the High-Risk Device, or to affect its safety or effectiveness.  Cypress is not liable, in whole or in part, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any use of a Cypress product as a Critical Component in a High-Risk Device. You shall indemnify and hold Cypress, including its affiliates, and its directors, officers, employees, agents, distributors, and assigns harmless from and against all claims, costs, damages, and expenses, arising out of any claim, including claims for product liability, personal injury or death, or property damage arising from any use of a Cypress product as a Critical Component in a High-Risk Device. Cypress products are not intended or authorized for use as a Critical Component in any High-Risk Device except to the limited extent that (i) Cypress’s published data sheet for the product explicitly states Cypress has qualified the product for use in a specific High-Risk Device, or (ii) Cypress has given you advance written authorization to use the product as a Critical Component in the specific High-Risk Device and you have signed a separate indemnification agreement.
<br />
Cypress, the Cypress logo, and combinations thereof, WICED, ModusToolbox, PSoC, CapSense, EZ-USB, F-RAM, and Traveo are trademarks or registered trademarks of Cypress or a subsidiary of Cypress in the United States or in other countries. For a more complete list of Cypress trademarks, visit www.infineon.com. Other names and brands may be claimed as property of their respective owners.
