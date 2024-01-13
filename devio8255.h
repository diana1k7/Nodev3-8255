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

void wrCtrlReg(int portId,  int data1){
  
