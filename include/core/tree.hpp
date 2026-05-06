#pragma once
#include "graph.hpp"
#include <vector>

struct Tree {
    core::node* root;
    std::vector<core::node*> children;

    Tree(core::node* root): root(root){}

    
};