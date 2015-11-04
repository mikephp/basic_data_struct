import wmi
import os
import sys
import platform
import time
import win32com.client as client

class DataPollster(object):

    def get_cpu(self):
        # Initilization
        c = wmi.WMI()
        data_dict = {}
        for cpu in c.Win32_Processor():
            device = cpu.DeviceID.lower()

            # Get cpu_usage
            data_dict[device] = {'volume':float(cpu.LoadPercentage), 'unit':'%'}
        return data_dict