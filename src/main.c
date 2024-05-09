/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/08/2024 06:36:30 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  , 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "scoring.h"

#define NUM_DICE 5
#define NUM_SCORE_CATEGORIES 13
#define EMPTY_SCORE_CAT_REPLACEMENT_STR "_"

#define CAN_ROLL(gameState) gameState.rollsThisSet < 3


// --- Structures ---
struct Die {
  int 	number;
  bool 	isLocked;
};

struct GameState {
  struct Die dice[NUM_DICE];
  int rollsThisSet;
  struct Score score;
};


char* scoreTypeNames[] =                 {"Ones",    "Twos",    "Threes",    "Fours",    "Fives",    "Sixes",    "Three of a Kind", "Four of a Kind", "Full House",   "Small Straight",   "Large Straight",   "Yahtzee",    "Chance"};
int (*scoreCalculatorFuncs[])(int[]) =   {ScoreOnes, ScoreTwos, ScoreThrees, ScoreFours, ScoreFives, ScoreSixes, ScoreThreeOfAKind, ScoreFourOfAKind, ScoreFullHouse, ScoreSmallStraight, ScoreLargeStraight, ScoreYahtzee, ScoreChance};

char* helpText = "roll:          Rolls the dice.\ndice:          Prints the current dice.\nlock:          Enters lock/unlock mode.\nscore:         Enters scoring mode, allowing you to score in a category.\nwhatismyscore: Prints your score to the screen.\n";



// --- Globals ---
struct GameState gameState;
bool runMainLoop = true;


// --- Functions ---
void clearStdin()
{
    char unusedBuffer[255];
    fgets(unusedBuffer, 255, stdin);  // This is a hack to clear out the leftover newline from stdin. I'm sure there's a better way to do this...
}


void rollDice()
{
  for (int i = 0; i < NUM_DICE; i++)
  {
    if (gameState.dice[i].isLocked)
      continue;

    gameState.dice[i].number = (rand() % 6) + 1;  // Dice are 1-6, but rand() % 6 spits out 0-5 so we need to add one.
  }
  gameState.rollsThisSet++;
}

void startNewSet()
{
  gameState.rollsThisSet = 0;
  for (int i = 0; i < NUM_DICE; i++)
  {
    gameState.dice[i].isLocked = false;
  }
}


void initScore()
{
  int* addr = &gameState.score.ones;
  for (int i = 0; i < NUM_SCORE_CATEGORIES; i++)
  {
    *addr = -1;
    addr++;
  }
  gameState.score.numOfRemainingCategories = NUM_SCORE_CATEGORIES;
}


void printDice()
{
    for (int i = 0; i < NUM_DICE; i++)
    {
      if (gameState.dice[i].isLocked)
	printf("\x1b[31m{%d}\x1b[0m ", gameState.dice[i].number);
      else
	printf("[%d] ", gameState.dice[i].number);
    }
    printf("\n");
}


void printScore()
{
  int* addr = &gameState.score.ones;
  for (int i = 0; i < NUM_SCORE_CATEGORIES; i++)
  {
    if (*addr == -1)
      printf("%s: %s, ", scoreTypeNames[i], EMPTY_SCORE_CAT_REPLACEMENT_STR);
    else
      printf("%s: %d, ", scoreTypeNames[i], *addr);
    addr++;
  }
  printf("\n\n");
  printf("Top half score bonus (sum of top half >= 63): %d\n", CalculateTopHalfBonus(gameState.score));
  printf("Top half score: %d\n\n", CalculateTopHalfSum(gameState.score));
  printf("Total: %d\n", CalculateTotal(gameState.score));
}


void lockingMode()
{
    int lockQuestionResponse;
    printf("Please type the index of the die you wish to (un)lock (1-5) and press Return. Type -1 when you're finished.\n");
    
    while (true)
    {
      printf("lock> ");
      scanf("%d", &lockQuestionResponse);  // WARN: This doesn't do any length checking on the user's input.
      clearStdin();
      lockQuestionResponse--;  // The user-facing range is 1 to 5, not 0 to 4. This is also why we add 1 to lockQuestionResponse in printf("Die %d's lock...").
      if (lockQuestionResponse < 0)
      {
	break;
      }

      if (lockQuestionResponse >= NUM_DICE)
      {
	printf("Uh-oh! That die doesn't exist. Please choose a number between 0 and 4.\n");
	continue;
      }

      gameState.dice[lockQuestionResponse].isLocked = !gameState.dice[lockQuestionResponse].isLocked;
      printf("Die %d's lock state is now %s\n", lockQuestionResponse + 1, gameState.dice[lockQuestionResponse].isLocked ? "locked" : "unlocked");
    }
}


void scoringMode()
{
  int whichCategoryResponse;

  int diceAsInts[5];
  for (int i = 0; i < NUM_DICE; i++)
  {
    diceAsInts[i] = gameState.dice[i].number;
  }

  printf("Please type the ID of the category you would like to score in.\n");
  for (int i = 0; i < NUM_SCORE_CATEGORIES; i++)
  {
    if (*(&gameState.score.ones + i) == -1)  // If category has not been filled
      printf("%d. %s (%d points)\n", i, scoreTypeNames[i], ( *scoreCalculatorFuncs[i] )(diceAsInts));
  }

  while (true)
  {
    printf("score> ");
    scanf("%d", &whichCategoryResponse);
    clearStdin();
    if (whichCategoryResponse < 0 || whichCategoryResponse > NUM_SCORE_CATEGORIES - 1)
    {
      printf("That's not a valid ID. Please type a number from 0 to %d.\n", NUM_SCORE_CATEGORIES - 1);
      continue;
    }
    if (*(&gameState.score.ones + whichCategoryResponse) != -1)
    {
      printf("You've already filled %s. Please select another category.\n", scoreTypeNames[whichCategoryResponse]);
      continue;
    }

    *(&gameState.score.ones + whichCategoryResponse) = (*scoreCalculatorFuncs[whichCategoryResponse])(diceAsInts);
    gameState.score.numOfRemainingCategories--;

    if (gameState.score.numOfRemainingCategories > 0)
    {
      startNewSet();
    }
    else
    {
      printf("Game over! Thanks for playing. Your final score was: %d\n", CalculateTotal(gameState.score));
      runMainLoop = false;
    }
    break;
  }
}


void processCommand(char* cmd)
{
  if (strcmp(cmd, "lock\n") == 0)
  {
    lockingMode();
  }
  else if (strcmp(cmd, "dice\n") == 0)
  {
    printDice();
  }
  else if (strcmp(cmd, "roll\n") == 0)
  {
    if (CAN_ROLL(gameState))
    {
      rollDice();
      printDice();
      if (!(CAN_ROLL(gameState)))  // If it's their last roll
	scoringMode();
    }
  }
  else if (strcmp(cmd, "score\n") == 0)
  {
    scoringMode();
  }
  else if (strcmp(cmd, "whatismyscore\n") == 0)
  {
    printScore();
  }
  else if (strcmp(cmd, "help\n") == 0)
  {
    printf(helpText);
  }
}


// Main loop of the game
void runGame()
{
  printf("> ");
  char commandText[255];
  fgets(commandText, 255, stdin);
  processCommand(commandText);
}

int main()
{
  printf("Welcome to C-Yahtzee!\n");
  gameState.rollsThisSet = 0;
  initScore();
  srand(time(NULL));
  while (runMainLoop)
    runGame();
  return 0;
}
