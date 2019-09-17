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

class point
{
public:
	double first;
	double second;
};

vector<point> fastfood;
vector<point> parks;
int readDis;
const double radius = 6371;
const double pi = 3.141592/180;
const string token = "648697435:AAFBxZcZf_Y9Uxt2DYm0aqa-hoNp22hAf9E";

Bot bot(token);

void load()
{
	ifstream file_1("places.txt");
	double d;
	while(file_1 >> d)
	{
		point p;
		p.first = d;
		file_1 >> d;
		p.second = d;
		parks.push_back(p);
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
	bot.getApi().sendMessage(msg->chat->id, "Distance is: " + msg->text);
	readDis = atoi(msg->text.c_str());
}

void onFastfood(Message::Ptr msg)
{
	string text;

	for (size_t i = 0; i < fastfood.size(); ++i)
		text += to_string(calcDist(parks[0], fastfood[i])) + "\n";		

	bot.getApi().sendMessage(msg->chat->id, "Distance to fastfood:\n " + text);
}

void onParks(Message::Ptr msg)
{
	string text;

	for (size_t i = 0; i < parks.size(); ++i)
		text += to_string(calcDist(parks[0], parks[i])) + "\n";		

	bot.getApi().sendMessage(msg->chat->id, "Distance to parks:\n " + text);
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
	vector<point> tmp_parks = parks;
	tmp_parks.erase(tmp_parks.begin());
	vector<point> coords = {parks[0]};

	for(int j = 0;j < 4;++j)
	{
		int p = rand() % tmp_parks.size();
		coords.push_back(tmp_parks[p]);
		tmp_parks.erase(tmp_parks.begin() + p);
	}

	placesSort(coords);
	coords.push_back(parks[0]);

	//dst
	double dst = 0;
	for (size_t i = 0; i < coords.size()-1; ++i)
		cout << corrds[i].first << ' ' << coords.second << endl;

	// url
	string url = "https://www.google.ru/maps/dir/";
	for (size_t i = 0; i < coords.size(); ++i)
		url += getCoords(coords[i]);
	url += "data=!3m1!4b1!4m2!4m1!3e2"; // magic, don't edit
	
	bot.getApi().sendMessage(msg->chat->id, "Заезд на " +/* to_string(int(ceil(dst))) */to_string(readDis) + " км.\n" + url);
}

int main()
{
	load();

	bot.getEvents().onNonCommandMessage([](Message::Ptr message) { onNonCommandMessage(message); });
	bot.getEvents().onCommand("start", [](Message::Ptr message) { onStart(message); });
	bot.getEvents().onCommand("fastfood", [](Message::Ptr message) { onFastfood(message); });
	bot.getEvents().onCommand("parks", [](Message::Ptr message) { onParks(message); });
	bot.getEvents().onCommand("route", [](Message::Ptr message) { onRoute(message); });

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
