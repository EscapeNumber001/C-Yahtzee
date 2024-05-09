/*
 * =====================================================================================
 *
 *       Filename:  scoring.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/08/2024 09:40:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "scoring.h"

int _ScoreSingleDigits(int number, int dice[])
{
  int total = 0;
  for (int i = 0; i < 5; i++)
  {
    if (dice[i] == number)
      total += number;
  }
  return total;
}

int _SumArray(int arr[], int arrLength)
{
  int result = 0;
  for (int i = 0; i < arrLength; i++)
  {
    result += arr[i];
  }
  return result;
}

void swap(int* xp, int* yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// An optimized version of Bubble Sort
void _SortArray(int arr[])
{
    int i, j;
    int n = 5;
    bool swapped;
    for (i = 0; i < n - 1; i++) {
        swapped = false;
        for (j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
                swapped = true;
            }
        }

        // If no two elements were swapped by inner loop,
        // then break
        if (swapped == false)
            break;
    }
}

int ScoreOnes(int dice[])
{
  return _ScoreSingleDigits(1, dice);
}

int ScoreTwos(int dice[]) 
{
  return _ScoreSingleDigits(2, dice);
}

int ScoreThrees(int dice[]) 
{
  return _ScoreSingleDigits(3, dice);
}

int ScoreFours(int dice[]) 
{
  return _ScoreSingleDigits(4, dice);
}

int ScoreFives(int dice[]) 
{
  return _ScoreSingleDigits(5, dice);
}

int ScoreSixes(int dice[]) 
{
  return _ScoreSingleDigits(6, dice);
}

int _ScoreNOfAKind(int n, int dice[])
{
  for (int i = 1; i < 7; i++)  // Iterate through numbers 1-6
  {
    int numOfDiceWithThisNumber = 0;
    for (int dieIdx = 0; dieIdx < 6; dieIdx++)  // Iterate through each die
    {
      if (dice[dieIdx] == i)
	numOfDiceWithThisNumber++;

      if (numOfDiceWithThisNumber == n)
	return _SumArray(dice, 5);
    }
  }
  return 0;
}

int ScoreThreeOfAKind(int dice[])
{
  return _ScoreNOfAKind(3, dice);
}


int ScoreFourOfAKind(int dice[])
{
  return _ScoreNOfAKind(4, dice);
}

int ScoreYahtzee(int dice[])
{
  if (_ScoreNOfAKind(5, dice))
    return 50;
  return 0;
}

int ScoreFullHouse(int dice[])
{
  _SortArray(dice);
  int uniqueNumber1 = 0;
  int uniqueNumber2 = 0;

  int uniqueNumber1Count = 0;
  int uniqueNumber2Count = 0;
  for (int i = 0; i < 5; i++)
  {
    if (!uniqueNumber1)
      uniqueNumber1 = dice[i];

    if (uniqueNumber1 && !uniqueNumber2 && dice[i] != uniqueNumber1)
      uniqueNumber2 = dice[i];


    if (dice[i] == uniqueNumber1)
      uniqueNumber1Count++;

    if (dice[i] == uniqueNumber2)
      uniqueNumber2Count++;
  }
  if (uniqueNumber1Count * uniqueNumber2Count == 6)  // A group of 3 and a group of 2 will always multiply to 6.
    return 25;
  return 0;
}

int _ScoreStraights(int dice[], bool isThisLargeStrt, int pointValue)
{
  _SortArray(dice);
  for (int i = 0; i < 4; i++)
  {
    if (dice[i + 1] == dice[i] + 1 && dice[i + 2] == dice[i] + 2 && dice[i + 3] == dice[i] + 3)  // TODO: There's definitely a better way to write this check without using a long chain of &&.
    {
      if (isThisLargeStrt && dice[i + 4] != dice[i] + 4)
	return 0;
      return pointValue;
    }
  }
  return 0;
}

int ScoreSmallStraight(int dice[])
{
  return _ScoreStraights(dice, false, 30);
}

int ScoreLargeStraight(int dice[])
{
  return _ScoreStraights(dice, true, 40);
}

int ScoreChance(int dice[])
{
  return _SumArray(dice, 5);
}

int CalculateTopHalfBonus(struct Score score)
{
  if (score.ones + score.twos + score.threes + score.fours + score.fives + score.sixes >= 63)
    return 35;
  return 0;
}

int CalculateTopHalfSum(struct Score score)
{
  int* addr = &score.ones;
  int total = 0;
  for (int i = 0; i < 6; i++)
  {
    int value = *(addr) < 0 ? 0 : *(addr); // Unfilled blanks are stored as -1, but we want to treat them as 0 in this case.
    total += value;
    addr++;
  }
  return total;
}


// TODO: This could probably be combined with CalculateTopHalfSum into a more generic function.
int CalculateTotal(struct Score score)
{
  int* addr = &score.ones;
  int total = 0;
  for (int i = 0; i < 13; i++)  // TODO: Replace `13` with NUM_SCORE_CATEGORIES 
  {
    int value = *(addr) < 0 ? 0 : *(addr); // Unfilled blanks are stored as -1, but we want to treat them as 0 in this case.
    total += value;
    addr++;
  }
  return total;
}
