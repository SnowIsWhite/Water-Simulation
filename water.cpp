#include "water.h"
#include <stdio.h>
#include <vector>

#include "grid.h"
#include "waterMolecule.h"
#include "objMolecule.h"
#include "implicit.h"

void Water::init(int resolution, int size){
	int i, j, k;
	//scope 값 설정
	scope = (size / resolution) * 1.5;
	(*this).resolution = resolution;
	(*this).size = size;
	//격자 나누기
	grid = new Grid();
	grid->init(resolution);

	//격자마다 돌면서 물분자 index를 하나씩 삽입하는 겁니다.
	//그와 동시에 water array에 물분자를 삽입합니다.	
	int temp = (size / resolution);
	for (i = 0; i<grid->grid_watArr.size(); i++){
		for (j = 0; j<grid->grid_watArr[i].size(); j++){
			for (k = 0; k<grid->grid_watArr[i][j].size(); k++){
				wp = new water_molecule(VECTOR<double>(k*temp + (temp / 2), i*temp + (temp / 2), -1*(j*temp + (temp / 2))), VECTOR<int>(i, j, k), 25);
				waterArr.push_back(wp);
				(grid->grid_watArr[i][j][k])->push_back(waterArr.size()-1);
			}
		}
	}

	//격자마다 돌면서 필요한 격자에 obj분자 index를 삽입한다.
	//그와 동시에 obj array에 obj를 삽입한다.
	for (i = 0; i<grid->grid_watArr.size(); i++){
		for (j = 0; j<grid->grid_watArr[i].size(); j++){
			for (k = 0; k<grid->grid_watArr[i][j].size(); k++){
				if (k >= resolution / 2 - 1 && k<=resolution / 2 + 2)
					if (j >= resolution / 2 - 1 && j <= resolution / 2 +2)
						if (i<=resolution - 1 && i>=resolution -6){
							obj = new obj_molecule(VECTOR<double>(k*temp + (temp / 2), i*temp + (temp / 2), -1 * (j*temp + (temp / 2))), VECTOR<int>(i, j, k), VECTOR<double>(0.0,-100.0,0.0), 25, 1.5);
							objArr.push_back(obj);
							(grid->grid_objArr[i][j][k])->push_back(objArr.size() - 1);
						}
			}
		}
	}
	return;
}

void Water::integrate(double dt, double temperature, double density, int putObj){
	coll = new Collision();
	coll->Init();
	//******* 물분자 update *******
	int total_molecules = resolution*resolution*resolution; //전체 분자수

	for (int i = 0; i < waterArr.size(); i++){
	//grid에서 주변의 물분자 배열 받아오기
		surrounding_watermol(waterArr[i]->position, waterArr[i]->gridnum);
		//(grid에서 주변의 obj 배열 받아오기)
		surrounding_objmol(waterArr[i]->position, waterArr[i]->gridnum);
	//주변 물분자에 대한 force합 계산
		for (int j = 0; j < surr_waterArr.size(); j++)
			waterArr[i]->watermol_force_integrate(waterArr[surr_waterArr[j]], scope); //parameter는 pointer다
		//(주변 obj분자에 대한 force합계산)
		for (int j = 0; j < surr_objArr.size(); j++)
			waterArr[i]->watermol_force_integrate(objArr[surr_objArr[j]], scope); //parameter는 pointer다
	//온도에 대한 열에너지 update
		waterArr[i]->update_temper(temperature, total_molecules);
	//적분
		waterArr[i]->final_integrate(dt, size);
	//주변 분자 배열 clear
		clearArray();
	//바뀐 pos에대한 gridnum update(분자 이동& 분자 내 정보 변화)
		update_water_gridnum(waterArr[i], i);
	//update된 pos에 대한 물분자 배열 받아오기
		surrounding_watermol(waterArr[i]->position, waterArr[i]->gridnum);
		//(update된 pos에 대한 obj 배열 받아오기)
		surrounding_objmol(waterArr[i]->position, waterArr[i]->gridnum);
	//update된 pos에서 collision check --> collision check 은 물분자 - obj 분자 사이에만 해줄 필요성이 있음
		collisioncheck(waterArr[i], coll);
	//주변 분자 배열 clear
		clearArray();
	}
	total_molecules = 100;
	//*******obj분자 update *******
	if (putObj == 1){
		for (int i = 0; i < objArr.size(); i++){
			//grid에서 주변의 obj분자 배열 받아오기
			surrounding_objmol(objArr[i]->position, objArr[i]->gridnum);
			//(grid에서 주변의 물분자 배열 받아오기)
			surrounding_watermol(objArr[i]->position, objArr[i]->gridnum);

			//주변 obj자에 대한 force합 계산
			for (int j = 0; j < surr_objArr.size(); j++)
				objArr[i]->objmol_force_integrate(objArr[surr_objArr[j]], scope); //parameter는 pointer다
			//중력, 부력 계산
			objArr[i]->update_gravity();

			//(주변 물분자에 대한 force합계산) -->밀도 또한 고려한다.
			for (int j = 0; j < surr_waterArr.size(); j++)
				objArr[i]->objmol_force_integrate(waterArr[surr_waterArr[j]],density, scope); //parameter는 pointer다
			//온도에 대한 열에너지 update
			objArr[i]->update_temper(temperature, total_molecules);
			//적분
			objArr[i]->final_integrate(dt, size);
			//주변 분자 배열 clear
			clearArray();
			//바뀐 pos에대한 gridnum update(분자 이동& 분자 내 정보 변화)
			update_obj_gridnum(objArr[i], i);
			//update된 pos에 대한 obj분자 배열 받아오기
			surrounding_objmol(objArr[i]->position, objArr[i]->gridnum);
			//(update된 pos에 대한 물분자 배열 받아오기)
			surrounding_watermol(objArr[i]->position, objArr[i]->gridnum);
			//update된 pos에서 collision check --> collision check 은 물분자 - obj 분자 / obj- obj분자사이에만 해줄 필요성이 있음
			collisioncheck(objArr[i], coll);
			//주변 분자 배열 clear
			clearArray();
		}
	}
	return;
}

