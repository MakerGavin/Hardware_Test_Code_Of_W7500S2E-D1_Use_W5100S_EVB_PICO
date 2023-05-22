# Hardware_Test_Code_Of_W7500S2E-D1_Use_W5100S_EVB_PICO
Hardware_Test_Code_Of_W7500S2E-D1_Use_W5100S_EVB_PICO
Use ChatGPT to develop W7500S2E-D1 Hardware test Code which Deployed in the W5100S_EVB_PICO
detailed introduction ：Hackster.io
https://www.hackster.io/gavinchiong/use-chatgpt-to-develop-a-hardware-test-code-for-w7500s2e-d1-bd1cdb
maker.wiznet.io
https://maker.wiznet.io/gavinchang/projects/use-chatgpt-to-develop-a-hardware-test-code-for-w7500s2e-d1/

W7500S2E-D1 uses WIZNET's W7500 as the MCU, the onboard switch chip can forward serial data to the Ethernet, and has two Ethernet ports(RJ45) onboard, which supports ring network topology, and can be connected between every two modules through a network cable, without the need for Each module is connected to the switch using a network cable, which is convenient for setting up a field network.

Before this product is released, I need to do a detailed test on this module, and I need to design a hardware test program to monitor the operating status of the module. For example, how many modules can be supported on one link at the same time, whether there will be disconnection during operation, and so on.
W7500S2E has a setting software running on a PC, named "WIZS2E ConfigTool", which can discover devices by broadcasting, and can view and configure modules. But if this program is deployed on a computer, the computer needs to be turned on 24 hours and cannot sleep.
My idea is to deploy the test program on the W5100S_EVB_PICO module. 
This EVB module has an Ethernet chip W5100S, which can monitor the running status of the module on the link by interacting with the data of the test network without the participation of the computer. View the status of each device on the web page.
