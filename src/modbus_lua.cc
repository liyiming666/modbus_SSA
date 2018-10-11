#include <map>
#include <list>
#include <unistd.h>
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
#include "libmodbus.h"

using namespace std;
using namespace std::placeholders;

mbtypes mbt;

int unbat(int nb , int sb , int value){
		//old值需要待获取，我们在此先设置一个
		int old = 0x5F;
		int n = ~(((1 << nb) -1) << sb);
		old = ( old & n);	
		if (value >= 0) {
			value = old | (value << sb);
		}
		else{
			value = ~value +1;
			value = old | (value << sb);
		}
		printf("%02x\n", (unsigned int)value);
		return old;
}

	mbtypes::mbtypes(void): _mt(_m[""]) {
		function<void(mbtypes&,SrRecord&)> f1 = &mbtypes::addCoil;
		m_mod["821"] = f1;
		function<void(mbtypes&,SrRecord&)> f2 = &mbtypes::saveConfigure;
		m_mod["817"] = f2;

		function<void(mbtypes&,SrRecord&)> f4 = &mbtypes::addRegister;
		m_mod["825"] = f4;
		function<void(mbtypes&,SrRecord&)> f5 = &mbtypes::addRegisterAlarm;
		m_mod["826"] = f5;
		function<void(mbtypes&,SrRecord&)> f6 = &mbtypes::addRegisterMeasurement;
		m_mod["827"] = f6;
		function<void(mbtypes&,SrRecord&)> f7 = &mbtypes::addRegisterEvent;
		m_mod["828"] = f7;
		function<void(mbtypes&,SrRecord&)> f14 = &mbtypes::addRegisterStatus;
		m_mod["831"] = f14;

		function<void(mbtypes&,SrRecord&)> f8 = &mbtypes::addCoilAlarm;
		m_mod["822"] = f8;
		function<void(mbtypes&,SrRecord&)> f9 = &mbtypes::addCoilEvent;
		m_mod["824"] = f9;
		function<void(mbtypes&,SrRecord&)> f13 = &mbtypes::addCoilStatus;
		m_mod["830"] = f13;
	
		function<void(mbtypes&,SrRecord&)> f12 = &mbtypes::setmbtype;
		m_mod["839"] = f12;
		m_mod["840"] = f12;
		
		
	}
