#include "mbed.h"
#include "C12832.h"
#include <string> 
using namespace std;

struct Player {
    int score;
    int pits[6];
    bool isTurn;
};

struct Pit {
    int pit;
    string player;
};

PwmOut red(p23); 
PwmOut green(p24);
PwmOut blue(p25);  

C12832 lcd(p5, p7, p6, p8, p11);
BusIn joy(p15, p12, p13, p16, p14);
Serial link(p9,p10); //tx ; rx

int HOLE_SIZE = 6;

void printMarble(float x, float y, int nb_boule, int color) { 
    if (nb_boule < 6) {
        int RADIUS = 2;
        float Pits[5][2] = {
            {-RADIUS, RADIUS}, {RADIUS, RADIUS}, 
            {-RADIUS, -RADIUS}, {RADIUS, -RADIUS}, {0, 0}
        };
        for (int i = 0; i < nb_boule; i++) {
            lcd.fillcircle(x + Pits[i][0], y + Pits[i][1], 1, color);
        }
    } else {
        lcd.locate(x - 2, y - 4);
        if (nb_boule >= 10) lcd.locate(x - 5, y - 4);
        lcd.printf("%d", nb_boule);
    }
}

void printGrenier(Player p1, Player p2) {
    lcd.locate(0, lcd.height() / 2 - 4);
    lcd.printf("%d", p2.score);
    lcd.locate(lcd.width() - 12, lcd.height() / 2 - 4);
    lcd.printf("%d", p1.score);
}

void printAll(Player p1, Player p2, int choosen_pit) {
    for (int row = 1; row < 3; row++) {
        int location = (row == 2) ? 3 : 1;
        for (int i = 1; i < 7; i++) {
            float x = lcd.width() * i / 7;
            float y = lcd.height() * location / 4;

            if (row == 1) printMarble(x, y, p1.pits[i - 1], 1);
            else printMarble(x, y, p2.pits[i - 1], 1);

            if ((row == 1 and p1.isTurn and choosen_pit + 1 == i) or
                (row == 2 and p2.isTurn and choosen_pit + 1 == i)) {
                lcd.circle(x, y, HOLE_SIZE + 1, 1);
            }
            lcd.circle(x, y, HOLE_SIZE, 1);
        }
    }
    printGrenier(p1, p2);
}

int choose_pit(Player player_1, Player player_2){
    int choice = 0;
    lcd.cls();
    printAll(player_1, player_2, choice);

    while(!joy[4]){
        if((joy[2]) and (choice > 0)){
            choice -= 1;
            lcd.cls();
            printAll(player_1, player_2, choice);

            // lcd.cls();
            // lcd.printf("Choix -1\n valeur actuelle %d", choice);
             wait(0.2);
        }
        else if((joy[3]) and (choice < 5 )){
            choice += 1;
            lcd.cls();
            printAll(player_1, player_2, choice);

            // lcd.cls();
            // lcd.printf("Choix +1\n valeur actuelle %d", choice);
            wait(0.2);
        }
    }
    // lcd.cls();
    // lcd.printf("Choix terminé\n valeur %d", choice);
    // wait(2);
    return choice;
}

