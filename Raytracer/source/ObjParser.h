#pragma once
#include <string>
#include <fstream>
#include <vector>
#include "EMath.h"

static bool ParseOBJ(const std::string& filename, std::vector<Elite::FPoint3>& vertices, std::vector<int>& indices)
{
	std::ifstream file(filename);
	if (!file)
		return false;
	std::vector<Elite::FPoint3> positions;
	vertices.clear();
	indices.clear();

	std::string sCommand;
	while (!file.eof())
	{
		file >> sCommand;
		if (sCommand == "v")
		{
			float x, y, z;
			file >> x >> y >> z;
			positions.push_back(Elite::FPoint3(x, y, z));
		}
		else if (sCommand == "f")
		{
			Elite::FPoint3 vertex{};
			size_t iPosition;
			for (size_t iFace = 0; iFace < 3; iFace++)
			{
				file >> iPosition;
				vertex = positions[iPosition - 1];

				if ('/' == file.peek())
				{
					file.ignore();
				}
				vertices.push_back(vertex);
				indices.push_back(uint32_t(vertices.size()) - 1);
			}
		}
		file.ignore(1000, '\n');
	}
}