/*	
	void init(mbdevices &mbd, SrNetHttp &http){

		cout << "mb size: " << mbd.getM().size() << endl;

		for(auto & p : mbd.getM()){
			string sid = p.first;
			string type = to_string(p.second.type);
			m_types mt;
			_m[type] = mt;
			http.clear();
			http.post("309," + type );
			SmartRest sr(http.response());
		    SrRecord r = sr.next();
			_mt = _m[type];
			while(r.size()){
				cout << "====>recv type: ";
				for (size_t i = 0; i < r.size(); i++) {
					cout << r.value(i) << " ";	
				}
				cout << endl;
				cout <<"_"<< r.value(0) << endl;	
				m_mod[r.value(0)](*this, r);
				//mbt.addCoil(SrRecord &r);
				r = sr.next();
			}
		}
	
	}
*/

	void mbtypes::addCoil(SrRecord &r){ //821
		string number = r.value(2);
		vector<string> v = {number, "", "", ""};
		if(r.value(3) == "false"){
			_m[_current_type].coil.push_back(v);
		}else{
			_m[_current_type].discrete_input.push_back(v);
		
		}	
		
/*
		printf("addCoil\n");
		string number = r.value(2);
		vector<string> v = {number, "", "", ""};
		_mt.coil.push_back(v);	
*/
	}


	void mbtypes::addCoilAlarm(SrRecord &r){ //822
		printf("addCoilAlarm\n");
		string number = r.value(2);
		string alarm = r.value(3);
		if(r.value(4) == "false"){
			for (auto& v:  _m[_current_type].coil) {
				if (v[_m[_current_type].number] == r.value(2)) {
					v[_m[_current_type].alarm] = alarm;
					break;
				}
			}
		}
		else {
			for (auto& v: _m[_current_type].discrete_input) {
				if (v[_m[_current_type].number] == r.value(2)) {
					v[_m[_current_type].alarm] = alarm;
					break;
				}
			}
		}
	}

	void mbtypes::addCoilEvent(SrRecord &r){ //824
		printf("addCoilEvent\n");
		string number = r.value(2);
		string event = r.value(3);
		if(r.value(4) == "false"){
			for (auto& v:  _m[_current_type].coil) {
				if (v[_m[_current_type].number] == r.value(2)) {
					v[_m[_current_type].event] = event;
					break;
				}
			}
		}
		else {
			for (auto& v: _m[_current_type].discrete_input) {
				if (v[_m[_current_type].number] == r.value(2)) {
					v[_m[_current_type].event] = event;
					break;
				}
			}
		}
	}

	void mbtypes::addCoilStatus(SrRecord &r){ //830
		printf("addCoilStatus\n");
		//_m[_current_type].coil[_m[_current_type].status] = status; 
		if (r.value(4) == "false")
			for (auto& v: _m[_current_type].coil) {
				if (v[_m[_current_type].number] == r.value(2)) {
					v[_m[_current_type].status] = "100";
					break;
				}
			}
		else {
			for (auto& v: _m[_current_type].discrete_input) {
				if (v[_m[_current_type].number] == r.value(2)) {
					v[_m[_current_type].status] = "101";
					break;
				}
			}
		}
		for (auto& v: _m[_current_type].coil) {
			for(auto& s: v){
				cout<< s <<" ";
			}
		}
		cout << endl;
		/*
		string number = r.value(2);
		for(auto& v: _mt.coil){
			if(v[0] == number){
				v[2] = r.value(3);
			}
		}
		*/
 	}		
	void mbtypes::saveConfigure(SrRecord &r){ //817
		printf("saveConfigure\n");
			
	}
	void mbtypes::addRegister(SrRecord &r){ //825
		printf("addRegister\n");
		string number = r.value(2);
		string startBit = r.value(3);
		string noBits = r.value(4);
		int f = atoi(r.value(5).c_str());
		int a = atoi(r.value(6).c_str());
		int c = atoi(r.value(7).c_str());
		string factor = to_string((float)(f/a/(10.0*10.0)*c));
		cout << factor <<endl;
		vector<string> v = {number, "", "", "", startBit, noBits, factor, "", ""};
		if(r.value(8) == "false"){
			_m[_current_type].holding_register.push_back(v);
		}else{
			_m[_current_type].input_register.push_back(v);
		}
		
	}
	void mbtypes::addRegisterAlarm(SrRecord &r){ //826
		printf("addRegisterAlarm\n");
		string number = r.value(2);
		string startBit = r.value(3);
		string alarm = r.value(4);
		if(r.value(5) == "false"){
			for(auto& v: _m[_current_type].holding_register){
				if(v[_m[_current_type].number] == number){
					v[_m[_current_type].alarm] = alarm;
					break;
				}
			}
		}
		else{
			for(auto& v: _m[_current_type].input_register){
				if(v[_m[_current_type].number] == number){
					v[_m[_current_type].alarm] = alarm;
					break;
				}
			}
			
		}
	}
	void mbtypes::addRegisterMeasurement(SrRecord &r){ //827
		printf("addRegisterMeasurement\n");
		string number = r.value(2);
		string startBit = r.value(3);
		string measurement = r.value(4);
		if(r.value(5) == "false"){
			for(auto& v: _m[_current_type].holding_register){
				if(v[_m[_current_type].number] == number){
					v[_m[_current_type].measurement] = measurement;
					break;
				}
			}
		}
		else{
			for(auto& v: _m[_current_type].input_register){
				if(v[_m[_current_type].number] == number){
					v[_m[_current_type].measurement] = measurement;
					break;
				}
			}
		}
		
	}
	void mbtypes::addRegisterEvent(SrRecord &r){ //828
		printf("addRegisterEvent\n");
		string number = r.value(2);
		string startBit = r.value(3);
		string event = r.value(4);
		if(r.value(5) == "false"){
			for(auto& v: _m[_current_type].holding_register){
				if(v[_m[_current_type].number] == number){
					v[_m[_current_type].event] = event;
					break;
				}
			}
		}
		else{
			for(auto& v: _m[_current_type].input_register){
				if(v[_m[_current_type].number] == number){
					v[_m[_current_type].event] = event;
					break;
				}
			}
		}
	}

	void mbtypes::addRegisterStatus(SrRecord &r){ //831
		printf("addRegisterStatus\n");
		string number = r.value(2);
		string startBit = r.value(3);
		if(r.value(5) == "false"){
			for(auto& v: _m[_current_type].holding_register){
				if(v[_m[_current_type].number] == number){
					v[_m[_current_type].status] = "101";
					break;
				}
			}
		}
		else{
			for(auto& v: _m[_current_type].input_register){
				if(v[_m[_current_type].number] == number){
					v[_m[_current_type].status] = "101";
					break;
				}
			}
		}
		for (auto& v: _m[_current_type].holding_register) {
			for(auto& s: v){
				cout<< s << " ";
			}
		}
		cout << endl;
	}		

	void mbtypes::setmbtype(SrRecord &r){ //839
		printf("setmbtype\n");
		_current_type = r.value(2);
	}
    void mbtypes::operator()(SrRecord &r, SrAgent &agent)
    {
		printf("mbtypes operator()\n");
		m_mod[r.value(0)](*this,r);
    }

	mbdevices::mbdevices(SrAgent& agent):_agent(agent){
		function<void(mbdevices&,SrRecord&)> f1 = &mbdevices::addDevice;
		m_mod["816"] = f1;		
		m_mod["847"] = f1;		
		m_mod["832"] = f1;		
		m_mod["848"] = f1;		
	}

	void mbdevices::addDevice(SrRecord &r){ //816 847 832 848
		printf("addDevices: %s\n",r.value(0).c_str());
		//string id = r.value(5);
		string slave = r.value(3);
		string sid = r.value(4);
		string s_type;
		//sscanf(r.value(5).c_str(), "/inventory/managedObjects/%s", s_type.c_str());
		size_t pos = r.value(5).find("/inventory/managedObjects/");
		if (pos != string::npos) {
			pos += sizeof("/inventory/managedObjects/")-1;
			s_type = r.value(5).substr(pos, r.value(5).size()-pos);
		}
		//int type =atoi(s_type.c_str());
		//printf("type:%d\n",type);
		cout << "stype: " << s_type << endl;
		string addr;
		string ops_id;
		if( r.size() == 7){
			ops_id = r.value(6);
		}		
		else{
			ops_id = "";	
		}	
		if( r.value(2) == "TCP")
			addr = r.value(2);
		else{
			addr = "";	
		}

		m_devices m;
		_m.insert(pair<string,m_devices>(sid,m));

		//if (_m.find(sid) != _m.end() && _m[sid].type.empty()) {
		_m[sid].addr = addr;
		_m[sid].slave = slave;
		_m[sid].type = s_type;
		_m[sid].ops_id = ops_id;
			
		if (mbt._m.find(s_type) == mbt._m.end()) {
			_agent.send("309," + s_type);
			m_types m;
			mbt._m.insert(pair<string, m_types>(s_type, m));
		}
	}
    void mbdevices::operator()(SrRecord &r ,SrAgent &agent)
    {
		printf("mbdevices operator()\n");
		m_mod[r.value(0)](*this, r);
    }
