#include <stdio.h>
#pragma map(calla, "CALLA")
extern int calla(int num1, int num2, int num3, int num4);


int CALLEE(int a, int b, int c, int d){
	int sum = calla(a,b,c,d);
    return sum;
}
