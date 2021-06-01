import time, random
import context
import paho.mqtt.client as mqtt

print ("Start")
INTERVAL = 10
next_reading = time.time()
client = mqtt.Client()
client.username_pw_set(username="uteq",password="uteq")

client.connect("graycloud.gq",1883)

light = 0
light_x = 0

try:
  while True:
    light = random.randrange(1023)
    print (light)

    if(light != light_x):
      light_x = light
      client.publish("CG/data1",light)
      print ("MQTT Sended")
      

    next_reading += INTERVAL
    sleep_time = next_reading-time.time()
    if sleep_time > 0:
      time.sleep(sleep_time)
except KeyboardInterrupt:
  pass

client.loop_stop()
client.disconnect()

#Edicion en atom
