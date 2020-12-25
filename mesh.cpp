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
        string type = result.substr(0, result.find(" "));
        if (type != "v" && type != "f")
        {
            continue;
        }

        int foundPos = result.find("//");

        // align different format
        while (foundPos != std::string::npos)
        {
            result = result.replace(foundPos, 2, "");
            foundPos = result.find("//");
        }

        vector<float> coord = getCoordFromStr(result);

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
        int anchor = heIdx;
        for(int j = 0; j < h_f[i].size(); j++)
        {
            Halfedge* e = &halfedges[heIdx];
            e->index = heIdx;
            int vid = int(h_f[i][j]);
            e->v = &vertice[vid-1];
            i_edges_map[vid].push_back(e);
            vertice[vid-1].e = e;
            e->f = &faces[i];
            
            e->next = &halfedges[anchor + (j + 1)%h_f[i].size()];
            e->prev = &halfedges[anchor + (j + h_f[i].size() - 1)%h_f[i].size()];
            
            heIdx++;
            if(faces[i].e == nullptr)
                faces[i].e = e;
        }
    }
    /*
        setup o edge of e
    */

    for(int i = 0;i < heIdx; i++)
    {
        Halfedge& e = halfedges[i];

        if (e.o)
            continue;
        
        vector<Halfedge*>& i_edges = i_edges_map[e.prev->v->index];
        for (int j = 0; j < i_edges.size(); j++)
        {
            Halfedge* eo = i_edges[j];
            if (eo->index == e.index)
                continue;
            if (eo->prev->v->index == e.v->index)
            {
                e.o = eo;
                eo->o = &e;
                // cerr << "gotcha!" << endl;
                break; 
            }
        }
    }

    // for(int m = 0; m < faces.size(); m++)
    // {
    //     Face t = faces[m];
    //     vector<Halfedge*> fe = t.Edges();
    //     for (int n = 0; n < fe.size(); n++)
    //     {
    //         cout << fe[n]->index << " ";
    //     }
    //     cout << endl;
    // }

    // for(int m = 0; m < vertice.size(); m++)
    // {
    //     Vertex t = vertice[m];
    //     vector<Halfedge*> fe = t.Edges();
    //     for (int n = 0; n < fe.size(); n++)
    //     {
    //         cout << fe[n]->index << " ";
    //     }
    //     cout << endl;
    // }
}

vector<vector<Halfedge*>> Mesh::Boundaries()
{
    vector<Halfedge*> boundary_e;
    vector<vector <Halfedge*>> boundaries;

    int heIdx = 0;
    for (int i = 0; i < halfedges.size(); i++)
    {
        if(halfedges[i].index != -1)
        {
            heIdx++;
        }
    }

    int archor = heIdx;
    for(int i = 0; i < heIdx; i++)
    {
        if(halfedges[i].o == nullptr)
        {
            // cout << archor << endl;
            Halfedge* e = &halfedges[archor];
            e->o = &halfedges[i];
            e->index = archor;
            e->v = halfedges[i].prev->v;

            halfedges[i].o = e;
            archor++;
            boundary_e.push_back(e);
        }
    }

    for (int i = 0; i < boundary_e.size(); i++)
    {
        int start = boundary_e[i]->v->index;  
        for(int j = 0; j < boundary_e.size(); j++)
        { 
            if (start == boundary_e[j]->o->v->index)
            {
                boundary_e[i]->next = boundary_e[j];
                boundary_e[j]->prev = boundary_e[i];
            }
        }
    }

    while(!boundary_e.empty())
    {
        vector<Halfedge*> boundary;
        Halfedge* start_e = boundary_e.front();
        boundary.push_back(start_e);
        boundary_e.erase(boundary_e.begin());
        Halfedge* next_e = start_e->next; 
        while(next_e != start_e)
        {
            boundary.push_back(next_e);
            boundary_e.erase(remove(boundary_e.begin(), boundary_e.end(), next_e), boundary_e.end());
            next_e = next_e->next;
        }
        boundaries.push_back(boundary);
    }

    return boundaries;
}

vector<Halfedge*>Face::Edges()
{
    vector<Halfedge*> res;
    Halfedge* start_e = this->e;
    res.push_back(start_e);

    Halfedge* next_e = start_e->next;
    while(next_e != nullptr)
    {
        if (next_e == start_e)
            break;
        res.push_back(next_e);
        next_e = next_e->next;
    }
    
    return res;
}

vector<Halfedge*>Vertex::Edges()
{
    vector<Halfedge*> res;
    Halfedge* start_e = this->e;

    res.push_back(start_e);

    Halfedge* next_e = start_e->next->o;
    
    while(next_e != nullptr)
    {
        if (next_e == start_e)
            break;
        res.push_back(next_e);
        next_e = next_e->next->o;
    }

    return res; 
}

void Mesh::ShowResult()
{
    vector<vector<Halfedge*>> b;
    b = this->Boundaries();

    cout << "# of Vertice: " << vertice.size() << endl;
    vector<int> std_v(vertice.size(), 0);
    
    for(int i = 0; i < vertice.size(); i++)
    {
        Vertex v = vertice[i];
        int valence = v.Edges().size();
        std_v[valence]++;
    }
    for(int i = 0; i < vertice.size(); i++)
    {
        if(std_v[i] != 0)
            cout << "valence-" << i << ": " << std_v[i] << endl;
    }

    cout << "# of Face: " << faces.size() << endl;
    vector<int> std_f(faces.size(), 0);

    for(int i = 0; i < faces.size(); i++)
    {
        Face f = faces[i];
        int degree = f.Edges().size();
        std_f[degree]++;
    }
    for(int i = 0; i < std_f.size(); i++)
    {
        if(std_f[i] != 0)
            cout << "degree-" << i << ": " << std_f[i] << endl;
    }

    cout << "border size: ";
    if(b.size() == 0)
    {
        cout << "none" << endl;
    } 
    else
    {
        for(int i = 0; i < b.size(); i++)
        {
            cout << b[i].size() << " ";
        }
        cout << endl;
    }
    
    

}

int main()
{
    Mesh m = Mesh();
    string filename;

    // cout << "input the mesh" << endl
    // cin >> filename;
    filename = "roof.obj";
    cout << "Loading the " << filename << endl;
    m.BuildFromObj(filename);
    // m.Boundaries();
    cout << "== Mesh Statics ==" << endl;

    m.ShowResult();
    // m.Boundaries();

    return 0;
}