// Add placing a stone in the grenier and updating score: Done
// check if last stone was put in a pit of the current player's or opponent's side: Done
Pit move_stones(Player &p1, Player &p2, int pit){
    // If it moves stones, returns the last pit that received a stone
    int stones;
    if(p1.isTurn){
        
        stones = p1.pits[pit];
        if(stones == 0){
        //     lcd.cls();
        // lcd.locate(0,0);
        // lcd.printf("no stones");
        // wait(2);
        Pit res = {-1, "p1"};
        return res;
    }
    } else {
       
        stones = p2.pits[pit];
        if(stones == 0){
        //      lcd.cls();
        // lcd.locate(0,0);
        // lcd.printf("no stones");
        // wait(2);
        Pit res = {-1, "p2"};
        return res;
    }
    }
   
    //lcd.cls();
    //        lcd.locate(0,0);
    //      lcd.printf("In move_stones\nstones: %d", stones);
    //    wait(1);
    // If the pit is already empty return -1
    
    // Place stones in the rest of the current players pits
    if(p1.isTurn){
        p1.pits[pit] = 0;
        pit += 1;
    } else{
        p2.pits[pit] = 0;
        pit -= 1;
    }
    
    while((stones > 0) and (0 <= pit and pit <= 5 )){
        // lcd.cls();
        // lcd.locate(0,0);
        // lcd.printf("partie current player");
        // wait(2);
        if(p1.isTurn){
            p1.pits[pit] += 1;
            if(stones == 1){
                Pit res = {pit, "p1"};
                return res;
            }
            pit += 1;
        } else{
            p2.pits[pit] += 1;
            if(stones == 1){
                Pit res = {pit, "p2"};
                return res;
            }
            pit -= 1;
        }

        stones -= 1;
        
        lcd.cls();
        lcd.locate(0,0);
        printAll(p1,p2, -1);
        wait(0.5);
        
    } 
    

    // Place stones in the rest of the pits 
    // stones += 1;
    while(stones > 0){
        // Mettre dans le grenier de current_player
        int opponent_pit;
        if(p1.isTurn){
            p1.score += 1;
            opponent_pit = 5;
            stones -= 1;
            if(stones == 0){
                Pit res = {0, "g1"};
                return res;
            }
        } else{
            p2.score += 1;
            opponent_pit = 0;
            stones -= 1;
            if(stones == 0){
                Pit res = {0, "g2"};
                return res;
            }
        
        lcd.cls();
        lcd.locate(0,0);
        printAll(p1,p2, -1);
        wait(0.5);
        }
        
        
        while((stones > 0) and (0 <= opponent_pit) and (opponent_pit <= 5 )){
            // lcd.cls();
            // lcd.locate(0,0);
            // lcd.printf("partie autre player");
            // wait(0.5);
            if(p1.isTurn){
                p2.pits[opponent_pit] += 1;
                if(stones == 1){
                    Pit res = {opponent_pit, "p2"};
                    return res;
                }
                opponent_pit -= 1;
            } else{
                p1.pits[opponent_pit] += 1;
                if(stones == 1){
                    Pit res = {opponent_pit, "p1"};
                    return res;
                }
                opponent_pit += 1;
            }            
            stones -= 1;
                
            lcd.cls();
            lcd.locate(0,0);
            printAll(p1,p2, -1);
            wait(0.5);
        }
       
        // Mettre dans le grenier de l'adversaire 
        int current_player_pit;
        if(p1.isTurn){
            p2.score += 1;
            current_player_pit = 0;
            if(stones == 1){
                Pit res = {0, "g1"};
                return res;
            }
        }else{
            p1.score += 1;
            current_player_pit = 5;
            if(stones == 1){
                Pit res = {0, "g2"};
                return res;
            }
        }
        stones -= 1;

        
        while((stones > 0) and (0 <= current_player_pit) and (current_player_pit <= 5 )){
            if(p1.isTurn){
                p1.pits[current_player_pit] += 1;
                if(stones == 1){
                    Pit res = {current_player_pit, "p1"};
                    return res;    
                }
                current_player_pit += 1; 
            } else{
                p2.pits[current_player_pit] += 1;
                if(stones == 1){
                    Pit res = {current_player_pit, "p2"};
                    return res;     
                }
                current_player_pit -= 1;
            }
            
            stones -= 1;
                
            lcd.cls();
            lcd.locate(0,0);
            printAll(p1,p2, -1);
            wait(0.5);
        }
      
    }
    Pit res = {pit, "p1"};
    return res;
}


bool take_stones(Player &p1, Player &p2, Pit last_pit){
    // Returns true if the stones can be taken (after taking them) and false if not
    // Checks if last stone was put in an empty pit of the current players' side
    // And increases the score if so
            // lcd.cls();
            // lcd.locate(0,0);
            // lcd.printf("last_pit: %d",
            //     last_pit.pit
            // );
            // wait(3);

    if((last_pit.player == "p1") and (p1.isTurn)){
        if(p1.pits[last_pit.pit] == 1){
        //     lcd.cls();
        //    lcd.locate(0,0);
        //     lcd.printf("In take pit");
        //     wait(0.5);
            // Take the stones and add them to current_player's score
            p1.score += p2.pits[last_pit.pit] + p1.pits[last_pit.pit];
            p2.pits[last_pit.pit] = 0;
            p1.pits[last_pit.pit] = 0;
            return true;
        }
    }

     if((last_pit.player == "p2") and (p2.isTurn)){
        if(p2.pits[last_pit.pit] == 1){
        //     lcd.cls();
        //    lcd.locate(0,0);
        //    lcd.printf("In take pit");
        //     wait(0.5);
            // Take the stones and add them to current_player's score

            p2.score += p1.pits[last_pit.pit] + p2.pits[last_pit.pit];
            p1.pits[last_pit.pit] = 0;
            p2.pits[last_pit.pit] = 0;
            return true;
        }
    }
    return false;
}

