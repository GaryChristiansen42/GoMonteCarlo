#include "../Board.h"
//#define BOOST_TEST_MODULE MyTest
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE( testBoard )
{
  int boardSize = 9;
  Board b1(boardSize);
  b1.init();
  BOOST_CHECK_EQUAL(b1.boardSize, boardSize);
  
  Board b2(boardSize);
  b2.init();
  BOOST_CHECK_EQUAL(b2.boardSize, boardSize);

  for(int i = 0; i < b2.boardSize; i++)
    for(int j = 0; j < b2.boardSize; j++)
      BOOST_CHECK_EQUAL(b2.positions[i][j]->color, Empty);
  
  //Valid moves 
  for(int i = 0; i < b1.boardSize; i++)
  {
    for(int j = 0; j < b1.boardSize; j++)
    {
      Point validMove(i, j);
      BOOST_CHECK_EQUAL(b1.isValidMove(validMove), true);
    }
  }
  
  //Pass
  Point pass(b1.boardSize, b1.boardSize);
  BOOST_CHECK_EQUAL(b1.isValidMove(pass), true);
  
  //Invalid moves
  Point invalidMove = Point(b1.boardSize+1, b1.boardSize);
  BOOST_CHECK_EQUAL(b1.isValidMove(invalidMove), false);
  
  invalidMove = Point(b1.boardSize, b1.boardSize+1);
  BOOST_CHECK_EQUAL(b1.isValidMove(invalidMove), false);
  
  invalidMove = Point(0, b1.boardSize);
  BOOST_CHECK_EQUAL(b1.isValidMove(invalidMove), false);
  
  invalidMove = Point(b1.boardSize, 0);
  BOOST_CHECK_EQUAL(b1.isValidMove(invalidMove), false);
  
  invalidMove = Point(0, -1);
  BOOST_CHECK_EQUAL(b1.isValidMove(invalidMove), false);
  
  invalidMove = Point(-1, 0);
  BOOST_CHECK_EQUAL(b1.isValidMove(invalidMove), false);
  
  invalidMove = Point(-1, -1);
  BOOST_CHECK_EQUAL(b1.isValidMove(invalidMove), false);
  
}

BOOST_AUTO_TEST_CASE( testCornerCapture )
{
  int boardSize = 9;
  Board b1(boardSize);
  b1.init();
  BOOST_CHECK_EQUAL(b1.boardSize, boardSize);

  b1.makeMove(Point(0,0)); //Black
  b1.makeMove(Point(1,0)); //White
  b1.makeMove(Point(2,0)); //Black
  b1.makeMove(Point(0,1)); //White //Capture 0,0
  b1.makeMove(Point(0,2)); //Black
  b1.makeMove(Point(8,0)); //White
  b1.makeMove(Point(1,1)); //Black
  b1.makeMove(Point(8,1)); //White
  b1.makeMove(Point(0,0)); //Black //Capture 1,0 0,1
  
  BOOST_CHECK_EQUAL(b1.positions[0][0]->color, Black);
  BOOST_CHECK_EQUAL(b1.positions[1][0]->color, Empty);
  BOOST_CHECK_EQUAL(b1.positions[0][1]->color, Empty);
}

BOOST_AUTO_TEST_CASE( testBoardKo )
{
  int boardSize = 5;
  Board b1(boardSize);
  b1.init();

  b1.makeMove(Point(0,0)); // Black
  b1.makeMove(Point(1,0)); // White
  b1.makeMove(Point(2,0)); // Black
  b1.makeMove(Point(0,1)); // White // Capture 0,0
  b1.makeMove(Point(1,1)); // Black
  b1.makeMove(Point(4,1)); // White
  b1.makeMove(Point(0,0)); // Black // Capture 1,0
  BOOST_CHECK_EQUAL(b1.isValidMove(Point(1,0)), false);

  Board b2(boardSize);
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
  b2.makeMove(Point(1, 2)); // White
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

  Board b3(boardSize);
  b3.init();
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

  b3.makeMove(Point(2, 4)); // Black
}

