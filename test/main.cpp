#include <iostream>
#include <tr1/memory>
#include <set>
#include <map>
#include <vector>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <arpa/inet.h>


using namespace std;
using std::tr1::shared_ptr;




int main()
{	
	char s[4];
	s[0] = 0;
	s[1] = 0;
	s[2] = 3;
	s[3] = 177;

	int  i = *(int *)s;
	i = ntohl(i);
	cout << i << endl;
	
	return 0;
}
