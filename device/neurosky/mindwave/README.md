# mindwave


![NeuroSky MindWave](neurosky-mind_wave.png)


#NeuroSky Developer Program 
[developer.neurosky.com](http://developer.neurosky.com) 

- ThinkGear Connector

"The ThinkGear Socket Protocol (TGSP) is a JSON-based protocol for the transmission and receipt of ThinkGear brainwave data between a client and a server. TGSP was designed to allow languages and/or frameworks without a standard serial port API (e.g. Flash and most scripting languages) to easily integrate brainwave-sensing functionality through socket APIs."


NeuroSky MindWave Json Response:
```
{"poorSignalLevel":200,"status":"scanning"}
{"poorSignalLevel":200,"status":"scanning"}
{"poorSignalLevel":200,"status":"scanning"}
{"poorSignalLevel":200,"status":"scanning"}
{"poorSignalLevel":200,"status":"scanning"}
{"poorSignalLevel":200,"status":"scanning"}
{"poorSignalLevel":200,"status":"scanning"}
{"poorSignalLevel":200,"status":"scanning"}
{"poorSignalLevel":200,"status":"scanning"}
```
```
{"blinkStrength":54}
```

```
{
	"eSense": {
		"attention": 69,
		"meditation": 66
	},
	"eegPower": {
		"delta": 510548,
		"theta": 234052,
		"lowAlpha": 54405,
		"highAlpha": 23841,
		"lowBeta": 21134,
		"highBeta": 13078,
		"lowGamma": 5397,
		"highGamma": 4070
	},
	"poorSignalLevel": 0
}
```


References:
https://github.com/borg/ThinkGear-Java-socket
https://github.com/WOLFI3654/ThinkGearConnector

Others:

https://github.com/topics/neurosky




