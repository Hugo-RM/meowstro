//Author: Jay P
//Desc: .cpp for displaying game statistics
#include "GameStats.hpp"
#include <iostream>
using namespace std;

GameStats::GameStats()
{
	score = 0;
	combo = 0; //willing to drop combo 
	hits = 0;
	misses = 0;
	accuracy = 0.0;
}
GameStats::GameStats(int score, int combo, int hits, int misses) //takes in hits and misses to calculate accuracy
{
	setScore(score); 
	setCombo(combo); //cut
	accuracy = (hits > 0) ? (static_cast<double>(hits) / (hits + misses)) * 100.0 : 0.0; // (1 - %ofMisses) * 100 = accuracy 
}
void GameStats::setScore(int score)
{
	this->score = score;
}
void GameStats::setCombo(int combo) //old yeller this
{
	this->combo = combo;
}
void GameStats::setHits(int hits)
{
	this->hits = hits;
}
void GameStats::setMisses(int misses)
{
	this->misses = misses;
}
void GameStats::setAccuracy(double accuracy)
{
	this->accuracy = accuracy;
}
int GameStats::getScore()const
{
	return score;
}
int GameStats::getCombo()const //this too
{
	return combo;
}
int GameStats::getHits()const
{
	return hits;
}
int GameStats::getMisses()const
{
	return misses;
}
double GameStats::getAccuracy()const
{
	return accuracy;
}
void GameStats::increaseScore(int score) //pass in 100 for perfect, 50 for good
{
	this->score += score;
}
void GameStats::resetStats()
{
	setScore(0);
	setCombo(0);
	setHits(0);
	setMisses(0);
	setAccuracy(0);
}
ostream& operator << (ostream& out, const GameStats& s) //displays stats at end of game
{
	out << "-*Final Stats!*-" << endl;
	out << "Score: " << s.getScore() << "!!!\n";
	out << "Combo: " << s.getCombo() << endl;
	out << "Accuracy: " << s.getAccuracy() << endl;
	if (s.getAccuracy() < 50.0)
	{
		out << "Oh.. thats kinda bad.." << endl;
	}
	else
	{
		out << "Well done!" << endl;
	}
	return out;
}
GameStats GameStats::operator++ (int) //adds 1 to hits
{
	hits++;
	accuracy = (static_cast<double>(hits) / (hits + misses)) * 100.0;
	return *this;
}
GameStats GameStats::operator-- (int) //adds 1 to misses
{
	misses++;
	accuracy = (hits > 0) ? (static_cast<double>(hits) / (hits + misses)) * 100.0 : 0.0;
	return *this;
}