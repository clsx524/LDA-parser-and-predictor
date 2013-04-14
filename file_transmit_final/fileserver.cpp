#include "common.h"  

int main(int argc, char* argv[]){  
   
    int sockfd = Serv_SockEstablish();  

    while(1){               
        char command[WORD_MAX_SIZE];bzero(command,WORD_MAX_SIZE);
        int connfd = Serv_ConnEstablish(sockfd);
        Serv_GetCommand(connfd, command);
        string clientcommand(command);
        string operation = clientcommand.substr(0,clientcommand.find("*_*"));

        //1. createuser + username + password, e.g. createuser*_*jianan*_*123456    response: Success! or Fail!
        //2. login + username + password, e.g. login*_*jianan*_*123456              response: Success! or Fail!
        //3. changeuser + username + password, e.g. login*_*jianan*_*12345678       response: Success! or Fail!
        if(operation == "createuser"){

            /* do whatever you want and get the results*/

            // success situaion
            int count4=send(connfd, "Success!" , WORD_MAX_SIZE ,0);
            // failure situation 
//          int count4=send(connfd, "Fail!" , WORD_MAX_SIZE ,0);              
            if(count4<0){  
                perror("Send response");  
                return 1;  
            }
            bzero(command,WORD_MAX_SIZE); 
        }else if(operation == "login"){

            /* do whatever you want and get the results*/
            
            // success situaion
            int count4=send(connfd, "Success!" , WORD_MAX_SIZE ,0);
            // failure situation 
//          int count4=send(connfd, "Fail!" , WORD_MAX_SIZE ,0);              
            if(count4<0){  
                perror("Send response");  
                return 1;  
            }
            bzero(command,WORD_MAX_SIZE);
        }else if(operation == "changeuser"){

            /* do whatever you want and get the results*/
            
            // success situaion
            int count4=send(connfd, "Success!" , WORD_MAX_SIZE ,0);
            // failure situation 
//          int count4=send(connfd, "Fail!" , WORD_MAX_SIZE ,0);              
            if(count4<0){  
                perror("Send response");  
                return 1;  
            }
            bzero(command,WORD_MAX_SIZE);            
        }
        //4. display + username + interfacename + category + movie or TV show, e.g. display*_*jianan*_*main*_*action
        else if(operation == "display"){
            string rest = clientcommand.substr(clientcommand.find("*_*")+3,clientcommand.size()-(clientcommand.find("*_*")+3));
            rest.erase(0, rest.find("*_*")+3);
            string interface = rest.substr(0, rest.find("*_*"));

            if(interface == "main"){
                /*functions for finding 12 hotest films or TV shows*/
                string filename[12]={"1.jpg","2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg","9.jpg", "10.jpg", "11.jpg" ,"12.jpg"};
                for(int i=0; i< 12; i++){
                    int connfd_temp = Serv_ConnEstablish(sockfd);  
                    Serv_SendFile(connfd_temp, filename[i]);
                    close(connfd_temp);
                }
                bzero(command,WORD_MAX_SIZE);
            }else if(interface == "what's new"){
                /*functions for finding 12 lastest films or TV shows*/
                string filename[12]={"1.jpg","2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg","9.jpg", "10.jpg", "11.jpg" ,"12.jpg"};
                for(int i=0; i< 12; i++){
                    int connfd_temp = Serv_ConnEstablish(sockfd);  
                    Serv_SendFile(connfd_temp, filename[i]);
                    close(connfd_temp);
                }
                bzero(command,WORD_MAX_SIZE);
            }else if(interface == "movies"){
                /*functions for finding 12 random films*/
                string filename[12]={"1.jpg","2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg","9.jpg", "10.jpg", "11.jpg" ,"12.jpg"};
                for(int i=0; i< 12; i++){
                    int connfd_temp = Serv_ConnEstablish(sockfd);  
                    Serv_SendFile(connfd_temp, filename[i]);
                    close(connfd_temp);
                }
                bzero(command,WORD_MAX_SIZE);
            }else if(interface == "TV shows"){
                /*functions for finding 12 random TV shows*/
                string filename[12]={"1.jpg","2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg","9.jpg", "10.jpg", "11.jpg" ,"12.jpg"};
                for(int i=0; i< 12; i++){
                    int connfd_temp = Serv_ConnEstablish(sockfd);  
                    Serv_SendFile(connfd_temp, filename[i]);
                    close(connfd_temp);
                }
                bzero(command,WORD_MAX_SIZE);
            }else if(interface == "favourites"){
                /*functions for finding 12 favourites for user*/
                string filename[12]={"1.jpg","2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg","9.jpg", "10.jpg", "11.jpg" ,"12.jpg"};
                for(int i=0; i< 12; i++){
                    int connfd_temp = Serv_ConnEstablish(sockfd);  
                    Serv_SendFile(connfd_temp, filename[i]);
                    close(connfd_temp);
                }
                bzero(command,WORD_MAX_SIZE);
            }else if(interface == "MTDetails"){
                //send struct
                struct MovieData movie;
                Serv_SetContent(movie, "Afro_Ninja:_Destiny", "2009", "88", "Mark Hicks", "", "An action comedy in the tradition of Jackie Chan and Bruce Lee, Afro Ninja: Destiny follows the journey of a hapless hero who's transformed into an unstoppable warrior after receiving an ancient magical sword. YouTube sensation Mark Hicks stars as Reggie Carson, a longtime martial artist who has just become the laughing stock of the entire internet. The joke is on the bad guys, however, when Reggie transforms into the formidable warrior known as Afro Ninja after coming into possession of a mystical sword and growing an instant afro haircut. Now, as gangsters move into his neighborhood to spread greed and corruption, it's up to Afro Ninja to keep the streets safe. Marla Gibb, Jim Kelly, and Natascha Hopkins co-star.",""); 
                Serv_SendStruct(connfd, movie);
                //send picture
                string filename="1.jpg";
                int connfd_temp = Serv_ConnEstablish(sockfd);  
                Serv_SendFile(connfd_temp, filename);
                close(connfd_temp);
                bzero(command,WORD_MAX_SIZE);
            }
        }        
        //5. changemovie + username + movie or TV show + movie/TV name + comments + rate(5 to 1) + favourite or not
        //, e.g. changemovie*_*jianan*_*movie*_*metrix*_*this is awesome*_*5*_*1
        //if something is missing, use blank " "
        else if(operation == "changemovie"){
            string rest = clientcommand.substr(clientcommand.find("*_*")+3, clientcommand.size()-(clientcommand.find("*_*")+3));
            rest.erase(0, rest.find("*_*")+3);
            rest.erase(0, rest.find("*_*")+3);
            rest.erase(0, rest.find("*_*")+3);
            string comments = rest.substr(0, rest.find("*_*"));
            cout << "The comments is: " << comments << endl;
            int count4=send(connfd, "Success!" , WORD_MAX_SIZE ,0);
            // failure situation 
//          int count4=send(connfd, "Fail!" , WORD_MAX_SIZE ,0);              
            if(count4<0){  
                perror("Send response");
                exit(1);
            } 
            bzero(command,WORD_MAX_SIZE);  
        }
        //6. search + movie or TV show + movie/TV name
        //default 10 pictures
        else if(operation == "search"){
            /*functions for finding 10 related movies*/
            string filename[10]={"1.jpg","2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg","9.jpg", "10.jpg"};
            for(int i=0; i< 10; i++){
                int connfd_temp = Serv_ConnEstablish(sockfd);  
                Serv_SendFile(connfd_temp, filename[i]);
                close(connfd_temp);
            }
            bzero(command,WORD_MAX_SIZE);
        }
        
        close(connfd); 
    }

    close(sockfd); 
    return 0;  
}  
