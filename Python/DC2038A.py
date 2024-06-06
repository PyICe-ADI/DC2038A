import os
from PyICe.lab_core import *
from PyICe.lab_instruments.AD5259 import AD5259
from PyICe import twi_instrument, lab_interfaces
from PyICe.models.thermistors.NTCS0402E3103FLT import NTCS0402E3103FLT

# Client list
PYICE_GUI_ADDRESS       = 1   # Must match expected firmware addresses
SMBUS_ADDRESS           = 2   # Must match expected firmware addresses
IDENTIFY_ADDRESS        = 3   # Must match expected firmware addresses
SMBALERT_PIN_ADDRESS    = 4   # Must match expected firmware addresses

class DC2038A(instrument):
    ''' DC2038A Demo Board, a base board that accepts the LTC4162 bench evaluation board that be given to customers.
        Has PyICe as a depdendency.'''
    def __init__(self, channel_master, comport, verbose=False):
        self._base_name = 'DC2038A'
        instrument.__init__(self, f"{self._base_name}")
        self.verbose            = verbose
        self.NTCS0402E3103FLT   = NTCS0402E3103FLT()
        interface_factory       = lab_interfaces.interface_factory()
        self.fast_twi_port      = interface_factory.get_labcomm_twi_interface(comport_name=comport, src_id=PYICE_GUI_ADDRESS, dest_id=SMBUS_ADDRESS, baudrate=115200, timeout=1)
        self.LTC4162            = twi_instrument.twi_instrument(self.fast_twi_port, PEC=True)
        self.AD5259             = AD5259(interface_twi=self.fast_twi_port, addr7=0x18, full_scale_ohms=100000)
        self.AD5259.add_all_channels(channel_base_name="AD5259")
        self.LTC4162.populate_from_file(os.path.dirname(os.path.dirname(os.path.abspath(__file__))) + "/Python/XML/LTC4162-SAD.xml")
        channel_master.add(self.LTC4162)
        channel_master.add(self.AD5259)
        self.add_channel_identify(self._base_name + "_board_id")
        self.add_channel_thermistor_Tc(self._base_name + "_thermistor_Tc")

    def __del__(self):
        '''Close interface (serial) ports on exit???'''
        pass

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

    def add_channel_thermistor_Tc(self, channel_name):
        def _set_thermistor_temp(Tc):
            setting = round(self.NTCS0402E3103FLT.voltage_ratio_from_Tc(Tc) * 255)
            self._send_payload(port=self.fast_twi_port, payload=setting)
        # def _get_wdpin_state():
            # self._send_payload(port=self.WDDIS_port, payload=WDDIS_GET_STATE)
            # return self._get_payload(port=self.WDDIS_port, datatype="integer")
        self.thermistor = channel(name=channel_name, write_function=_set_thermistor_temp)
        # self.thermistor._read = _get_wdpin_state
        return self._add_channel(self.thermistor)