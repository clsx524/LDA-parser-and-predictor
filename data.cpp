#include "database.h"
#include "utils.h"
#include <iostream>

using namespace std;

int main(int argc, char ** argv) {
    database data;

    string path;
    string url;
    string user;
    string password;
    string database;
    string u;
    string p;

    if (argc == 2) {
        path = argv[1];
        data.init(path);

    } else if (argc == 6) {
        path = argv[1];
        url = argv[2];
        user = argv[3];
        password = argv[4];
        database = argv[5];
        data.init(path, url, user, password, database);
    } else if (argc == 3) {
        u = argv[1];
        p = argv[2];       
    } else {
        cout << "Invalid number of arguments." << endl;
        return 1;
    }

    //data.initDatabase();
    cout << data.search(path) << endl;



    return 0;
}
