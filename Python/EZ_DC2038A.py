import sys, os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from PyICe.lab_utils.banners import print_banner
from DC2038A import DC2038A
from PyICe import lab_core

print_banner("", "Please enter the COM port number for your AQ48 Demo Board.", "It can be obtained from the Windows Device Manager.", "")
comport = input("Enter COM port number, COM")
master = lab_core.channel_master()
master.add(DC2038A(master, comport=f"COM{comport}", verbose=False))
master.gui()