#include "../Board.h"
#include "../Patterns.h"

#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include <iostream>

BOOST_AUTO_TEST_CASE( testBoard )
{
  Board b1;
  b1.init();
  
  Board b2;
  b2.init();

  for(int i = 0; i < BOARD_SIZE; i++)
    for(int j = 0; j < BOARD_SIZE; j++)
      BOOST_CHECK_EQUAL(b2.positions[i][j].color, Empty);
  
  //Valid moves 
  for(int i = 0; i < BOARD_SIZE; i++)
  {
    for(int j = 0; j < BOARD_SIZE; j++)
    {
      Point validMove(i, j);
      BOOST_CHECK_EQUAL(b1.isValidMove(validMove), true);
    }
  }
  
  //Pass
  Point pass(BOARD_SIZE, BOARD_SIZE);
  BOOST_CHECK_EQUAL(b1.isValidMove(pass), true);
  
  //Invalid moves
  Point invalidMove = Point(BOARD_SIZE+1, BOARD_SIZE);
  BOOST_CHECK_EQUAL(b1.isValidMove(invalidMove), false);
  
  invalidMove = Point(BOARD_SIZE, BOARD_SIZE+1);
  BOOST_CHECK_EQUAL(b1.isValidMove(invalidMove), false);
  
  invalidMove = Point(0, BOARD_SIZE);
  BOOST_CHECK_EQUAL(b1.isValidMove(invalidMove), false);
  
  invalidMove = Point(BOARD_SIZE, 0);
  BOOST_CHECK_EQUAL(b1.isValidMove(invalidMove), false);
  
  invalidMove = Point(0, -1);
  BOOST_CHECK_EQUAL(b1.isValidMove(invalidMove), false);
  
  invalidMove = Point(-1, 0);
  BOOST_CHECK_EQUAL(b1.isValidMove(invalidMove), false);
  
  invalidMove = Point(-1, -1);
  BOOST_CHECK_EQUAL(b1.isValidMove(invalidMove), false);
  
}

BOOST_AUTO_TEST_CASE( testBoardHash )
{
  Board b1;
  b1.init();
  
  Board b2;
  b2.init();

  BOOST_CHECK_EQUAL(b1.getHash(), b2.getHash());
  
  b1.makeMove(Point(0, 0));
  BOOST_CHECK_EQUAL(b1.getHash() != b2.getHash(), true);

  b2.makeMove(Point(0, 0));
  BOOST_CHECK_EQUAL(b1.getHash(), b2.getHash());
}


BOOST_AUTO_TEST_CASE( testCornerCapture )
{
  Board b1;
  b1.init();

  b1.makeMove(Point(0,0)); //Black
  b1.makeMove(Point(1,0)); //White
  b1.makeMove(Point(2,0)); //Black
  b1.makeMove(Point(0,1)); //White //Capture 0,0
  b1.makeMove(Point(0,2)); //Black
  b1.makeMove(Point(8,0)); //White
  b1.makeMove(Point(1,1)); //Black
  b1.makeMove(Point(8,1)); //White
  b1.makeMove(Point(0,0)); //Black //Capture 1,0 0,1
  
  BOOST_CHECK_EQUAL(b1.positions[0][0].color, Black);
  BOOST_CHECK_EQUAL(b1.positions[1][0].color, Empty);
  BOOST_CHECK_EQUAL(b1.positions[0][1].color, Empty);
}

