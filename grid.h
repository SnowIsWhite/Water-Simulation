#pragma once
#include <stdio.h>
#include <list>
#include <vector>
#include <math.h>

#include "waterMolecule.h"

class Grid{
public:
	std::vector<std::vector<std::vector<std::list<int> *> > > grid_watArr; //���� �ȿ� �ִ� ������ index����
	//list of int�� ���Ҹ� ���� 3���� �迭
	std::vector<std::vector<std::vector<std::list<int> *> > > grid_objArr; //���� �ȿ� �ִ� ������ index����
	
	Grid(){}
	~Grid(){}
	void init(int resolution);	//grid ����
};