#pragma once
#include <stdio.h>
#include <math.h>
#include <vector>
#include <iostream>
#include "vector.h"
#include <algorithm>

class Imp_Func{
public:
	Imp_Func(){}
	~Imp_Func(){}
	virtual double f(double x, double y, double z) = 0;
	VECTOR<double> df(double x, double y, double z){
		double dx = 0.001;
		double dy = 0.001;
		double dz = 0.001;
		double left = f(x - dx, y, z);
		double right = f(x + dx, y, z);
		double down = f(x, y - dy, z);
		double up = f(x, y + dy, z);
		double back = f(x, y, z - dz);
		double forward = f(x, y, z + dz);

		return VECTOR<double>((right - left) / (2 * dx), (up - down) / (2 * dy), (forward - back) / (2 * dz));
	}
};
class Imp_Circle : public Imp_Func{
public:
	//a(x-p)^2 + b(y-q)^z + c(z-r)^2 +d = 0
	double a, b, c, d, p, q, r;
	Imp_Circle(){
		a = 1; b = 1; c = 1; d = -1;
		p = 0; q = 0; r = 0;
	}
	~Imp_Circle(){}
	Imp_Circle(double a, double b, double c, double d, double p, double q, double r){
		this->a = a;
		this->b = b;
		this->c = c;
		this->d = pow(d,2);
		this->p = p;
		this->q = q;
		this->r = r;
	}
	double f(double x, double y, double z){
		return a*(x - p)*(x - p) + b*(y - q)*(y - q) + c*(z - r)*(z - r) - d;
	}
};

class Collision{
public:
	std::vector<Imp_Func *> func;
	Collision(){}
	~Collision(){
		for (auto f : func){
			delete f;
		}
		func.clear();
	}

	void Init(){
		Imp_Func *f;
		//circle (x-1)^2 +(y-1)^2 + (z-1)^2 = 1
		f = new Imp_Circle(1, 1, 1, 1, 1, 1, -1);
		func.push_back(f);
	}

	bool is_collision(double x, double y, double z){
		bool result = false;
		for (auto f : func){
			if (f->f(x, y, z) <= 0){
				result |= true;
			}
		}
		return result;
	}

	VECTOR<double> deltaf(double x, double y, double z){
		VECTOR<double> result;
		result.update(0.0, 0.0, 0.0);
		for (auto f : func)
		{
			if (f->f(x, y, z) <= 0){
				VECTOR<double> df = f->df(x, y, z);
				double phi = f->f(x, y, z);
				double dfl = df.Magnitude();
				df = df / dfl;
				phi = fabs(phi);
				result = result + (phi*df);
			}
		}
		return result;
	}
};