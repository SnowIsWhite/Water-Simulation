#include <stdio.h>
#include <vector>
#include <math.h>
#include "vector.h"
#include "waterMolecule.h"
#include "objMolecule.h"

void water_molecule::update_force(VECTOR<double> delta_force){
	force = force + delta_force;
	return;
}
void water_molecule::update_temper(double temper, int total_molecules){
	double deltaT = temper - (*this).temperature;
	if (temper == 0){
		velocity.update(0.0, 0.0, 0.0);
	}
	else{
		if (deltaT != 0){
			(*this).temperature = temper;
			double deltaQ = 1 * mass * deltaT; //c = 1
			double delta;
			if (deltaQ>0) delta = sqrt(fabs(deltaQ) / (2 * mass*total_molecules));
			else delta = -1*(sqrt(fabs(deltaQ) / (2 * mass*total_molecules)));
			delta *= 10;
			if ((1 + delta) > 0){
				velocity.x = (1 + delta)*velocity.x;
				velocity.y = (1 + delta)*velocity.y;
				velocity.z = (1 + delta)*velocity.z;
			}
			else
				velocity.update(0.0, 0.0, 0.0);
		}
	}
	return;
}

void water_molecule::watermol_force_integrate(water_molecule *wptr, double scope){
	//������-������ �η� ���
	if ((*this).position.x == wptr->position.x && (*this).position.y == wptr->position.y && (*this).position.z == wptr->position.z) return;	//���� ������ ��� pass
	//���� ���� ���
	VECTOR<double> unit = position.UnitVector(wptr->position);
	//���ڰ� �Ÿ����
	double len = position.distance(wptr->position);
	double temp_force = 5 * (1 - (len / scope));
	unit.operator*((1 / sqrt(3))*temp_force);
	update_force(unit);
	return;
}
void water_molecule::watermol_force_integrate(obj_molecule *optr, double scope){
	//������-obj���� �η� ���
	//���� ���� ���
	VECTOR<double> unit = position.UnitVector(optr->position);
	//���ڰ� �Ÿ����
	double len = position.distance(optr->position);
	double temp_force = 2.5 * (1 - (len / scope));
	unit.operator*((1 / sqrt(3))*temp_force);
	update_force(unit);
	return;
}
void water_molecule::final_integrate(double dt, int size){
	acceleration = force / mass;
	velocity = velocity + (acceleration.operator*(dt));
	position = position + (velocity.operator*(dt));
	//���鿡 ���� collision check
	if (position.x < 0){
		position.x = 0;
		velocity.x = -0.8*velocity.x;
	}
	else if (position.x > size){
		position.x = size;
		velocity.x = -0.8*velocity.x;
	}

	if (position.y < 0){
		position.y = 0;
		velocity.y = -0.8*velocity.y;
	}
	else if (position.y > size){
		position.y = size;
		velocity.y = -0.8*velocity.y;
	}

	if (position.z < (-1*size)){
		position.z = (-1*size);
		velocity.z = -0.8*velocity.z;
	}
	else if (position.z > 0){
		position.z = 0;
		velocity.z = -0.8*velocity.z;
	}
	return;
}

