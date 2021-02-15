#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <iostream>

using namespace std;

class Person{
	public:
	int age;
	string name;
	string firstName;
	
	Person(int age);
	Person(string& name);
	~Person();
	void display(ostream& flux);
		
};

ostream& operator<<(ostream& flux, Person& person);

void changeName(Person& person);
#endif
