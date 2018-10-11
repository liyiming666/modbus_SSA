#include <iostream>
#include <stdint.h>
#include "modbus.h"
#include "modbus-private.h"
#include "modbus_lua.h"

#include "libmodbus.h"

using namespace std;

Poll::Poll(mbdevices& mbd, mbtypes& mbt)
	:_mbd(mbd),_mbt(mbt){}

Poll::~Poll(){}

void Poll::operator()(SrTimer &timer, SrAgent &agent){
	cout << _mbd._m.size() << endl;
	for (auto& m: _mbd._m) {
		string sid = m.first;
		cout << "sid: " << sid << endl;
		string type =m.second.type;
		SrNetHttp http(agent.server() + "/s", type, agent.auth());
		//COIL
		vector<vector<string>>& co = _mbt._m[type].coil;
		for (auto& v: co) {
			string number = v[0];
			string alarm = v[1];
			string event = v[2];
			string status = v[3];
			char bits[10];
			cout << "number: " << number << endl;
			int ret = Modbus::readCo(atoi(number.c_str()), 1, bits);
			for (int i = 0; i < ret; i++) {
				printf("%02x ", (unsigned char)bits[i]);
			}
			printf("\n");
			if (bits[0] == 0x01) {
				//string msg = "15," + type + "\n";	
				string msg = "";
				msg += alarm + "," + sid  + "\n";
				msg += event + "," + sid + "," + to_string(bits[0]) + "\n";
				msg += status + "," + sid + "," + to_string(bits[0]) + "\n";
				http.post(msg); 
			}	
		}	
		//discrete_input
		vector<vector<string>>& di = _mbt._m[type].discrete_input;
		for (auto& v: di) {
			string number = v[0];
			string alarm = v[1];
			string event = v[2];
			string status = v[3];
			char bits[10];
			int ret = Modbus::readDi(atoi(number.c_str()), 1, bits);
			for (int i = 0; i < ret; i++) {
				printf("%02x ", (unsigned char)bits[i]);
			}
			printf("\n");
			if (bits[0] == 0x01) {
				//string msg = "15," + type + "\n";	
				string msg = "";
				msg += alarm + "," + sid  + "\n";
				msg += event + "," + sid + "," + to_string(bits[0]) + "\n";
				msg += status + "," + sid + "," + to_string(bits[0]) + "\n";
				http.post(msg); 
			}	
		}	
		//holding_register
		vector<vector<string>>& hr = _mbt._m[type].holding_register;
		for(auto& v: hr){
			string number = v[0];
			string alarm = v[1];
			string event = v[2];
			string status = v[3];
			char bits[20];
			int ret = Modbus::readHr(atoi(number.c_str()),1 , bits);
			for (int i = 0; i < ret; i++) {
				printf("%02x ", (unsigned char)bits[i]);
			}
			printf("\n");
			if (bits[0] == 0x01) {
				string msg = "";
				msg += alarm + "," + sid  + "\n";
				msg += event + "," + sid + "," + to_string(bits[0]) + "\n";
				msg += status + "," + sid + "," + to_string(bits[0]) + "\n";
				http.post(msg); 
			}	
			
		}
		//input_register
		vector<vector<string>>& ir = _mbt._m[type].input_register;
		for(auto& v: ir){
			string number = v[0];
			string alarm = v[1];
			string event = v[2];
			string status = v[3];
			char bits[20];
			int ret = Modbus::readIr(atoi(number.c_str()),1 , bits);
			for (int i = 0; i < ret; i++) {
				printf("%02x ", (unsigned char)bits[i]);
			}
			printf("\n");
			if (bits[0] == 0x01) {
				string msg = "";
				msg += alarm + "," + sid  + "\n";
				msg += event + "," + sid + "," + to_string(bits[0]) + "\n";
				msg += status + "," + sid + "," + to_string(bits[0]) + "\n";
				http.post(msg); 
			}	
		}
	}
}

Modbus::~Modbus(){}
int Modbus::readCo(int num, int nb, char* bits){
	modbus_t* mtx = modbus_new_rtu("/dev/ttyUSB0", 9600, 'N', 8, 1);
	int ret = modbus_set_slave(mtx, 1);
	ret = modbus_connect(mtx);
	ret = modbus_read_bits(mtx , num-1, nb , (uint8_t*)bits ); 

/***********************************************************************/
//	拿到mobus rtu请求报文
	uint8_t req[12];
	int req_length = mtx->backend->build_request_basis(mtx, MODBUS_FC_READ_COILS, num-1, nb, req);
	int msg_length = mtx->backend->send_msg_pre(req, req_length);
	cout << "coil "<<endl;
	cout << "--->ret_length: " << msg_length << endl;
	for (int i = 0; i < msg_length; i++) {
		printf("%02x ", req[i]);
	}	
	printf("\n");
/***********************************************************************/

	return ret;		
}

