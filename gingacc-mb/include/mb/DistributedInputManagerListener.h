
#ifndef DISTRIBUTEDINPUTMANAGERLISTENER_H
#define DISTRIBUTEDINPUTMANAGERLISTENER_H

#include "mb/InputManager.h"
#include "mb/remote/StubClient.h"
#include "mb/interface/IInputEvent.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {

class DistributedInputManagerListener : public InputManager
{
public:
	DistributedInputManagerListener (
		const GingaScreenID &screenId, const std::string& address, int port);

protected:
	void run ();

private:
	StubClient* _stubClient;
};

class RemoteInputEvent : public IInputEvent
{
public:
	RemoteInputEvent (map <string, string>& args)
	{
		this->_args = args;
		
		/*for (size_t i = 0; i < args.size(); i++)
		{
			pair <string, string> entry = args.at(i);
			if (entry.first == "keyCode")
				_keyCode = atoi (entry.second.c_str());
			else if (entry.first == "type")
				_type = atoi (entry.second.c_str());
			else if (entry.first == "isButtonPress")
				_isButtonPressType = (bool) atoi (entry.second.c_str());
			else if (entry.first == "isMotion")
				_isMotionType = (bool) atoi (entry.second.c_str());
			else if (entry.first == "isPressed")
				_isPressedType = (bool) atoi (entry.second.c_str());
			else if (entry.first == "isKey")
				_isKeyType = (bool) atoi (entry.second.c_str());
			else if (entry.first == "isApplication")
				_isApplicationType = (bool) atoi (entry.second.c_str());

			args.push_back (make_pair("axisX", pt.get<std::string> ("axisX")));
			args.push_back (make_pair("axisY", pt.get<std::string> ("axisY")));
			args.push_back (make_pair("axisZ", pt.get<std::string> ("axisZ")));
		}
		*/
	}

	~RemoteInputEvent () {};

	void clearContent() {};

	void setKeyCode(GingaScreenID scrId, const int keyCode) 
	{ _args["keyCode"] = keyCode; } 

	virtual const int getKeyCode(GingaScreenID) 
	{ return atoi (_args["keyCode"].c_str()); }

	unsigned int getType() 
	{ return atoi (_args["type"].c_str()); }

	void* getApplicationData() 
	{ return NULL;}

	bool isButtonPressType() 
	{ return (bool) atoi (_args["isButtonPress"].c_str()); }

	bool isMotionType() 
	{ return (bool) atoi (_args["_isMotion"].c_str());}

	bool isPressedType() 
	{ return (bool) atoi (_args["isPressed"].c_str());}

	bool isKeyType() 
	{ return (bool) atoi (_args["isKey"].c_str());}

	bool isApplicationType() 
	{ return (bool) atoi (_args["isApplication"].c_str());}

	void setAxisValue(int x, int y, int z) 
	{ 
		_args["axisX"] = to_string(x); 
		_args["axisY"] = to_string(y); 
		_args["axisZ"] = to_string(z); 
	}
	void getAxisValue(int* x, int* y, int* z) 
	{
		if (x != NULL)
			*x = atoi(_args["axisX"].c_str()); 

		if (y != NULL)
		*y = atoi(_args["axisY"].c_str()); 

		if (z != NULL)
			*z = atoi(_args["axisZ"].c_str()); 
	}
	void* getContent() 
	{ return NULL; }

private:
	map <string, string> _args;	
};


}
}
}
}
}
}

#endif //DISTRIBUTEDINPUTMANAGERLISTENER_H