#include "Manager.h"
using namespace std;

int main() {
    Manager manager(3);  // value is order of bptree
    manager.run("command.txt");

    // manager.LOAD();
    // manager.ADD_BP("luis", 100, 230079, 5000);
    // manager.ADD_BP("alex", 200, 210038, 5900);
    // manager.ADD_BP("ryan", 300, 220094, 8200);
    // manager.ADD_BP("steven", 400, 170027, 9700);
    // manager.SEARCH_BP_NAME("alex");
    // manager.SEARCH_BP_RANGE("c", "g");
    // manager.PRINT_BP();
    // manager.ADD_ST_DEPTNO(100);
    // manager.ADD_ST_NAME("steven");
    // manager.ADD_ST_NAME("ryan");
    // manager.ADD_ST_NAME("alex");
    // manager.PRINT_ST(100);
    // manager.DELETE();
    // manager.PRINT_ST(100);

    return 0;
}