BOOST_AUTO_TEST_CASE( testBoardKo )
{
  Board b1;
  b1.init();

  b1.makeMove(Point(0,0)); // Black
  b1.makeMove(Point(1,0)); // White
  b1.makeMove(Point(2,0)); // Black
  b1.makeMove(Point(0,1)); // White // Capture 0,0
  b1.makeMove(Point(1,1)); // Black
  b1.makeMove(Point(4,1)); // White
  b1.makeMove(Point(0,0)); // Black // Capture 1,0
  BOOST_CHECK_EQUAL(b1.isValidMove(Point(1,0)), false);

  Board b2;
  b2.init();
  b2.makeMove(Point(1, 0)); // Black
  b2.makeMove(Point(0, 0)); // White
  b2.makeMove(Point(1, 1)); // Black
  b2.makeMove(Point(0, 1)); // White
  b2.makeMove(Point(2, 1)); // Black
  b2.makeMove(Point(0, 2)); // White
  b2.makeMove(Point(2, 3)); // Black
  b2.makeMove(Point(1, 3)); // White
  b2.makeMove(Point(2, 4)); // Black
  b2.makeMove(Point(1, 4)); // White
  b2.makeMove(Point(3, 2)); // Black
  b2.makeMove(Point(2, 0)); // White
  b2.makeMove(Point(3, 3)); // Black
  b2.makeMove(Point(3, 0)); // White
  b2.makeMove(Point(4, 2)); // Black
  b2.makeMove(Point(3, 1)); // White
  b2.makeMove(Point(4, 4)); // Black
  /*
  WWW--
  BB-WW
  WB-BB
  WWBB-
  --B-B
  */

  b2.makeMove(Point(2, 2)); // White
  /*
  WWW--
  BB-WW
  WBWBB
  WWBB-
  --B-B
  */
  b2.makeMove(Point(1, 2)); // Black
  /*
  WWW--
  BBBWW
  WB-BB
  WWBB-
  --B-B
  */
  b2.makeMove(Point(2, 2)); // White
  /*
  WWW--
  ---WW
  W-WBB
  WWBB-
  --B-B
  */

/*
  Board b3;
  b3.init();

  b3.makeMove(Point(3, 5)); // Black
  b3.makeMove(Point(BOARD_SIZE, BOARD_SIZE)); // White

  b3.makeMove(Point(5, 4)); // Black
  b3.makeMove(Point(BOARD_SIZE, BOARD_SIZE)); // White

  b3.makeMove(Point(4, 2)); // Black
  b3.makeMove(Point(1, 2)); // White

  b3.makeMove(Point(4, 4)); // Black
  b3.makeMove(Point(2, 3)); // White

  b3.makeMove(Point(2, 4)); // Black
  b3.makeMove(Point(2, 2)); // White

  b3.makeMove(Point(0, 2)); // Black
  b3.makeMove(Point(3, 1)); // White

  b3.makeMove(Point(0, 4)); // Black
  b3.makeMove(Point(0, 3)); // White

  b3.makeMove(Point(3, 3)); // Black
  b3.makeMove(Point(0, 1)); // White

  b3.makeMove(Point(1, 3)); // Black
  b3.makeMove(Point(3, 2)); // White
 
  b3.makeMove(Point(1, 4)); // Black
  b3.makeMove(Point(3, 4)); // White

  b3.makeMove(Point(2, 4)); // Black*/
}

