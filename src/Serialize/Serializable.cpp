#include "Serialize/Serializable.h"
#include "Game/Util.h"
#include <json.hpp>
#include <fstream>
#include <iostream>

namespace nyaa {

class JsonWrapper
{
public:
	nlohmann::json j;
};

Serializable::Serializable(std::string name, Serializable* parent, std::string jsonPath)
	:name(name)
	,jsonPath(jsonPath)
	,parent(parent)
	,jw(new JsonWrapper())
{
	if(parent == nullptr)
	{
		std::ifstream inf(jsonPath);
		if(inf.is_open())
		{
			inf >> jw->j;
		}
	}
}

Serializable::~Serializable()
{
	//upload();
	delete jw;
}

void Serializable::upload()
{
	if(parent && parent->jw)
	{
		nlohmann::json& j = parent->jw->j;
		for(auto& pair : strings)
		{
			if(!pair.second) continue;
			j[name][pair.first] = *pair.second;
		}
		
		for(auto& pair : integers)
		{
			if(!pair.second) continue;
			j[name][pair.first] = *pair.second;
			//Util::PrintLnFormat("~Serializable(): wrote %s(%d) to parent", pair.first.c_str(), *pair.second);
		}
		
		for(auto& pair : floats)
		{
			if(!pair.second) continue;
			j[name][pair.first] = *pair.second;
		}

		for(auto& pair : doubles)
		{
			if(!pair.second) continue;
			j[name][pair.first] = *pair.second;
		}


		for (auto& pair : bools)
		{
			if (!pair.second) continue;
			j[name][pair.first] = *pair.second;
		}
	}
	else
	{
		nlohmann::json& j = jw->j;
		//root node
		for(auto& pair : strings)
		{
			if(!pair.second) continue;
			j[pair.first] = *pair.second;
		}
		
		for(auto& pair : integers)
		{
			if(!pair.second) continue;
			j[pair.first] = *pair.second;
		}
		
		for(auto& pair : floats)
		{
			if(!pair.second) continue;
			j[pair.first] = *pair.second;
		}

		for(auto& pair : doubles)
		{
			if(!pair.second) continue;
			j[pair.first] = *pair.second;
		}

		for (auto& pair : bools)
		{
			if (!pair.second) continue;
			j[pair.first] = *pair.second;
		}

		//save 2 disk
 		std::ofstream of(jsonPath);
		if(of.is_open())
		{
			of << j.dump(4);
			of.close();
		}
		else
		{
			std::cerr << "save of root json failed!!! :( \"" << jsonPath << "\"" << std::endl;
		}
	}
}

void Serializable::add(const std::string& name, std::string& str)
{
	strings[name] = &str;
}

void Serializable::add(const std::string& name, int& i)
{
	//Util::PrintLnFormat("Adding serial int&");
	integers[name] = &i;
	//Util::PrintLnFormat("[GOOD] Added serial int&");
}

void Serializable::add(const std::string& name, float& f)
{
	floats[name] = &f;
}

void Serializable::add(const std::string& name, double& d)
{
	doubles[name] = &d;
}

void Serializable::add(const std::string& name, bool& b)
{
	bools[name] = &b;
}

void Serializable::Read()
{
	// Check if this is a root-level object (parent is nullptr)
	bool isRootObject = (parent == nullptr);
	
	nlohmann::json& j = isRootObject ? jw->j : parent->jw->j;
	
	//Util::PrintLnFormat("Serializable::Read(): called");
	
	for(auto& pair : strings)
	{
		if(!pair.second) continue;
		if(isRootObject) {
			// For root objects, read directly from the root JSON
			if(!j.is_null() && j.contains(pair.first)) {
				*pair.second = j[pair.first];
			}
		} else {
			// For child objects, read from the named section
			if(!j[name].is_null() && j[name].contains(pair.first)) {
				*pair.second = j[name][pair.first];
			}
		}
	}
	
	//Util::PrintLnFormat("Serializable::Read(): string reads complete");
	for(auto& pair : integers)
	{
		if(!pair.second) continue;
		if(isRootObject) {
			// For root objects, read directly from the root JSON
			if(!j.is_null() && j.contains(pair.first)) {
				*pair.second = j[pair.first];
				//Util::PrintLnFormat("Serializable::Read(): set %s to %d.", pair.first.c_str(), *pair.second);
			}
		} else {
			// For child objects, read from the named section
			if(!j[name].is_null() && j[name].contains(pair.first)) {
				*pair.second = j[name][pair.first];
				//Util::PrintLnFormat("Serializable::Read(): set %s to %d.", pair.first.c_str(), *pair.second);
			}
		}
	}
	
	for(auto& pair : floats)
	{
		if(!pair.second) continue;
		if(isRootObject) {
			// For root objects, read directly from the root JSON
			if(!j.is_null() && j.contains(pair.first)) {
				*pair.second = j[pair.first];
			}
		} else {
			// For child objects, read from the named section
			if(!j[name].is_null() && j[name].contains(pair.first)) {
				*pair.second = j[name][pair.first];
			}
		}
	}

	//Util::PrintLnFormat("Serializable::Read(): int reads complete");
	for(auto& pair : doubles)
	{
		if(!pair.second) continue;
		if(isRootObject) {
			// For root objects, read directly from the root JSON
			if(!j.is_null() && j.contains(pair.first)) {
				*pair.second = j[pair.first];
			}
		} else {
			// For child objects, read from the named section
			if(!j[name].is_null() && j[name].contains(pair.first)) {
				*pair.second = j[name][pair.first];
			}
		}
	}

	for (auto& pair : bools)
	{
		if (!pair.second) continue;
		if(isRootObject) {
			// For root objects, read directly from the root JSON
			if(!j.is_null() && j.contains(pair.first)) {
				*pair.second = j[pair.first];
			}
		} else {
			// For child objects, read from the named section
			if(!j[name].is_null() && j[name].contains(pair.first)) {
				*pair.second = j[name][pair.first];
			}
		}
	}

	//Util::PrintLnFormat("Serializable::Read(): double reads complete");
}

}
