all:
	clang++ -Wall -Wextra main.cpp -o bot --std=c++11 -I/usr/local/include -lTgBot -lboost_system -lssl -lcrypto -lpthread

run: all
	torsocks ./bot

commit:
	git add --a
	git commit
	git push origin master
