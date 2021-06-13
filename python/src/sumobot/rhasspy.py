# pyuhand - control your uhand
# Copyright (C) 2021  Pete <github@kthxbye.us>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import glob
import time

import paho.mqtt.client as mqtt
import threading
import time
import json



class SumobotSkill(object):
    def __init__(self):
        self._msgThread = threading.Thread(target = self._run)
        self._mqtt_client = mqtt.Client()
        self._mqtt_client.on_connect = self._onConnect
        self._mqtt_client.on_message = self._onMessage
        
        
    def _onConnect(self, client, userdata, flags, rc):
        # subscribe to all messages
        client.subscribe('hermes/intent/sumobot:circle')
        client.subscribe('hermes/intent/sumobot:muege')
    def start(self):
        self._mqtt_client.connect('rhasspy.local', 1883)
        self._msgThread.start()
    
    def _run(self):
        self._mqtt_client.loop_forever()
        print("Ended Skill")
    
    def _runCommand(self, timeSpan, left, right, honk): 
        elapsed = 0
        delay = 0.05
        while elapsed < timeSpan:
            returnMsg = {
                    "left" : left, 
                    "right" : right, 
                    "honk": honk
                    }
            self._mqtt_client.publish("remote", json.dumps(returnMsg))
            time.sleep(delay)
            elapsed += delay
        returnMsg = {
                    "left" : 0, 
                    "right" : 0, 
                    "honk": 0
                    }
        self._mqtt_client.publish("remote", json.dumps(returnMsg))
        
    def _circle(self):
        time.sleep(2)
        self._runCommand(3, 1, -1, 0)
        self._runCommand(3, -1, 1, 0)
        self._runCommand(0.2, 0, 0, 1)
    
    def _dance(self):
        time.sleep(2)
        self._runCommand(0.3,  1,  1, 0) # forward
        self._runCommand(0.3, -1, -1, 0) # backard
        self._runCommand(0.3, -1,  1, 0) # left
        self._runCommand(0.3,  1, -1, 0) # right
        self._runCommand(3,  -1, 1, 0) # left circle

    def _beep(self):
        pass
        
    def stop(self):
        print("Skill should end")
        self._mqtt_client.disconnect()
        print("mqtt disconnected")

    def _onMessage(self, client, userdata, msg):
        data = json.loads(msg.payload.decode("utf-8"))
        sessionId = data['sessionId']
        print("TOPIC:"+ msg.topic)

        if("sumobot:circle" in msg.topic):
            text = "I am crazy! "
            function = self._circle
        if("sumobot:muege" in msg.topic):
            text = "I want to dance with mu ke! "
            function = self._dance
        waveThread = threading.Thread(target = function)
        waveThread.start()
    

        print("Hello for %s" % (sessionId))
        print(sessionId)
        
        returnMsg = {
            "sessionId" : sessionId, 
            "text": text
            }
        
        client.publish("hermes/dialogueManager/endSession", json.dumps(returnMsg))




skill = SumobotSkill()
skill.start()
while(True):
    time.sleep(5)
skill.stop()
