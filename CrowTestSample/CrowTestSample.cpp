// CrowTestSample.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "crow_all.h"
#include <iostream>
#include <fstream>
#include <vector>

std::vector< char > fileContents;

struct Middleware
{
	std::string message;

	Middleware()
	{
		message = " ";
	}

	void setMessage(std::string newMsg)
	{
		message = newMsg;
	}

	struct context
	{
	};

	void before_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/)
	{
		CROW_LOG_DEBUG << " - MESSAGE: " << message;
	}

	void after_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/)
	{
		// no-op
	}
};

std::string readIn(std::string fileName) {
	{
		std::ifstream file(fileName, std::ios::in | std::ios::binary | std::ios::ate);
		if (!file.is_open())
			throw std::runtime_error("couldn't open");

		fileContents.resize(file.tellg());

		file.seekg(0, std::ios::beg);
		if (!file.read(&fileContents[0], fileContents.size()))
			throw std::runtime_error("failed to read");
	}
	return std::string(fileContents.data(), fileContents.size());
}

int main(){
	
	crow::App<Middleware> app;
	app.get_middleware<Middleware>().setMessage("hello");
	std::string fileName = "Text.txt";
	std::string fileNameS = "/" + fileName;
	std::cout << fileNameS;
	
	crow::mustache::set_base(".");
	
	CROW_ROUTE(app, "/<string>") ([](const crow::request& req, crow::response& res, std::string str) {
		res.write(readIn(str));
		res.end();
	});

	app.port(18080).multithreaded().run();
}




