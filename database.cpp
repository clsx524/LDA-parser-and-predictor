#include "database.h"

bool database::initDatabase() {
	stringstream sql;
	stringstream msg;
    
	cout << "#Starting ... " << endl;
    //sql::Driver * driver = sql::mysql::get_driver_instance();
    std::auto_ptr< sql::Connection > con(driver->connect(url, user, password));
    std::auto_ptr< sql::Statement > stmt(con->createStatement());
    
    /* Create a test table demonstrating the use of sql::Statement.execute() */
    stmt->execute("USE " + db);
    stmt->execute("DROP TABLE IF EXISTS media_info");
    stmt->execute("CREATE TABLE media_info (title VARCHAR(100), year CHAR(4), length SMALLINT UNSIGNED, director VARCHAR(20), cast VARCHAR(50), content VARCHAR(200), wiki VARCHAR(10000), number SMALLINT UNSIGNED)");
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
        	sql.str("");
        	sql << "INSERT INTO media_info (title, year, length, director, cast, content, wiki, number) VALUES (";
        	sql << "'" << strtok.token(0) << "', "; // title
        	sql << "'" << strtok.token(1) << "', "; // year
        	sql << strtok.token(2) << ", ";         // length
        	sql << "'" << strtok.token(3) << "', "; // director
        	sql << "'" << strtok.token(4) << "', "; // cast
        	sql << "'" << strtok.token(5) << "', "; // content
        	sql << "'";
        	for (vector<string>::size_type i = 6; i < strtok.count_tokens(); i++) {
            	sql << strtok.token(i);
        	}
        	sql << "', '" << size+1 << "')" ;
        
        	//sql << test_data[i].id << ", '" << test_data[i].label << "')";
        	stmt->execute(sql.str());
        	strtok.clear();
    	}
    	sql.str();
    	sql << "ALTER TABLE media_info ADD number INT UNSIGNED NOT NULL AUTO_INCREMENT KEY";
    	stmt-> execute(sql.str());
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

bool database::search(const string& query) const {
	int row = 0;
	stringstream sql;
	try {
		auto_ptr<Connection> con(driver->connect(url, user, password));
		auto_ptr<Statement> stmt(con->createStatement());

		stmt->execute("USE " + db);
		sql << "SELECT title, director FROM media_info WHERE title LIKE \"";
		sql << query << "%\" OR title LIKE \"%" << query << "\" OR director LIKE \"";
		sql << query << "%\" OR director LIKE \"%" << query << "\"";
		cout << sql.str() << endl;
		std::auto_ptr<ResultSet> res(stmt->executeQuery(sql.str()));
		cout << "#\t Fetching search for " << query << endl;
		row = 0;
		while (res->next()) {
			cout << "#\t\t Fetching row " << row;
			cout << "#\t title = " << res->getString("title");
			cout << "#\t director = " << res->getString("director") << endl;
			row++;
		}
		stmt.reset(NULL);
	} catch (sql::SQLException &e) {
		cout << "#Error: " << e.what() << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		return false;
	}

	return true;	
}

bool database::preciseFetch(int index) const {
	stringstream out;
	try {
		auto_ptr<Connection> con(driver->connect(url, user, password));
		auto_ptr<Statement> stmt(con->createStatement());

		stmt->execute("USE " + db);
		out << "SELECT title, director, year, content, number FROM media_info WHERE number=" << index;
		std::auto_ptr<ResultSet> res(stmt->executeQuery(out.str()));
		cout << "# Fetching search for " << index << endl;
		if (res->next()) {
			cout << "# title = " << res->getString("title") << endl;
			cout << "# director = " << res->getString("director") << endl;
			cout << "# year = " << res->getInt("year") << endl;
			cout << "# content = " << res->getString("content") << endl;
		} else {
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

