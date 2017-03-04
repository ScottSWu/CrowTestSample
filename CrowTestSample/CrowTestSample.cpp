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

int main(){
	
	crow::App<Middleware> app;
	app.get_middleware<Middleware>().setMessage("hello");

	{
		std::ifstream file("bee.png", std::ios::in | std::ios::binary | std::ios::ate);
		if (!file.is_open())
			throw std::runtime_error("couldn't open");

		fileContents.resize(file.tellg());

		file.seekg(0, std::ios::beg);
		if (!file.read(&fileContents[0], fileContents.size()))
			throw std::runtime_error("failed to read");
	}
	
	
	crow::mustache::set_base(".");

	CROW_ROUTE(app, "/").name("hello")([]() {
		return "hi";
	});

	CROW_ROUTE(app, "/path/")  ([]() {
		return "Trailing slash test case..";
	});

	CROW_ROUTE(app, "/bee.png") ([]() {
		return std::string(fileContents.data(), fileContents.size());
	});

	app.port(18080).multithreaded().run();
}




