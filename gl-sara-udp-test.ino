#include <ArduinoLog.h>

#define WAIT_FOR_DEBUG_CONSOLE // ako je aktivno, setup ceka da se konektuje debug monitor
// #define ONLY_PASSTHROUGH   // ako je aktivno, ne procesiraju se komande za seriala vec samo sve salje na modemov stream
// #define DISABLE_LOGGING // ako je aktivno, Arudino-log library je NOOP

unsigned long BAUD = 115200;
int UDP_SOCKET_INDICATOR = 0; // od 0 do 6, pogledati dokumentaciju ublox => sara
String SERVER_IP_ADDRESS = "46.101.183.211";
String SERVER_PORT = "2222";
const String AT_COMMAND_READ_UDP_RESPONSE = "AT+USORF=0,1024"; // cita udp response buffer sa cipa. Pogledati dokumentaciju AT komandi za objasnjenje
bool socketInitialized = false;

// region contants
#define MODEM_DELAY_SEND 300
#define MODEM_DELAY_READ 200
// endregion contants

/* Board check - SODAQ SARA */
#if defined(ARDUINO_SODAQ_SARA)
#define DEBUG_STREAM SerialUSB
#define MODEM_STREAM Serial1
#else
#error "Please select the SODAQ SARA as your board"
#endif

void setup()
{

  delay(50);
  Log.begin(LOG_LEVEL_VERBOSE, &DEBUG_STREAM);
  // Log.setPrefix(printTimestamp); // Uncomment to get timestamps as prefix
  // Log.setSuffix(printNewline); // Uncomment to get newline as suffix
  pinMode(SARA_ENABLE, OUTPUT);
  pinMode(SARA_TX_ENABLE, OUTPUT);
  pinMode(SARA_R4XX_TOGGLE, OUTPUT);

  digitalWrite(SARA_ENABLE, HIGH);
  digitalWrite(SARA_TX_ENABLE, HIGH);
  digitalWrite(SARA_R4XX_TOGGLE, LOW);

  // Start communication
  DEBUG_STREAM.begin(BAUD);
  MODEM_STREAM.begin(BAUD);
  delay(50);

#if defined(WAIT_FOR_DEBUG_CONSOLE)
  while (!DEBUG_STREAM)
    ; // ceka na konekciju monitora
  delay(50);
#endif

  Log.notice("****          Initializing...                " CR);
  printHelpText();
}

// Forward every message to the other serial
void loop()
{
  // Log.notice("loop()");
  handlePassthrough();
}

void handlePassthrough()
{
  if (DEBUG_STREAM.available())
  {
#if defined(ONLY_PASSTHROUGH)
    while (DEBUG_STREAM.available())
    {
      MODEM_STREAM.write(DEBUG_STREAM.read());
    }
#else
    String commandFromSerialMonitor = DEBUG_STREAM.readString();
    if (commandFromSerialMonitor.startsWith("-test"))
    {
      // TODO procesiranje test metode
      Log.notice("TEST command" CR);
    }
    else if (commandFromSerialMonitor.startsWith("-proba"))
    {
      Log.notice("Izvrsavam UDP Probu!" CR);
      sendStringAsUdpMessage("trla baba lan");
    }
    else if (commandFromSerialMonitor.startsWith("-?") || commandFromSerialMonitor.startsWith("-help"))
    {
      printHelpText();
    }
    else if (commandFromSerialMonitor.startsWith("-stat"))
    {
      checkConnectionStatus();
    }
    else
    {
      MODEM_STREAM.print(commandFromSerialMonitor);
      Log.verbose("Sent to modem, waiting.... command: \" %s \"" CR, commandFromSerialMonitor.c_str());
      // delay(MODEM_DELAY_SEND);
    }
#endif
  }

  String responseFromModem;
  if (MODEM_STREAM.available())
  {
    responseFromModem = MODEM_STREAM.readString();
    Log.notice(responseFromModem.c_str());
  }
}

