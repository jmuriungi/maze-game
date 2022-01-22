#include "../headers/maze.h"

// Defualt constructor sets everything to zero
simpleMaze::simpleMaze() {
    this->countUp = 0;
    this->countDown = 0;
    this->countLeft = 0;
    this->countRight = 0;
    this->width = 0;
    this->length = 0;
    this->maze = std::vector< std::vector<FILL_TYPE> >(this->width, std::vector<FILL_TYPE>(this->length, WALL));
}

// Constructor with given width and length (h) and seed for generation
simpleMaze::simpleMaze(int w, int h, int seed) {
    this->countUp = 0;
    this->countDown = 0;
    this->countLeft = 0;
    this->countRight = 0;
    this->width = w;
    this->length = h;
    // Setting maze to be width x length and filled with OPEN spaces
    this->maze = std::vector< std::vector<FILL_TYPE> >(this->width, std::vector<FILL_TYPE>(this->length, WALL));

    generateMaze(seed);
}

void simpleMaze::generateMaze(int seed) {
    // Setting rand with seed
    srand(seed);

    // Settings
    int start_x, start_y;
    start_x = this->width / 2;
    start_y = this->length -2;
    this->maze[start_x][start_y] = START;

    //generate random numbers for row, randRow array?
    //generate random numbers for column randCol array?
    //fill random rows with w's for walled areas and o's for paths
    //fill random columns with w's for walled areas and o's for paths
    //fill ra
    createSolution(start_x, start_y);
    for (int i = 0; i < 1400; i++) {
        createBranch(-1); // "infinite" length
        //createBranch(5); // Setting the length maximum to be 5
    }
}


bool simpleMaze::checkWalls( float pos_x,float pos_z, float min_distance){
    // Gets position in grid space, with rounding up
    int int_posX = int(pos_x + 0.5f);
    int int_posZ = int(pos_z + 0.5f);
    // Check if the position is inside the maze at all, if not then there is no concern for walls
    if (int_posX > this->width || int_posX < 0 || int_posZ > this->length || int_posZ < 0) {
        return false;
    } 
    if (this->maze[int_posX][int_posZ] == WALL) {
        return true;
    }
    // Check X dimension
    if (int_posX > 0) {
        if (this->maze[int_posX-1][int_posZ] == WALL) {
            if((pos_x + 0.5f) - int_posX < min_distance) {
                return true;
            }
        }
    }
    if (int_posX < this->width-1) {
        if (this->maze[int_posX+1][int_posZ] == WALL) {
            if((pos_x + 0.5f) - int_posX > (1-min_distance)) {
                return true;
            }
        }
    }
    // Check Z dimension
    if (int_posZ < this->length-1) {
        if (this->maze[int_posX][int_posZ+1] == WALL) {
            if((pos_z + 0.5f) - int_posZ > (1-min_distance)) {
                return true;
            }
        }
    }
    if (pos_z > 0) {
        if (this->maze[int_posX][int_posZ-1] == WALL) {
            if ((pos_z + 0.5f) - int_posZ < min_distance) {
                return true;
            }
        }
    }
    
    return false;
}


void simpleMaze::createBranch(int max_length) {
    // base case // range: 1 - (dim-2) [no edges]
    int x = rand() % (this->width-2) + 1;
    int y = rand() % (this->length-2) + 1;
    int cur_length = 0;
    bool infinite;
    if (max_length < 0) {
        infinite = true;
    }
    else {
        infinite = false;
    }
    bool atEdge = false;
    bool goNext = true;
    DIRECTION prev_dir, curr_dir;

    prev_dir = DIRECTION(rand() % 4);
    do {
        // Assign where we are as open (continue building the branch)
        this->maze[x][y] = OPEN;
        cur_length++;
        // Check if we ran into an another opening (another open spot not our own)
        goNext = contBranch(x, y, prev_dir);

        if (goNext) {
            // pick a direction to move that is not same as previous and go there
            do {
                curr_dir = DIRECTION(rand() % 4);
                // Playing with enumeration order to check against previous
            } while(((curr_dir+2) % 4 == prev_dir));

            switch (curr_dir) {
                case(UP):
                    y++;
                    break;
                case(DOWN):
                    y--;
                    break;
                case(LEFT):
                    x--;
                    break;
                case(RIGHT):
                    x++;
                    break;
            }

            prev_dir = curr_dir;

            // If that new movment is an edge, then mark atEdge as true
            if (x == this->width-1 || y == this->length-1 || x == 0 || y == 0) {
                atEdge = true;
            }
        }
    } while (goNext && !atEdge && (cur_length <= max_length || infinite));

}

// Checker to see if we have ran into another open spot that is not from previous step (loop created)
bool simpleMaze::contBranch(int x, int y, DIRECTION prev_dir)
{   // check if adjacent spots are open, return true if yes
    if (x > 0) { 
        if ((this->maze[x-1][y] != WALL) && prev_dir != RIGHT)
            return false;
    }
    if (x < this->width && (this->maze[x+1][y] != WALL) && prev_dir != LEFT) {
        return false;
    }
    if (y > 0 && (this->maze[x][y-1] != WALL) && prev_dir != UP) {
        return false;
    }
    if (y < this->length && (this->maze[x][y+1] != WALL) && prev_dir != DOWN) {
        return false;
    }
    // If none of the directions led to an open space (all are walls), then return false
    return true;
}

