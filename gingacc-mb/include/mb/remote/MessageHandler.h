
#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include "mb/DSMDefs.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <vector>
#include <map>
#include <string>
#include <sstream>

using boost::property_tree::ptree;

class MessageHandler
{
public:
	static std::string createRequestMessage (
		const MethodRequested& type, std::vector<std::string>& args)
	{
		ptree pt;

		pt.put ("Method.code", type);
		pt.put ("Method.argc", args.size());

		for(size_t i = 0; i < args.size(); i++) 
		{
			pt.put ("Method.arg" + std::to_string(i), args.at(i));
		}
		std::stringstream ss;
		write_json(ss, pt);

		return ss.str();
	}

	static std::string createResponseMessage (
			const ResponseCode& code)
		{
			ptree pt;

			pt.put ("Response.code", code);
			pt.put ("Response.argc", 0);

			std::stringstream ss;
			write_json(ss, pt);

			return ss.str();
		}

	static std::string createResponseMessage (
		const ResponseCode& code, std::vector<std::string>& args)
	{
		ptree pt;

		pt.put ("Response.code", code);
		pt.put ("Response.argc", args.size());

		for(size_t i = 0; i < args.size(); i++) 
		{
			pt.put ("Response.arg" + std::to_string(i), args.at(i));
		}
		std::stringstream ss;
		write_json(ss, pt);

		return ss.str();
	}

	static bool extractRequestMessage (
		const std::string& request, MethodRequested& code, std::vector<std::string>& args)
	{
		boost::property_tree::ptree pt;
		std::stringstream ss;
		ss << request;

		try
		{
			boost::property_tree::read_json(ss, pt);

			code = (MethodRequested) pt.get<int>("Method.code");
			int numArgs = pt.get<int> ("Method.argc");
		
			for (size_t i = 0; i < numArgs; i++)
			{
				std::string key = "Method.arg" + std::to_string(i);
				std::string value = pt.get<std::string>(key);

				args.push_back(value);
			}
		}
		catch (std::exception& e)
		{
			code = (MethodRequested) -1;
			args.clear();
			std::cout << e.what () << std::endl;
			return false;
		}

		return true;
	}

	static bool extractResponseMessage (
		const std::string& response, ResponseCode& code, std::vector<std::string>& args)
	{
		boost::property_tree::ptree pt;
		std::stringstream ss;
		ss << response;

		try
		{
			boost::property_tree::read_json(ss, pt);

			code = (ResponseCode) pt.get<int>("Response.code");
			int numArgs = pt.get<int> ("Response.argc");
		
			for (size_t i = 0; i < numArgs; i++)
			{
				std::string key = "Response.arg" + std::to_string(i);
				std::string value = pt.get<std::string>(key);

				args.push_back(value);
			}
		}
		catch (std::exception& e)
		{
			code = (ResponseCode) ResponseCode::INTERNAL_ERROR;
			args.clear();
			std::cout << e.what () << std::endl;
			return false;
		}

		return true;
	}

	static std::string createEventMessage (
		const EventType& type, 
		const std::vector< std::pair<std::string,std::string> >& args)
	{
		ptree pt;

		pt.put ("Event.type", type);
		pt.put ("Event.argc", args.size());

		for(size_t i = 0; i < args.size(); i++) 
		{
			pt.put (args.at(i).first, args.at(i).second);
		}
		std::stringstream ss;
		write_json(ss, pt);

		return ss.str();
	}

	static bool extractEventMessage (
		const std::string message, 
		std::map <std::string, std::string> &args)
	{
		boost::property_tree::ptree pt;
		std::stringstream ss;
		ss << message;

		try
		{
			boost::property_tree::read_json(ss, pt);
			int numArgs = pt.get<int> ("Event.argc");
			
			if (numArgs < 10 ) return false;
			
			args["type"] = pt.get<std::string> ("type");
			args["keyCode"] = pt.get<std::string> ("keyCode");
			args["isButtonPress"] = pt.get<std::string> ("isButtonPress");
			args["isMotion"] = pt.get<std::string> ("isMotion");
			args["isPressed"] = pt.get<std::string> ("isPressed");
			args["isKey"] = pt.get<std::string> ("isKey");
			args["isApplication"] = pt.get<std::string> ("isApplication");
			args["axisX"] = pt.get<std::string> ("axisX");
			args["axisY"] = pt.get<std::string> ("axisY");
			args["axisZ"] = pt.get<std::string> ("axisZ");

		}
		catch (std::exception& e)
		{
			args.clear();
			std::cout << e.what () << std::endl;
			return false;
		}

		return true;
	}
};

#endif //MESSAGEHANDLER_H
