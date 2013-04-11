#include "database.h"
#include "utils.h"
#include <iostream>
#include <vector>

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
    } else if (argc == 4) {
        u = argv[1];
        p = argv[2];
        path = argv[3];
    }
    // } else {
    //     cout << "Invalid number of arguments." << endl;
    //     return 1;
    // }

    data.initDatabase();
    // vector<pair<int, string> > ans = data.search("life", 10, 0);
    // for (vector<pair<int, string> >::size_type i = 0; i < ans.size(); i++) {
    //     cout << ans[i].first << " " << ans[i].second << endl;
    // }
    //cout << data.changePassword(u,p,path);

    // vector<string> ans = data.preciseFetch(430);
    // for (vector<string>::size_type i = 0; i < ans.size(); i++) {
    //     cout << ans[i] << endl;
    // }   

    //cout << data.fetchPic(1212) << endl;

    // vector<pair<int, string> > ans = data.fetchLatest(10);
    // for (vector<pair<int, string> >::size_type i = 0; i < ans.size(); i++) {
    //     cout << ans[i].first << " " << ans[i].second << endl;
    // }   

    //vector<int> ans = data.hotTypeCollect("xin", 10, "TV");
    // vector<int> ans = data.FavoriteCollect("xin", 10);
    // for (vector<int>::size_type i = 0; i < ans.size(); i++) {
    //     cout << ans[i] << endl;
    // } 
    //cout << data.addComment("xin", 123, "good", 1, 5) << endl;

    return 0;
}