void game_over(Player &p1, Player &p2) {
    lcd.cls();
    lcd.locate(0, 0);
    if (p1.score > p2.score) {
        lcd.printf("Player 1 wins!");
    } else if (p1.score < p2.score) {
        lcd.printf("Player 2 wins!");
    } else {
        lcd.printf("It's a draw!");
    }
    wait(3);
}

/*
int choose_player(){
    bool selection = false;
    while(true){
        if(joy[2] or joy[3]){
            selection = !selection;
            lcd.cls();
            lcd.locate(0, 0);
            if(selection){
                lcd.printf("Choose player : \nPlayer 1 <-\nPlayer2");
            }
            else if(selection){
                lcd.printf("Choose player : \nPlayer 1 \nPlayer2 <-");
            }
            
            // lcd.cls();
            // lcd.printf("Choix -1\n valeur actuelle %d", choice);
             wait(0.2);
        }
        if (joy[4].read() == 1) {
            break;
        }

    }
    return selection;
}

void test_printAll(){
    Player p1 = {0, {4, 4, 4, 4, 4, 4}, false};
    Player p2 = {0, {4, 4, 4, 4, 4, 4}, true};
    while(1){
        p1.isTurn = !p1.isTurn;
        p2.isTurn = !p2.isTurn;
        for(int i =0; i<6; i++){
            printAll(p1, p2, i);
            wait(0.8);
            lcd.cls();
        } 
    }
}
*/
bool isGameOver(Player p1, Player p2){
    bool res1 = true, res2 = true;
    for (int i = 0; i < 6; i++) {
        if (p1.pits[i] > 0) {
            res1 = false;
            break;
        }
    }

    for (int i = 0; i < 6; i++) {
        if (p2.pits[i] > 0) {
            res2 = false;
            break;
        }
    }
    return (res1 or res2);
}

int main(){
    int choice = 0; Pit last_pit = {-1 , ""} ;
    bool error = false;
    while (1) {
        lcd.cls();
        lcd.locate(0,0);
        lcd.printf("Start game !");
        wait(1);
        lcd.cls();

        Player p1 = {0, {4, 4, 4, 4, 4, 4}, true};
        Player p2 = {0, {4, 4, 4, 4, 4, 4}, false};
        lcd.locate(0,0);
        
        while (true) {  
            lcd.cls();
            // lcd.locate(0,0);
            // lcd.printf("turn: %d  %d", p1.isTurn, p2.isTurn);
            // wait(0.5);
            if(p1.isTurn){
                red = 0;
                green = 1;
                blue = (208)/255;
            }else{
                red = 1-(166)/255;
                green = 1;
                blue = 0;
            }


            printAll(p1, p2, -1);

            choice = 0; Pit last_pit = {0 , ""} ;
            
            error = false;

            // do{
            //     lcd.cls(); 
            //     lcd.locate(0,0);
            //     if(error){lcd.printf("Error. Choose again.");}
            //     wait(1);
            //     choice = choose_pit(p1, p2);
            //     last_pit = move_stones(p1, p2, choice);
            //     if(last_pit.pit==-1){ error = true;}
            // }while(last_pit.pit == -1);

            // int n= 0;
            //     int choice = choose_pit(p1, p2);
            //     if(p1.isTurn){
            //         n = p1.pits[choice];
            //         while(n <= 0){
            //             choice = choose_pit(p1, p2);
            //             n = p1.pits[choice];
            //         }
            //     } else{
            //         n = p2.pits[choice];
            //         while(n <= 0){
            //             choice = choose_pit(p1, p2);
            //             n = p2.pits[choice];
            //         }
            //     }
            //     last_pit = move_stones(p1,p2,choice);
           
            choice = choose_pit(p1, p2);
            last_pit = move_stones(p1, p2, choice);


            take_stones(p1,p2, last_pit);

            printAll(p1,p2,-1);

            if(p1.isTurn){
                p1.isTurn = false; 
                p2.isTurn = true;
            }
            else{
                p1.isTurn = true; 
                p2.isTurn = false;
            }


            printAll(p1,p2,-1);


            // bool isGameOver = true;
            // if()
          

            if (isGameOver(p1,p2)) {
                game_over(p1, p2);
                break;
            }
        }
    }
}

