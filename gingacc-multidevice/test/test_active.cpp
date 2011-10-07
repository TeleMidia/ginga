#include <sstream>
#include <iostream>

using namespace std;

#include "multidevice/services/RemoteEventService.h"

using namespace ::br::pucrio::telemidia::ginga::core::multidevice;

int main(int argc, char *argv[]) {
	bool result = false;

	RemoteEventService* res = new RemoteEventService();

	clog << "[INICIO DO TESTE]\n";

	clog << "\n[p0]\n";
	res->addDeviceClass(2);

	clog << "\n[p1]\n";
	res->addDevice(2,1,"192.168.1.102");

	//clog << "\n[p2]\n";
	//res->addDocument(2,"doc.ncl","<ncl></ncl>");
	//sleep(3);
	clog << "\n[p3]\n";
	res->startDocument(2,"tests/proview-device/proview-device.ncl");
	clog << "\n[p3.1]\n";
	sleep(10);
	clog << "\n[p4]\n";
	res->stopDocument(2,"doc.ncl");


	//teste 2//
/*
	TCPClientConnection* tcp;
	tcp = new TCPClientConnection("localhost","22222");

	clog << "\n[p1]\n";
	result = tcp->post("0 ADD document.ncl 5\n<ncl>\n");
	clog << result << "\n";

	sleep(3);

	clog << "[p2]\n";
	result = tcp->post("0 START document.ncl 0\n");
	clog << result << "\n";

	sleep(3);
	clog << "[p3]\n";
	result = tcp->post("0 ASDADD teste.oi 0\n");
	clog << result << "\n";
	clog << "[FIM DO TESTE]\n";
	return 0;
*/
}
