#include "database.h"

bool database::initDatabase() {
	stringstream sql;
	stringstream msg;
	string pathPic;
    
	cout << "#Starting ... " << endl;
    //sql::Driver * driver = sql::mysql::get_driver_instance();
    std::auto_ptr< sql::Connection > con(driver->connect(url, user, password));
    std::auto_ptr< sql::Statement > stmt(con->createStatement());
    
    /* Create a test table demonstrating the use of sql::Statement.execute() */
    stmt->execute("USE " + db);
    stmt->execute("DROP TABLE IF EXISTS media_info");
    stmt->execute("CREATE TABLE media_info (title VARCHAR(100), category VARCHAR(20), year CHAR(4), length SMALLINT UNSIGNED, director VARCHAR(50), cast VARCHAR(500), content VARCHAR(2000), wiki VARCHAR(10000), pic VARCHAR(200), type VARCHAR(10))");
    cout << "#\t media_info table created" << endl;
    
    cout << "#Starting add files ... " << endl;
    vector<string> pathset;
    strtokenizer strtok;
    
    vector<string>::size_type size = 0;
    utils::addfile(path, pathset, size);
    
    /* Populate the test table with data */
    try {
    	for (vector<string>::size_type i = 0; i < size; i++) {
        	utils::readfile(pathset[i], strtok);
        	pathPic = pathset[i].substr(0, pathset[i].find_last_of("/"));
        	sql.str("");
        	sql << "INSERT INTO media_info (title, category, year, length, director, cast, content, wiki, pic, type) VALUES ('";
        	
        	sql << strtok.token(2) << "', '"; // title
        	sql << strtok.token(1) << "', '"; // category
        	sql << strtok.token(3) << "', "; // year
        	sql << strtok.token(4) << ", '";  // length
        	sql << strtok.token(5) << "', '"; // director
        	sql << strtok.token(6) << "', '"; // cast
        	sql << strtok.token(7) << "', '"; // content
        	for (vector<string>::size_type i = 8; i < strtok.count_tokens()-1; i++) {
            	sql << strtok.token(i);
        	}
        	sql << "', '" << pathPic << "/[Image]" << strtok.token(strtok.count_tokens()-1) << ".jpg', '";
        	sql << strtok.token(0) << "')";  // M or TV
        	stmt->execute(sql.str());
        	strtok.clear();
    	}
    	stmt-> execute("ALTER TABLE media_info ADD number INT UNSIGNED NOT NULL AUTO_INCREMENT KEY");
    	stmt.reset(NULL);
    } catch (sql::SQLException &e) {
		cout << "#Error: " << e.what() << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		return false;
	}

    cout << "#\t Test table populated" << endl;
    return true;
}

bool database::addUser(const string& name, const string& pwd) const {
	try {
		auto_ptr<Connection> con(driver->connect(url, user, password));
		auto_ptr<Statement> stmt(con->createStatement());
		stringstream sql;

    	stmt->execute("USE " + db);
    	sql << "SELECT username, password FROM users WHERE username = '" << name << "'";
    	auto_ptr<ResultSet> res(stmt->executeQuery(sql.str()));
    	if (res->next()) {
    		cout << "Username has existed, please choose another one!" << endl;
    		return false;
    	}
    	sql.str("");
		sql << "INSERT INTO users (username, password) VALUES (";
		sql << "'" << name << "', '" << pwd << "')";
		stmt->execute(sql.str());
		stmt.reset(NULL);
	} catch (sql::SQLException &e) {
		cout << "#Error: " << e.what() << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		return false;
	}
	return true;
}

bool database::login(const string& name, const string& pwd) const {
	stringstream msg;
	try {
		auto_ptr<Connection> con(driver->connect(url, user, password));
		auto_ptr<Statement> stmt(con->createStatement());
    	stmt->execute("USE " + db);
    	msg << "SELECT username, password FROM users WHERE username = '" << name << "'";

    	auto_ptr<ResultSet> res(stmt->executeQuery(msg.str()));
		if (!res->next()) {
			cout << "No such username!" << endl;
			return false;
		}
		if ((res->getString("username") != name) || (res->getString("password") != pwd)) {
			cout << "Invalid username or password!" << endl;
			return false;
		}
		stmt.reset(NULL);
	} catch (sql::SQLException &e) {
		cout << "#Error: " << e.what() << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		return false;
	}
	return true;
}

