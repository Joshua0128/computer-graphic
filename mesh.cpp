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

vector<vector <float>> h_v;
vector<vector <float>> h_f;

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

    // for (int i = 0; i < 3; i++)
    // {
    //     cout << obj_size[i] << " ";
    // }


    fstream fin;
    fin.open(filename, ios::in);

    int vIdx = 0, fIdx = 0, heIdx = 0;
    
    // reserve
    vertice.reserve(obj_size[0]);
    faces.reserve(obj_size[1]);
    halfedges.reserve(obj_size[2]);

    for(int i = 0;i < obj_size[0]; i++)
    {
        Vertex v;
        vertice.push_back(v);
    }

    for(int i = 0;i < obj_size[1]; i++)
    {
        Face f;
        faces.push_back(f);
    }

    for(int i = 0;i < obj_size[2]; i++)
    {
        Halfedge e;
        halfedges.push_back(e);
    }

    h_v.resize(obj_size[0]);
    h_f.resize(obj_size[1]);
    

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
        // for(int i = 0; i < coord.size(); i++)
        // {
        //     cout << coord[i] << " ";
        // }
        // cout << endl;

        if (type == "v")
        {
            h_v[vIdx] = coord;
            vIdx++;
        }
        else if (type == "f")
        {
            h_f[fIdx] = coord;
            fIdx++;
        }
    }
    
    // assign vertex index, pos.
    for(int i = 0; i < h_v.size(); i++)
    {
        Vertex v;
        v.index = i+1;
        glm::vec3 p(0.0f);
        for (int j = 0; j < h_v[i].size(); j++)
        {
            if (h_v[i].size() < 3)
                cerr << "out of bound" << i << endl;
            p[j] = h_v[i][j];
        }
        v.pos = p;
        vertice[i] = v;
    }


    /*
        assign index to f.
    */
    for(int i = 0;i < h_f.size(); i++)
    {
        Face f;
        f.index = i+1;
        faces[i] = f;
    }

    // a map to store record of vertex-halfedge
    unordered_map<int, vector<Halfedge*>> i_edges_map;

    heIdx = 0;
    for(int i = 0;i < h_f.size(); i++)
    {
        /*
            assign v, f, index to e
            add e into halfedges
            assign e to f.
            setup next, prev to e.
            assign e to v.
        */
        for(int j = 0; j < h_f[i].size(); j++)
        {
            Halfedge* e = &halfedges[heIdx];
            e->index = heIdx;
            int vid = int(h_f[i][j]);
            e->v = &vertice[vid-1];
            i_edges_map[vid].push_back(e);
            vertice[vid-1].e = e;
            e->f = &faces[i];
            
            e->next = &halfedges[(heIdx+1)%h_f[i].size()];
            e->prev = &halfedges[(heIdx + h_f[i].size() - 1)%h_f[i].size()];
            
            heIdx++;
            if(faces[i].e == nullptr)
                faces[i].e = e;
        }
    }

    for(int k = 0; k < heIdx; k++)
    {
        Halfedge* e = &halfedges[k];
        // cout << e->v->index << endl;
        for(int i = 0; i < 3; i++)
        {
            cout << e->v->index << " ";
            e = e->prev;
        }
        cout << endl;
    }


    /*
        setup opposite edge
    */
//    cout << heIdx;
    for(int i = 0;i < heIdx; i++)
    {
        Halfedge& e = halfedges[i];
        // cout << e.v->index << endl;
        // cout << e.prev->v->index << endl;
        
        // if (e.o)
        //     continue;
        
        vector<Halfedge*>& i_edges = i_edges_map[e.prev->v->index];
        for (int j = 0; j < i_edges.size(); j++)
        {
            // cout << e.prev->v->index << ": ";
            // cout << i_edges[j]->v->index << ", ";
            // Halfedge* eo = i_edges[j];
            // if (eo->index == e.index)
            //     continue;
            // if (eo->prev->v->index == e.v->index)
            // {
            //     e.o = eo;
            //     eo->o = &e;
            //     cerr << "gotchar!";
            //     break; 
            // }
        }
        // cout << endl;
    }

    for(int i = 0;i < heIdx; i++)
    {
        Halfedge& e = halfedges[i];
        if(e.o != nullptr)
            cout << e.o->index << endl;
    }





}



int main()
{
    Mesh m = Mesh();
    string filename;

    cout << "input the mesh" << endl;
    // cin >> filename;
    filename = "test.obj";
    cout << "Loading the " << filename << endl;
    m.BuildFromObj(filename);
    cout << "Result" << endl;
    return 0;
}