int Modbus::readDi(int num, int nb, char* bits){
	modbus_t* mtx = modbus_new_rtu("/dev/ttyUSB0", 9600, 'N', 8, 1);
	int ret = modbus_set_slave(mtx, 1);
	ret = modbus_connect(mtx);
	ret = modbus_read_input_bits(mtx , num-1, nb , (uint8_t*)bits ); 
//	拿到mobus rtu请求报文
	uint8_t req[12];
	int req_length = mtx->backend->build_request_basis(mtx, MODBUS_FC_READ_DISCRETE_INPUTS, num-1, nb, req);
	int msg_length = mtx->backend->send_msg_pre(req, req_length);
	cout << "discrete_input "<<endl;
	cout << "--->ret_length: " << msg_length << endl;
	for (int i = 0; i < msg_length; i++) {
		printf("%02x ", req[i]);
	}	
	printf("\n");
	return ret;		
}

int Modbus::readHr(int num, int nb, char* bits){
	modbus_t* mtx = modbus_new_rtu("/dev/ttyUSB0", 9600, 'N', 8, 1);
	int ret = modbus_set_slave(mtx, 1);
	ret = modbus_connect(mtx);
	ret = modbus_read_registers(mtx , num-1, nb , (uint16_t*)bits ); 
//	拿到mobus rtu请求报文
	uint8_t req[12];
	int req_length = mtx->backend->build_request_basis(mtx, MODBUS_FC_READ_HOLDING_REGISTERS, num-1, nb, req);
	int msg_length = mtx->backend->send_msg_pre(req, req_length);
	cout << "holding_register "<<endl;
	cout << "--->ret_length: " << msg_length << endl;
	for (int i = 0; i < msg_length; i++) {
		printf("%02x ", req[i]);
	}	
	printf("\n");
	return ret;		
}

int Modbus::readIr(int num, int nb, char* bits){
	modbus_t* mtx = modbus_new_rtu("/dev/ttyUSB0", 9600, 'N', 8, 1);
	int ret = modbus_set_slave(mtx, 1);
	ret = modbus_connect(mtx);
	ret = modbus_read_input_registers(mtx , num-1, nb , (uint16_t*)bits ); 
//	拿到mobus rtu请求报文
	uint8_t req[12];
	int req_length = mtx->backend->build_request_basis(mtx , MODBUS_FC_READ_INPUT_REGISTERS, num-1, nb, req);
	int msg_length = mtx->backend->send_msg_pre(req, req_length);
	cout << "input_register "<<endl;
	cout << "--->ret_length: " << msg_length << endl;
	for (int i = 0; i < msg_length; i++) {
		printf("%02x ", req[i]);
	}	
	printf("\n");
	return ret;		
}

int Modbus::writeCo(int slave,int num, int nb){
	modbus_t* mtx = modbus_new_rtu("/dev/ttyUSB0", 9600, 'N', 8, 1);
	int ret = modbus_set_slave(mtx, slave);	
	ret = modbus_connect(mtx);
	ret = modbus_write_bit(mtx, num-1, nb);
//	拿到mobus rtu请求报文
	uint8_t req[12];
	int req_length = mtx->backend->build_request_basis(mtx, MODBUS_FC_WRITE_SINGLE_COIL, num-1, nb?0xFF00:0, req);
//	int msg_length = mtx->backend->send_msg_pre(req, req_length);
	cout << "setCOil "<<endl;
	cout << "--->ret_length: " << req_length << endl;
	for (int i = 0; i < req_length; i++) {
		printf("%02x ", req[i]);
	}	
	printf("\n");
	return ret;		
}

int Modbus::writeHr(int slave,int num, int nb){
	cout << "writeHr -> nb : " << nb << endl;
	modbus_t* mtx = modbus_new_rtu("/dev/ttyUSB0", 9600, 'N', 8, 1);
	int ret = modbus_set_slave(mtx, slave);	
	ret = modbus_connect(mtx);
	ret = modbus_write_register(mtx, num-1, nb);
//	拿到mobus rtu请求报文
	uint8_t req[12];
	int req_length = mtx->backend->build_request_basis(mtx, MODBUS_FC_WRITE_SINGLE_REGISTER, num-1, nb, req);
//	int msg_length = mtx->backend->send_msg_pre(req, req_length);
	cout << "setRegister "<<endl;
	cout << "--->ret_length: " << req_length << endl;
	for (int i = 0; i < req_length; i++) {
		printf("%02x ", req[i]);
	}	
	printf("\n");
	return ret;		
}




