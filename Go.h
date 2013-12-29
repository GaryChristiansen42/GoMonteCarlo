#include <sstream>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <vector>

#include <assert.h>

#include <boost/foreach.hpp>

using namespace std;

#define foreach BOOST_FOREACH

#define BOARD_SIZE 9

enum GameResult
{
	None = -1,
	Draw = 0,
	WhiteWon = 1,
	BlackWon = 2
};

enum Player
{
	Empty = 0,
	White = 1,
	Black = 2
};

//(BOARD_SIZE, BOARD_SIZE) is pass
struct Point
{
	int row;
	int column;

	Point(){ }
	Point(int row, int column)
	{
		this->row = row;
		this->column = column;
	}
	
	bool operator==(const Point &p)
	{
		if (this->row == p.row
				&& this->column == p.column)
			return true;
		return false;
	}
};

class Group
{
public:
	std::vector<Point*> stones;
	Player color;
	
	Group(Player color)
	{
		this->color = color;
		stones.clear();
	}
	
	~Group()
	{
		foreach(Point* stone, stones)
			delete stone;
	}
	
	void addStone(Point* p)
	{
		stones.push_back(p);
	}
	
	bool isAdjacent(Point* p)
	{
		foreach(Point* stone, stones)
		{
			if(stone->row == p->row+1 && stone->column == p->column)
				return true;
			if(stone->row == p->row-1 && stone->column == p->column)
				return true;
			if(stone->row == p->row && stone->column == p->column+1)
				return true;
			if(stone->row == p->row && stone->column == p->column-1)
				return true;
		}
		return false;
	}
	
	bool hasLiberties(int** positions)
	{
		foreach(Point* stone, stones)
		{	
			//free above
			if(stone->column > 0 && positions[stone->row][stone->column-1] == Empty)
				return true;
			//free below
			if(stone->column < BOARD_SIZE-1 && positions[stone->row][stone->column+1] == Empty)
				return true;
			//free to the left
			if(stone->row > 0 && positions[stone->row-1][stone->column] == Empty)
				return true;
			if(stone->row < BOARD_SIZE-1 && positions[stone->row+1][stone->column] == Empty)
				return true;
		}
		return false;
	}
};

void printGroup(Group* g)
{
	foreach(Point* stone, g->stones)
	{
		cout << "r=" << stone->row << " c=" << stone->column << endl;
	}
}
	
class Board
{
public:
	int** positions;
	std::vector<Group*> blackGroups;
	std::vector<Group*> whiteGroups;
	Player turn;
	Player computer;
	
	Point lastMove;
	Point secondLastMove;

	~Board()
	{
		for(int r = 0; r < BOARD_SIZE; r++)
			delete[] positions[r];
		delete[] positions;
		
		foreach(Group* g, blackGroups)
			delete g;
		foreach(Group* g, whiteGroups)
			delete g;
	}

	void initialize(Player computer)
	{
		positions = new int*[BOARD_SIZE];
		for(int r = 0; r < BOARD_SIZE; r++)
		{
			positions[r] = new int[BOARD_SIZE];
			memset(positions[r], 0, BOARD_SIZE*sizeof(int));
		}

		turn = Black;
		this->computer = computer;
		
		lastMove = Point(-1, -1);
		secondLastMove = Point(-1, -1);
	}

	Board* clone()
	{
		Board* b = new Board();
		b->positions = new int*[BOARD_SIZE];
		for(int r = 0; r < BOARD_SIZE; r++)
		{
			b->positions[r] = new int[BOARD_SIZE];
			memcpy(b->positions[r], positions[r], BOARD_SIZE*sizeof(int));
		}

		b->turn = turn;
		b->computer = computer;
		
		b->lastMove = lastMove;
		b->secondLastMove = secondLastMove;
		return b;
	}

	bool isValidMove(Point move)
	{
		Point pass(BOARD_SIZE, BOARD_SIZE);
		if(move == pass) //passes are valid
			return true;
		else if(move.column >= BOARD_SIZE || move.row >= BOARD_SIZE || move.row < 0 || move.column < 0)
			return false;
		else if(positions[move.row][move.column] != Empty)
			return false;
		return true;
	}