BOOST_AUTO_TEST_CASE( testBoardPositionalSuperKo )
{
  Board b1;
  b1.init();

  std::list<unsigned long int> previousHashes;
  previousHashes.push_back(b1.getHash());

  b1.makeMove(Point(0,1)); // Black
  previousHashes.push_back(b1.getHash());
  
  b1.makeMove(Point(1,0)); // White
  previousHashes.push_back(b1.getHash());
  
  b1.makeMove(Point(1,1)); // Black
  previousHashes.push_back(b1.getHash());
  
  b1.makeMove(Point(2,1)); // White
  previousHashes.push_back(b1.getHash());
  
  b1.makeMove(Point(1,2)); // Black
  previousHashes.push_back(b1.getHash());
  
  b1.makeMove(Point(3,0)); // White
  previousHashes.push_back(b1.getHash());
  
  b1.makeMove(Point(0,2)); // Black
  previousHashes.push_back(b1.getHash());

  b1.makeMove(Point(0,0)); // Black
  previousHashes.push_back(b1.getHash());

  b1.makeMove(Point(2,0)); // White
  previousHashes.push_back(b1.getHash());

  b1.eliminatePositionalSuperKo(previousHashes);
  BOOST_CHECK_EQUAL(b1.isValidMove(Point(1,0)), false);


  // Shouldn't fail here
  Board b2;
  b2.init();

  previousHashes.clear();
  previousHashes.push_back(b2.getHash());
  b2.eliminatePositionalSuperKo(previousHashes);

  b2.makeMove(Point(2,2)); // Black
  previousHashes.push_back(b2.getHash());
  b2.eliminatePositionalSuperKo(previousHashes);

  b2.makeMove(Point(2,3)); // White
  previousHashes.push_back(b2.getHash());
  b2.eliminatePositionalSuperKo(previousHashes);

  b2.makeMove(Point(1,3)); // Black
  previousHashes.push_back(b2.getHash());
  b2.eliminatePositionalSuperKo(previousHashes);

  b2.makeMove(Point(1,2)); // White
  previousHashes.push_back(b2.getHash());
  b2.eliminatePositionalSuperKo(previousHashes);

  b2.makeMove(Point(3,3)); // Black
  previousHashes.push_back(b2.getHash());
  b2.eliminatePositionalSuperKo(previousHashes);

  b2.makeMove(Point(2,1)); // White
  previousHashes.push_back(b2.getHash());
  b2.eliminatePositionalSuperKo(previousHashes);

  b2.makeMove(Point(3,1)); // Black
  previousHashes.push_back(b2.getHash());
  b2.eliminatePositionalSuperKo(previousHashes);

  b2.makeMove(Point(1,1)); // White
  previousHashes.push_back(b2.getHash());
  b2.eliminatePositionalSuperKo(previousHashes);

  b2.makeMove(Point(3,2)); // Black
  previousHashes.push_back(b2.getHash());
  b2.eliminatePositionalSuperKo(previousHashes);

  b2.makeMove(Point(3,0)); // White
  previousHashes.push_back(b2.getHash());
  b2.eliminatePositionalSuperKo(previousHashes);

  b2.makeMove(Point(0,0)); // Black
  previousHashes.push_back(b2.getHash());
  b2.eliminatePositionalSuperKo(previousHashes);

  b2.makeMove(Point(2,4)); // White
  previousHashes.push_back(b2.getHash());
  b2.eliminatePositionalSuperKo(previousHashes);

  b2.makeMove(Point(1,4)); // Black
  previousHashes.push_back(b2.getHash());

  /*printf("before\n");
  for (int i = 0; i < b2.numLegalMoves; i++)
    printf("%d %d %d\n", b2.legalMoves[i]->row, b2.legalMoves[i]->column, b2.legalMoves[i]->legal);
  b2.eliminatePositionalSuperKo(previousHashes);
  printf("after\n");
  for (int i = 0; i < b2.numLegalMoves; i++)
    printf("%d %d %d\n", b2.legalMoves[i]->row, b2.legalMoves[i]->column, b2.legalMoves[i]->legal);
  BOOST_CHECK_EQUAL(b2.isValidMove(Point(3,4)), true);

  b2.show();*/
}

