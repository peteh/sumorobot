import paho.mqtt.client as mqtt
import threading
import time
import json
import time
from threading import Thread
import copy

from inputs import get_gamepad
import signal
from xbox360controller import Xbox360Controller

DEAD_ZONE_AXIS = 0.2
DEAD_ZONE_TRIGGER = 0.01

class GamePad():
    def __init__(self):

        
        self._joyState = {}
        # buttons
        self._joyState["bA"] = False
        self._joyState["bB"] =  False
        self._joyState["bX"] =  False
        self._joyState["bY"] =  False
        
        # mode button
        self._joyState["bMode"] =  False

        # shoulder triggers
        self._joyState["bTL"] = False
        self._joyState["bTR"] = False

        # shoulder trigger axis
        self._joyState["aTL"] = False
        self._joyState["aTR"] = False

        # left stick
        self._joyState["aXL"] = 0.
        self._joyState["aYL"] = 0.

        # right stick
        self._joyState["aXR"] = 0.
        self._joyState["aYR"] = 0.

        self._joyThread = threading.Thread(target = self._joy)

        #self._controller = Xbox360Controller(0, axis_threshold=DEAD_ZONE_AXIS)
        # Button A events
        #self._controller.button_a.when_pressed = self.on_button_pressed
        #self._controller.button_a.when_released = self.on_button_released

        # Left and right axis move event
        #self._controller.axis_l.when_moved = self.on_axis_moved
        #self._controller.axis_r.when_moved = self.on_axis_moved

    def on_axis_moved(self, axis):
        print('Axis {0} moved to {1} {2}'.format(axis.name, axis.x, axis.y))

    def on_button_pressed(self, button):
        print('Button {0} was pressed'.format(button.name))


    def on_button_released(button):
        print('Button {0} was released'.format(button.name))

    def start(self):
        self._joyThread.start()

    def getState(self):
        return self._joyState

    def _normalizeAxis(self, raw):
        val = round( - raw / 32767, 2)
        if abs(val) < DEAD_ZONE_AXIS:
            val = 0.
        return val
    
    def _normalizeTrigger(self, raw):
        val = round(raw / 255, 2)
        if abs(val) < DEAD_ZONE_TRIGGER:
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
                
                if(event.code == 'BTN_WEST'):
                    self._joyState["bY"] = bool(event.state)
                    changed = True
                if(event.code == 'BTN_NORTH'):
                    self._joyState["bX"] = bool(event.state)
                    changed = True
                
                if(event.code == 'BTN_MODE'):
                    self._joyState["bMode"] = bool(event.state)
                    changed = True
                    
                # shoulder triggers
                if(event.code == 'BTN_TR'):
                    self._joyState["bTR"] = bool(event.state)
                    changed = True
                if(event.code == 'BTN_TL'):
                    self._joyState["bTL"] = bool(event.state)
                    changed = True
                
                # shoulder triggers
                if(event.code == 'ABS_Z'):
                    self._joyState["aTL"] = self._normalizeTrigger(event.state)
                    changed = True
                if(event.code == 'ABS_RZ'):
                    self._joyState["aTR"] = self._normalizeTrigger(event.state)
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

class TwoWheelGuyStrategy():
    def __init__(self, mqtt):
        self._mqtt = mqtt
    
    def process(self, previousState, newState):
        msg = {}
        msg["left"] = newState["aXL"]
        msg["right"] = newState["aXR"]
        msg["honk"] = newState["bTR"]

        msg["overrideLedLeft"] = newState["bX"]
        msg["overrideLedRight"] = newState["bB"]
        msg["overrideLedMiddle"] = newState["bA"]

        msg["overrideDistance"] = newState["bTL"]
        self._mqtt.publish("remote/2wheelguy", json.dumps(msg))

class SpiderPiStrategy():
    def __init__(self, mqtt):
        self._mqtt = mqtt
    
    def process(self, previousState, newState):
        msg = {}
        mode = "stop"
        speed = (newState["aTL"] + newState["aTR"]) / 2.

        # forward
        if(newState["aXR"] > 0):
            mode = "forward"
        # backward
        elif(newState["aXR"] < 0):
            mode = "backward"
        #right strafe
        elif(newState["aYR"] < 0):
            mode = "strafe_r"
        #left strafe
        elif(newState["aYR"] > 0):
            mode = "strafe_l"
        #right turn
        elif(newState["aYL"] < 0):
            mode = "turn_r"
        #left turn
        elif(newState["aYL"] > 0):
            mode = "turn_l"
        
        msg["mode"] = mode
        msg["speed"] = speed
        print(msg)
        self._mqtt.publish("remote/spiderpi", json.dumps(msg))

class HotwordBeep(object):
    def __init__(self, gamepad):
        self._msgThread = threading.Thread(target = self._run)
        self._gamepad = gamepad
        self._mqtt_client = mqtt.Client()
        self._mqtt_client.on_connect = self._onConnect
        self._mqtt_client.on_message = self._onMessage
        self._previousState = gamepad.getState()

        self._strategies = []
        self._strategies.append(TwoWheelGuyStrategy(self._mqtt_client))
        self._strategies.append(SpiderPiStrategy(self._mqtt_client))
        self._currentStrategy = 0
        
        
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
    
    def sendMessage(self):
        state = self._gamepad.getState()
        if self._previousState["bMode"] == False and state["bMode"] == True:
            print("Mode pressed, switching")
            self._currentStrategy = (self._currentStrategy + 1) % len(self._strategies)
            print(self._currentStrategy)

        
        self._strategies[self._currentStrategy].process(self._previousState, state)

        self._previousState = copy.deepcopy(state)

    def stop(self):
        print("Skill should end")
        self._mqtt_client.disconnect()
        print("mqtt disconnected")
        
    def _onMessage(self, client, userdata, msg):
        if msg.topic == "hermes/intent/ChangeTVState":
            msgPayload = json.loads(msg.payload.decode("utf-8"))
            self._handleShutup(msgPayload)
            
gamepad = GamePad()
gamepad.start()

skill = HotwordBeep(gamepad)
skill.start()


while(True):
    try:
        skill.sendMessage()
        time.sleep(0.02)
    except KeyboardInterrupt:
        break
skill.stop()