BOOST_AUTO_TEST_CASE( testIsSuicide )
{
  int boardSize = 6;
  Board b1(boardSize);
  b1.init();
    
  //Black
  BOOST_CHECK_EQUAL(b1.isSuicide(Point(0,0)), false);
  b1.makeMove(Point(0,1)); 
    
  //White
  BOOST_CHECK_EQUAL(b1.isSuicide(Point(0,0)), false);
  b1.makeMove(Point(0,2));
    
  //Black
  BOOST_CHECK_EQUAL(b1.isSuicide(Point(0,0)), false);
  b1.makeMove(Point(1,0));
    
  //White
  BOOST_CHECK_EQUAL(b1.isSuicide(Point(0,0)), true);
  b1.makeMove(Point(1,1));
    
  //Black
  BOOST_CHECK_EQUAL(b1.isSuicide(Point(0,0)), false);
  b1.makeMove(Point(5,5));
    
  //White
  BOOST_CHECK_EQUAL(b1.isSuicide(Point(0,0)), false);
  b1.makeMove(Point(2,0));
    
  //Black
  BOOST_CHECK_EQUAL(b1.isSuicide(Point(0,0)), true);
  b1.makeMove(Point(4,5));
    
  //White
  BOOST_CHECK_EQUAL(b1.isSuicide(Point(0,0)), false);
  b1.makeMove(Point(0,0));
    
  //Black
  BOOST_CHECK_EQUAL(b1.isSuicide(Point(1,0)), true);
  BOOST_CHECK_EQUAL(b1.isSuicide(Point(0,1)), true);
    
  /*  
    W _ W _ _
    _ W _ _ _
    W _ _ _ _
    _ _ _ _ B
    _ _ _ _ B
  */
    
  boardSize = 5;
  Board b2(boardSize);
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
  //White
  BOOST_CHECK_EQUAL(b2.isSuicide(Point(0,2)), false);
  
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
  BOOST_CHECK_EQUAL(score, -36);
}

BOOST_AUTO_TEST_CASE( testGroup )
{
  int boardSize = 9;
  Board b1(boardSize);
  b1.init();
  BOOST_CHECK_EQUAL(b1.boardSize, boardSize);

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
  
  BOOST_CHECK_EQUAL(b1.positions[0][0]->color, Black);
  BOOST_CHECK_EQUAL(b1.positions[1][0]->color, Empty);
  BOOST_CHECK_EQUAL(b1.positions[0][1]->color, Empty);
  
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
  int boardSize = 9;
  Board b1(boardSize);
  b1.init();
  b1.makeMove(Point(stoneX,stoneY));
  Group* g = *b1.blackGroups.begin();
  Point p = Point(adjStoneX,adjStoneY);
  Point *p2 = b1.getPoint(&p);
  return g->isAdjacent(p2);
}

BOOST_AUTO_TEST_CASE( testGroupAdjacent )
{
  int boardSize = 9;
  for(int row = 0; row < boardSize; row++)
  {
    for(int column = 0; column < boardSize; column++)
    {
      //above
      if(row > 0)
        BOOST_CHECK_EQUAL(testAdjacent(row, column, row-1, column), true);
      //below
      if(row < boardSize-1)
        BOOST_CHECK_EQUAL(testAdjacent(row, column, row+1, column), true);
      //left
      if(column > 0)
        BOOST_CHECK_EQUAL(testAdjacent(row, column, row, column-1), true);
      //above
      if(column < boardSize-1)
        BOOST_CHECK_EQUAL(testAdjacent(row, column, row, column+1), true);
      
      //pass
      BOOST_CHECK_EQUAL(testAdjacent(row, column, boardSize, boardSize), false);
    }
  }
}

BOOST_AUTO_TEST_CASE( testGroupNumLiberties )
{
  int boardSize = 5;
  Board b1(boardSize);
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
  Board b1(Black);
  b1.init();
  b1.makeMove(Point(stoneX,stoneY));
  Group* g = *b1.blackGroups.begin();
  Point p = Point(adjStoneX,adjStoneY);
  Point *p2 = b1.getPoint(&p);
  return g->isAdjacent(p2);
}

BOOST_AUTO_TEST_CASE( testGroupHasLiberties )
{
  int boardSize = 9;
  for(int row = 0; row < boardSize; row++)
  {
    for(int column = 0; column < boardSize; column++)
    {
      //above
      if(row > 0)
        BOOST_CHECK_EQUAL(testAdjacent(row, column, row-1, column), true);
      //below
      if(row < boardSize-1)
        BOOST_CHECK_EQUAL(testAdjacent(row, column, row+1, column), true);
      //left
      if(column > 0)
        BOOST_CHECK_EQUAL(testAdjacent(row, column, row, column-1), true);
      //above
      if(column < boardSize-1)
        BOOST_CHECK_EQUAL(testAdjacent(row, column, row, column+1), true);
      
      //pass
      BOOST_CHECK_EQUAL(testAdjacent(row, column, boardSize, boardSize), false);
    }
  }
}


BOOST_AUTO_TEST_CASE( testBoardClone )
{
  int boardSize = 9;
  Board* b1 = new Board(boardSize);
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
  int boardSize = 9;
  Board* b1 = new Board(boardSize);
  b1->init();
  for(int row = 0; row < boardSize-1; row++)
  {
    for(int column = 0; column < boardSize; column++)
    {
      b1->makeMove(Point(row, column));
      b1->makeMove(Point(boardSize, boardSize));
    }
  }
  
  Board* b1Clone = b1->clone();
  float score = b1Clone->getTaylorScore(4.5);
  BOOST_CHECK_EQUAL(score, 76.5);
  
  Board* b1Clone2 = b1->clone();
  score = b1Clone2->getTaylorScore(0.5);
  BOOST_CHECK_EQUAL(score, 80.5);
}
