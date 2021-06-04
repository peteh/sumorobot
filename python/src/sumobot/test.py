import paho.mqtt.client as mqtt
import threading
import time
import json
import time
from threading import Thread


from inputs import get_gamepad

class HotwordBeep(object):
    def __init__(self):
        self._msgThread = threading.Thread(target = self._run)
        self._joyThread = threading.Thread(target = self._joy)
        self._mqtt_client = mqtt.Client()
        self._mqtt_client.on_connect = self._onConnect
        self._mqtt_client.on_message = self._onMessage
        self._joyState = {}
        self._joyState["left"] = 0.
        self._joyState["right"] = 0.
        self._joyState["honk"] = 0
        
    def _onConnect(self, client, userdata, flags, rc):
        # subscribe to all messages
        client.subscribe("hermes/intent/ChangeTVState")
    
    def start(self):
        self._mqtt_client.connect('rhasspy.local', 1883)
        self._msgThread.start()
        self._joyThread.start()
    
    def _joy(self):
        while 1:
            events = get_gamepad()
            changed = False
            for event in events:
                #print(event.ev_type, event.code, event.state)
                if(event.code == 'BTN_TR'):
                    self._joyState["honk"] = event.state
                    changed = True
                if(event.code == 'ABS_Y'):
                    axis = round( - event.state / 32767, 2)
                    if abs(axis) < 0.15:
                        axis = 0.
                    self._joyState["left"] = axis
                    changed = True
                if(event.code == 'ABS_RY'):
                    axis = round( - event.state / 32767, 2)
                    if abs(axis) < 0.15:
                        axis = 0.
                    self._joyState["right"] = axis
                    changed = True
                if(changed):
                    print("Left: %.2f" % (self._joyState["left"]))
                    print("Right: %.2f" % (self._joyState["right"]))
                    print("Honk: %d" % (self._joyState["honk"]))
                    #self.sendMessage()

    def _run(self):
        self._mqtt_client.loop_forever()
        print("Ended Skill")
    
    def sendMessage(self):
        msg = json.dumps(self._joyState)
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
while(True):
    try:
        skill.sendMessage()
        time.sleep(0.02)
    except KeyboardInterrupt:
        break
skill.stop()
