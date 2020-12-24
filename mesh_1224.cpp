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
    // vertice.resize(obj_size[0]);
    // faces.resize(obj_size[1]);
    // halfedges.resize(obj_size[2]);
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
    for(int i =0; i < h_v.size(); i++)
    {
        Vertex v;
        v.index = i+1;
        glm::vec3 p(0.0f);
        for (int j = 0; j < h_v[i].size(); j++)
        {
            if (h_v[i].size() < 3)
                cerr << "out of bound" << i << endl;
            p[j] = h_v[i][j];
            // cout << h_v[i][j] << " ";
        }
        // cout << p.x << " " << p.y << " " << p.z << endl;
        v.pos = p;
        vertice[i] = v;
    }

    // verify vertex in vertice
    // for(int i = 0; i < vertice.size(); i++)
    // {
    //     cout << i << ": " << vertice[i].pos.x << ", " << vertice[i].pos.y << ", " << vertice[i].pos.z <<endl;
    // }


    /*
    [v] assign index to f.
    */
    for(int i = 0;i < h_f.size(); i++)
    {
        Face f;
        f.index = i+1;
        faces[i] = f;
    }




    // a map to store record of vertex-halfedge
    unordered_map<int, vector<Halfedge*>> o_edges_map;

    heIdx = 0;
    for(int i = 0;i < h_f.size(); i++)
    {
        /*
            assign v, f, index to e
            add e into halfedges
            assign e to f.
            assign e to v.
        */
        for(int j = 0; j < h_f[i].size(); j++)
        {
            Halfedge e;
            e.index = heIdx;
            int vid = int(h_f[i][j]);
            e.v = &vertice[vid-1];

            // cout << e.index << endl;
            vertice[vid-1].e = &e;
            e.f = &faces[i];
            halfedges[heIdx] = e;
            heIdx++;
            
            // cout << e.f->index << endl;
            if(faces[i].e == nullptr)
                faces[i].e = &e;
            
            // cout << faces[i].e->index << endl;
        }
        /*
            assgin next, prev to e.
        */
        for(int j = 0; j < h_f[i].size(); j++)
        {
            e.g
        }
        cout << faces[i].e->index << endl;
    }

    for(int i = 0; i < h_f.size(); i++)
    {
        cout << faces[i].e->index << endl;
        // Halfedge tmp = *(faces[i].e);
        // cout << tmp.index << endl;     
        // cout << i << endl;
        // cout << faces[i].index << endl;
        // cout << &(faces[i].e) << endl;
        // cout << faces[i].e->index << endl;
        // cout << &(faces[i].e->index) << endl;
    }


    // for(int i = 0; i <  heIdx; i++)
    // {
    //     cout << halfedges[i].v->index << endl;
    //     cout << halfedges[i].f->index << endl;
    // }

    // for(int i = 0; i < faces.size(); i++)
    // {
    //     cout << faces[i].index << " ";
    //     // cout << faces[i].e->index << endl;
    //     Face t = faces[i];
    //     Halfedge et = *t.e;
    //     cout << et.index << endl;
    //     cout << faces[i].e->index << endl;
    //     // cout << vertice[i].index << endl;
    //     // cout << vertice[i].pos.x << endl;
    //     // cout << vertice[i].e->index << endl;
    // }



    // for(int i = 0;i < h_f.size(); i++)
    // {
    //     for(int j = 0; j < h_f[i].size(); j++)
    //     {
    //         Halfedge e;
    //         e.index = heIdx;
    //         int vid = int(h_f[i][j]);
    //         // if (vertice[vid].e == nullptr)
    //         //     cerr << "error" << endl; 
    //         // cout << vertice[vid].index << endl;
    //         e.v = &vertice[vid];
    //         cout << vid << endl;
    //         // vertice[0].e = nullptr;
    //         e.f = &faces[i];
    //         halfedges[heIdx] = e;
    //         heIdx++;
            
    //         if(faces[i].e == nullptr)
    //             faces[i].e = &e;
    //     }
    // }

    /*
    [ ] assign next, prev to e;
    */


}



int main()
{
    Mesh m = Mesh();
    string filename;

    cout << "input the mesh" << endl;
    // cin >> filename;
    filename = "cow.obj";
    cout << "Loading the " << filename << endl;
    m.BuildFromObj(filename);
    cout << "Result" << endl;
    return 0;
}