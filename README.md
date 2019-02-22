# gl-sara-udp-test - NB-IoT UDP Example
## 

UDP echo example - UDP read/write. 


## Compatibility

* <img src="https://www.arduino.cc/favicon.ico" height="16" width="16"> [ SODAQ Sara R410m board](https://support.sodaq.com/sodaq-one/sara/) - tested
*  other uBlox N2XX, R4XX chips - untested. Check AT commands manual for your board and add support in preprocessors directives

## Prerequisites
* [SODAQ Sara board support](http://support.sodaq.com/sodaq-one/arduino-ide-setup/)
* [Arduino-Log library](https://github.com/thijse/Arduino-Log) 
* any echo UDP server. You may use this code to host your own: https://github.com/garagelabbelgrade/gl-udp-test-echo-server


## WARNING
**This code is for demo (educational) purpose only!** It is not production-ready, performance optimized or intended to be used as seed project. Usage may result in charges by telecom carrier.  **Use at your own risk**

## Quickstart

### Run simple echo demo:

- Edit sourcecode to set echo server&apos;s IP address: 

```c++
String SERVER_IP_ADDRESS = "XXX.XXX.XXX.XXX";
String SERVER_PORT = "2222";
```

-  Connect to serial/debug console.  Controller  will print out welcome message. Example:

```
: ****          Initializing...                ****
N:  ###################### DEMO ##############

 -? - help (prints this text) 
 -proba - Proba slanja UDP poruke; 
 -debug - executes debug command 
 <any other string> - sends data to a modem to execute as AT command. Example: AT+CCLK?
■
```

- Send `-proba` command via serial/debug console.  Controller will send pre-fixed UDP message to defined ip/port; Example output:

```

N: Izvrsavam UDP Probu!
N: Initializing socket...N: 
>> Komanda za slanje : AT+USOCR=17
N: 
<< Odgovor sa na AT komandu : AT+USOCR=17

+USOCR: 0

OK

N: Socket initialization done;N: 
Komanda za slanje : AT+USOST=0,"46.101.183.211",2222,13,"trla baba lan"
N: 
>> Komanda za slanje : AT+USOST=0,"46.101.183.211",2222,13,"trla baba lan"
N: 
<< Odgovor sa na AT komandu : AT+USOST=0,"46.101.183.211",2222,13,"trla baba lan"

+USOST: 0,13

OK

N: Salje se komanda za citanje odgovora : AT+USORF=0,1024
N: 
>> Komanda za slanje : AT+USORF=0,1024
N: 
<< Odgovor sa na AT komandu : AT+USORF=0,1024


+USORF: 0,"46.101.183.211",2222,13,"trla baba lan"

OK

N: 
Odgovor: AT+USORF=0,1024


+USORF: 0,"46.101.183.211",2222,13,"trla baba lan"

OK

```

- ... inspect source code and learn how to send AT commands to controller programmatically.

# AT Commands refrerence:
For full reference quide check official [SODAQ Sara support website](http://support.sodaq.com/sodaq-one/sodaq-sara-r410m/).


AT Command | Comment
------------ | -------------
AT+URAT=8 | Locks module to NB-IoT protocol
AT+COPS=1,2,"22005" | Locks module to specific telecom operater. 22005 in this example represents VIP Mobile from Serbia (c220 o05).
AT+CGDCONT=1,"IP","vip.iot" | APN settings  (for VIP Mobile in this example)
AT+UBANDMASK=1,524288| setting specific band if needed (check with your carrier before usage)
AT+CFUN=15|Saves previouslly-entered settings to internal memory and reboots communicational chip to apply settings.
AT+CSQ | Checks for base signal. will be ~30 if ok. 99 - no signal.
AT+CGATT? | Checks if connected. Will return 1 if connected and ready.
AT+CCLK? | Queries for current UTC time broadcasted by carrier
AT+USOCR=17 | Opens new `UDP` socket. Will return socket identifier (0 - 6)
AT+USOST=0,"45.76.90.233",6000,4,"asdf" | Sends UDP message to server.  parts: <br>>0 - socket identifier (socket created in previous step) <br> > server &apos;s ip address  <br> > server&apos;s port <br> > length of our UDP message <br> > data (message)



#Serial console commands

List of suppored commands:
- `-?` or `-help` - prints help text
- `-proba`  - initializes outbound UDP socket and sends test UDP message
asdf
- `-debug` - does nothing
-  [any other text] - if you send any other string it will be passed to modem (use for AT commands).

# _
<p align="center">Made with :heart: by the <a href="https://garagelab.rs">GarageLab Belgrade</a> team using <a href="https://code.visualstudio.com/">vscode</a> :copyright: 2019</p>