void Water::surrounding_watermol(VECTOR<double> pos, VECTOR<int> gridnum){
	//for문을 돌면서 격자선택, scope 범위를 넘어가면 continue, scope범위 안에 있으면 배열에 삽입
	int tempi = gridnum.x - 1;
	int tempj = gridnum.y - 1;
	int tempk = gridnum.z - 1;
	int i, j, k;
	for (i = 0; i < 3; i++){
		if ((tempi + i) < 0 || (tempi + i) >= resolution) continue;
		else{
			for (j = 0; j < 3; j++){
				if ((tempj + j) < 0 || (tempj + j) >= resolution) continue;
				else{
					for (k = 0; k < 3; k++){
						if ((tempk + k) < 0||(tempk+k)>=resolution) continue;
						else{
							std::list<int>::iterator iter;
							for (iter = grid->grid_watArr[tempi + i][tempj + j][tempk + k]->begin(); iter != grid->grid_watArr[tempi + i][tempj + j][tempk + k]->end(); iter++){
								if (waterArr[*iter]->position.x == pos.x && waterArr[*iter]->position.y == pos.y && waterArr[*iter]->position.z == pos.z) continue; //자기 자신일 경우 제외
								if (pos.distance(waterArr[*iter]->position) > scope) continue; //scope 밖에 있을때 제외
								//scope 안에 있을 때 배열 삽입
								surr_waterArr.push_back(*iter);
							}
						}
					}
				}
			}
		}
	}
	return;
}
void Water::surrounding_objmol(VECTOR<double> pos, VECTOR<int> gridnum){
	//for문을 돌면서 격자선택, scope 범위를 넘어가면 continue, scope범위 안에 있으면 배열에 삽입
	int tempi = gridnum.x - 1;
	int tempj = gridnum.y - 1;
	int tempk = gridnum.z - 1;
	int i, j, k;
	for (i = 0; i < 3; i++){
		if ((tempi + i) < 0 || (tempi + i) >= resolution) continue;
		else{
			for (j = 0; j < 3; j++){
				if ((tempj + j) < 0 || (tempj + j) >= resolution) continue;
				else{
					for (k = 0; k < 3; k++){
						if ((tempk + k) < 0 || (tempk + k) >= resolution) continue;
						else{
							std::list<int>::iterator iter;
							for (iter = grid->grid_objArr[tempi + i][tempj + j][tempk + k]->begin(); iter != grid->grid_objArr[tempi + i][tempj + j][tempk + k]->end(); iter++){
								if (objArr[*iter]->position.x == pos.x && objArr[*iter]->position.y == pos.y && objArr[*iter]->position.z == pos.z) continue; //자기 자신일 경우 제외
								if (pos.distance(objArr[*iter]->position) > scope) continue; //scope 밖에 있을때 제외
								//scope 안에 있을 때 배열 삽입
								surr_objArr.push_back(*iter);
							}
						}
					}
				}
			}
		}
	}
	return;
}

