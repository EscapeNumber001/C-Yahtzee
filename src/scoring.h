#include <stdbool.h>

struct Score {
  int ones;
  int twos;
  int threes;
  int fours;
  int fives;
  int sixes;

  int threeOfAKind;
  int fourOfAKind;
  int fullHouse;
  int smallStraight;
  int largeStraight;
  int yahtzee;
  int chance;

  int numOfRemainingCategories;
};

int _ScoreSingleDigits(int numberToCheck, int dice[]);
int ScoreOnes(int dice[]);
int ScoreTwos(int dice[]);
int ScoreThrees(int dice[]);
int ScoreFours(int dice[]);
int ScoreFives(int dice[]);
int ScoreSixes(int dice[]);

int _ScoreNOfAKind(int n, int dice[]);
int ScoreThreeOfAKind(int dice[]);
int ScoreFourOfAKind(int dice[]);
int ScoreYahtzee(int dice[]);

int ScoreFullHouse(int dice[]);

int _ScoreStraights(int dice[], bool isThisLargeStrt, int pointValue);
int ScoreSmallStraight(int dice[]);
int ScoreLargeStraight(int dice[]);

int ScoreChance(int dice[]);

int CalculateTopHalfBonus(struct Score score);
int CalculateTopHalfSum(struct Score score);

int CalculateTotal(struct Score score);
