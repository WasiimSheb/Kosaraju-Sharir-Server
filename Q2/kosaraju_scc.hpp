#ifndef KOSARAJU_SCC_H
#define KOSARAJU_SCC_H

#include <iostream>
#include <vector>
#include <stack>
#include <list>
#include <deque>
#include <fstream>

using namespace std;

// Declarations for List Implementation (std::list)
void fillOrderList(int v, vector<bool>& visited, stack<int>& Stack, const list<list<int>>& adj);
void DFSUtilList(int v, vector<bool>& visited, const list<list<int>>& transposedAdj, vector<int>& component);
list<list<int>> getTransposeList(int n, const list<list<int>>& adj);
void findSCCsList(int n, int m, const vector<pair<int, int>>& edges);

// Declarations for Deque Implementation (std::deque)
void fillOrderDeque(int v, vector<bool>& visited, stack<int>& Stack, const vector<deque<int>>& adj);
void DFSUtilDeque(int v, vector<bool>& visited, const vector<deque<int>>& transposedAdj, vector<int>& component);
vector<deque<int>> getTransposeDeque(int n, const vector<deque<int>>& adj);
void findSCCsDeque(int n, int m, const vector<pair<int, int>>& edges);

// Declarations for Vector of Vectors Implementation
void fillOrderVectorVec(int v, vector<bool>& visited, stack<int>& Stack, const vector<vector<int>>& adj);
void DFSUtilVectorVec(int v, vector<bool>& visited, const vector<vector<int>>& transposedAdj, vector<int>& component);
vector<vector<int>> getTransposeVectorVec(const vector<vector<int>>& adj);
void findSCCsVectorVec(int n, int m, const vector<pair<int, int>>& edges);

// Declarations for Vector of Lists Implementation
void fillOrderVectorList(int v, vector<bool>& visited, stack<int>& Stack, const vector<list<int>>& adj);
void DFSUtilVectorList(int v, vector<bool>& visited, const vector<list<int>>& transposedAdj, vector<int>& component);
vector<list<int>> getTransposeVectorList(const vector<list<int>>& adj);
void findSCCsVectorList(int n, int m, const vector<pair<int, int>>& edges);

#endif // KOSARAJU_SCC_H
