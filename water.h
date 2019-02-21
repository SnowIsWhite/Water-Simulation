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
	std::vector<int> surr_waterArr; //� ������ scope �ݰ��� ��� ����
	std::vector<int> surr_objArr; //� obj���� scope �ݰ��� ��� ����
	int resolution, size;
	double scope;	//���� Ž�� �ݰ�
	
	Water(){}
	~Water(){
		for (std::vector<water_molecule *>::size_type i = 0; i < waterArr.size(); i++){ delete waterArr[i]; }
		for (std::vector<obj_molecule *>::size_type i = 0; i < objArr.size(); i++){ delete objArr[i]; }
	}

	void init(int resolution, int size);	//resolution�� �޾Ƽ� grid�� ������ ������ ����, waterArr������Ʈ(girdnum����), scope ������Ʈ(resolution ������ 1.5������)
	void integrate(double dt, double temperature, double density, int putObj);	//��� ���� update, main�� update���� ȣ��

	void surrounding_watermol(VECTOR<double> pos, VECTOR<int> gridnum); //�ֺ� �����ڸ� �迭�� ����
	void surrounding_objmol(VECTOR<double> pos, VECTOR<int> gridnum); //�ֺ� obj���ڸ� �迭�� ����
	void update_water_gridnum(water_molecule *wptr, int entity); //�����ڿ� ���� gridnum ������Ʈ
	void update_obj_gridnum(obj_molecule *optr, int entity);
	void collisioncheck(water_molecule *wptr, Collision *coll); //�����ڰ� ��ü�϶� �浹ó��
	void collisioncheck(obj_molecule *optr, Collision *coll); //ojb���ڰ� ��ü�϶� �浹ó��
	void clearArray(); //surrounding �迭 Ŭ����
	void draw(int drawObj, int showWater);
};