	//Fuego
	void getSimpleScore(float* whiteScore, float* blackScore)
	{
		for(int row = 0; row < BOARD_SIZE; row++)
		{
			for(int column = 0; column < BOARD_SIZE; column++)
			{
				if(positions[row][column] == White)
				{
					*whiteScore = *whiteScore + 1;
				}
				else if(positions[row][column] == Black)
				{
					*blackScore = *blackScore + 1;
				}
			}
		}
		//printf("%f %f\n", *whiteScore, *blackScore);
	}
	
	bool isGameOver(GameResult *result)
	{
		Point pass(BOARD_SIZE, BOARD_SIZE);
		if(lastMove == pass && secondLastMove == pass)
		{
			float whiteScore;
			float blackScore;
			getSimpleScore(&whiteScore, &blackScore);
			if(whiteScore > blackScore)
				*result = WhiteWon;
			else if(blackScore > whiteScore)
				*result = BlackWon;
			else
				*result = Draw;
			return true;
		}
		return false;
	}
	
	void updateStructures(Point move)
	{
		vector<Group*>* groupsToSearch = (turn == Black) ? &blackGroups : &whiteGroups;
		Group* updatedGroup = NULL;
		foreach(Group* group, *groupsToSearch)
		{
			//if adjacent to structure of same color, then join that structure
			if(group->isAdjacent(&move))
			{
				group->addStone(new Point(move.row, move.column));
				updatedGroup = group;
				break;
			}
		}
		if(!updatedGroup)
		{
			Group* group = new Group((turn == Black) ? Black : White);
			group->addStone(new Point(move.row, move.column));
			groupsToSearch->push_back(group);
			//updatedGroup = group;
		}
		
		if(updatedGroup)
		{
			//Find groups adjacent to updated group
			vector<Group*> adjacentGroups;
			foreach(Group* group, *groupsToSearch)
			{
				if(group == updatedGroup)
					continue;
				foreach(Point* stone, updatedGroup->stones)
				{
					if(group->isAdjacent(stone))
					{	
						bool inAdjacent = false;
						foreach(Group* adjacentGroup, adjacentGroups)
						{
							if(group == adjacentGroup)
							{
								inAdjacent = true;
								break;
							}
						}
						if(!inAdjacent)
							adjacentGroups.push_back(group);
					}
				}
			}
		
			//combines adjacent groups
			foreach(Group* adjacentGroup, adjacentGroups)
			{
				if(adjacentGroup == updatedGroup)
					continue;
				while(adjacentGroup->stones.size() > 0)
				{
					updatedGroup->stones.push_back(adjacentGroup->stones.front());
					adjacentGroup->stones.erase(adjacentGroup->stones.begin());
				}
			}	
			foreach(Group* adjacentGroup, adjacentGroups)
			{
				for(vector<Group*>::iterator it = groupsToSearch->begin(); it != groupsToSearch->end(); ++it)//int j = 0; j < groupsToSearch->size(); j++)
				{
					if((*it) == adjacentGroup)
					{
						groupsToSearch->erase(it);
						break;
					}
				}
				delete adjacentGroup;
			}
		}
	}

