#include "model.h"
#include "transmit.h"
#include "utils.h"
//#include <boost/thread.hpp>

using namespace std;

// void HandleTCPClient(TCPSocket *sock);     // TCP client handling function
// void *ThreadMain(void *arg);               // Main program of a thread

void operation(model& lda, transmit * client) {
    strtokenizer cmd;
    database db;
    string command;
    cout << "here" << endl;
    while(1) {    
        cmd.clear();
        *client >> command;
        cout << command << endl;

        cmd.parse(command, "*_*");
        assert(cmd.count_tokens() != 0);
        string operation = cmd.token(0);

        //1. createuser + username + password, e.g. createuser*_*jianan*_*123456    response: Success! or Fail!
        //2. login + username + password, e.g. login*_*jianan*_*123456              response: Success! or Fail!
        //3. changeuser + username + password, e.g. login*_*jianan*_*12345678       response: Success! or Fail!
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
        }else if(operation == "changeuser"){
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
            assert(cmd.count_tokens() == 4);
            string interface = cmd.token(2);

            if(interface == "main"){
                /*functions for finding 12 hotest films or TV shows*/
                vector<int> col = db.hotCollect(cmd.token(1), N_DISP);
                vector<int> can = lda.ranking(col);
                stringstream out;
                for (int i = 0; i < N_DISP; i++) {
                    out << can[i];
                    client->SendFile(db.fetchPic(can[i]), out.str());
                    out.str("");
                }
            } else if(interface == "what's new"){
                /*functions for finding 12 lastest films or TV shows*/
                vector<pair<int, string> > late = db.fetchLatest(N_DISP);
                stringstream out;
                for (int i = 0; i < N_DISP; i++) {
                    out << late[i].first;
                    client->SendFile(late[i].second, out.str());
                    out.str("");
                }
            } else if(interface == "movies"){
                /*functions for finding 12 random films*/
                vector<int> col = db.hotTypeCollect(cmd.token(1), N_DISP, "Movies");
                vector<int> can = lda.ranking(col);
                stringstream out;
                for (int i = 0; i < N_DISP; i++) {
                    out << can[i];
                    client->SendFile(db.fetchPic(can[i]), out.str());
                    out.str("");
                }
            }else if(interface == "TV shows"){
                /*functions for finding 12 random TV shows*/
                vector<int> col = db.hotTypeCollect(cmd.token(1), N_DISP, "TV");
                vector<int> can = lda.ranking(col);
                stringstream out;
                for (int i = 0; i < N_DISP; i++) {
                    out << can[i];
                    client->SendFile(db.fetchPic(can[i]), out.str());
                    out.str("");
                }
            }else if(interface == "favourites"){
                /*functions for finding 12 favourites for user*/
                vector<int> can = db.FavoriteCollect(cmd.token(1), N_DISP);
                if (can.empty()) { 
                    *client << "Emtpy favourites";
                    continue;
                }
                stringstream out;
                for (int i = 0; i < N_DISP; i++) {
                    out << can[i];
                    client->SendFile(db.fetchPic(can[i]), out.str());
                    out.str("");
                }
            }else if(interface == "MTDetails"){
                vector<string> arg = db.preciseFetch(atoi(cmd.token(1).c_str()));
                assert(arg.size() == 9);
                client->SendStruct(arg);
            }
        }        
        //5. changemovie + username + number + comments + rate(5 to 1) + favourite or not
        //, e.g. changemovie*_*jianan*_*movie*_*metrix*_*this is awesome*_*5*_*1
        //if something is missing, use blank " "
        else if (operation == "changemovie") {
            string comments = cmd.token(6);
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
            vector<pair<int, string> > p = db.search(cmd.token(2), 10, 0);
            stringstream out;
            for (int i = 0; i < N_DISP; i++) {
                out << p[i].first;
                client->SendFile(p[i].second, out.str());
                out.str("");
            }
        }
    }
}

int main(int argc, char ** argv) {
    model lda;
    int port;
    
    if (utils::parse_args(argc, argv, &lda, port)) {
        return 1;
    }
    
    if (lda.model_status != MODEL_STATUS_SERVER) {
        lda.init();
    }
    
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
    
    assert(lda.model_status == MODEL_STATUS_SERVER);
    transmit sv(port);
    while(1) {
        transmit *client = new transmit();
        sv.accept(*client);
        operation(lda, client);
        //boost::thread newThread(operation, client);
    }
    
    return 0;
}
