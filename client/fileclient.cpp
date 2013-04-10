#include "transmit.h"  

int main(int argc, char* argv[]){  

    string inputCommand;
    char url[20]; 
    cout << "Input the server URL:" << endl;
    cin >> url;

    while(1){
        //input command
        getline(cin,inputCommand);
        string operation = inputCommand.substr(0,inputCommand.find("*_*"));
        //1. createuser + username + password, e.g. createuser*_*jianan*_*123456    response: Success! or Fail!
        //2. login + username + password, e.g. login*_*jianan*_*123456              response: Success! or Fail!
        //3. changeuser + username + password, e.g. changeuser*_*jianan*_*12345678       response: Success! or Fail!
        if(operation == "createuser" || operation == "login" || operation == "changeuser"){
            int clientfd= Clie_SockEstablish();
            Clie_ClientConnect(clientfd, url);
            Clie_SendCommand(clientfd, inputCommand.c_str());
            Clie_GetResponse(clientfd);
            close(clientfd); 
        }
        //4. display + username + interfacename + category, e.g. display*_*jianan*_*main*_*action or display*_*jianan*_*MTDetails*_*action
        else if(operation == "display"){
            string rest = inputCommand.substr(inputCommand.find("*_*")+3,inputCommand.size()-(inputCommand.find("*_*")+3));
            rest.erase(0, rest.find("*_*")+3);
            string interface = rest.substr(0, rest.find("*_*"));
            if(interface == "main" || interface == "what's new" || interface == "movies" || interface == "TV shows" || interface == "favourites"){
                int clientfd= Clie_SockEstablish();
                Clie_ClientConnect(clientfd, url);
                Clie_SendCommand(clientfd, inputCommand.c_str());
                for(int i=0; i<12;i++){
                    int clientfdx = Clie_SockEstablish();;
                    Clie_ClientConnect(clientfdx, url);
                    Clie_SaveContent(clientfdx, url);
                    close(clientfdx);
                }
                close(clientfd);                
            }else if(interface == "MTDetails"){
                //get info struct
                int clientfd_str= Clie_SockEstablish();
                Clie_ClientConnect(clientfd_str, url);
                Clie_SendCommand(clientfd_str, inputCommand.c_str());
                Clie_SaveStruct(clientfd_str);
                close(clientfd_str);    
                //get picture
                int clientfd_pic= Clie_SockEstablish();
                Clie_ClientConnect(clientfd_pic, url);
                Clie_SendCommand(clientfd_pic, inputCommand.c_str());
                Clie_SaveContent(clientfd_pic, url);
                close(clientfd_pic);         
            }
        }
        //5. changemovie + username + movie or TV show + movie/TV name + comments + rate(5 to 1) + favourite or not
        //, e.g. changemovie*_*jianan*_*movie*_*metrix*_*this is awesome*_*5*_*1
        //if something is missing, use blank " "
        else if(operation == "changemovie"){
            int clientfd= Clie_SockEstablish();
            Clie_ClientConnect(clientfd, url);
            Clie_SendCommand(clientfd, inputCommand.c_str());
            Clie_GetResponse(clientfd);
            close(clientfd);             
        }
        //6. search + movie or TV show + movie/TV name, e.g. search*_*metrix*_*movie
        //default 10 pictures
        else if(operation == "search"){
            int clientfd= Clie_SockEstablish();
            Clie_ClientConnect(clientfd, url);
            Clie_SendCommand(clientfd, inputCommand.c_str());
            for(int i=0; i<10;i++){
                int clientfdx = Clie_SockEstablish();;
                Clie_ClientConnect(clientfdx, url);
                Clie_SaveContent(clientfdx, url);
                close(clientfdx);
            }
            close(clientfd);  
        }


        if(inputCommand == "quit"){
            break;
        }
    }    
    return 0;  
}  

