#include <iostream>
#include <string>
#include "Person.h"
#include <vector>

using namespace std;

vector<int>* multiply(vector<int>& x){
	vector<int> *result = new vector<int>(2,3);
	for(int i=0;i<x.size();i++){
		x[i]=x[i]*2;
	}
	return result;
}

int main(int argc, char** argv){

	vector<int> x = vector<int>();
	x.push_back(7);
	x.push_back(1);
	
	vector<int>* y = multiply(x);
	
	cout<<"[";
	for(int i=0;i<x.size()-1;i++){
		cout<<x[i]<<",";
	}
	cout<<x[x.size()-1]<<"]"<<endl;
	x=vector<int>();
	cout<<x.size()<<endl;
	cout<<y->size()<<endl;
	delete y;
	return 0;
}