bool database::changePassword(const string& name, const string& origin, const string& pwd) const {
	bool loginSuccess = login(name, origin);
	if (!loginSuccess) {
		cout << "Original password incorrect!" << endl;
		return false;
	}
	stringstream msg;
	try {
		auto_ptr<Connection> con(driver->connect(url, user, password));
		auto_ptr<Statement> stmt(con->createStatement());
    	stmt->execute("USE " + db);
    	msg << "UPDATE users SET password='" << pwd << "' WHERE username='" << name << "'";
    	stmt->execute(msg.str());
    	stmt.reset(NULL);
	} catch (sql::SQLException &e) {
		cout << "#Error: " << e.what() << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		return false;
	}
	return true;
}

vector<int> database::search(const string& query, int num, int pos) const {
	int row = 0;
	stringstream sql;
	vector<int> p;
	try {
		auto_ptr<Connection> con(driver->connect(url, user, password));
		auto_ptr<Statement> stmt(con->createStatement());

		stmt->execute("USE " + db);
		sql << "SELECT title, director, number FROM media_info WHERE title LIKE \"";
		sql << query << "%\" OR title LIKE \"%" << query << "\" OR director LIKE \"";
		sql << query << "%\" OR director LIKE \"%" << query << "\"";
		cout << sql.str() << endl;
		std::auto_ptr<ResultSet> res(stmt->executeQuery(sql.str()));
		cout << "#\t Fetching search for " << query << endl;
		row = pos;
		while (res->next()) {
			p.push_back(res->getInt("number"));
			row++;
			if (row == num) {break;}
		}
		stmt.reset(NULL);
	} catch (sql::SQLException &e) {
		cout << "#Error: " << e.what() << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	return p;	
}

void database::preciseFetchDisp(int index) const {
	stringstream out;
	try {
		auto_ptr<Connection> con(driver->connect(url, user, password));
		auto_ptr<Statement> stmt(con->createStatement());

		stmt->execute("USE " + db);
		out << "SELECT title, length, director, cast, year, content, number, wiki, pic FROM media_info WHERE number=" << index;
		std::auto_ptr<ResultSet> res(stmt->executeQuery(out.str()));
		cout << "# Fetching search for " << index << endl;
		if (res->next()) {
			cout << "title: " << res->getString("title") << endl;
			cout << "content: " << res->getString("content") << endl;
		} 
		stmt.reset(NULL);
	} catch (sql::SQLException &e) {
		cout << "#Error: " << e.what() << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	return;	
}

string database::preciseFetch(int index) const {
	stringstream out;
	string arg, tmp;
	try {
		auto_ptr<Connection> con(driver->connect(url, user, password));
		auto_ptr<Statement> stmt(con->createStatement());

		stmt->execute("USE " + db);
		out << "SELECT title, length, director, cast, year, content, number, wiki, pic FROM media_info WHERE number=" << index;
		std::auto_ptr<ResultSet> res(stmt->executeQuery(out.str()));
		cout << "# Fetching search for " << index << endl;
		if (res->next()) {
			arg = res->getString("title");
			arg.append("^^^^^");
			arg.append(res->getString("year"));
			arg.append("^^^^^");
			arg.append(res->getString("length"));
			arg.append("^^^^^");
			if ((tmp=res->getString("director")) == "") {
				arg.append("TBD");
			} else {
				arg.append(tmp);
			}
			arg.append("^^^^^");
			if ((tmp=res->getString("cast")) == "") {
				arg.append("TBD");
			} else {
				arg.append(tmp);
			}
			arg.append("^^^^^");
			if ((tmp=res->getString("content")) == "") {
				arg.append("TBD");
			} else {
				arg.append(tmp);
			}
			arg.append("^^^^^");
			//arg.push_back(res->getString("wiki"));
			string pic = res->getString("pic");
			pic = "temp/" + pic;
			arg.append(pic.substr(pic.find_last_of("/")+1));
			arg.append("^^^^^");
			arg.append(res->getString("number"));
		} 
		stmt.reset(NULL);
	} catch (sql::SQLException &e) {
		cout << "#Error: " << e.what() << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	return arg;	
}

vector<string> database::fetchPic(int index) {
	stringstream out;
	vector<string> info;
	try {
		auto_ptr<Connection> con(driver->connect(url, user, password));
		auto_ptr<Statement> stmt(con->createStatement());

		stmt->execute("USE " + db);
		out << "SELECT pic, number, title FROM media_info WHERE number=" << index;
		std::auto_ptr<ResultSet> res(stmt->executeQuery(out.str()));
		cout << "# Fetching search for " << index << endl;
		if (res->next()) {
			info.push_back(res->getString("number"));
			info.push_back(res->getString("title"));
			info.push_back(res->getString("pic"));
		} else {
			throw sql::SQLException("fetch error");
		}
		stmt.reset(NULL);
	} catch (sql::SQLException &e) {
		cout << "#Error: " << e.what() << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	return info;	
}

vector<int> database::fetchLatest(int num) {
	int row = 0;
	stringstream out;
	vector<int> p;
	try {
		auto_ptr<Connection> con(driver->connect(url, user, password));
		auto_ptr<Statement> stmt(con->createStatement());

		stmt->execute("USE " + db);
		out << "SELECT year, number FROM media_info ORDER BY year DESC";
		std::auto_ptr<ResultSet> res(stmt->executeQuery(out.str()));
		row = 0;
		while (res->next()) {
			p.push_back(res->getInt("number"));
			row++;
			if (row == num) { break; } 
		} 
		stmt.reset(NULL);
	} catch (sql::SQLException &e) {
		cout << "#Error: " << e.what() << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	return p;	
}

vector<int> database::hotCollect(string username, int num) {
	int row = 0;

	vector<int> p;
	try {
		auto_ptr<Connection> con(driver->connect(url, user, password));
		auto_ptr<Statement> stmt(con->createStatement());

		stmt->execute("USE " + db);
		std::auto_ptr<ResultSet> res(stmt->executeQuery("SELECT username, favorite, score, number FROM comments WHERE 	favorite=1 ORDER BY score DESC"));
		row = 0;
		while (res->next()) {
			p.push_back(res->getInt("number"));
			row++;
			if (row == num) { break; }
		}
		if (p.empty()) {
			std::auto_ptr<ResultSet> res(stmt->executeQuery("SELECT year, number FROM media_info ORDER BY year DESC"));
			row = 0;
			while (res->next()) {
				p.push_back(res->getInt("number"));
				row++;
				if (row == num) { break; } 
			} 
		}
		stmt.reset(NULL);
	} catch (sql::SQLException &e) {
		cout << "#Error: " << e.what() << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	return p;
}

vector<int> database::hotTypeCollect(string username, int num, string type) {
	int row = 0;
	stringstream out;
	vector<int> p;
	try {
		auto_ptr<Connection> con(driver->connect(url, user, password));
		auto_ptr<Statement> stmt(con->createStatement());

		stmt->execute("USE " + db);
		out << "SELECT username, favorite, score, number, type FROM comments WHERE favorite=1 AND type='" << type << "' ORDER BY score DESC";
		std::auto_ptr<ResultSet> res(stmt->executeQuery(out.str()));
		row = 0;
		while (res->next()) {
			p.push_back(res->getInt("number"));
			row++;
			if (row == num) { break; }
		}
		if (p.empty()) {
			out.str("");
			out << "SELECT year, number, type FROM media_info WHERE type='" << type << "' ORDER BY year DESC";
			std::auto_ptr<ResultSet> res(stmt->executeQuery(out.str()));
			row = 0;
			while (res->next()) {
				p.push_back(res->getInt("number"));
				row++;
				if (row == num) { break; } 
			} 
		}

		stmt.reset(NULL);
	} catch (sql::SQLException &e) {
		cout << "#Error: " << e.what() << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	return p;
}

vector<int> database::hotTypeCollectWithType(string username, int num, string type, string category) {
	int row = 0;
	stringstream out;
	vector<int> p;
	try {
		auto_ptr<Connection> con(driver->connect(url, user, password));
		auto_ptr<Statement> stmt(con->createStatement());

		stmt->execute("USE " + db);
		out << "SELECT username, favorite, score, number, type, category FROM comments WHERE favorite=1 AND type='" << type << "' AND category='" << category << "' ORDER BY score DESC";
		std::auto_ptr<ResultSet> res(stmt->executeQuery(out.str()));
		row = 0;
		while (res->next()) {
			p.push_back(res->getInt("number"));
			row++;
			if (row == num) { break; }
		}
		if (p.empty()) {
			out.str("");
			out << "SELECT year, number, type, category FROM media_info WHERE type='" << type << "' ORDER BY year DESC";
			std::auto_ptr<ResultSet> res(stmt->executeQuery(out.str()));
			row = 0;
			while (res->next()) {
				p.push_back(res->getInt("number"));
				row++;
				if (row == num) { break; } 
			} 
		}

		stmt.reset(NULL);
	} catch (sql::SQLException &e) {
		cout << "#Error: " << e.what() << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	return p;
}

vector<int> database::FavoriteCollect(string username, int num) {
	int row = 0;
	vector<int> p;
	stringstream msg;
	try {
		auto_ptr<Connection> con(driver->connect(url, user, password));
		auto_ptr<Statement> stmt(con->createStatement());

		stmt->execute("USE " + db);
		msg << "SELECT username, favorite, score, number, type FROM comments WHERE favorite=1 AND username='" << username << "' ORDER BY score DESC";
		std::auto_ptr<ResultSet> res(stmt->executeQuery(msg.str()));
		row = 0;
		while (res->next()) {
			p.push_back(res->getInt("number"));
			row++;
			if (row == num) { break; }
		}
		stmt.reset(NULL);
	} catch (sql::SQLException &e) {
		cout << "#Error: " << e.what() << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	return p;
}

bool database::addComment(string username, int number, string comment, bool favor, int score) {
	stringstream msg;
	try {
		auto_ptr<Connection> con(driver->connect(url, user, password));
		auto_ptr<Statement> stmt(con->createStatement());
    	stmt->execute("USE " + db);
    	msg << "SELECT username, number FROM comments WHERE number='" << number << "' AND username='" << username << "'";
    	std::auto_ptr<ResultSet> res(stmt->executeQuery(msg.str()));
    	msg.clear();
    	if (res->next()) {
    		msg.str("");
    		msg << "UPDATE comments SET comments='" << comment << "' WHERE username='" << username << "' AND number='" << number << "'";
    		stmt->execute(msg.str());
    	} else {
    		msg.str("");
    		pair<string,string> cat = getInfo("media_info", number);
    		msg << "INSERT INTO comments (username, number, comments, favorite, score, type, category) VALUES (";
    		msg << "'" << username << "', '" << number << "', '" << comment << "', " << favor << ", '" << score << "', '" << cat.first << "', '" << cat.second << "')";
    		stmt->execute(msg.str());
    	}
    	stmt.reset(NULL);
	} catch (sql::SQLException &e) {
		cout << "#Error: " << e.what() << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		return false;
	}
	return true;
}

pair<string,string> database::getInfo(string table, int number) {
	stringstream out;
	string str;
	try {
		auto_ptr<Connection> con(driver->connect(url, user, password));
		auto_ptr<Statement> stmt(con->createStatement());

		stmt->execute("USE " + db);
		out << "SELECT number,type,category FROM " << table << " WHERE number=" << number;
		std::auto_ptr<ResultSet> res(stmt->executeQuery(out.str()));
		if (res->next()) {
			return pair<string,string>(res->getString("type"),res->getString("category"));
		} 
		stmt.reset(NULL);
	} catch (sql::SQLException &e) {
		cout << "#Error: " << e.what() << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	return pair<string,string>("","");
}
