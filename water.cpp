#include "water.h"
#include <stdio.h>
#include <vector>

#include "grid.h"
#include "waterMolecule.h"
#include "objMolecule.h"
#include "implicit.h"

void Water::init(int resolution, int size){
	int i, j, k;
	//scope �� ����
	scope = (size / resolution) * 1.5;
	(*this).resolution = resolution;
	(*this).size = size;
	//���� ������
	grid = new Grid();
	grid->init(resolution);

	//���ڸ��� ���鼭 ������ index�� �ϳ��� �����ϴ� �̴ϴ�.
	//�׿� ���ÿ� water array�� �����ڸ� �����մϴ�.	
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

	//���ڸ��� ���鼭 �ʿ��� ���ڿ� obj���� index�� �����Ѵ�.
	//�׿� ���ÿ� obj array�� obj�� �����Ѵ�.
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
	//******* ������ update *******
	int total_molecules = resolution*resolution*resolution; //��ü ���ڼ�

	for (int i = 0; i < waterArr.size(); i++){
	//grid���� �ֺ��� ������ �迭 �޾ƿ���
		surrounding_watermol(waterArr[i]->position, waterArr[i]->gridnum);
		//(grid���� �ֺ��� obj �迭 �޾ƿ���)
		surrounding_objmol(waterArr[i]->position, waterArr[i]->gridnum);
	//�ֺ� �����ڿ� ���� force�� ���
		for (int j = 0; j < surr_waterArr.size(); j++)
			waterArr[i]->watermol_force_integrate(waterArr[surr_waterArr[j]], scope); //parameter�� pointer��
		//(�ֺ� obj���ڿ� ���� force�հ��)
		for (int j = 0; j < surr_objArr.size(); j++)
			waterArr[i]->watermol_force_integrate(objArr[surr_objArr[j]], scope); //parameter�� pointer��
	//�µ��� ���� �������� update
		waterArr[i]->update_temper(temperature, total_molecules);
	//����
		waterArr[i]->final_integrate(dt, size);
	//�ֺ� ���� �迭 clear
		clearArray();
	//�ٲ� pos������ gridnum update(���� �̵�& ���� �� ���� ��ȭ)
		update_water_gridnum(waterArr[i], i);
	//update�� pos�� ���� ������ �迭 �޾ƿ���
		surrounding_watermol(waterArr[i]->position, waterArr[i]->gridnum);
		//(update�� pos�� ���� obj �迭 �޾ƿ���)
		surrounding_objmol(waterArr[i]->position, waterArr[i]->gridnum);
	//update�� pos���� collision check --> collision check �� ������ - obj ���� ���̿��� ���� �ʿ伺�� ����
		collisioncheck(waterArr[i], coll);
	//�ֺ� ���� �迭 clear
		clearArray();
	}
	total_molecules = 100;
	//*******obj���� update *******
	if (putObj == 1){
		for (int i = 0; i < objArr.size(); i++){
			//grid���� �ֺ��� obj���� �迭 �޾ƿ���
			surrounding_objmol(objArr[i]->position, objArr[i]->gridnum);
			//(grid���� �ֺ��� ������ �迭 �޾ƿ���)
			surrounding_watermol(objArr[i]->position, objArr[i]->gridnum);

			//�ֺ� obj�ڿ� ���� force�� ���
			for (int j = 0; j < surr_objArr.size(); j++)
				objArr[i]->objmol_force_integrate(objArr[surr_objArr[j]], scope); //parameter�� pointer��
			//�߷�, �η� ���
			objArr[i]->update_gravity();

			//(�ֺ� �����ڿ� ���� force�հ��) -->�е� ���� ����Ѵ�.
			for (int j = 0; j < surr_waterArr.size(); j++)
				objArr[i]->objmol_force_integrate(waterArr[surr_waterArr[j]],density, scope); //parameter�� pointer��
			//�µ��� ���� �������� update
			objArr[i]->update_temper(temperature, total_molecules);
			//����
			objArr[i]->final_integrate(dt, size);
			//�ֺ� ���� �迭 clear
			clearArray();
			//�ٲ� pos������ gridnum update(���� �̵�& ���� �� ���� ��ȭ)
			update_obj_gridnum(objArr[i], i);
			//update�� pos�� ���� obj���� �迭 �޾ƿ���
			surrounding_objmol(objArr[i]->position, objArr[i]->gridnum);
			//(update�� pos�� ���� ������ �迭 �޾ƿ���)
			surrounding_watermol(objArr[i]->position, objArr[i]->gridnum);
			//update�� pos���� collision check --> collision check �� ������ - obj ���� / obj- obj���ڻ��̿��� ���� �ʿ伺�� ����
			collisioncheck(objArr[i], coll);
			//�ֺ� ���� �迭 clear
			clearArray();
		}
	}
	return;
}

