// CrowTestSample.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "crow_all.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <urlmon.h>
#include <unordered_set>
#include <mutex>

#pragma comment( lib, "urlmon" )


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

std::wstring MimeTypeFromString(const std::wstring& str) {

	LPWSTR pwzMimeOut = NULL;
	HRESULT hr = FindMimeFromData(NULL, str.c_str(), NULL, 0,
		NULL, FMFD_URLASFILENAME, &pwzMimeOut, 0x0);
	if (SUCCEEDED(hr)) {
		std::wstring strResult(pwzMimeOut);
		// Despite the documentation stating to call operator delete, the
		// returned string must be cleaned up using CoTaskMemFree
		CoTaskMemFree(pwzMimeOut);
		return strResult;
	}

	return L"";
}

// Convert an UTF8 string to a wide Unicode String
std::wstring utf8_decode(const std::string &str)
{
	if (str.empty()) return std::wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

// Convert a wide Unicode string to an UTF8 string
std::string utf8_encode(const std::wstring &wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

int main(){
	
	crow::SimpleApp app;
//	app.get_middleware<Middleware>().setMessage("hello");

	std::mutex mtx;;
	std::unordered_set<crow::websocket::connection*> users;
	
	//crow::mustache::set_base(".");

	int index = 0;

	CROW_ROUTE(app, "/ws")
		.websocket()
		.onopen([&](crow::websocket::connection& conn) {
		CROW_LOG_INFO << "new websocket connection";
		std::lock_guard<std::mutex> _(mtx);
		users.insert(&conn);
	})
		.onclose([&](crow::websocket::connection& conn, const std::string& reason) {
		CROW_LOG_INFO << "websocket connection closed: " << reason;
		std::lock_guard<std::mutex> _(mtx);
		users.erase(&conn);
	})
		.onmessage([&](crow::websocket::connection& /*conn*/, const std::string& data, bool is_binary) {
		std::lock_guard<std::mutex> _(mtx);
		for (auto u : users)
			if (is_binary) {
				u->send_binary("bee.png");
				
				std::cout << data;
				std::cout << "hello";
			}
			else {
				std::string file = std::to_string(index) + ".JPG";
				u->send_binary(readIn(file));
				std::cout << file << std::endl;
				index = (index + 1) % 8;
				//std::cout << data;
			}
	});
		
	CROW_ROUTE(app, "/<string>") ([](const crow::request& req, crow::response& res, std::string str) {
		int index = str.rfind('.');
		std::wstring place = utf8_decode(str.substr(index));
		std::string str1(utf8_encode(MimeTypeFromString(place)));
		res.add_header("Content-Type", str1);
		res.write(readIn(str));
		res.end();
	}); 

	app.port(18080).multithreaded().run();
}




