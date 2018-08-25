#include <csignal>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>

#include <tgbot/tgbot.h>

using namespace std;
using namespace TgBot;

typedef pair<double, double> point;

vector<point> places;
const double radius = 6371;
const double pi = 3.141592/180;
const string token = "648697435:AAFBxZcZf_Y9Uxt2DYm0aqa-hoNp22hAf9E";
point meet = {55.690673, 37.499047};

Bot bot(token);

void load()
{
	ifstream file("places.txt");

	double d;
	while(file >> d)
	{
		point p;
		p.first = d;
		file >> d;
		p.second = d;
		places.push_back(p);
	}
}

double calcDist(point p1, point p2)
{
	double dphi = (p1.first - p2.first)*pi;
	double dpsi = (p1.second - p2.second)*pi;

	double s1 = sin(dphi/2)*sin(dphi/2);
	double s2 = sin(dpsi/2)*sin(dpsi/2);
	double c = cos(p1.first*pi)*cos(p2.first*pi);

	return radius*2*asin(sqrt(s1 + c*s2));
}

void onStart(Message::Ptr msg)
{
	bot.getApi().sendMessage(msg->chat->id, "Hi!");
}

void onNonCommandMessage(Message::Ptr msg)
{
	cout << "User wrote " << msg->text << endl;
	bot.getApi().sendMessage(msg->chat->id, "Your msg is: " + msg->text);
}

void onPlaces(Message::Ptr msg)
{
	string text;

	for (size_t i = 0; i < places.size(); ++i)
		text += to_string(calcDist(meet, places[i])) + "\n";		

	bot.getApi().sendMessage(msg->chat->id, "Distance to macs:\n " + text);
}

string getCoords(point p)
{
	return to_string(p.first) + "," + to_string(p.second) + "/";
}

void placesSort(vector<point> &v)
{
	for(size_t i = 0;i < v.size()-1;++i)
	{
		double dis = calcDist(v[i], v[i+1]);
		int m = i + 1;
		for(size_t j = i + 1 ;j < v.size();++j)
			if(dis > calcDist(v[i], v[j]))
			{
				dis = calcDist(v[i], v[j]);
				m=j;
			}
		swap(v[i+1], v[m]);
	}	
}

void onRoute(Message::Ptr msg)
{
	string url = "https://www.google.ru/maps/dir/";

	int n1 = rand() % places.size();
	int n2 = rand() % places.size();
	while (n1 == n2)
		n2 = rand() % places.size();
	int n3 = rand() % places.size();
	while (n3 == n1 || n3 == n2)
		n3 = rand() % places.size();
	int n4 = rand() % places.size();
	while (n4 == n1 || n4 == n2 || n4 == n3)
		n4 = rand() % places.size();
	int n5 = rand() % places.size();
	while (n5 == n1 || n5 == n2 || n5 == n3 || n5 == n4)
		n5 = rand() % places.size();

	vector<point> coords = {meet, places[n1], places[n2], places[n3], places[n4], places[n5]};

	placesSort(coords);

	double dst = 0;
	for (size_t i = 0; i < coords.size()-1; ++i)
		dst += calcDist(coords[i], coords[i+1]);
	dst += calcDist(coords[coords.size()-1], meet);
	dst *= 1.2;

	for (size_t i = 0; i < coords.size(); ++i)
		url += getCoords(coords[i]);
	url += getCoords(meet);

	url += "data=!3m1!4b1!4m2!4m1!3e2"; // magic, don't edit
	bot.getApi().sendMessage(msg->chat->id, "Заезд на " + to_string(int(ceil(dst))) + " км.\n" + url);
}

int main()
{
	bot.getEvents().onNonCommandMessage([](Message::Ptr message) { onNonCommandMessage(message); });
	bot.getEvents().onCommand("start", [](Message::Ptr message) { onStart(message); });
	bot.getEvents().onCommand("places", [](Message::Ptr message) { onPlaces(message); });
	bot.getEvents().onCommand("route", [](Message::Ptr message) { onRoute(message); });

	load();
	srand(time(0));

	try 
	{
		cout << "Bot username: " << bot.getApi().getMe()->username << endl;
		bot.getApi().deleteWebhook();

		TgLongPoll longPoll(bot);
		cout << "Long poll started" << endl;
		while (true) 
			longPoll.start();
	} 
	catch (exception& e) 
	{
		cout << "error: " << e.what() << endl;
	}
}