void Water::update_water_gridnum(water_molecule *wptr, int entity){
	//바뀐 pos가 속할 grid계산
	int temp = size / resolution;
	VECTOR<int> newgrid;	//(i,j,k)
	newgrid.z = (int)floor(wptr->position.x / temp);	//k
	newgrid.x = (int)floor(wptr->position.y / temp);	//i
	newgrid.y = (int)floor((-1 * wptr->position.z) / temp);	//j
	if (newgrid.x >= resolution) newgrid.x = resolution-1;
	if (newgrid.x < 0) newgrid.x = 0;

	if (newgrid.y >= resolution) newgrid.y = resolution-1;
	if (newgrid.y < 0) newgrid.y = 0;

	if (newgrid.z >= resolution) newgrid.z = resolution-1;
	if (newgrid.z < 0) newgrid.z = 0;
	//기존 grid와 같은지 확인, 같으면 return 다르면 변화
	if (wptr->gridnum.x == newgrid.x && wptr->gridnum.y == newgrid.y && wptr->gridnum.z == newgrid.z) return;
	else{
		//기존 gridnum을 보고 gird_watArr에서 해당하는 index를 제거
		(grid->grid_watArr[wptr->gridnum.x][wptr->gridnum.y][wptr->gridnum.z])->remove(entity);
		//newgrid에 해당하는 grid_watArr index에 entity update
		(grid->grid_watArr[newgrid.x][newgrid.y][newgrid.z])->push_back(entity);
		//water molecule의 gridnum정보 update
		wptr->gridnum.update(newgrid.x,newgrid.y,newgrid.z);
	}
	return;
}
void Water::update_obj_gridnum(obj_molecule *optr, int entity){
	//바뀐 pos가 속할 grid계산
	int temp = size / resolution;
	VECTOR<int> newgrid;	//(i,j,k)
	newgrid.z = (int)floor(optr->position.x / temp);	//k
	newgrid.x = (int)floor(optr->position.y / temp);	//i
	newgrid.y = (int)floor((-1 * optr->position.z) / temp);	//j
	if (newgrid.x >= resolution) newgrid.x = resolution - 1;
	if (newgrid.x < 0) newgrid.x = 0;

	if (newgrid.y >= resolution) newgrid.y = resolution - 1;
	if (newgrid.y < 0) newgrid.y = 0;

	if (newgrid.z >= resolution) newgrid.z = resolution - 1;
	if (newgrid.z < 0) newgrid.z = 0;
	//기존 grid와 같은지 확인, 같으면 return 다르면 변화
	if (optr->gridnum.x == newgrid.x && optr->gridnum.y == newgrid.y && optr->gridnum.z == newgrid.z) return;
	else{
		//기존 gridnum을 보고 gird_watArr에서 해당하는 index를 제거
		(grid->grid_objArr[optr->gridnum.x][optr->gridnum.y][optr->gridnum.z])->remove(entity);
		//newgrid에 해당하는 grid_watArr index에 entity update
		(grid->grid_objArr[newgrid.x][newgrid.y][newgrid.z])->push_back(entity);
		//water molecule의 gridnum정보 update
		optr->gridnum.update(newgrid.x, newgrid.y, newgrid.z);
	}
	return;
}
void Water::collisioncheck(water_molecule *wptr, Collision *coll){
	//물분자 - 물분자
	/*for (int i = 0; i < surr_waterArr.size(); i++){
		if (waterArr[surr_waterArr[i]]->position.x == wptr->position.x && waterArr[surr_waterArr[i]]->position.y == wptr->position.y && waterArr[surr_waterArr[i]]->position.z == wptr->position.z)
			continue;
		((Imp_Circle*)(coll->func[0]))->p = waterArr[surr_waterArr[i]]->position.x;
		((Imp_Circle*)(coll->func[0]))->q = waterArr[surr_waterArr[i]]->position.y;
		((Imp_Circle*)(coll->func[0]))->r = waterArr[surr_waterArr[i]]->position.z;
		((Imp_Circle*)(coll->func[0]))->d = waterArr[surr_waterArr[i]]->radius;
		double dx = 1.00;
		if (coll->is_collision(wptr->position.x, wptr->position.y, wptr->position.z)){
			VECTOR<double> df = coll->deltaf(wptr->position.x, wptr->position.y, wptr->position.z).operator*(dx);
			wptr->position = wptr->position + df;
			wptr->collision_handle(waterArr[surr_waterArr[i]]);
			break;	//충돌처리 하나만 해줘도 벗어날 수 있음
		}
	}*/

	//물분자 - obj 분자
	for (int i = 0; i < surr_objArr.size(); i++){
		((Imp_Circle*)(coll->func[0]))->p = objArr[surr_objArr[i]]->position.x;
		((Imp_Circle*)(coll->func[0]))->q = objArr[surr_objArr[i]]->position.y;
		((Imp_Circle*)(coll->func[0]))->r = objArr[surr_objArr[i]]->position.z;
		((Imp_Circle*)(coll->func[0]))->d = objArr[surr_objArr[i]]->radius;
		double dx = 1.00;
		if (coll->is_collision(wptr->position.x, wptr->position.y, wptr->position.z)){
			VECTOR<double> df = coll->deltaf(wptr->position.x, wptr->position.y, wptr->position.z).operator*(dx);
			wptr->position = wptr->position + df;
			wptr->collision_handle(objArr[surr_objArr[i]]);
			break;	//충돌처리 하나만 해줘도 벗어날 수 있음
		}
	}
	return;
}

