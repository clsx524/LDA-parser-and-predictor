#include "model.h"
#include "transmit.h"
#include "utils.h"
//#include <boost/thread.hpp>

using namespace std;

void operation(transmit& sv, model& lda, transmit * client) {
    strtokenizer cmd;
    database db;
    string command;
    cout << "here1" << endl;
    try { 
        *client >> command;
        cout << command << endl;
        cmd.parse(command, "*_*");

        string operation = cmd.token(0);

        //1. createuser + username + password, e.g. createuser*_*jianan*_*123456    response: Success! or Fail!
        //2. login + username + password, e.g. login*_*jianan*_*123456              response: Success! or Fail!
        //3. changeuser + username + old + password, e.g. login*_*jianan*_*12345678       response: Success! or Fail!
        if(operation == "createuser"){
            assert(cmd.count_tokens() == 3);
            bool ans = db.addUser(cmd.token(1),cmd.token(2));
            if (ans) {
                *client << "Success";
            } else {
                *client << "Fail"; 
            }
        } else if(operation == "login"){
            assert(cmd.count_tokens() == 3);
            bool ans = db.login(cmd.token(1),cmd.token(2));
            if (ans) {
                *client << "Success";
            } else {
                *client << "Fail"; 
            }
        } else if(operation == "changeuser"){
            assert(cmd.count_tokens() == 4);
            bool ans = db.changePassword(cmd.token(1), cmd.token(2), cmd.token(3));
            if (ans) {
                *client << "Success";
            } else {
                *client << "Fail"; 
            }            
        }
        //4. display + username + interfacename + category + movie or TV show, e.g. display*_*jianan*_*main*_*action
        //   display + number + interface
        else if(operation == "display"){
            assert(cmd.count_tokens() >=3);
            string interface = cmd.token(2);

            if(interface == "main"){
                assert(cmd.count_tokens() == 3);
                /*functions for finding 12 hotest films or TV shows*/
                vector<int> col = db.hotCollect(cmd.token(1), 10);
                vector<int> can = lda.ranking(col,"","");
                    
                for (int i = 0; i < N_DISP; i++) {
                    sv.accept(*client);
                    vector<string> info = db.fetchPic(can[i]);
                    // info[0] - number ; info[1] - name ; info[2] - picname
                    client->SendFile(info);
                    client->close();
                }
            } else if(interface == "new"){
                assert(cmd.count_tokens() == 3);
                /*functions for finding 12 lastest films or TV shows*/
                vector<int> late = db.fetchLatest(N_DISP);
                cout << late.size() << endl;
                for (int i = 0; i < N_DISP; i++) {
                    sv.accept(*client);
                    vector<string> info = db.fetchPic(late[i]);
                    client->SendFile(info);
                    client->close();
                }
            } else if(interface == "movies" && cmd.count_tokens() == 3){
                /*functions for finding 12 random films*/
                vector<int> col = db.hotTypeCollect(cmd.token(1), N_DISP, "Movies");
                vector<int> can = lda.ranking(col, "movies", "");
                for (int i = 0; i < N_DISP/2; i++) {
                    sv.accept(*client);
                    vector<string> info = db.fetchPic(can[i]);
                    client->SendFile(info);
                    client->close();
                }
            } else if(interface == "movies" && cmd.count_tokens() == 4){
                /*functions for finding 12 random films with category*/
                vector<int> col = db.hotTypeCollectWithType(cmd.token(1), N_DISP, "Movies", cmd.token(3));
                vector<int> can = lda.ranking(col, "movies", cmd.token(3));
                for (int i = 0; i < N_DISP/2; i++) {
                    sv.accept(*client);
                    vector<string> info = db.fetchPic(can[i]);
                    client->SendFile(info);
                    client->close();
                }
            } else if(interface == "TV" && cmd.count_tokens() == 3){
                /*functions for finding 12 random TV shows*/
                vector<int> col = db.hotTypeCollect(cmd.token(1), N_DISP, "TV");
                vector<int> can = lda.ranking(col, "TV", "");
                for (int i = 0; i < N_DISP/2; i++) {
                    sv.accept(*client);
                    vector<string> info = db.fetchPic(can[i]);
                    client->SendFile(info);
                    client->close();
                }
            } else if(interface == "TV" && cmd.count_tokens() == 4){
                /*functions for finding 12 random TV shows with category*/
                vector<int> col = db.hotTypeCollectWithType(cmd.token(1), N_DISP, "TV", cmd.token(3));
                vector<int> can = lda.ranking(col, "TV", cmd.token(3));
                for (int i = 0; i < N_DISP/2; i++) {
                    sv.accept(*client);
                    vector<string> info = db.fetchPic(can[i]);
                    client->SendFile(info);
                    client->close();
                }
            } else if(interface == "favourites"){
                assert(cmd.count_tokens() == 3);
                /*functions for finding 12 favourites for user*/
                vector<int> can = db.FavoriteCollect(cmd.token(1), N_DISP);
                int num = can.size();
                if (can.empty()) { 
                    *client << "Emtpy favourites";
                } else {
                    stringstream out;
                    out << num;
                    *client << out.str();
                    for (int i = 0; i < num; i++) {
                        sv.accept(*client);
                        vector<string> info = db.fetchPic(can[i]);
                        client->SendFile(info);
                        client->close();
                    }
                }
            } else if(interface == "MTDetails"){
                //display + number + MTDetails
                assert(cmd.count_tokens() == 3);
                int i = atoi(cmd.token(1).c_str());
                vector<string> arg = db.preciseFetch(i);
                assert(arg.size() == 9);
                client->SendStruct(arg);
                client->close();
                // send pic
                sv.accept(*client);
                vector<string> info = db.fetchPic(i);
                client->SendFile(info);
                client->close();
            }
        }        
        //5. changemovie + username + number + comments + rate(5 to 1) + favourite or not
        //, e.g. changemovie*_*jianan*_*23*_*this is awesome*_*5*_*1
        //if something is missing, use blank " "
        else if (operation == "changemovie") {
            string comments = cmd.token(3);
            cout << "The comments is: " << comments << endl;
            bool ans = db.addComment(cmd.token(1), atoi(cmd.token(2).c_str()), cmd.token(3), atoi(cmd.token(5).c_str()), atoi(cmd.token(4).c_str()));
            if (ans) {
                *client << "Success";
            } else {
                *client << "Fail"; 
            }  
        }
        //6. search + movie or TV show + movie/TV name
        //default 10 pictures
        else if(operation == "search"){
            /*functions for finding 10 related movies*/
            assert(cmd.count_tokens() == 2);
            cout << cmd.token(1)<< endl;
            vector<int> p = db.search(cmd.token(1), 10, 0);
            
            stringstream out;
            if (p.size() > 10) {
                out << 10;
            } else {
                out << p.size();
            }
            *client << out.str();

            for (int i = 0; i < (int)p.size(); i++) {
                sv.accept(*client);
                vector<string> info = db.fetchPic(p[i]);
                client->SendFile(info);
                client->close();
            }
        } 
    } catch (SocketException &) {
        delete client;
        return;
    }
}

int main(int argc, char ** argv) {
    model lda;
    int port;
    
    if (utils::parse_args(argc, argv, &lda, port)) {
        return 1;
    }
    
    lda.init();
    
    if (lda.model_status == MODEL_STATUS_EST || lda.model_status == MODEL_STATUS_ESTC) {
        lda.estimate();
    }
    
    if (lda.model_status == MODEL_STATUS_INF) {
        lda.inference();
    }
    
    if (lda.model_status == MODEL_STATUS_PREPROCESS) {
        lda.preprocess();
    }
    
    if (lda.model_status == MODEL_STATUS_RANKING) {
        lda.ranking();
    }
    
    if (lda.model_status == MODEL_STATUS_CLASSIFIER) {
        lda.classification();
    }
    
    if (lda.model_status == MODEL_STATUS_SERVER) {
        transmit sv(port);
        while(1) {
            cout << "again" << endl;
            transmit *client = new transmit();
            sv.accept(*client);
            cout << "almost enter" << endl;
            operation(sv, lda, client);
            //boost::thread newThread(operation, client);
        }
    }

    return 0;
}
