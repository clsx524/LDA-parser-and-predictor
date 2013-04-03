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

// #if __STDC_VERSION__ < 199901L
// #  if __GNUC__ >= 2
// #    define EXAMPLE_FUNCTION __FUNCTION__
// #  else
// #    define EXAMPLE_FUNCTION "(function n/a)"
// #  endif
// #elif defined(_MSC_VER)
// #  if _MSC_VER < 1300
// #    define EXAMPLE_FUNCTION "(function n/a)"
// #  else
// #    define EXAMPLE_FUNCTION __FUNCTION__
// #  endif
// #elif (defined __func__)
// #  define EXAMPLE_FUNCTION __func__
// #else
// #  define EXAMPLE_FUNCTION "(function n/a)"
// #endif

// #ifndef __LINE__
//   #define __LINE__ "(line number n/a)"
// #endif

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
	bool addUser(const string& name, const string& pwd);
	bool login(const string& name, const string& pwd);
	bool search(const string& query);

};




#endif
