// CrowTestSample.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "crow_all.h"
#include <iostream>
#include <fstream>

std::string content;


int main()
{
	crow::SimpleApp app;

	CROW_ROUTE(app, "/")([]() {
		return "Hello world";
	});

	/*
	void get_file_contents_async(..., std::function<void(const std::string&)> on_ready)
	{
		std::string file_contents;
		...
			if (is_ready == true && on_ready != nullptr)
				on_ready(file_contents);
	}
	*/


	CROW_ROUTE(app, "/file").methods("POST"_method)([] (const crow::request& req) {
		

		if (req.method == "POST"_method)
		{
			auto x = crow::json::load(req.body);
			content = "You have submitted a POST request";
			return crow::response(200, content);
		}
		//auto response = crow::response(400);
		//response.set_header("Content-Type", "application/octet-stream"); // or set to whatever you need

		return crow::response(0, "error");
	});

	CROW_ROUTE(app, "/json")
		([] {
		std::string x;
		//crow::json::wvalue x;
		x = "<h1> hello</h1><br/><h3> world</h3><img src='~/bee.PNG'>";
		return x;
	});

	app.port(18080).multithreaded().run();
}
/* std::string const get_file_contents(...)
{
	...
		return file_contents;
} */




