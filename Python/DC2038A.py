import os
from PyICe.lab_core import *
from PyICe.lab_instruments.AD5259 import AD5259
from PyICe import twi_instrument, lab_interfaces
from PyICe.models.thermistors.NTCS0402E3103FLT import NTCS0402E3103FLT

# Client list
PYICE_GUI_ADDRESS   = 1   # Must match expected firmware addresses
SMBUS_ADDRESS       = 2   # Must match expected firmware addresses
IDENTIFY_ADDRESS    = 3   # Must match expected firmware addresses
SMBALERT_ADDRESS    = 4   # Must match expected firmware addresses
EMPTY               = ""

class DC2038A(instrument):
    ''' DC2038A Demo Board, an LTC4162 bench evaluation board that be given to customers.
        Has PyICe as a depdendency.'''
    def __init__(self, channel_master, comport, variant_file, verbose=False):
        self._base_name = 'DC2038A'
        instrument.__init__(self, f"{self._base_name}")
        self.verbose            = verbose
        self.NTCS0402E3103FLT   = NTCS0402E3103FLT()
        interface_factory       = lab_interfaces.interface_factory()
        self.main_twi_port      = interface_factory.get_labcomm_twi_interface(comport_name=comport, src_id=PYICE_GUI_ADDRESS, dest_id=SMBUS_ADDRESS,    baudrate=115200, timeout=1)
        self.IDENT_port         = interface_factory.get_labcomm_raw_interface(comport_name=comport, src_id=PYICE_GUI_ADDRESS, dest_id=IDENTIFY_ADDRESS, baudrate=115200, timeout=1)
        self.SMBALERT_port      = interface_factory.get_labcomm_raw_interface(comport_name=comport, src_id=PYICE_GUI_ADDRESS, dest_id=SMBALERT_ADDRESS, baudrate=115200, timeout=4)
        self.LTC4162            = twi_instrument.twi_instrument(self.main_twi_port, PEC=True)
        self.AD5259             = AD5259(interface_twi=self.main_twi_port, addr7=0x18, full_scale_ohms=100000)
        self.AD5259.add_all_channels(channel_base_name="AD5259")
        self.LTC4162.populate_from_file(os.path.dirname(os.path.dirname(os.path.abspath(__file__))) + f"/Python/XML/{variant_file}")
        channel_master.add(self.LTC4162)
        channel_master.add(self.AD5259)
        self.add_channel_identify(self._base_name + "_board_id")
        self.add_channel_thermistor_tdegc(self._base_name + "_thermistor_tdegc")
        self.add_channel_smbalertpin(self._base_name + "_smbalertpin")
        self.LTC4162["thermistor_voltage"].set_display_format_str("0.1f")
        self.LTC4162["die_temp"].set_display_format_str("0.1f")
        self.LTC4162["bsr"].set_display_format_str("0.3f")
        self.LTC4162["ibat"].set_display_format_str("0.3f")
        self.LTC4162["iin"].set_display_format_str("0.3f")
        self.LTC4162["vbat"].set_display_format_str("0.3f")
        self.LTC4162["vbat_filt"].set_display_format_str("0.3f")
        self.LTC4162["vcharge_dac"].set_display_format_str("0.3f")
        self.LTC4162["vcharge_setting"].set_display_format_str("0.3f")
        self.LTC4162["vin"].set_display_format_str("0.3f")
        self.LTC4162["vout"].set_display_format_str("0.3f")
        self.LTC4162["thermal_reg_end_temp"].set_display_format_str("0.1f")
        self.LTC4162["thermal_reg_start_temp"].set_display_format_str("0.1f")
        self.LTC4162["jeita_t1"].set_display_format_str("0.1f")
        self.LTC4162["jeita_t2"].set_display_format_str("0.1f")
        self.LTC4162["jeita_t3"].set_display_format_str("0.1f")
        self.LTC4162["jeita_t4"].set_display_format_str("0.1f")
        self.LTC4162["jeita_t5"].set_display_format_str("0.1f")
        self.LTC4162["jeita_t6"].set_display_format_str("0.1f")
        self.LTC4162["vcharge_jeita_2"].set_display_format_str("0.3f")
        self.LTC4162["vcharge_jeita_3"].set_display_format_str("0.3f")
        self.LTC4162["vcharge_jeita_4"].set_display_format_str("0.3f")
        self.LTC4162["vcharge_jeita_5"].set_display_format_str("0.3f")
        self.LTC4162["vcharge_jeita_6"].set_display_format_str("0.3f")
        self.LTC4162["thermistor_voltage_hi_alert_limit"].set_display_format_str("0.1f")
        self.LTC4162["thermistor_voltage_lo_alert_limit"].set_display_format_str("0.1f")
        self.LTC4162["die_temp_hi_alert_limit"].set_display_format_str("0.1f")

    def _send_payload(self, port, payload):
        if self.verbose:
            print(f"Sending payload: {payload.encode('latin1')}")
        port.send_payload(payload)

    def _get_payload(self, port, datatype):
        x = port.receive_packet()
        if self.verbose:
            print(f"Received packet {x}")
        if datatype == "integer":
            return port.parser.payload_buffer_as_integer
        elif datatype == "string":
            return port.parser.payload_buffer_as_string
        else:
            raise(f"DC2038A: Don't know data type {datatype}.")

    def add_channel_identify(self, channel_name):
        def _get_id():
            self._send_payload(port=self.IDENT_port, payload="?")
            return self._get_payload(self.IDENT_port, datatype="string")
        new_channel = channel(channel_name, read_function=_get_id)
        return self._add_channel(new_channel)

    def add_channel_thermistor_tdegc(self, channel_name):
        def _set_thermistor_temp(temperature):
            setting = self.NTCS0402E3103FLT.voltage_ratio_from_tdegc(rbias=10e3, tdegc=temperature)
            self.AD5259.wiper_channel.write(setting)
        def _get_thermistor_temp():
            return self.NTCS0402E3103FLT.tdegc_from_voltage_ratio(rbias=10e3, ratio=self.AD5259.wiper_channel.read())
        self.thermistor = channel(name=channel_name, write_function=_set_thermistor_temp)
        self.thermistor.set_display_format_str("0.1f")
        self.thermistor._read = _get_thermistor_temp
        return self._add_channel(self.thermistor)
        
    def add_channel_smbalertpin(self, channel_name):
        def _get_smbalert_pin():
            self._send_payload(port=self.SMBALERT_port, payload=EMPTY)
            return self._get_payload(port=self.SMBALERT_port, datatype="integer")
        new_channel = channel(channel_name, read_function=_get_smbalert_pin)
        return self._add_channel(new_channel)