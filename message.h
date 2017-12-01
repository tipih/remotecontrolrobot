#pragma once
//Set Baud Rate AT + BAUD = *Query baud rate AT + BAUD ?
//
//Set Frequency
//AT + FREQ = **
//
//Query frequency
//AT + FREQ ? Set number of retransmissions AT + RETRY = ** Query number of retransmissions AT + RETRY ?
//
//Set whether to return
//AT + BACK = *
//
//Query whether to return
//AT + BACK ? Set TX ID AT + TID = *…* Query TX ID AT + TID ?
//Set RX ID
//AT + RID = *…*
//
//Query RX ID
//AT + RID ? Set Transmit Power AT + POWER = ** Query Transmit Power AT + POWER ?
//
//Restore Defaults
//AT + RESET
//
//Query firmware version
//AT + INF

//Baud Rate		AT Command		Baud Rate		AT Command 
//2400			AT + BAUD = 0	38400			AT + BAUD = 5
//4800			AT + BAUD = 1	57600			AT + BAUD = 6 
//9600			AT + BAUD = 2	115200			AT + BAUD = 7
//14400			AT + BAUD = 3	128000			AT + BAUD = 8 
//19200			AT + BAUD = 4	256000			AT + BAUD = 9


enum messageId {
	sendPID,
	msgType2,
	writeEprom,
	sendBalancePoint,
	recvAnalyze,
	sendDeadband,
	sendSteeringCmd,
	enableUltrasoundSensor
};
#define endOfMessage 0xff;

struct __attribute__((packed)) debug_info {
	byte id;
	byte data1;
	byte data2;
	byte data3;
	byte data4;
	float SelfBalancePidSetpoint;
	float PidSetpoint;
	byte end;
};