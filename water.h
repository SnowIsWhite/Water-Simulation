#pragma once
#include <stdio.h>
#include <math.h>
#include <vector>
#include "vector.h"
#include "grid.h"
#include "waterMolecule.h"
#include "objMolecule.h"
#include "implicit.h"

class Water{
public:
	Grid *grid;
	water_molecule *wp;
	obj_molecule *obj;
	Collision *coll;
	std::vector<water_molecule*> waterArr;
	std::vector<obj_molecule*> objArr;
	std::vector<int> surr_waterArr; //어떤 물분자 scope 반경의 모든 분자
	std::vector<int> surr_objArr; //어떤 obj분자 scope 반경의 모든 분자
	int resolution, size;
	double scope;	//분자 탐색 반경
	
	Water(){}
	~Water(){
		for (std::vector<water_molecule *>::size_type i = 0; i < waterArr.size(); i++){ delete waterArr[i]; }
		for (std::vector<obj_molecule *>::size_type i = 0; i < objArr.size(); i++){ delete objArr[i]; }
	}

	void init(int resolution, int size);	//resolution을 받아서 grid를 나누고 물분자 삽입, waterArr업데이트(girdnum정보), scope 업데이트(resolution 길이의 1.5배정도)
	void integrate(double dt, double temperature, double density, int putObj);	//모든 것을 update, main의 update에서 호출

	void surrounding_watermol(VECTOR<double> pos, VECTOR<int> gridnum); //주변 물분자를 배열에 삽입
	void surrounding_objmol(VECTOR<double> pos, VECTOR<int> gridnum); //주변 obj분자를 배열에 삽입
	void update_water_gridnum(water_molecule *wptr, int entity); //물분자에 대한 gridnum 업데이트
	void update_obj_gridnum(obj_molecule *optr, int entity);
	void collisioncheck(water_molecule *wptr, Collision *coll); //물분자가 주체일때 충돌처리
	void collisioncheck(obj_molecule *optr, Collision *coll); //ojb분자가 주체일때 충돌처리
	void clearArray(); //surrounding 배열 클리어
	void draw(int drawObj, int showWater);
};


