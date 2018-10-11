
#ifndef __MODBUS_LUA_H__
#define __MODBUS_LUA_H__

#include <map>
#include <list>
#include <iostream>
#include <functional>
#include <sragent.h>
#include <srnethttp.h>
#include <srutils.h>
#include <srreporter.h>
#include <integrate.h>
#include "srdevicepush.h"
#include <regex>
#include "srlogger.h"

using namespace std;
using namespace std::placeholders;

#pragma once

typedef struct {
	map <string ,string> coil;//线圈
	map <string ,string> discrete_input;//离散输入量
	map <string ,string> holding_register;//保持寄存器
	map <string ,string> input_register;//输入寄存器
	string addr;//ip地址
	string slave;//设备的modbus地址
	string type;//设备对应的设备数据库模型的系统id
	string ops_id;
}m_devices;

typedef struct {
	vector <vector<string>> coil;//线圈
	vector <vector<string>> discrete_input;//离散输入量
	vector <vector<string>> holding_register;//保持寄存器
	vector <vector<string>> input_register;//输入寄存器
	int number = 0;
	int alarm = 1;
	int event = 2;
	int status = 3;//状态模板号
	
	int startBits = 4;//起始地址
	int noBits = 5;//比特数
	float factor = 6;//因数倍数
	int measurement = 7;
	bool sign = 8;//是否是输入量
}m_types;


class mbtypes:public SrMsgHandler{
public:
	mbtypes(void);

	void addCoil(SrRecord &r); //821

	void addCoilAlarm(SrRecord &r); //822

	void addCoilEvent(SrRecord &r); //824

	void addCoilStatus(SrRecord &r); //830

	void saveConfigure(SrRecord &r); //817

	void addRegister(SrRecord &r); //825

	void addRegisterAlarm(SrRecord &r); //826

	void addRegisterMeasurement(SrRecord &r); //827

	void addRegisterEvent(SrRecord &r); //828

	void addRegisterStatus(SrRecord &r); //831

	void setmbtype(SrRecord &r); //839

    virtual void operator()(SrRecord &r, SrAgent &agent);

public:
	map <string ,m_types> _m; //key:type value:通过接口调的模型

private:		
	map<string ,function<void(mbtypes&,SrRecord&)>> m_mod;
	//map<string, vector<string, map<int, int>>> TYPENUM;
	m_types& _mt;
	string _current_type;
};

//mbtypes mbt;

class mbdevices:public SrMsgHandler{
public:
	mbdevices(SrAgent& agent);

	void addDevice(SrRecord &r); //816 847 832 848

    virtual void operator()(SrRecord &r ,SrAgent &agent);
public:
	map <string ,m_devices> _m;//key :sid value :结构体
private:
	map<string ,function<void(mbdevices&,SrRecord&)>> m_mod;
	SrAgent &_agent;
};

class mbMsg:public SrMsgHandler{
public:
	mbMsg(SrAgent& agent , mbdevices& mbd );
	void setCoil(SrRecord &r); //833

	void setRegister(SrRecord &r); //834

    virtual void operator()(SrRecord &r ,SrAgent &agent);
private:
	map<string ,function<void(mbMsg&,SrRecord&)>> m_mod;
	SrAgent &_agent;
	mbdevices& _mbd; 
};




#endif // __MODBUS_LUA_H_ 
