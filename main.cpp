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
	vector<int> good;
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
	ifstream file_1("parks.txt");
//	ifstream file_2("fastfood.txt");
	double d;
	parks.clear();
	fastfood.clear();
	while(file_1 >> d)
	{
		point p;
		p.first = d;
		file_1 >> d;
		p.second = d;

		int str_num;	
		char c;
		while (file_1 >> c)
		{
			if (c  == '#')
				break;
			else if (c == ' ')
				continue;

			file_1.putback(c);
			file_1 >> str_num;
			p.good.push_back(str_num);
		}

		string s;
		file_1 >> s;
		parks.push_back(p);
	}

/*
	while(file_2 >> d)
	{
		string a;
		point p;
		p.first = d;
		file_2 >> d;
		p.second = d;
		file_2 >> a;
		fastfood.push_back(p);
	}
*/
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
	load();

	vector<point> coords = {parks[0]};
	int p = 0;
	for(int j = 0;j < 4;++j)
	{
		p = parks[p].good[rand() % parks[p].good.size()];
		cout << p << endl;
		for(size_t i = 0;i < parks.size();++i)
		{
			vector<int>::iterator used = find(parks[i].good.begin(), parks[i].good.end(), p);
			if(used != parks[i].good.end())
				parks[i].good.erase(used);
		}
		coords.push_back(parks[p]);
	}
	coords.push_back(parks[0]);

	//dst
	double dst = 0;
	for (size_t i = 0; i < coords.size()-1; ++i)
		dst += calcDist(coords[i], coords[i+1]);
	dst *= 1.2;

	// url
	string url = "https://www.google.ru/maps/dir/";
	for (size_t i = 0; i < coords.size(); ++i)
		url += getCoords(coords[i]);
	url += "data=!3m1!4b1!4m2!4m1!3e2"; // magic, don't edit

	bot.getApi().sendMessage(msg->chat->id, "Заезд на " +/* to_string(int(ceil(dst))) */to_string(readDis) + " км.\n" + url);
}

int main()
{
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
