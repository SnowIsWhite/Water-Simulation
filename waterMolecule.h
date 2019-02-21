#pragma once
#include <stdio.h>
#include <vector>
#include <math.h>
#include <ctime>
#include <cstdlib>

#include "vector.h"
#include "objMolecule.h"
class water_molecule{
public:
	double mass;
	double density;
	double radius;
	double temperature;
	VECTOR<int> gridnum; //내가 속한 grid의 번호 --> 위치가 바뀔 때마다 update를 할 때 grid array에서 위치를 찾기 위해 사용됨.
	VECTOR<double> position;
	VECTOR<double> velocity;
	VECTOR<double> acceleration;
	VECTOR<double> force;
	water_molecule(void){}
	water_molecule(VECTOR<double> init_pos, VECTOR<int> init_gridnum, double init_temp){
		mass = 0.18;
		density = 1.0;
		radius = 0.2;
		temperature = init_temp;
		gridnum = init_gridnum;
		position = init_pos;
		velocity.update(0.0, 0.0, 0.0);
		acceleration.update(0.0, 0.0, 0.0);
		force.update(0.0, 0.0, 0.0);
	}
	~water_molecule(){}

	double getPosX(void){ return position.x; }
	double getPosY(void){ return position.y; }
	double getPosZ(void){ return position.z; }
	
	void update_force(VECTOR<double> delta_force); //force update
	void update_temper(double temper, int total_molecules);//열에너지 계산
	void watermol_force_integrate(water_molecule *wptr, double scope); //물분자-물분자 인력 계산
	void watermol_force_integrate(obj_molecule *optr, double scope); //물분자-obj분자 인력 계산
	void final_integrate(double dt, int size);// 최종 pos계산
	void collision_handle(water_molecule *wptr);//충돌시 처리(양쪽 서로 속도 방향 변화)
	void collision_handle(obj_molecule *optr);//충돌시 처리(양쪽 서로 속도 방향 변화)
	void draw();
};