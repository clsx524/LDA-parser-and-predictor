#ifndef _DATABASE_H
#define	_DATABASE_H

#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <driver/mysql_public_iface.h>
#include "utils.h"
#include "strtokenizer.h"

using namespace std;
using namespace sql;

// Connection properties
#define CONNECT_DB   "media"
#define CONNECT_HOST "unix:///usr/local/zend/mysql/tmp/mysql.sock" //"tcp://127.0.0.1:3306"
#define CONNECT_USER "xin"
#define CONNECT_PASS "1"
#define CONNNCT_PATH "data"
#define CONNECT_USERTBL "users"

class database {
private:
	string path;
	string url;
	string user;
	string password;
	string db;
	string usertbl;
	Driver * driver;
public:
	database() {
		init("");
		driver = sql::mysql::get_driver_instance();
	}
	~database() {}

	void init(const string& pt) {
		path = pt;
		url = CONNECT_HOST;
		user = CONNECT_USER;
		password = CONNECT_PASS;
		db = CONNECT_DB;
		usertbl = CONNECT_USERTBL;
	}

	void init(const string& pt, const string& ur, const string& u, const string& p, const string& d) {
		path = pt;
		url = ur;
		user = u;
		password = p;
		db = d;	
	}

	bool initDatabase();
	bool addUser(const string& name, const string& pwd) const;
	bool login(const string& name, const string& pwd) const;
	bool changePassword(const string& name, const string& origin, const string& pwd) const;
	vector<int> search(const string& query, int num, int pos) const;
	string preciseFetch(int index) const;
	vector<string> fetchPic(int index);
	vector<int> fetchLatest(int num);
	vector<int> hotCollect(string username, int num);
	vector<int> hotTypeCollect(string username, int num, string type);
	vector<int> hotTypeCollectWithType(string username, int num, string type, string category);
	vector<int> FavoriteCollect(string username, int num);
	bool addComment(string username, int number, string comment, bool favor, int score);
	pair<string,string> getInfo(string table, int number);
};

#endif
