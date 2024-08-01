#include "savePlot.h"


map<string, vector<float>> graphs;


void saveGraph(string key, vector<float> xValues, vector<float> yValues)
{
    xValues.insert(xValues.end(), yValues.begin(), yValues.end());
    graphs.emplace(key, xValues);
}

void savePoint(string key, float xValue, float yValue)
{
    if (graphs.find(key) == graphs.end())
    {
        graphs.emplace(key, vector<float>());
    }
    graphs.at(key).insert(graphs.at(key).begin() + graphs.at(key).size() / 2, 1, xValue);
    graphs.at(key).emplace_back(yValue);
}

void writeToFile()
{
    ofstream graphFile("graphs.txt");


    for (auto it = graphs.begin(); it != graphs.end(); ++it)
    {
        graphFile << it->first;
        for (size_t i = 0; i < it->second.size(); i++)
        {
            graphFile << " " << it->second[i];
        }
        graphFile << endl;
    }

    graphFile.close();
}