	void removeDeadStones(Player color)
	{
		vector<Group*>* firstGroups = (color == Black) ? &blackGroups : &whiteGroups;
		//vector<Group*> secondGroup = (turn == Black) ? blackGroups : whiteGroups;
		
		vector<Group*> deadGroups;
		foreach(Group* group, *firstGroups)
			if(!(group->hasLiberties(positions)))
				deadGroups.push_back(group);
				
		//beg1:
		foreach(Group* deadGroup, deadGroups)
		{
			//remove dead stones on board
			foreach(Point* stone, deadGroup->stones)
				positions[stone->row][stone->column] = Empty;
				
			for(vector<Group*>::iterator groupIterator = firstGroups->begin(); groupIterator != firstGroups->end(); ++groupIterator)
			{
				if(deadGroup == (*groupIterator))
				{
					firstGroups->erase(groupIterator);
					break;
				}
			}
			delete deadGroup;
			//deadGroups.erase(it);
			//goto beg1;
		}
		
		/*deadGroups.clear();
		for(vector<Group*>::iterator it = secondGroup.begin(); it != secondGroup.end(); ++it)
		{
			if(!(*it)->hasLiberties(positions))
			{
				deadGroups.push_back((*it));
			}
		}
		
		beg:
		for(vector<Group*>::iterator it = deadGroups.begin(); it != deadGroups.end(); ++it)
		{
			for(int i = 0; i < (*it)->stones.size(); i++)
			{
				Point* p = (*it)->stones.at(i);
				positions[p->row][p->column] = Empty;
			}
			for(vector<Group*>::iterator it2 = secondGroup.begin(); it2 != secondGroup.end(); ++it2)
			{
				if((*it) == (*it2))
				{
					secondGroup.erase(it2);
					break;
				}
			}
			delete (*it);
			goto beg;
		}*/

		/*std::vector<Point*> whiteStones;
		std::vector<Point*> blackStones;
		//find all stones
		for(int row = 0; row < BOARD_SIZE; row++)
		{
			for(int column = 0; column < BOARD_SIZE; column++)
			{
				if(positions[row][column] == White)
					whiteStones.push_back(new Point(row, column));
				else if(positions[row][column] == Black)
					blackStones.push_back(new Point(row, column));
			}
		}
		
		//find all structures
		for(int i = 0; i < whiteStones.size(); i++)
		{
			bool inStructure = false;
			for(int j = 0; j < structures.size(); j++)
			{
				Structure structure = structures.at(j);
				//if adjacent to structure of same color, then join that structure
				if(structure.color != White)
					continue;
				if(structure.isAdjacent(whiteStones.at(i)))
				{
					structure.addStone(whiteStones.at(i));
				}
			}
			if(!inStructure)
			{
				Structure structure(positions, White);
				structure.addStone(whiteStones.at(i));
				structures.push_back(structure);
			}
		}*/
		
		
		//cout << "wS= " << whiteStones.size() << " bS= " << blackStones.size() << "\n";
		
	}
	
	void makeMove(Point move)
	{
		//Don't place stone for passes
		Point pass(BOARD_SIZE, BOARD_SIZE);
		if(!(move == pass))
		{
			if(positions[move.row][move.column] != Empty)
			{
				cout << "Illegal Move\nRow: " << move.row << "\nColumn: " << move.column << endl;
				assert(false);
			}
			positions[move.row][move.column] = turn;

			updateStructures(move);
			removeDeadStones((turn == Black ? White : Black));
			removeDeadStones((turn == Black ? Black : White));
		}
		
		//cout << whiteGroups.size() << " " << blackGroups.size() << "\n";
		
		turn = (turn == Black ? White : Black);
		
		secondLastMove = lastMove;
		lastMove = move;
	}

	void makeRandomMove()
	{
		Point move(-1, -1);
		do
		{
			//+1 includes passing
			move.row = rand() % BOARD_SIZE+1;
			move.column = rand() % BOARD_SIZE+1;
		} while(!isValidMove(move));

		makeMove(move);
	}

	GameResult playRandomGame()
	{
		GameResult result;
		while(!isGameOver(&result))
		{
			makeRandomMove();
		}
		return result;
	}

	void show()
	{
		stringstream boardString;
	
		for(int row = 0; row < BOARD_SIZE+1; row++)
		{
			for(int column = 0; column < BOARD_SIZE+1; column++)
			{
				if(row == BOARD_SIZE && column == BOARD_SIZE)
				{
					boardString << "  ";
				}
				else if(row == BOARD_SIZE)
					boardString << column << " ";
				else if(column == BOARD_SIZE)
					boardString << row << " ";	
				else
				{
					switch(positions[row][column])
					{
					case 0:
						boardString << "- ";
						break;
					case White:
						boardString << "W ";
						break;
					case Black:
						boardString << "B ";
						break;
					}
				}
			}
		
			boardString << "\n";
		}
		boardString << "\n";
		Point pass(BOARD_SIZE, BOARD_SIZE);
		if(lastMove == pass)
			boardString << (((int)turn == Black) ? "Black" : "White") << " passed\n";
		cout << boardString.str();
	}
};