bool simpleMaze::createSolution(int cur_x, int cur_y) {
    DIRECTION dir;
    int next_x, next_y;
    // Base case
    // If we are at an edge (left or right sides)
    if (cur_x == 0 || cur_x == this->width) {
        // Quick bound check to make sure we can look down
        // if not at bottom row
        if (cur_y < this->length - 1) {
            // If down is not start (we didn't create a loop)
            if (this->maze[cur_x][cur_y + 1] != START) {
                // Mark current location as an end to the maze
                this->maze[cur_x][cur_y] = END;
                return true;
            }
            else {
                // This is an invalid path
                this->maze[cur_x][cur_y] = WALL; // ? not sure what to do with invalid end
                return false;
            }
        }
        // If not at top row
        if (cur_y > 0) {
            // If up is not start (we didn't create a loop)
            if (this->maze[cur_x][cur_y - 1] != START) {
                // Mark current location as an end to the maze
                this->maze[cur_x][cur_y] = END;
                return true;
            }
            else {
                // This is an invalid path
                this->maze[cur_x][cur_y] = WALL; // ? not sure what to do with invalid end
                return false;
            }
        }
    }
    else if (cur_y == 0 || cur_y == this->length) {
        // If not at left column
        if (cur_x > 0) {
            // If up is not start (we didn't create a loop)
            if (this->maze[cur_x - 1][cur_y] != START) {
                // Mark current location as an end to the maze
                this->maze[cur_x][cur_y] = END;
                return true;
            }
            else {
                // This is an invalid path
                this->maze[cur_x][cur_y] = WALL; // ? not sure what to do with invalid end
                return false;
            }
        }
        // Quick bound check to make sure we can look down
        // if not at bottom row
        if (cur_x < this->width - 1) {
            // If down is not start (we didn't create a loop)
            if (this->maze[cur_x + 1][cur_y] != START) {
                // Mark current location as an end to the maze
                this->maze[cur_x][cur_y] = END;
                return true;
            }
            else {
                // This is an invalid path
                this->maze[cur_x][cur_y] = WALL; // ? not sure what to do with invalid end
                return false;
            }
        }
    }


    else { // We are not at an edge, so we must continue on
        dir = DIRECTION(rand() % 4);
        for (int i = 0; i < 6; i++) {
            switch (dir) {
                case (UP):
                    countUp++;
                    next_x = cur_x;
                    next_y = cur_y - 1;
                    if (/*(this->maze[next_x][next_y] != SOLUTION &&*/ this->maze[next_x][next_y] != START) {
                        this->maze[next_x][next_y] = SOLUTION;   // "move up"
                        // If this leads to the end (a successfully created solution)
                        if (createSolution(next_x, next_y)) {
                            return true;
                        }
                        else {
                        // If this did not lead to a successful end, then undo the movement
                            this->maze[next_x][next_y] = WALL;
                        // Try another direction
                        }
                }
                    break;
                case (LEFT):
                    countLeft++;
                    next_x = cur_x - 1;
                    next_y = cur_y;

                    if (/*(this->maze[next_x][next_y] != SOLUTION && */this->maze[next_x][next_y] != START) {
                        // "move left"
                        this->maze[next_x][next_y] = SOLUTION;
                        // If this leads to the end (a successfully created solution)
                        if (createSolution(next_x, next_y)) {
                            return true;
                        }
                        else {
                            // If this did not lead to a successful end, then undo the movement
                            this->maze[next_x][next_y] = WALL;
                        }
                    }
                    break;
                case (RIGHT):
                    countRight++;
                    next_x = cur_x + 1;
                    next_y = cur_y;
                    if (/*(this->maze[next_x][next_y] != SOLUTION && */this->maze[next_x][next_y] != START) {
                    // "move right"
                    this->maze[next_x][next_y] = SOLUTION;
                    // If this leads to the end (a successfully created solution)
                    if (createSolution(next_x, next_y)) {
                        return true;
                    }
                    else {
                        // If this did not lead to a successful end, then undo the movement
                        this->maze[next_x][next_y] = WALL;
                    }
                }
                    break;
                case (DOWN):
                    countDown++;
                    next_x = cur_x;
                    next_y = cur_y - 1;
                    if (/*(this->maze[next_x][next_y] != SOLUTION && */this->maze[next_x][next_y] != START) {
                        // "move down"
                        this->maze[next_x][next_y] = SOLUTION;
                        // If this leads to the end (a successfully created solution)
                        if (createSolution(next_x, next_y)) {
                            return true;
                        }
                        else {
                            // If this did not lead to a successful end, then undo the movement
                            this->maze[next_x][next_y] = WALL;
                        }
                    }
                    break;
            }
            // If here, then the direction tried did not lead to a ending and need to try a new direction
            dir = DIRECTION((dir + rand() % 3 + 1) % 4);
        }
        // If all directions did not lead to a successful end, then this is a "lost cause" and should back out
        this->maze[cur_x][cur_y] = WALL;
        return false;
    }
    return false;
}


std::ostream& operator<<(std::ostream& output, const simpleMaze& mazeObj) {
    output << "countUp: " << mazeObj.countUp << std::endl;
    output << "countDown: " << mazeObj.countDown << std::endl;
    output << "countLeft: " << mazeObj.countLeft<< std::endl;
    output << "countRight: " << mazeObj.countRight << std::endl;
    for (int row = mazeObj.length-1; row >= 0; row--) {
        for (int col = 0; col < mazeObj.width ; col++) {
            switch (mazeObj.maze[col][row]) {
            case (START):
                output << "S";
                break;
            case (END):
                output << "E";
                break;
            case (OPEN):
                output << " ";
                break;
            case (WALL):
                output << "█";
                break;
            case (ITEM):
                output << "I";
                break;
            case (SOLUTION):
                output << "□";
                break;
            default:
                output << "?";
                break;
            }
        }
        output << "\n";
    }
    return output;
}

