#include <iostream>
//#include <tr1/memory>
#include <set>
//#include <vector>

//using namespace std;
//using std::tr1::shared_ptr;


template<class T>
inline T & GetStdSetElement(std::_Rb_tree_const_iterator<T>  std_set_iterator)
{
	return *(T *)&(*std_set_iterator);
}



int main()
{	
	using namespace std;

	set<int> iset;
	pair< set<int>::iterator, bool> res = iset.insert(4);
	
	int * p = &GetStdSetElement(res.first);
	(*p)++;
	
	cout << *( iset.begin() ) << endl;
	
	return 0;
}
