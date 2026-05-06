#include <string> 
#include <hash_set> 
#include <vector> 

namespace core{ 

struct point { 
    int x; 
    int y;

    bool operator==(const point& other) const { 
        return (x == other.x && y == other.y);
    }
}; 

struct node { 
    point p; 
    int f = 0; // f + g or just g 
    int g = 0; // dist from start 
    int h = 0; // dist from goal 
    int nextRequiredNumber = 0;
    node* parent = nullptr; 
    char lastMove; 

    int getF() const { return g + h; } 

    bool operator>(const node& other) const { 
        return getF() > other.getF();
    }
};

class graph { 
private: 
    int rows;
    int cols;
    std::vector<std::vector<char>> grid;
    std::vector<std::vector<int>> weights;
    std::vector<point> numberPos; 
    point goalPos; 
public: 
   
}; 


} // namespace core 
