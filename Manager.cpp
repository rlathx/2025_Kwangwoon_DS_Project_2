#include "Manager.h"

void Manager::run(const char* command) {

}

void Manager::LOAD() {

}

void Manager::ADD_BP() {

}

void Manager::SEARCH_BP_NAME(string name) {

}

void Manager::SEARCH_BP_RANGE(string start, string end) {

}

void Manager::ADD_ST_DEPTNO(int dept_no) {

}

void Manager::ADD_ST_NAME(string name) {

}

void Manager::PRINT_BP() {

}

void Manager::PRINT_ST() {

}

void Manager::DELETE() {

}

void Manager::printErrorCode(int n) {
	flog << "========ERROR========\n";
	flog << n << "\n";
	flog << "=====================\n\n";
}

void Manager::printSuccessCode(string success) {
	flog << "========" << success << "========\n";
	flog << "Success" << "\n";
	flog << "====================\n\n";
}