/*
	map<string, m_devices>& getM() { 	
		return _m; 
	}
*/
	mbMsg::mbMsg(SrAgent& agent , mbdevices& mbd):_agent(agent),_mbd(mbd){
		function<void(mbMsg&,SrRecord&)> f1 = &mbMsg::setCoil;
 		m_mod["833"] = f1;	
		function<void(mbMsg&,SrRecord&)> f2 = &mbMsg::setRegister;
 		m_mod["834"] = f2;	
	}

    void mbMsg::operator()(SrRecord &r ,SrAgent &agent)
    {
		printf("mbMsg operator()\n");
		m_mod[r.value(0)](*this, r);
    }

	void mbMsg::setCoil(SrRecord &r){ //833
		printf("setCoil\n");
		string opt_id = r.value(2);
		//agent.send(agent.ID(), "300," opt_id + ",EXECUTING");
		string sid = r.value(3);
		string msg_addr = r.value(4);
		string msg_number =r.value(5);
		string msg_slave =_mbd._m[sid].slave ;
		int ret = Modbus::writeCo(atoi(msg_slave.c_str()),
			atoi(msg_addr.c_str()),
			atoi(msg_number.c_str()));				
		
	}
	void mbMsg::setRegister(SrRecord &r){ //834
		printf("setRegister\n");
		string opt_id = r.value(2);
		string sid = r.value(3);
		string msg_addr = r.value(4);
		string msg_startBit =r.value(5);
		string msg_noBits =r.value(6);
		string msg_number =r.value(7);
		cout << "value: " << msg_number << endl;
		int nb = atoi(msg_noBits.c_str());
		int sb = atoi(msg_startBit.c_str());
		int value = atoi(msg_number.c_str());
		cout << "addr :" <<msg_addr <<endl;
		cout << "msg_moBits: " << msg_noBits << "msg_startBit :"<< msg_startBit << endl;
		cout << "nb: " << nb << "sb :"<< sb << endl;
		value = unbat(nb , sb , value);
		cout << "value: " << value << endl;
		string msg_slave =_mbd._m[sid].slave;
		int ret = Modbus::writeHr(atoi(msg_slave.c_str()),
			atoi(msg_addr.c_str()),
			value);
	}
