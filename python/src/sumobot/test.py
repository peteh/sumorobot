import paho.mqtt.client as mqtt
import threading
import time
import json
import time
from threading import Thread


from inputs import get_gamepad


class GamePad():
    def __init__(self):
        self._joyState = {}
        # buttons
        self._joyState["bA"] = False
        self._joyState["bB"] =  False
        self._joyState["bX"] =  False
        self._joyState["bY"] =  False
        
        # shoulder triggers
        self._joyState["bTL"] = False
        self._joyState["bTR"] = False

        # left stick
        self._joyState["aXL"] = 0.
        self._joyState["aYL"] = 0.

        # right stick
        self._joyState["aXR"] = 0.
        self._joyState["aYR"] = 0.

        self._joyThread = threading.Thread(target = self._joy)

    def start(self):
        self._joyThread.start()

    def getMessage(self):
        return self._joyState

    def _normalizeAxis(self, raw):
        val = round( - raw / 32767, 2)
        if abs(val) < 0.15:
            val = 0.
        return val

    def _joy(self):
        while 1:
            events = get_gamepad()
            changed = False
            for event in events:
                print(event.ev_type, event.code, event.state)
                # Buttons
                if(event.code == 'BTN_SOUTH'):
                    self._joyState["bA"] = bool(event.state)
                    changed = True
                if(event.code == 'BTN_EAST'):
                    self._joyState["bB"] = bool(event.state)
                    changed = True
                
                if(event.code == 'BTN_NORTH'):
                    self._joyState["bY"] = bool(event.state)
                    changed = True
                if(event.code == 'BTN_WEST'):
                    self._joyState["bX"] = bool(event.state)
                    changed = True
                    
                # shoulder triggers
                if(event.code == 'BTN_TR'):
                    self._joyState["bTR"] = bool(event.state)
                    changed = True
                if(event.code == 'BTN_TL'):
                    self._joyState["bTL"] = bool(event.state)
                    changed = True
                
                # left stick
                if(event.code == 'ABS_X'):
                    self._joyState["aYL"] = self._normalizeAxis(event.state)
                    changed = True
                if(event.code == 'ABS_Y'):
                    self._joyState["aXL"] = self._normalizeAxis(event.state)
                    changed = True

                # right stick
                if(event.code == 'ABS_RX'):
                    self._joyState["aYR"] = self._normalizeAxis(event.state)
                    changed = True
                if(event.code == 'ABS_RY'):
                    self._joyState["aXR"] = self._normalizeAxis(event.state)
                    changed = True
                if(changed):
                    pass
                    #self.sendMessage()
                print(self._joyState)
        

class HotwordBeep(object):
    def __init__(self):
        self._msgThread = threading.Thread(target = self._run)
        
        self._mqtt_client = mqtt.Client()
        self._mqtt_client.on_connect = self._onConnect
        self._mqtt_client.on_message = self._onMessage
        self._joyState = {}
        self._joyState["left"] = 0.
        self._joyState["right"] = 0.
        self._joyState["honk"] = 0
        
    def _onConnect(self, client, userdata, flags, rc):
        # subscribe to all messages
        #client.subscribe("hermes/intent/ChangeTVState")
        pass
    
    def start(self):
        self._mqtt_client.connect('rhasspy.local', 1883)
        self._msgThread.start()
        

    def _run(self):
        self._mqtt_client.loop_forever()
        print("Ended Skill")
    
    def sendMessage(self, msg):
        msg = json.dumps(msg)
        #print(msg)
        self._mqtt_client.publish("remote", msg)

    def stop(self):
        print("Skill should end")
        self._mqtt_client.disconnect()
        print("mqtt disconnected")
        
    def _onMessage(self, client, userdata, msg):
        if msg.topic == "hermes/intent/ChangeTVState":
            msgPayload = json.loads(msg.payload.decode("utf-8"))
            self._handleShutup(msgPayload)

skill = HotwordBeep()
skill.start()

gamepad = GamePad()
gamepad.start()
while(True):
    try:
        skill.sendMessage(gamepad.getMessage())
        time.sleep(0.02)
    except KeyboardInterrupt:
        break
skill.stop()
