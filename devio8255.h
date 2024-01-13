// ============================
// 8255 dual port i/o driver
// (C) Diana Kanecki 2023 All Rights Reserved
// Version 1.0.0
// ============================
// uses Afafruit mcp23017 library
// code usage
//   #include "devio8255.h"
// ============================

extern Adafruit_MCP23X17 mcp;

#define port1reset mcp.pa4
#define port1cs mcp.pa5
#define port2reset mcp.pa6
#define port2cs mcp.pa7
#define A0 mcp.p2
#define A1 mcp.p3
#define RD mcp.pa0
#define WR mcp.p1

int portReset[2]= { port1reset, port2reset } ;
int portCS[2] = { port1cs, port2cs } ;
int addr1[2] = { A0, A1 } ;

#define rCtrl 0
#define rPortA 1
#define rPortB 2
#define rPortC 3

#define D0a mcp.pb0
#define D1a mcp.pb1
#define D2a mcp.pb2
#define D3a mcp.pb3
#define D4a mcp.pb4
#define D5a mcp.pb5
#define D6a mcp.pb6
#define D7a mcp.pb7

int dataBus [8] = { D0a, D1a, D2a, D3a, D4a, D5a, D5a, D6a, D7a }

void wrCtrlReg(int portId,  int data1){
  int b1;
  int tmp = rCtrl;

// select port chip
digitalWrite(portCS[portId], HIGH);

// select port register
for (int i=0; i<=1; i++){
  b1 = tmp & 0x1;
  digitalWrite(addr1[i], b1);
  tmp = tmp << 1;
}

// write ctrl data
for (int i=0; i<=7; i++) {
    b1 = data1 & 0x1;
    digitalWrite(dataBus[i], b1);
    data1 = data1 << 1;
}

digitalWrite(WR, LOW);
delay(1);
digitalWrite(WR, HIGH);
  
// end io function
digitalWrite(portCS[portId], LOW);
}