BOOST_AUTO_TEST_CASE( testIsSuicide )
{
  Board b1;
  b1.init();
    
  //Black
  BOOST_CHECK_EQUAL(b1.isSuicide(Point(0,0), Black, White), false);
  b1.makeMove(Point(0,1)); 
    
  //White
  BOOST_CHECK_EQUAL(b1.isSuicide(Point(0,0), White, Black), false);
  b1.makeMove(Point(0,2));
    
  //Black
  BOOST_CHECK_EQUAL(b1.isSuicide(Point(0,0), Black, White), false);
  b1.makeMove(Point(1,0));
    
  //White
  BOOST_CHECK_EQUAL(b1.isSuicide(Point(0,0), White, Black), true);
  b1.makeMove(Point(1,1));
    
  //Black
  BOOST_CHECK_EQUAL(b1.isSuicide(Point(0,0), Black, White), false);
  b1.makeMove(Point(4,4));
    
  //White
  BOOST_CHECK_EQUAL(b1.isSuicide(Point(0,0), White, Black), false);
  b1.makeMove(Point(2,0));
    
  //Black
  BOOST_CHECK_EQUAL(b1.isSuicide(Point(0,0), Black, White), true);
  b1.makeMove(Point(3,4));
    
  //White
  BOOST_CHECK_EQUAL(b1.isSuicide(Point(0,0), White, Black), false);
  b1.makeMove(Point(0,0));
    
  //Black
  BOOST_CHECK_EQUAL(b1.isSuicide(Point(1,0), Black, White), true);
  BOOST_CHECK_EQUAL(b1.isSuicide(Point(0,1), Black, White), true);
    
  /*  
    W _ W _ _
    _ W _ _ _
    W _ _ _ _
    _ _ _ _ B
    _ _ _ _ B
  */
    
  Board b2;
  b2.init();
    
  b2.makeMove(Point(0,0)); //Black
  b2.makeMove(Point(2,1)); //White
    
  b2.makeMove(Point(0,1)); //Black
  b2.makeMove(Point(2,2)); //White
    
  b2.makeMove(Point(0,3)); //Black
  b2.makeMove(Point(2,4)); //White
   
  b2.makeMove(Point(0,4)); //Black
  b2.makeMove(Point(3,0)); //White
  
  b2.makeMove(Point(1,0)); //Black
  b2.makeMove(Point(3,1)); //White
    
  b2.makeMove(Point(1,1)); //Black
  b2.makeMove(Point(3,2)); //White
    
  b2.makeMove(Point(1,2)); //Black
  b2.makeMove(Point(3,3)); //White
    
  b2.makeMove(Point(1,3)); //Black
  b2.makeMove(Point(3,4)); //White
    
  b2.makeMove(Point(1,4)); //Black
  b2.makeMove(Point(4,0)); //White
  
  b2.makeMove(Point(2,0)); //Black
  b2.makeMove(Point(4,2)); //White
    
  b2.makeMove(Point(2,3)); //Black
  b2.makeMove(Point(4,4)); //White
    
  b2.makeMove(Point(5,5)); //Black
  
  /*
    B B _ B B
    B B B B B
    B W W B W
    W W W W W
    W _ W _ W
  */

  b2.makeMove(Point(0, 5)); // White
  b2.makeMove(Point(BOARD_SIZE, BOARD_SIZE)); // Black

  b2.makeMove(Point(1, 5)); // White
  b2.makeMove(Point(BOARD_SIZE, BOARD_SIZE)); // Black

  b2.makeMove(Point(2, 5)); // White
  b2.makeMove(Point(BOARD_SIZE, BOARD_SIZE)); // Black

  //White
  BOOST_CHECK_EQUAL(b2.isSuicide(Point(0,2), White, Black), false);
  
  b2.makeMove(Point(0,2)); //White
  /* 11 captured, 13 territory, 12 stones
    _ _ W _ _
    _ _ _ _ _
    _ W W _ W
    W W W W W
    W _ W _ W
  */
  
  Board* b2Clone = b2.clone();
  float score = b2Clone->getTaylorScore(0);
  BOOST_CHECK_EQUAL(score, -25);
}

BOOST_AUTO_TEST_CASE( testGroup )
{
  Board b1;
  b1.init();

  b1.makeMove(Point(0,0)); //Black
  b1.makeMove(Point(1,0)); //White
  b1.makeMove(Point(2,0)); //Black
  b1.makeMove(Point(0,1)); //White //Capture 0,0
  b1.makeMove(Point(0,2)); //Black
  b1.makeMove(Point(8,0)); //White
  b1.makeMove(Point(1,1)); //Black
  b1.makeMove(Point(8,1)); //White
  b1.makeMove(Point(0,0)); //Black //Capture 1,0 0,1
  b1.makeMove(Point(8,2)); //White
  
  BOOST_CHECK_EQUAL(b1.positions[0][0].color, Black);
  BOOST_CHECK_EQUAL(b1.positions[1][0].color, Empty);
  BOOST_CHECK_EQUAL(b1.positions[0][1].color, Empty);
  
  BOOST_CHECK_EQUAL(b1.whiteGroups.size(), 1);
  BOOST_CHECK_EQUAL(b1.blackGroups.size(), 4);
  
  b1.makeMove(Point(1,0)); //Black connect 0,0 2,0 1,1
  
  BOOST_CHECK_EQUAL(b1.whiteGroups.size(), 1);
  BOOST_CHECK_EQUAL(b1.blackGroups.size(), 2);
  
  Point adjacent(0,1);
  BOOST_CHECK_EQUAL(b1.blackGroups.size(), 2);
  
  /*b1.show();
  
  foreach(Group* g, b1.whiteGroups)
  {
    cout << "Group\n";
    foreach(Point* p, g->stones)
    {
      cout << p->row << " " << p->column << endl;
    }
  }*/
}

