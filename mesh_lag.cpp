#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include "glm/glm.hpp"
#include "mesh.h"
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

#define TMP_SIZE 100

vector<int> obj_size;
char line[TMP_SIZE];

vector<vector<float>> h_v;
vector<vector <int>> h_f;

vector<int> getObjSize(string filename)
{
    fstream fin;
	fin.open(filename, ios::in);
	int vCount = 0;
	int fCount = 0;
	int eCount = 0;
	while (fin.getline(line, sizeof(line), '\n')) {
		string result = string(line);
		string type = result.substr(0, result.find(" "));
		// if there is an empty line
		if (type != "v" && type != "f") {
			continue;
		}

		// vectex count increase
		if (type == "v") {
			vCount++;
		}
		// face count increase
		else if (type == "f") {
			int foundPos = result.find(" ");
			while (foundPos != std::string::npos && foundPos != result.length() - 1) {
				result = result.replace(foundPos, 1, "");
				foundPos = result.find(" ");
				eCount++;
			}
			fCount++;
		}
	}
	
	vector<int> result = { vCount, fCount, eCount*2 };

    // for (int i = 0; i < 3; i++)
    // {
    //     cout << result[i] << " ";
    // }
    // cout << endl;


	return result;
}

vector<float> getCoordFromStr(string str) {
	string s = str;
	vector<float> results;
	int space = s.find(" ");
	while (space != std::string::npos) {
		string newStr = s.substr(space + 1, s.length());
		if (newStr.length() == s.length() || newStr.length() == 0) {
			break;
		}

		s = newStr;
        string val = newStr.substr(0, newStr.find(" "));
        // if(val == "\r")
        // {
        //     cerr << "fuck";
        // }
        if(val != "\r")
        {
            results.push_back(std::stof(val));
        }
		space = s.find(" ");
	}

	return results;
}

void Mesh::BuildFromObj(string filename)
{
    obj_size = getObjSize(filename);
    fstream fin;
    fin.open(filename, ios::in);

    int vIdx = 0, fIdx = 0, heIdx = 0;
    
    // reserve
    // vertice.resize(obj_size[0]);
    // faces.resize(obj_size[1]);
    // halfedges.resize(obj_size[2]);
    h_v.resize(obj_size[0]);
    h_f.resize(obj_size[1]);
    


    // a map to store record of vertex-halfedge
    unordered_map<int, vector<Halfedge*>> o_edges_map;

    while(fin.getline(line, sizeof(line), '\n'))
    {
        string result = string(line);
        // cout << result << endl;
        string type = result.substr(0, result.find(" "));
        // cout << type << endl;
        if (type != "v" && type != "f")
        {
            continue;
        }
        // cout << result << endl;
        int foundPos = result.find("//");
        // cout << foundPos;

        // align different format
        while (foundPos != std::string::npos)
        {
            result = result.replace(foundPos, 2, "");
            foundPos = result.find("//");
        }
        // cout << result << endl;
        vector<float> coord = getCoordFromStr(result);
        for(int i = 0; i < coord.size(); i++)
        {
            cout << coord[i] << " ";
        }
        cout << endl;

        if (type == "v")
        {
            vertice[vIdx] = *new Vertex();
            vertice[vIdx].index = vIdx;
            vertice[vIdx].pos = glm::vec3(coord[0], coord[1], coord[2]);
            vIdx++;
        }
        else if (type == "f")
        {
            faces[fIdx] = *new Face();
            faces[fIdx].index = fIdx;
            
            // bind vertex and halfedges
            for (int i = 0; i < coord.size(); i++)
            {
                halfedges[heIdx + i] = *new Halfedge();
                // cout << coord[i] << endl;
                // cout << vertice[coord[i]].index << endl;
                // cout << vertice[coord[i]] << endl;
                Vertex* v = &vertice[coord[i]];
                halfedges[heIdx + i].v = v;
                vertice[coord[i]].e = halfedges[heIdx + i];
                // o_edges_map[vertice[coord[i]]].append(halfedges[heIdx + i]);
                // halfedges[]
            }

            // setup next halfedge
            // for (int i = 0;i < coord.size(); i++)
            // {
            //     halfedges[heIdx + i].next = 
            // }


            fIdx++;
        }
    }
}



int main()
{
    Mesh m = Mesh();
    string filename;

    cout << "input the mesh" << endl;
    // cin >> filename;
    filename = "lilium.obj";
    cout << "Loading the " << filename << endl;
    m.BuildFromObj(filename);
    cout << "Result" << endl;
    return 0;
}