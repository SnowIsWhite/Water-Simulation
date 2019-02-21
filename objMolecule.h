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
	VECTOR<int> gridnum; //���� ���� grid�� ��ȣ --> ��ġ�� �ٲ� ������ update�� �� �� grid array���� ��ġ�� ã�� ���� ����.
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
	void update_temper(double temper, int total_molecules);//�������� ���
	void update_gravity();
	void objmol_force_integrate(obj_molecule *optr, double scope);//obj-obj�� �η� ���
	void objmol_force_integrate(water_molecule *wptr, double density, double scope);//obj-���� �η� ���
	void final_integrate(double dt, int size);// ���� pos���
	void collision_handle(obj_molecule *optr);//�浹�� ó��(���� ���� �ӵ� ���� ��ȭ)
	void collision_handle(water_molecule *wptr);//�浹�� ó��(���� ���� �ӵ� ���� ��ȭ)
	void draw();
};