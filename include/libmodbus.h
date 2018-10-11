
#ifndef __LIBMODBUS_H__
#define __LIBMODBUS_H__ 

#pragma once

#include <iostream>
#include "modbus_lua.h"

using namespace std;
class Poll:public SrTimerHandler{
public:
	Poll(mbdevices& mbd, mbtypes& mbt);

	virtual ~Poll();

	void operator()(SrTimer &timer, SrAgent &agent);

private:
	mbdevices& _mbd;
	mbtypes& _mbt;
};
class Modbus{
public:
	~Modbus();
	static int readCo(int num, int nb, char* bits);
	static int readDi(int num, int nb, char* bits);
	static int readHr(int num, int nb, char* bits);
	static int readIr(int num, int nb, char* bits);
	static int writeCo(int slave,int num, int nb);
	static int writeHr(int slave,int num, int nb);
private:
	Modbus();
};

#endif // __LIBMODBUS_H__
