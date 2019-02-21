#pragma once
#include <stdio.h>
#include <list>
#include <vector>
#include <math.h>

#include "waterMolecule.h"

class Grid{
public:
	std::vector<std::vector<std::vector<std::list<int> *> > > grid_watArr; //격자 안에 있는 물분자 index정보
	//list of int를 원소를 갖는 3차원 배열
	std::vector<std::vector<std::vector<std::list<int> *> > > grid_objArr; //격자 안에 있는 물분자 index정보
	
	Grid(){}
	~Grid(){}
	void init(int resolution);	//grid 생성
};