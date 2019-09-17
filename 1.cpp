#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cassert>
#include <exception>
#include <cstdlib>
#include <cassert>

#define PI 3.141592

using namespace std;

class point
{
	public:
		double first;
		double second;
};

vector<point> firstPush;
vector<point> places;
const double radius = 6371;

double calcDist(point p1, point p2) // Вычисляем расстояние между точками через координаты
{
	double dphi = (p1.first - p2.first) * PI;
	double dpsi = (p1.second - p2.second) * PI;

	double s1 = sin(dphi / 2) * sin(dphi / 2);
	double s2 = sin(dpsi / 2) * sin(dpsi / 2);
	double c = cos(p1.first * PI) * cos(p2.first * PI);

	return radius * 2 * asin(sqrt(s1 + c * s2));
}

void placesSort(vector<point> &v) // Сортируем массив по принципу нахождения ближайшей точки
{
	for(size_t i = 0; i < v.size()-1; ++i)
	{
		double dis = calcDist(v[i], v[i+1]);
		int m = i + 1;
		for(size_t j = i + 1 ; j < v.size(); ++j)
			if(dis > calcDist(v[i], v[j]))
			{
				dis = calcDist(v[i], v[j]);
				m = j;
			}
		swap(v[i+1], v[m]);
	}	
}

void createFirstPush() // Создаем массив со всеми координатами
{
	ifstream file("places.txt");
	double readNum;
	while(file >> readNum)
	{
		point p;
		p.first = readNum;
		file >> readNum;
		p.second = readNum;
		firstPush.push_back(p);
	}
}

int regen(int num, vector<int> numbers, vector<point> pl) // Создаем рандомное число, которого пока не было
{
	bool check_1 = 0;
	bool check_2 = 1;
	while(check_1 != 1)
	{
		for(int i = 0; i < numbers.size(); ++i)
		{
			if(num == numbers[i]) {num = rand() %pl.size(); check_2 = 0;}
			if(i == numbers.size() - 1 && check_2 == 1) check_1 = 1;
		}
		check_2 = 1;
	}
	return num;
}

void gen(int placesSize, vector<point> fromPush) // Создание массива(маршрута)
{
	vector<int> numbers;
	int num=rand() %fromPush.size();
	int placesNum = 1;
	while(placesNum <= placesSize - 2)
	{
		places.push_back(fromPush[num]);
		numbers.push_back(num);
		num = regen(num, numbers, fromPush);
		++placesNum;
	}
}

bool checkIntersection(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) // Проверка на пересечение двух отрезков
{
	double Ua, Ub, numerator_a, numerator_b, denominator;
	denominator=(y4-y3)*(x1-x2)-(x4-x3)*(y1-y2);
	if (denominator == 0){
		if ( (x1*y2-x2*y1)*(x4-x3) - (x3*y4-x4*y3)*(x2-x1) == 0 && (x1*y2-x2*y1)*(y4-y3) - (x3*y4-x4*y3)*(y2-y1) == 0)
			return 1;
		else return 0;
	}
	else{
		numerator_a=(x4-x2)*(y4-y3)-(x4-x3)*(y4-y2);
		numerator_b=(x1-x2)*(y4-y2)-(x4-x2)*(y1-y2);
		Ua=numerator_a/denominator;
		Ub=numerator_b/denominator;
		if(Ua >=0 && Ua <=1 && Ub >=0 && Ub <=1)
			return 1;
		else return 0;
	}
}

double angle(double x1, double y1, double x2, double y2, double x3, double y3) // Угол между векторами
{ 
	return acos((x1-x3)*(x2-x3)+((y1-y3)*(y2-y3))/(sqrt((x1-x3)*(x1-x3)+(y1-y3)*(y1-y3))*sqrt((x2-x3)*(x2-x3)+(y2-y3)*(y2-y3))))*180/PI;
}

int main(int argc, char *argv[])
{
	cout.precision(6);
	int k; cin >> k;
	point start;
	start.first = 55.690673; 
	start.second = 37.499047;
	places.push_back(start);
	createFirstPush();
	gen(k, firstPush);
	placesSort(places);
	places.push_back(start);
	bool check1 = 0;
	bool check2 = 0;
	for(;;)
	{
		check1 = 0;
		check2 = 0;
		for(int i = 0; i < places.size() - 1; ++i)
			for(int j = i + 2; j < places.size() - 2; ++j)
				if(checkIntersection(places[i].first, places[i].second, places[i+1].first, places[i+1].second, places[j].first, places[j].second, places[j+1].first, places[j+1].second) == 1)
					check1 = 1;
		for(int i=1; i<places.size(); ++i)
			if(abs(angle(places[i-1].first, places[i-1].second, places[i+1].first, places[i+1].second, places[i].first, places[i].second)-angle(places[i].first, places[i].second, places[i+2].first, places[i+2].second, places[i+1].first, places[i+1].second)) > 30)
				check2 = 1;
		if(check1 == 0 && check2 == 0)
		{
			for(int i = 0; i < places.size(); ++i)
				cout << fixed << places[i].first << ' ' << places[i].second << endl;
			break;
		}
		else 
		{
			for(int i=0; i<places.size(); ++i)
				cout << angle(places[i].first, places[i].second, places[i+1].first, places[i+1].second, places[i+2].first, places[i+2].second) << endl;
			places.clear();
			places.push_back(start);
			gen(k, firstPush);
			placesSort(places);
			places.push_back(start);
		}	
	}
}
/*
   bool checkRoute(vector<point> completePush) // Проверка на выпуклость маршрута 
   {
   bool bulge = true; // Полагаем многоугольник выпуклым (bulge-выпуклость)
   double t = completePush[completePush.size()-1].first * completePush[0].second - completePush[0].first * completePush[completePush.size()-1].second;
   double z = t / abs(t);
   double p = 1;
   for(int i = 0; i < completePush.size() - 1; ++i)
   {
   double r = completePush[i].first * completePush[i+1].second - completePush[i+1].first * completePush[i].second;
   p = p * z * r / abs(r);
   if(p < 0)
   {
   bulge = false;
   break;
   }
   }	
   return bulge;
   }
   */