bool checkConnectionStatus()
{
  MODEM_STREAM.println("AT+CGATT?");
  delay(MODEM_DELAY_READ);

  String responseFromModem = "";
  if (MODEM_STREAM.available())
  {
    responseFromModem = MODEM_STREAM.readString();
  }
  Log.notice("Response from modem: " CR);
  Log.notice(responseFromModem.c_str());
  String substringObj = responseFromModem.substring(20, 21);
  Log.notice(substringObj.c_str());

  if (substringObj.equals("1"))
  {
    Log.verbose("We are connected!" CR);
    return true;
    Log.notice("Not connected!" CR);
    return false;
  }
  else
  {
    Log.notice("Not connected! - error during check" CR);
    return false;
  }
}

void printHelpText()
{
  Log.notice("\n###################### DEMO ##############\n\n -? - help (prints this text) \n -proba - Proba slanja UDP poruke; \n -debug - executes debug command \n <any other string> - sends data to a modem to execute as AT command. Example: AT+CCLK?\n\n");
}

void initializeSocket()
{
  delay(100); // dodatni delay prvi put, resava random "no phone" gresku

  if (!socketInitialized)
  {
    Log.notice("Initializing socket...");
    sendAtCommandToModem("AT+USOCR=17");
    socketInitialized = true;
    Log.notice("Socket initialization done;");
    delay(100);
  }
  else
  {
    Log.notice("Socket already initialized;");
  }
}

String sendAtCommandToModem(const String atCommandAsString)
{
  Log.notice("\n>> Komanda za slanje : %s" CR, atCommandAsString.c_str());
  MODEM_STREAM.println(atCommandAsString);

  delay(MODEM_DELAY_SEND);

  String responseFromModem;

  if (MODEM_STREAM.available())
  {
    responseFromModem = MODEM_STREAM.readString(); // procitaj sta je modem odgovorio. Ovde ide uspeh komande, greske i sl.
    Log.notice("\n<< Odgovor sa na AT komandu : %s" CR, responseFromModem.c_str());
  }
  else
  {
    Log.notice("<< No response from modem;" CR);
  }
  return responseFromModem;
}

void sendStringAsUdpMessage(String data)
{
  String command = generateUdpSendAtCommand(data);
  initializeSocket();
  Log.notice("\nKomanda za slanje : %s" CR, command.c_str());
  sendAtCommandToModem(command);

  // region process response
  {
    Log.notice("Salje se komanda za citanje odgovora : %s" CR, AT_COMMAND_READ_UDP_RESPONSE.c_str());

    String responseFromUpdRead = sendAtCommandToModem(AT_COMMAND_READ_UDP_RESPONSE);

    Log.notice("\nOdgovor: %s" CR, responseFromUpdRead.c_str());

    // TODO obraditi (parsirati) odgovor modema na AT komandu za citanje (AT+USORF=0,1024)
    // primer odgovora:
    // AT+USORF=0,1024
    //
    // +USORF: 0,"46.101.183.211",2222,13,"trla baba lan"
    //
    // OK

    // format je - socket, ip posiljaoca, port posiljaoca, duzina poruke, podaci
  }
  // endregion process response
}

String generateUdpSendAtCommand(const String data)
{
  int dataLength = data.length();
  String commandToExecute = "AT+USOST=";
  commandToExecute += String(UDP_SOCKET_INDICATOR);
  commandToExecute += (",\"" + SERVER_IP_ADDRESS + "\"," + SERVER_PORT + ",");
  commandToExecute += (dataLength);
  commandToExecute += (",");
  commandToExecute += ("\"" + data + "\"");
  //Log.notice(("Komanda: " + commandToExecute + ";").c_str());
  return commandToExecute;
}

void printTimestamp(Print *_logOutput)
{
  char c[12];
  int m = sprintf(c, "%10lu ", millis());
  _logOutput->print(c);
}
