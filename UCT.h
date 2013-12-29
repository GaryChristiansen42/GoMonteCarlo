#include "Go.h"

#include <math.h>
#include <vector>
#include <assert.h>
#include <stdio.h>

#if _MSC_VER
#define snprintf _snprintf
#endif

#define MAX_SIMULATIONS 10

//Implemented using http://www.cameronius.com/cv/mcts-survey-master.pdf

struct Node
{
	int visits;
	double totalRewards;
	Point move;
	
	Board* state;
	
	Node* bestNode;
	Node* child;
	Node* sibling;
	Node* parent;

	Node(Point move, Board* state, Node* parent)
	{
		visits = 0;
		totalRewards = 0.0;
		this->move = move;
		
		this->state = state;
		
		bestNode = NULL;
		child = NULL;
		sibling = NULL;
		this->parent = parent;
	}
	
	~Node()
	{
		if(state)
			delete state;
		if(child)
			delete child;
		if(sibling)
			delete sibling;
	}

	void addChild(Node* newChild)
	{
		if(child == NULL)
			child = newChild;
		else
		{
			Node* temp = child;
			while(temp->sibling != NULL)
				temp = temp->sibling;
			temp->sibling = newChild;
		}
	}
	
	bool isChild(Point move)
	{
		Node* next = child;
		while(next != NULL)
		{
			if(next->move.row == move.row && next->move.column == move.column)
				return true;
			next = next->sibling;
		}
		return false;
	}
};

void printNode(Node* n, char* spaces="")
{
	cout << spaces << "n (" << n->move.row << " " << n->move.column << ") V " << n->visits << " R " << n->totalRewards << endl;
	
	Node* next = n->child;
	while(next != NULL)
	{
		char moreSpaces[100];
		snprintf(moreSpaces, 100, "%s ", spaces);
		printNode(next, moreSpaces);
		next = next->sibling;
	}
}

Node* bestChild(Node* node)
{
	const double c = 1 / sqrt((float)2);
	//const double c = 1;
	Node* bestChild = node->child;
	double bestScore = -100000;
	Node* next = node->child;
	while(next != NULL)
	{
		double uctScore = next->totalRewards / next->visits 
			+ c*sqrt(
				(2*log((float)node->visits)) / next->visits
			);
		if(bestScore < uctScore)
		{
			bestChild = next; 
			bestScore = uctScore;
		}
		next = next->sibling;
	}
	return bestChild;
}

Node* getNewChild(Node* node)
{
	vector<Node*> possibleChildren;
	for(int r = 0; r < BOARD_SIZE; r++)
	{
		for(int c = 0; c < BOARD_SIZE; c++)
		{
			if(node->state->positions[r][c] == Empty)
			{
				Point move(r, c);
				Board* b = node->state->clone();
				b->makeMove(move);
				Node* child = new Node(move, b, node);
				if(!node->isChild(move))
					possibleChildren.push_back(child);
				else
					delete child;
			}
		}
	}
	
	//Add pass move to possible moves
	Point passMove(BOARD_SIZE, BOARD_SIZE);
	Board* passBoard = node->state->clone();
	passBoard->makeMove(passMove);
	Node* passChild = new Node(passMove, passBoard, node);
	possibleChildren.push_back(passChild);
	
	if(possibleChildren.size() == 0)
		return NULL;
	else //Pick random child
	{
		int choice = rand() % possibleChildren.size();
		Node* chosenChild = possibleChildren[choice];
		for(int i = 0; i < possibleChildren.size(); i++)
			if(i != choice)
				delete possibleChildren[i];
		return chosenChild;
	}
}

Node* TreePolicy(Node* node)
{
	GameResult r;
	while(!node->state->isGameOver(&r)) //Not terminal
	{
		Node* newChild = getNewChild(node);
		if(newChild != NULL)
		{
			node->addChild(newChild);
			return newChild;
		}
		else
		{
			node = bestChild(node);
		}
	}
	return node;
}

int DefaultPolicy(Node* node)
{
	Board* clone = node->state->clone();
	GameResult r = clone->playRandomGame();
	delete clone;
	if((node->state->turn == Black ? White : Black) == (int)r)
		return 1;
	else if(r != Draw)
		return -1;
	else if(r == Draw)
		return 0;
	else
		assert(false);
}

void backup(Node* v, int reward)
{
	//If not me, then its updating opponent
	bool me = true;
	while(v != NULL)
	{
		v->visits++;
		if(me)
			v->totalRewards += reward;
		else
			v->totalRewards -= reward;
		v = v->parent;
		me = !me;
	}
}

Node* UCTSearch(Node* root, Board* currentBoard)
{
	for(int i = 0; i < MAX_SIMULATIONS; i++)
	{
		Node* v = TreePolicy(root);
		int reward = DefaultPolicy(v);
		backup(v, reward);
		
		if(i % 1000 == 0)
			cout << i << endl;
	}
	Node* best = root->child;
	Node* next = root->child;
	while(next != NULL)
	{
		if(next->visits > best->visits)
			best = next;
		next = next->sibling;
	}
	
	return best;
}