void Water::surrounding_watermol(VECTOR<double> pos, VECTOR<int> gridnum){
	//for���� ���鼭 ���ڼ���, scope ������ �Ѿ�� continue, scope���� �ȿ� ������ �迭�� ����
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
								if (waterArr[*iter]->position.x == pos.x && waterArr[*iter]->position.y == pos.y && waterArr[*iter]->position.z == pos.z) continue; //�ڱ� �ڽ��� ��� ����
								if (pos.distance(waterArr[*iter]->position) > scope) continue; //scope �ۿ� ������ ����
								//scope �ȿ� ���� �� �迭 ����
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
	//for���� ���鼭 ���ڼ���, scope ������ �Ѿ�� continue, scope���� �ȿ� ������ �迭�� ����
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
								if (objArr[*iter]->position.x == pos.x && objArr[*iter]->position.y == pos.y && objArr[*iter]->position.z == pos.z) continue; //�ڱ� �ڽ��� ��� ����
								if (pos.distance(objArr[*iter]->position) > scope) continue; //scope �ۿ� ������ ����
								//scope �ȿ� ���� �� �迭 ����
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
	//�ٲ� pos�� ���� grid���
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
	//���� grid�� ������ Ȯ��, ������ return �ٸ��� ��ȭ
	if (wptr->gridnum.x == newgrid.x && wptr->gridnum.y == newgrid.y && wptr->gridnum.z == newgrid.z) return;
	else{
		//���� gridnum�� ���� gird_watArr���� �ش��ϴ� index�� ����
		(grid->grid_watArr[wptr->gridnum.x][wptr->gridnum.y][wptr->gridnum.z])->remove(entity);
		//newgrid�� �ش��ϴ� grid_watArr index�� entity update
		(grid->grid_watArr[newgrid.x][newgrid.y][newgrid.z])->push_back(entity);
		//water molecule�� gridnum���� update
		wptr->gridnum.update(newgrid.x,newgrid.y,newgrid.z);
	}
	return;
}
void Water::update_obj_gridnum(obj_molecule *optr, int entity){
	//�ٲ� pos�� ���� grid���
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
	//���� grid�� ������ Ȯ��, ������ return �ٸ��� ��ȭ
	if (optr->gridnum.x == newgrid.x && optr->gridnum.y == newgrid.y && optr->gridnum.z == newgrid.z) return;
	else{
		//���� gridnum�� ���� gird_watArr���� �ش��ϴ� index�� ����
		(grid->grid_objArr[optr->gridnum.x][optr->gridnum.y][optr->gridnum.z])->remove(entity);
		//newgrid�� �ش��ϴ� grid_watArr index�� entity update
		(grid->grid_objArr[newgrid.x][newgrid.y][newgrid.z])->push_back(entity);
		//water molecule�� gridnum���� update
		optr->gridnum.update(newgrid.x, newgrid.y, newgrid.z);
	}
	return;
}
void Water::collisioncheck(water_molecule *wptr, Collision *coll){
	//������ - ������
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
			break;	//�浹ó�� �ϳ��� ���൵ ��� �� ����
		}
	}*/

	//������ - obj ����
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
			break;	//�浹ó�� �ϳ��� ���൵ ��� �� ����
		}
	}
	return;
}

void Water::collisioncheck(obj_molecule *optr, Collision *coll){
	//obj���� - obj����
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
			break;	//�浹ó�� �ϳ��� ���൵ ��� �� ����
		}
	}

	//obj���� - �� ����
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
			break;	//�浹ó�� �ϳ��� ���൵ ��� �� ����
		}
	}

	return;
}
void Water::clearArray(){
	std::vector<int>().swap(surr_waterArr);
	std::vector<int>().swap(surr_objArr);
	return;
}