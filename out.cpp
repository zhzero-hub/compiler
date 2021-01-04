#include <iostream>
using namespace std;

int main(void)
{

	int x;
	int div;
      	int rem;
 
	x = 100;

      	while (x > 0)
     	{
           		div = x/10;
	   	rem = x-(div*10);
           		cout << rem << endl;
	   	x = div;
      	}
	return 0;
}

