# DC2038A
A software and firmware stack that will allow the LTC4162's DC2038A eval board to be operated with PyICe

The original DC2038A firmware and software stacks are not well supported by ADI.
They have been lost to the annals of time, especially through the ADI acquisition of LTC and the resulting attrition of the orignal developers and contractors.

This project attempts to reboot the DC2038A with a fresh new firmware stack based on the BCB606 eval board project, the refreshed labcomm protocol and, the now public, PyICe.

HOW TO:

1) The DC2038A will need to be flashed with the firmware located here using the Arduino Software IDE.
    • The board type should be selected as "Arduino Zero Native USB Port" (SAMD21 processor).
    • The following libraries will need to be added to the Arduino depdendency list through the "Sktech -> Include Library" menu.
        • AsyncDelay
        • FlashStorage_SAMD
        • SoftWire
        
2) PIP Install PyICe or create a development environment with PyICe directly from GitHub.
3) Power the DC2038A board and plug in the USB connector.
4) Run EZ_DC2038A.py
5) Enter the COM port number of the DC2038A when requested.