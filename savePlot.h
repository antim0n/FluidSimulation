#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <vector>


// #include <fstream>

using namespace std;

// function declaration


void saveGraph(string key, vector<float> xValues, vector<float> yValues);

void savePoint(string key, float xValue, float yValue);

void writeToFile();