void water_molecule::collision_handle(water_molecule *wptr){
	if ((*this).position.x == wptr->position.x && (*this).position.y == wptr->position.y && (*this).position.z == wptr->position.z) return;
	//�ݹ߷¿� ���� �ݹ߰���� 1���� ũ�ٰ� �����Ѵ�.
	double e = 1.5;
	double Ma = (*this).mass;
	double Mb = wptr->mass;
	VECTOR<double> xaxis(1, 0, 0);
	VECTOR<double> yaxis(0, 1, 0);
	VECTOR<double> zaxis(0, 0, 1);
	VECTOR<double> Va = velocity;
	VECTOR<double> Vb = wptr->velocity;
	//x' ������ �������� 
	VECTOR<double> xx = position.UnitVector(wptr->position);

	//***x'���� ���
	//x�� ���̿����� cos,sin��
	double cos;
	if ((xaxis.Magnitude()*xx.Magnitude()) != 0)
		cos = xaxis.InnerProduct(xx) / (xaxis.Magnitude()*xx.Magnitude());
	else
		cos = 1;
	double sin = sqrt(1 - (cos*cos));
	//��ü a
	double Vaxx = Va.x * cos + Va.y*sin;
	//��ü b
	double Vbxx = Vb.x*cos + Vb.y*sin;
	//�浹 �� 
	if (Vaxx != 0 && Vbxx != 0){
		Vaxx = ((Ma - (Mb*e)) / ((Ma + Mb)*(Vaxx))) + ((Mb + (Mb*e)) / ((Ma + Mb)*Vbxx));
		Vbxx = ((Ma + (Ma*e)) / ((Ma + Mb)*(Vaxx))) + ((Mb - (Ma*e)) / ((Ma + Mb)*Vbxx));
	}

	//***y'���� ���
	double Vayy = Va.y*cos - Va.x*sin;
	double Vbyy = Vb.y*cos - Va.y*sin;
	//***z'���� ��� z�� ���� ��ȭ ����

	Va.x = Vaxx*cos - Vayy*sin;
	Va.y = Vaxx*sin + Vayy*cos;
	Va.z = -0.8*Va.z;
	Vb.x = Vbxx*cos - Vbyy*sin;
	Vb.y = Vbxx*sin + Vbyy*cos;
	Vb.z = -0.8*Vb.z;

	//final update
	(*this).velocity.x = Va.x;
	(*this).velocity.y = Va.y;
	(*this).velocity.z = Va.z;

	wptr->velocity.x = Vb.x;
	wptr->velocity.y = Vb.y;
	wptr->velocity.z = Vb.z;

	return;
}

void water_molecule::collision_handle(obj_molecule *optr){
	//�ݹ߷¿� ���� �ݹ߰���� 1���� ũ�ٰ� �����Ѵ�.
	double e = 1.2;
	double Ma = (*this).mass;
	double Mb = optr->mass;
	VECTOR<double> xaxis(1, 0, 0);
	VECTOR<double> yaxis(0, 1, 0);
	VECTOR<double> zaxis(0, 0, 1);
	VECTOR<double> Va = velocity;
	VECTOR<double> Vb = optr->velocity;
	//x' ������ �������� 
	VECTOR<double> xx = position.UnitVector(optr->position);

	//***x'���� ���
	//x�� ���̿����� cos,sin��
	double cos;
	if ((xaxis.Magnitude()*xx.Magnitude()) != 0)
		cos = xaxis.InnerProduct(xx) / (xaxis.Magnitude()*xx.Magnitude());
	else
		cos = 1;
	double sin = sqrt(1 - (cos*cos));
	//��ü a
	double Vaxx = Va.x * cos + Va.y*sin;
	//��ü b
	double Vbxx = Vb.x*cos + Vb.y*sin;
	//�浹 �� 
	if (Vaxx != 0 && Vbxx != 0){
		Vaxx = ((Ma - (Mb*e)) / ((Ma + Mb)*(Vaxx))) + ((Mb + (Mb*e)) / ((Ma + Mb)*Vbxx));
		Vbxx = ((Ma + (Ma*e)) / ((Ma + Mb)*(Vaxx))) + ((Mb - (Ma*e)) / ((Ma + Mb)*Vbxx));
	}

	//***y'���� ���
	double Vayy = Va.y*cos - Va.x*sin;
	double Vbyy = Vb.y*cos - Va.y*sin;
	//***z'���� ��� z�� ���� ��ȭ ����

	Va.x = Vaxx*cos - Vayy*sin;
	Va.y = Vaxx*sin + Vayy*cos;
	Va.z = -0.8*Va.z;
	Vb.x = Vbxx*cos - Vbyy*sin;
	Vb.y = Vbxx*sin + Vbyy*cos;
	Vb.z = -0.8*Vb.z;

	//final update
	(*this).velocity.x = Va.x;
	(*this).velocity.y = Va.y;
	(*this).velocity.z = Va.z;

	optr->velocity.x = Vb.x;
	optr->velocity.y = Vb.y;
	optr->velocity.z = Vb.z;

	return;
}