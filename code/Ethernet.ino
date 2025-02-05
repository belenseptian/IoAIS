void initEthernet()
{
  Serial.println("Begin Ethernet");
  displayText(1, 0, 10, "Connecting...", true);
  Ethernet.init(5);   // MKR ETH Shield

  if (Ethernet.begin(mac)) 
  { // Dynamic IP setup
      Serial.println("DHCP OK!");
  }
  else
  {
      Serial.println("Failed to configure Ethernet using DHCP");
      // Check for Ethernet hardware present
      if (Ethernet.hardwareStatus() == EthernetNoHardware) 
      {
        Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
        while (true) 
        {
          delay(1); // do nothing, no point running without Ethernet hardware
        }
      }
      if (Ethernet.linkStatus() == LinkOFF) 
      {
        Serial.println("Ethernet cable is not connected.");
      }

      IPAddress ip(MYIPADDR);
      IPAddress dns(MYDNS);
      IPAddress gw(MYGW);
      IPAddress sn(MYIPMASK);
      Ethernet.begin(mac, ip, dns, gw, sn);
      Serial.println("STATIC OK!");
  }
  connected = 1;
  delay(5000);

  Serial.print("Local IP : ");
  Serial.println(Ethernet.localIP());
  Serial.print("Subnet Mask : ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("Gateway IP : ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("DNS Server : ");
  Serial.println(Ethernet.dnsServerIP());

  Serial.println("Ethernet Successfully Initialized");
}

void sendtoWeb(char * nmea)
{
  // if you get a connection, report back via serial:
  if (EClient.connect(server, 8080)) 
  {
    offline = 0;
    String nmeaString = String(nmea);
    // Make a HTTP request:
    EClient.println("POST /api/sentences HTTP/1.1");
    EClient.println("Host: 103.84.207.245");
    EClient.println("Content-Type: application/x-www-form-urlencoded");
    EClient.println("Connection: close");
    EClient.println("User-Agent: Arduino/1.0");
    EClient.print("Content-Length: ");
    EClient.println(nmeaString.length());
    EClient.println();
    EClient.print(nmeaString);
    EClient.println(); 
    displayText(1, 0, 10, "NETSEND OK", true);
  } 
  else 
  {
    // if you didn't get a connection to the server:
    offline = 1;
    displayText(1, 0, 10, "NET FAIL", true);
  }
}