int main(){
	const char* server = "liyiming.quarkioe.cn";
	const char* deviceID = "20180725";

	srLogSetLevel(SRLOG_DEBUG);

	string srversion, srtemplate;

	if (readSrTemplate("srtemplate.txt", srversion, srtemplate) != 0)
    {
	    return 0;
    }


	Integrate igt;
	SrAgent agent(server, deviceID, &igt);

	if (agent.bootstrap("./credential")) {
		return -1;
	}
	if (agent.integrate(srversion, srtemplate)) // integrate to Cumulocity
    {
        return 0;
    }

	//要修改的，先写死 	
	string ops = "\"\"\"c8y_Command\"\",\"\"c8y_ModbusDevice\"\","
		   "\"\"c8y_SetCoil\"\",\"\"c8y_ModbusConfiguration\"\","
		   "\"\"c8y_SerialConfiguration\"\",\"\"c8y_LogfileRequest\"\","
  		   "\"\"c8y_SetRegister\"\"\"" ;
	agent.send("327," + agent.ID() + "," + ops);
	
	agent.send("321," + agent.ID() + "," + "10,1000");
	agent.send("335," + agent.ID() + "," + "9600,8,N,1");
	
	mbdevices mbd =mbdevices(agent);
	mbMsg mbg =mbMsg(agent,mbd);
	
	agent.addMsgHandler(816,&mbd);
	agent.addMsgHandler(847,&mbd);
	agent.addMsgHandler(832,&mbd);
	agent.addMsgHandler(848,&mbd);
	agent.addMsgHandler(821,&mbt);
	agent.addMsgHandler(817,&mbt);
	agent.addMsgHandler(825,&mbt);
	agent.addMsgHandler(826,&mbt);
	agent.addMsgHandler(827,&mbt);
	agent.addMsgHandler(828,&mbt);
	agent.addMsgHandler(831,&mbt);
	agent.addMsgHandler(822,&mbt);
	agent.addMsgHandler(824,&mbt);
	agent.addMsgHandler(830,&mbt);
	agent.addMsgHandler(833,&mbg);
	agent.addMsgHandler(834,&mbg);
	agent.addMsgHandler(839,&mbt);
	agent.addMsgHandler(840,&mbt);

	agent.send("323," + agent.ID());
	
	
	Poll p(mbd, mbt);
	SrTimer t(5*1000, &p);
	agent.addTimer(t);
	t.start();

	SrReporter reporter(server, agent.XID(), agent.auth(), agent.egress, agent.ingress);
	if (reporter.start() != 0){
		return 0;	
	}
	

	SrDevicePush push(server, agent.XID(), agent.auth(), agent.ID(), agent.ingress);
	if (push.start() != 0){
		return 0;
	}
	agent.loop();
	return 0;
}






