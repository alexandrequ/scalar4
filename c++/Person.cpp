#include "Person.h"


Person::Person(int age): age(age){

}

Person::Person(string& name){
	firstName=string(name);
}

Person::~Person(){
}

void Person::display(ostream& flux){
	flux<<"nom:"<<this->firstName<<endl<<"age:"<<age;
}

ostream& operator<<(ostream& flux, Person& person){
	person.display(flux);
	return flux;
}

void changeName(Person& person){
	string nom("jeanmi");
	person.firstName=nom;
}
