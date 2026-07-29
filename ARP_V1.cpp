#include <iostream>
#include <cmath>

using namespace std;

int main()
{
	double Rm, momentum, volitality, Rasset, m1, m2, m3, c, mt0, mt1, ret1, ret2, ret3, mean, variance;
	int k=3;
	char answer = 'y';
	
	cout << "This is a program used to predict asset return based on 3 crucial factors; market returns, momentum and volitality." << endl;
	
	while (answer == 'y'){
	
	cout << "Enter coefficient m1, m2, m3 and c respectively: " << endl;
	cin >> m1 >> m2 >> m3 >> c;
	
	cout << "Enter market index price today: " << endl;
	cin >> mt1;
	cout << "Enter market index price yesterday: " << endl;
	cin >> mt0;
	
	Rm = (mt1-mt0)/mt0;
	
	for (int i=1; i<k; i++) {
		cout << "Enter past three returns: ";
		cin >> ret1 >> ret2 >> ret3;
		momentum = ret1 + ret2 + ret3;
		
	}
	
	
	mean= (ret1 + ret2 + ret3)/3;
	
    variance=(pow((ret1 - mean), 2) +
    pow((ret2 - mean), 2) +
    pow((ret3 - mean), 2)) / 3;
    
	volitality= pow(variance, 0.5);
	
	Rasset= (m1*Rm)+(m2*momentum)+(m3*volitality);
	
	cout << "Predicted asset return is " << Rasset << "." << endl;
	cout << "Market return is used to measure how overall market is moving, momentum to observe trend and volatility to see asset fluctuations. " << endl;
	
	cout << "Do you want to make another calculation? y/n:";
	cin >> answer;
	
	
}
	
	return 0;
	
	
}
