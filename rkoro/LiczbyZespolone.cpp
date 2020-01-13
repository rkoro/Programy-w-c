
#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <conio.h>
#include <math.h>
#include <string>
using namespace std;

class complex
{
	private:
		float re;
		float im;
	public:
		float getRe()
		{
			return re;
		}
		float getIm()
		{
			return im;
		}
		
		complex operator+(complex &a)
		{
			complex wynik;
			wynik.im=this->im + a.im;
			wynik.re=this->re + a.re;
			return wynik;
		}
		complex operator*(complex &a)
		{
			complex wynik;
			wynik.re=this->re * a.re -this->im * a.im;
			wynik.im=this->re * a.im +this->im * a.re;
			return wynik;
		}
		complex random_cmplx(complex &i)
		{
			i.re=rand()%10-5;
			i.im=rand()%10-5;
			return i;
		}
};

ostream& operator<<(std::ostream& stream,complex &liczba)
		{
			if(liczba.getIm() <0)
				stream<<liczba.getRe()<<"-"<<(-1)*liczba.getIm()<<"i";
			else
				stream<<liczba.getRe()<<"+"<<liczba.getIm()<<"i";
			return stream;
		}


int _tmain(int argc, _TCHAR* argv[])
{

	while(1)
	{
		cout<<"Wyswietlanie losowych liczb zespolonych"<<endl;
		complex j[4];
		for(int i=0;i<4;i++)
		{
		cout<<endl<<"z"<<i<<"= ";
		j[i].random_cmplx(j[i]);
		cout<<j[i];
		}
		complex suma[2];
		cout<<endl<<endl<<"Przykladowe dodawanie"<<endl;
		for(int i=0;i<2;i++)
		{
			suma[i]=j[i]+j[i+1];   
		}
		cout<<"z1+z2= "<<suma[0]<<endl;
		cout<<"z1+z2= "<<suma[1]<<endl;

		cout<<endl<<"Przykladowe mnozenie"<<endl;
		for(int i=0;i<2;i++)
		{
			suma[i]=j[i]*j[i+1];
		}
		cout<<"z1*z2= "<<suma[0]<<endl;
		cout<<"z3*z4= "<<suma[1]<<endl;
		break;
		-
	printf("\nWcisnij e aby wyjsc, cokolwiek aby kontynuowac.\n");
	dec=_getch();
	if(dec=='e')
		break;
	else
		continue;
	}
	return 666;
}