bool testAdjacent(int stoneX, int stoneY, int adjStoneX, int adjStoneY)
{
  Board b1;
  b1.init();
  b1.makeMove(Point(stoneX,stoneY));
  Group* g = *b1.blackGroups.begin();
  Point p = Point(adjStoneX,adjStoneY);
  Point *p2 = b1.getPoint(&p);
  return g->isAdjacent(p2);
}

BOOST_AUTO_TEST_CASE( testGroupAdjacent )
{
  for(int row = 0; row < BOARD_SIZE; row++)
  {
    for(int column = 0; column < BOARD_SIZE; column++)
    {
      //above
      if(row > 0)
        BOOST_CHECK_EQUAL(testAdjacent(row, column, row-1, column), true);
      //below
      if(row < BOARD_SIZE-1)
        BOOST_CHECK_EQUAL(testAdjacent(row, column, row+1, column), true);
      //left
      if(column > 0)
        BOOST_CHECK_EQUAL(testAdjacent(row, column, row, column-1), true);
      //above
      if(column < BOARD_SIZE-1)
        BOOST_CHECK_EQUAL(testAdjacent(row, column, row, column+1), true);
      
      //pass
      BOOST_CHECK_EQUAL(testAdjacent(row, column, BOARD_SIZE, BOARD_SIZE), false);
    }
  }
}

BOOST_AUTO_TEST_CASE( testGroupNumLiberties )
{
  Board b1;
  b1.init();
  b1.makeMove(Point(0, 0)); // Black
  Group* g = *b1.blackGroups.begin();
    
  BOOST_CHECK_EQUAL(g->numLiberties(), 2);
    
  b1.makeMove(Point(0, 1)); //White
  BOOST_CHECK_EQUAL(g->numLiberties(), 1);
  
  b1.makeMove(Point(1, 0)); //Black
  BOOST_CHECK_EQUAL(g->numLiberties(), 2);
    
  b1.makeMove(Point(1, 1)); //White
  BOOST_CHECK_EQUAL(g->numLiberties(), 1);
    
  b1.makeMove(Point(2, 0)); //Black
  BOOST_CHECK_EQUAL(g->numLiberties(), 2);
}

bool testHasLiberties(int stoneX, int stoneY, int adjStoneX, int adjStoneY)
{
  Board b1;
  b1.init();
  b1.makeMove(Point(stoneX,stoneY));
  Group* g = *b1.blackGroups.begin();
  Point p = Point(adjStoneX,adjStoneY);
  Point *p2 = b1.getPoint(&p);
  return g->isAdjacent(p2);
}

BOOST_AUTO_TEST_CASE( testGroupHasLiberties )
{
  for(int row = 0; row < BOARD_SIZE; row++)
  {
    for(int column = 0; column < BOARD_SIZE; column++)
    {
      //above
      if(row > 0)
        BOOST_CHECK_EQUAL(testAdjacent(row, column, row-1, column), true);
      //below
      if(row < BOARD_SIZE-1)
        BOOST_CHECK_EQUAL(testAdjacent(row, column, row+1, column), true);
      //left
      if(column > 0)
        BOOST_CHECK_EQUAL(testAdjacent(row, column, row, column-1), true);
      //above
      if(column < BOARD_SIZE-1)
        BOOST_CHECK_EQUAL(testAdjacent(row, column, row, column+1), true);
      
      //pass
      BOOST_CHECK_EQUAL(testAdjacent(row, column, BOARD_SIZE, BOARD_SIZE), false);
    }
  }
}


