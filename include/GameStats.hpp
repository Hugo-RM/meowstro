//Author: Jay P
//Desc: .hpp file for displaying game statistics
#pragma once
#include <iostream>
using namespace std;

class GameStats
{
public:
	//constructors
	GameStats();
	GameStats(int score, int combo, int hits, int misses);
	//setters n getters
	void setScore(int score);
	void setCombo(int combo); //willing to drop combo </3
	void setHits(int hits);
	void setMisses(int misses);
	void setAccuracy(double accuracy);
	int getScore()const;
	int getCombo()const; //willing to drop combo </3
	int getHits()const;
	int getMisses()const;
	double getAccuracy()const;
	void increaseScore(int score);
	//others
	friend ostream& operator << (ostream& out, const GameStats& s);
	GameStats operator++ (int); //adds to hits and score
	GameStats operator-- (int); //adds to misses
private: 
	int score, combo, hits, misses;
	double accuracy;
};
