#pragma once
#include <stdio.h>
#include <vector>
#include <math.h>
#include <ctime>
#include <cstdlib>

#include "vector.h"

class water_molecule;
class obj_molecule{
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
	obj_molecule(void){}
	obj_molecule(VECTOR<double> init_pos, VECTOR<int> init_gridnum, VECTOR<double> init_vel, double init_temp, double init_dense){
		mass = 1.0;
		density = init_dense;
		radius = 0.4;
		temperature = init_temp;
		gridnum = init_gridnum;
		position = init_pos;
		velocity = init_vel;
		acceleration.update(0.0, 0.0, 0.0);
		force.update(0.0, 0.0, 0.0);
	}
	~obj_molecule(){}

	double getPosX(void){ return position.x; }
	double getPosY(void){ return position.y; }
	double getPosZ(void){ return position.z; }

	void update_force(VECTOR<double> delta_force); //force update
	void update_temper(double temper, int total_molecules);//열에너지 계산
	void update_gravity();
	void objmol_force_integrate(obj_molecule *optr, double scope);//obj-obj간 인력 계산
	void objmol_force_integrate(water_molecule *wptr, double density, double scope);//obj-물간 인력 계산
	void final_integrate(double dt, int size);// 최종 pos계산
	void collision_handle(obj_molecule *optr);//충돌시 처리(양쪽 서로 속도 방향 변화)
	void collision_handle(water_molecule *wptr);//충돌시 처리(양쪽 서로 속도 방향 변화)
	void draw();
};