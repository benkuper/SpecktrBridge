#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "MIDIMessage.h"

//#define DEVICE_NAME "GSKR"
#define DEVICE_NAME "DSKR"

#define MIDI_SERVICE_UUID "03b80e5a-ede8-4b33-a751-6ce34ec4c700"
#define BLEMIDI_CHAR_UUID "7772e5db-3868-4112-a1a9-f2669d106bf3"

static BLEUUID serviceUUID(MIDI_SERVICE_UUID);
static BLEUUID    charUUID(BLEMIDI_CHAR_UUID);

BLEAddress * address;
BLERemoteCharacteristic * blec;
bool doConnect = false;
bool isConnected = false;

//pre-declarations
void connectDevice(BLEAddress address);

//Callback
class MyAdvertisedDeviceCallbacks : 
  public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice d) {
    //Serial.print("BLE Advertised Device found: ");
    //Serial.println(d.toString().c_str());

    if (d.haveServiceUUID() && d.getServiceUUID().equals(serviceUUID)) {
      if(d.getName() == DEVICE_NAME)
      {
          address = new BLEAddress(d.getAddress());
          doConnect = true;
          d.getScan()->stop();
      }
    }
   }
};

void setup() {
  Serial.begin(9600);
  delay(500); //safe
  sendCommand('c',0);
  BLEDevice::init("");
  initScan();
}

void loop() {
  
   if (doConnect == true) {
    if (connectToServer()) {
      isConnected = true;
      sendCommand('c',1);
    } else {
      sendCommand('c',0);
      //Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    
    doConnect = false;
    delay(500);
  }

  if(isConnected)
  {
    readMIDI();
    delay(10);
  }
  
}

void initScan()
{
  //Serial.println("Init Scan...");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  //Serial.println("Start scan");
  pBLEScan->start(30);
}

void readMIDI()
{
   std::string s = blec->readValue();
    
    if(s.length() >= 5)
    {
      MIDIMessage value = MIDIMessage((byte *)s.c_str());
      
      if(value.timestamp != lastValue.timestamp)
      {
        switch(value.status)
        {
        
          case 144:
          {
          //Serial.print("Note On ");
          //Serial.print(value.data1);
          //Serial.print('\t');
          //Serial.println(value.data2);
          byte v[2]{value.data1,value.data2};
          sendCommand('n',v,2);
          }
          break;

          case 128:
          {
          //Serial.print("Note Off ");
          //Serial.println(value.data1);
          sendCommand('o',value.data1);
          }
          break;
  
          case 176:
          {
         //Control Change
          }
          break;
        }
        
        lastValue = value;
      }
    }  
}


//Connection
bool connectToServer()
{
  BLEClient * c = BLEDevice::createClient();
  
  /* 
  Serial.print("Connect to device ");
  Serial.print(DEVICE_NAME);
  Serial.print("...");
  */
  
  c->connect(*address);
  
  BLERemoteService* pRemoteService = c->getService(serviceUUID);
  if(pRemoteService == nullptr) return false;
  
  blec = pRemoteService->getCharacteristic(charUUID);
  if(blec == nullptr) return false;
  
  return true;
}

void sendCommand(char command, byte value)
{
  Serial.write(command);
  if(value == 255) value = 254;
  Serial.write(value); 
  Serial.write(255);
}

void sendCommand(char command, byte* values, int numValues)
{
  Serial.write(command);
  for(int i=0;i<numValues;i++) 
  {
    byte val = values[i];
    if(val == 255) val = 254;
    Serial.write(val);  
  }
  
  Serial.write(255);
}




