#pragma once
#include <includes.h>
#include <sdk.h>

class c_tables {
public:
	struct
	{
		std::vector<CClientPlayer*> players;
		std::vector<CClientVehicle*> vehicles;
		std::vector<CClientPed*> peds;
		std::vector<CResource*> resources;
		std::vector<CLuaArgument*> arguments;
	} entity;
};

inline c_tables* table = new c_tables();