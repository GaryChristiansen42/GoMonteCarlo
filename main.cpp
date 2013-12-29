#include "UCT.h"
#include <time.h>
#include <fstream>

Node *root;

void playerMove(Node** currentNode, Board* b)
{
	bool validInput = false;
	Point choice(-1, -1);
	while(!validInput)
	{
	
		cout << "Row: ";
		string input;
		cin >> input;
		choice.row = atoi(input.c_str());
		
		cout << "Column: ";
		cin >> input;
		choice.column = atoi(input.c_str());
		
		cout << "\n";
		
		if(b->isValidMove(choice))
			validInput = true;
		else
			cout << "That position is not valid\n"; 
	}	
	
	cout << "Row: " << choice.row << "\nColumn: " << choice.column << endl;
	
	b->makeMove(choice);
	Board* bCopy = b->clone();
	Node* newCurrentNode = new Node(choice, bCopy, (*currentNode));
	(*currentNode)->addChild(newCurrentNode);
	(*currentNode) = newCurrentNode;
}

void printTabs(ofstream* file, int numTabs)
{
	for(int i = 0; i < numTabs; i++)
		(*file) << "\t";
}

void showTree(ofstream* file, Node* root, int numTabs)
{
	if(root == NULL)
		return;
	printTabs(file, numTabs);
	(*file) << "Node\n";
	printTabs(file, numTabs);
	(*file) << "Move: " << root->move.row << " " << root->move.column << endl;
	printTabs(file, numTabs);
	//(*file) << "Wins: " << root->wins << " Losses: " << root->losses << " Visits: " << root->visits << endl;
	printTabs(file, numTabs);
	(*file) << "Children: \n";
	showTree(file, root->child, numTabs+1);
	showTree(file, root->sibling, numTabs);
}

void computerMove(Node** currentNode, Board* b)
{
	Node* newCurrentNode = UCTSearch((*currentNode), b);
	(*currentNode) = newCurrentNode;
	b->makeMove((*currentNode)->move);
	cout << "Row: " << (*currentNode)->move.row << "\nColumn: " << (*currentNode)->move.column << endl;

	/*ofstream myfile;
	myfile.open ("tree.txt");
	showTree(&myfile, root, 0);
	myfile.close();*/
}

int main(int in, char* args[])
{
	cout << "TicTacToe MonteCarloAI\n";

	srand ((unsigned int)time(NULL));
	
	Board* b = new Board();
	b->initialize(Black);
	
	Node *currentNode;
	Point m(-1, -1);
	root = new Node(m, b->clone(), NULL);
	currentNode = root;
	
	b->show();

	GameResult result = None;
	while(!b->isGameOver(&result))
	{
		if(b->turn == b->computer)
			computerMove(&currentNode, b);
		else
			computerMove(&currentNode, b);
			//playerMove(&currentNode, b);
		b->show();
	}
	
	if(result == Draw)
		cout << "Draw\n";
	else if((int)result == (int)Black)
		cout << "Black Won\n";
	else 
		cout << "White Won\n";
	
	float whiteScore = 0, blackScore = 0;
	b->getSimpleScore(&whiteScore, &blackScore);
	cout << "\nWhite: " << whiteScore << "\nBlack: " << blackScore << endl;
	
	delete root;
	delete b;
	return 0;
}
