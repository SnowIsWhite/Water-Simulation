#include <stdio.h>
#include <list>
#include <vector>
#include <math.h>

#include "waterMolecule.h"
#include "grid.h"

void Grid::init(int resolution){
	//100/resolution만큼 가로, 세로, 높이 배열 생성
	int width = resolution;
	int height = resolution;
	int length = resolution;
	int i,j,k;

	for (k = 0; k < width; k++){
		std::vector<std::vector<std::list<int> *> > temp2;
		for (j = 0; j < length; j++){
			//list를 height에 push
			std::vector<std::list<int> *> temp;
			for (i = 0; i < height; i++){
				std::list<int> *lt = new std::list < int > ;
				temp.push_back(lt);
			}
			temp2.push_back(temp);
		}
		grid_watArr.push_back(temp2);
	}

	for (k = 0; k < width; k++){
		std::vector<std::vector<std::list<int> *> > temp2;
		for (j = 0; j < length; j++){
			//list를 height에 push
			std::vector<std::list<int> *> temp;
			for (i = 0; i < height; i++){
				std::list<int> *lt = new std::list < int >;
				temp.push_back(lt);
			}
			temp2.push_back(temp);
		}
		grid_objArr.push_back(temp2);
	}
	return;
}