BOOST_AUTO_TEST_CASE( testBoardClone )
{
  Board* b1 = new Board();
  b1->init();
  b1->makeMove(Point(1,1));

  BOOST_CHECK_EQUAL(b1->blackGroups.size(), 1);
  BOOST_CHECK_EQUAL(b1->whiteGroups.size(), 0);

  Board* bClone = b1->clone();
  BOOST_CHECK_EQUAL(bClone->blackGroups.size(), 1);
  BOOST_CHECK_EQUAL(bClone->whiteGroups.size(), 0);
  

  delete bClone;
  delete b1;
}

BOOST_AUTO_TEST_CASE( testBoardTaylorScore )
{
  Board* b1 = new Board();
  b1->init();
  for(int row = 0; row < BOARD_SIZE-1; row++)
  {
    for(int column = 0; column < BOARD_SIZE; column++)
    {
      b1->makeMove(Point(row, column));
      b1->makeMove(Point(BOARD_SIZE, BOARD_SIZE));
    }
  }
  
  Board* b1Clone = b1->clone();
  float score = b1Clone->getTaylorScore(4.5);
  BOOST_CHECK_EQUAL(score, BOARD_SIZE*BOARD_SIZE-4.5);
  
  Board* b1Clone2 = b1->clone();
  score = b1Clone2->getTaylorScore(0.5);
  BOOST_CHECK_EQUAL(score, BOARD_SIZE*BOARD_SIZE-0.5);
}

BOOST_AUTO_TEST_CASE( testPattern )
{
  Pattern p;

  std::string patternsFile("testPatterns.pat");
  std::ifstream in;
  in.open(patternsFile);
  BOOST_CHECK_EQUAL(in.is_open(), true);
  in >> p;

  BOOST_CHECK_EQUAL(p.hash, "____W____");
  BOOST_CHECK_EQUAL(p.goodMoves.size(), 1);
  BOOST_CHECK_EQUAL(p.goodMoves[0].first, -1);
  BOOST_CHECK_EQUAL(p.goodMoves[0].second, -1);


  Board b1;
  b1.init();
  b1.makeMove(Point(1, 1));
  p = Pattern(&b1.positions[1][1]);
  BOOST_CHECK_EQUAL(p.hash, "____B____");

  p = Pattern(&b1.positions[0][0]);
  BOOST_CHECK_EQUAL(p.hash, "OOOO__O_B");


  in >> p;
  BOOST_CHECK_EQUAL(p.hash, "OOOOBWO__");
  BOOST_CHECK_EQUAL(p.goodMoves[0].first, 1);
  BOOST_CHECK_EQUAL(p.goodMoves[0].second, 0);

  // invertColors
  p.invertColor();
  BOOST_CHECK_EQUAL(p.hash, "OOOOWBO__");
  BOOST_CHECK_EQUAL(p.goodMoves[0].first, 1);
  BOOST_CHECK_EQUAL(p.goodMoves[0].second, 0);

  // rotate90
  p.rotate90();
  BOOST_CHECK_EQUAL(p.hash, "OOO_WO_BO");
  BOOST_CHECK_EQUAL(p.goodMoves[0].first, 0);
  BOOST_CHECK_EQUAL(p.goodMoves[0].second, -1);
}

BOOST_AUTO_TEST_CASE( testPatterns )
{
  Board b1;
  b1.init();
  b1.makeMove(Point(0, 1));
  b1.makeMove(Point(0, 0));

  std::string patternFile("testPatterns.pat");
  Patterns patterns;
  patterns.init(patternFile);

  BOOST_CHECK_EQUAL(patterns.initialized, true);

  BOOST_CHECK_EQUAL(patterns.hashTable.size(), 10);

  Point* move = patterns.getMove(&b1);
  BOOST_CHECK_EQUAL(*move == Point(1, 0), true);
}
