// CrowTestSample.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "crow_all.h"


int main()
{
	crow::SimpleApp app;

	CROW_ROUTE(app, "/")([]() {
		return "Hello world";
	});

	CROW_ROUTE(app, "/file")([](
		) {
		return "Bye world";
	});


	CROW_ROUTE(...)([...](...)->crow::response
	{
		auto response = crow::response(get_file_contents(...));
		response.set_header("Content-Type", "application/octet-stream"); // or set to whatever you need

		return response;
	});

	app.port(18080).multithreaded().run();
}
std::string const get_file_contents(...)
{
	...
		return file_contents;
}




