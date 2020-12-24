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

int main()
{
    unordered_map<int, vector<int>> a;
    vector<int> tmp;
    pair<int, vector<int>> item (1, tmp);
    a.insert(item);
    a.at(1).push_back(7);
    a.at(1).push_back(8);

    if (a.at(1))
        cout << "bingo" << endl;


    for (auto& x: a)
    {
        cout << x.first << endl;
        for (auto&y : x.second)
        {
            cout << y << " ";
        }
    }

    return 0;
}