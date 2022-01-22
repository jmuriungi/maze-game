#ifndef _MAZE_H_
#define _MAZE_H_

#include <vector>
#include <stdlib.h> // For rand
#include <iostream>

// Enumeration for type of content in maze
enum FILL_TYPE {
    START, // Start of the maze
    END,   // End of the maze (an exit)
    OPEN,  // Empty space that the player can move through
    WALL,  // Non-empty space that the player can't move through
    ITEM,  // Maze collectibles for higher scores
    SOLUTION  // A verified path solution (for generation)
};

class simpleMaze {
private:
    enum DIRECTION {
        UP, 
        LEFT,
        DOWN,
        RIGHT
    };
public:

    int width, length;  // maze dimensions: width->x and length->z
    std::vector< std::vector<FILL_TYPE> > maze;

    int countUp, countDown, countLeft, countRight;

    simpleMaze();

    // Constructor with given dimensions and seed to use for generating the maze
    simpleMaze(int w, int h, int seed);

    // Method for creating a maze, takes in seed as value for initializing rand()
    void generateMaze(int seed);

    // Recursive method
    bool createSolution(int cur_x, int cur_y);

    void createBranch(int max_length);

    // Method for determinnig if the maze has a solution (start and exit)
    bool verifySolution();
    
    bool contBranch(int x, int y, DIRECTION prev_dir);

    // Boolean to check for if the current position (a 3D position with floats) is too close to the walls with given min_distance
    
    bool checkWalls(float pos_x,float pos_z, float min_distance);
    bool checkLocation(int );
};

std::ostream& operator<<(std::ostream& output, const simpleMaze& mazeObj);


#endif