void Water::collisioncheck(obj_molecule *optr, Collision *coll){
	//obj분자 - obj분자
	for (int i = 0; i < surr_objArr.size(); i++){
		if (objArr[surr_objArr[i]]->position.x == optr->position.x && objArr[surr_objArr[i]]->position.y == optr->position.y && objArr[surr_objArr[i]]->position.z == optr->position.z)
			continue;
		((Imp_Circle*)(coll->func[0]))->p = objArr[surr_objArr[i]]->position.x;
		((Imp_Circle*)(coll->func[0]))->q = objArr[surr_objArr[i]]->position.y;
		((Imp_Circle*)(coll->func[0]))->r = objArr[surr_objArr[i]]->position.z;
		((Imp_Circle*)(coll->func[0]))->d = objArr[surr_objArr[i]]->radius;
		double dx = 1.00;
		if (coll->is_collision(optr->position.x, optr->position.y, optr->position.z)){
			VECTOR<double> df = coll->deltaf(optr->position.x, optr->position.y, optr->position.z).operator*(dx);
			optr->position = optr->position + df;
			optr->collision_handle(objArr[surr_objArr[i]]);
			break;	//충돌처리 하나만 해줘도 벗어날 수 있음
		}
	}

	//obj분자 - 물 분자
	for (int i = 0; i < surr_waterArr.size(); i++){
		((Imp_Circle*)(coll->func[0]))->p = waterArr[surr_waterArr[i]]->position.x;
		((Imp_Circle*)(coll->func[0]))->q = waterArr[surr_waterArr[i]]->position.y;
		((Imp_Circle*)(coll->func[0]))->r = waterArr[surr_waterArr[i]]->position.z;
		((Imp_Circle*)(coll->func[0]))->d = waterArr[surr_waterArr[i]]->radius;
		double dx = 1.00;
		if (coll->is_collision(optr->position.x, optr->position.y, optr->position.z)){
			VECTOR<double> df = coll->deltaf(optr->position.x, optr->position.y, optr->position.z).operator*(dx);
			optr->position = optr->position + df;
			optr->collision_handle(waterArr[surr_waterArr[i]]);
			break;	//충돌처리 하나만 해줘도 벗어날 수 있음
		}
	}

	return;
}
void Water::clearArray(){
	std::vector<int>().swap(surr_waterArr);
	std::vector<int>().swap(surr_objArr);
	return;
}