# ha-garage
My home automation device for the garage.  This home automation device controls my garage door as well as providing some sensor data.

## Parts List
1. [Adafruit HUZZAH ESP8266](https://www.adafruit.com/products/2471)
2. [SainSmart 2-Channel 5v Relay Module](http://www.sainsmart.com/arduino-pro-mini.html)

## Code
1. Copy **mywifi.sample.h** to **mywifi.h**
2. Edit **mywifi.h** to set the SSID for your Wifi connection as well as the password.
3. Copy **mymqttbroker.sample.h** to **mymqttbroker.sample.h**
4. Edit **mymqttbroker.sample.h** to set the broker ip, port, user name and password
5. Compile the sketch and upload it to the device.

## MQTT Topics and Messages
The following key words are used to describe how the MQTT topic is organized.  
* **{homeid}** is the Home Id you want to use to identify which home the device is in. 
* **{roomid}** is the Room Id you want to use to identify which room the device is in.  
* **{deviceid}** is the Device Id you want to use to all for multiple devices of the same type in a given room.  

### MQTT Subscriptions
The following subscription topics should be supported by the device.  

Tell the garage door to open  
```
/{homeid}/{roomid}/{deviceid}/cover/cmd open
```

Tell the garage door to close
```
/{homeid}/{roomid}/{deviceid}/cover/cmd close
```

Tell the garage door to stop
```
/{homeid}/{roomid}/{deviceid}/cover/cmd stop
```

### MQTT Publications
The following topics and messages are published by this device when it's state has changed.

When the garage door is open.
```
/{homeid}/{roomid}/{deviceid}/cover/state open 
```

When the garage door is opening.
```
/{homeid}/{roomid}/{deviceid}/cover/state opening 
```

When the garage door is closed.
```
/{homeid}/{roomid}/{deviceid}/cover/state closed 
```

When the garage door is closing.
```
/{homeid}/{roomid}/{deviceid}/cover/state closing 
```

The time it took for the garage door to close in milliseconds.
```
/{homeid}/{roomid}/{deviceid}/cover/closetime 30000 
```

The time it took for the garage door to open in milliseconds.
```
/{homeid}/{roomid}/{deviceid}/cover/opentime 30000
```

The temperature in the room.
```
/{homeid}/{roomid}/{deviceid}/temperature 55 
```

The humidity in the room.
```
/{homeid}/{roomid}/{deviceid}/humidity 55 
```
