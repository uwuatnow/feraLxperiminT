#pragma once
#include "Game/Globals.h"
#include <string>
#include <map>

#define NYAA_SERIAL_ADD(ptr, var, cast) ptr->add(#var, cast##var)

namespace nyaa {

class Serializable
{
public:
	template<typename T>
	using Data = std::map<std::string, T*>;
	
public:
	Serializable(std::string name, Serializable* parent, std::string jsonPath/* = "root.json"*/);
	
	~Serializable();

public:
	void add(const std::string& name, std::string& str);
	
	void add(const std::string& name, int& i);
	
	void add(const std::string& name, float& f);

	void add(const std::string& name, double& d);

	void add(const std::string& name, bool& b);

	//make sure to add all ptrs before calling!
	//will overwrite ur data
	void Read();

	void upload();
	
public:
	std::string name;
	std::string jsonPath;
	Serializable* parent;
	
	Data<std::string> strings;
	Data<int> integers;
	Data<float> floats;
	Data<double> doubles;
	Data<bool> bools;
	
	class JsonWrapper* jw;
};

}
