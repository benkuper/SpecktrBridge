class MIDIMessage
{
public:
  MIDIMessage(){}
  MIDIMessage(byte * msg) :
   header(msg[0]),
   timestamp(msg[1]),
   status(msg[2]),
   data1(msg[3]),
   data2(msg[4])
   {
   }
  byte header;
  byte timestamp;
  byte status;
  byte data1;
  byte data2;
};

MIDIMessage lastValue;
