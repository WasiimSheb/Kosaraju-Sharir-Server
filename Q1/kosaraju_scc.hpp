#ifndef KOSARAJU_SCC_H
#define KOSARAJU_SCC_H

#include <iostream>
#include <vector>
#include <stack>
#include <list>
#include <unordered_map>

using namespace std;

// Function to perform DFS and fill the stack with vertices in the order of their finishing times
void fillOrder(int v, vector<bool>& visited, stack<int>& Stack, const unordered_map<int, list<int>>& adj);

// Function to perform DFS on the transposed graph
void DFSUtil(int v, vector<bool>& visited, const unordered_map<int, list<int>>& transposedAdj, vector<int>& component);

// Function to get the transposed (reverse) graph
unordered_map<int, list<int>> getTranspose(int n, const unordered_map<int, list<int>>& adj);

// Function to find and print all strongly connected components
void findSCCs(int n, int m, const vector<pair<int, int>>& edges);

#endif // KOSARAJU_SCC_H


