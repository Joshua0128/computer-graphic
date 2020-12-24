#include "glm/glm.hpp"
#include <stdlib.h>
#include <iostream>
#include <vector>

using namespace std;

class Halfedge; class Face; class Vertex;


class Vertex
{
    public:
        Halfedge* e;
        int index;
        glm::vec3 pos;
        vector<Halfedge*> Edges();

    Vertex()
    {
        init();
    }

    void init()
    {
        e = nullptr;
        index = -1;
        pos = glm::vec3(0, 0, 0);
    }
};


class Face
{
    public:
        Halfedge* e;
        int index;
        vector<Halfedge*> Edges();

    Face()
    {
        init();
    }

    void init()
    {
        e = nullptr;
        index = -1;
    }
};

class Halfedge
{
    public:
        Halfedge* o;
        Halfedge* next;
        Halfedge* prev;
        int index;
        Vertex* v;
        Face* f;

    Halfedge()
    {
        init();
    }

    void init()
    {
        o = nullptr;
        next = nullptr;
        prev = nullptr;
        index = -1;
        v = nullptr;
        f = nullptr;
    }
};
    


class Mesh
{
    public:
        vector<Halfedge> halfedges;
        vector<Vertex> vertice;
        vector<Face> faces;

        void BuildFromObj(std::string filename);
        vector<vector<Halfedge*>> Boundaries();
};