#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#define lowestroomconstant 10
int stagesizey;
int stagesizex;
int roomconstant;//how ofte rooms appear
int buildtime; //howlong we build for
int roomlengthmax;
int roomlenghtmin;
int branches;

typedef struct playerattributes {
    unsigned int x;
    unsigned int y;
    int HP;
    int MaxHP;
    unsigned int Gold;
    unsigned int level;
    char standingon; //this is what block the player is currently standing on;
} playerattributes;

struct playerattributes player;
//lets have it make a path randomly
void buildroom(int x, int y);

int main(int argc, const char * argv[]) {
    
    stagesizex=64;
    stagesizey=64;
    
    roomconstant=255;//how often rooms appear
    buildtime=1000; //howlong we build for
    roomlengthmax=24;
    roomlenghtmin=10;
    branches=5;
    player.MaxHP=30;
    
    //this reintiizlizes varibles based on the envirometnal variables given by the user
    for (int i=1; i<argc; ++i){
        switch (*argv[i]){
            case 'x':
                ++i;
                stagesizex=atoi(argv[i]);
                break;
            case 'y':
                ++i;
                stagesizey=atoi(argv[i]);
                break;
            case 'h':
                printf("x <n> width of levels \ny <n> hieght of levels");
                return 0;
                
            
        }
    }
    
    
    
     

    
    buildtime=buildtime/branches;
    
    if (roomconstant<lowestroomconstant){
        roomconstant=lowestroomconstant;
    }
    char map[stagesizey][stagesizex];
    
    for (unsigned short y=0; y<stagesizey; ++y){
        for (unsigned short x=0; x<stagesizex; ++x){

            map[y][x]=' ';

            if (x==0 || x==stagesizex-2 || y==0 || y==stagesizey-1){
                map[y][x]= '#';
            }
            if (x==stagesizex-1){
                map[y][x]='\n';
            }
            
        }
    }
    map[stagesizey-1][stagesizex-1]='\0';
    player.HP=player.MaxHP;
    
    
    while(player.HP>0){
        //level generation
        
        int startingx=(rand()%stagesizex+1)-2, startingy=(rand()%stagesizex+1)-2;
        int room;
        player.x=startingx;
        player.y=startingy;
        int direction;
        int buildtotal=0;
        
        
        while(buildtotal<branches){
            direction=rand()&0x3;
            int buildcounter=0;
            int x=startingx, y=startingy;
            while(1){
                
                map[y][x]='.';
                
                
                room=rand()%roomconstant;
                int xlength, ylength;
                switch(room) {//this will decide what we do, whether we move, or build a room
                    case 0:
                        if(map[y][x+1]==' ' && map[y][x-1]==' ' && map[y+1][x]==' ' && map[y-1][x]==' '){
                            xlength=(rand()%(roomlengthmax-roomlenghtmin)+roomlenghtmin);
                            ylength=(rand()%(roomlengthmax-roomlenghtmin)+roomlenghtmin);
                            for (int Y=y;Y<ylength+y;++Y){
                                for (int X=x;X<xlength+x; ++X){
                                    map[y][x]='.';
                                }
                            }
                        }
                        break;
                    case 1:
                        map[y][x]='$';
                        break;
                    default :
                        break;
                }
                if (room>(roomconstant-(roomconstant/5))){ //turn off thischeck in cave mode
                direction=rand()&0x3;
            }
                
            move:
                switch (direction){
                    case 0:
                        ++x;
                        break;
                    case 1:
                        --y;
                        break;
                    case 2:
                        --x;
                        break;
                    case 3:
                        ++y;
                        break;
                    default:
                        break;
                }
                
                if (map[y][x]=='#'){
                    direction=(direction+2)&0x3;
                    goto move;
                }
                
                ++buildcounter;
                if (buildcounter==buildtime){
                    break;
                }
                
            }
            ++buildtotal;
            if (buildtotal==branches){
                map [y][x]='>';
            }
        }
        
        
        // player
        bool level=false;
         
        while (!level){
            
        switch (player.standingon){
            case '$':
                player.standingon='.';
                player.Gold+=5;
        }
            
            map[player.y][player.x]='@';
            
            printf("level:%d \n", player.level+1);
            for(int y=0; y<stagesizey; ++y){
                for(int x=0; x<stagesizex; ++x){
                    printf("%c",map[y][x]);
                }
            }
            
            printf("\n HP:%d/%d  Gold:%d \n \n", player.HP, player.MaxHP,player.Gold);
            
            char input=getchar(); //player input
            
            switch (input){
                case 'h':
                    if (map[player.y][player.x-1]!=' ' &&  map[player.y][player.x-1]!='#'){
                        map[player.y][player.x]=player.standingon;
                        --player.x;
                        player.standingon=map[player.y][player.x];
                    }
                    break;
                case 'j':
                    if(map[player.y-1][player.x]!=' ' &&  map[player.y+1][player.x]!='#'){
                        map[player.y][player.x]=player.standingon;
                        --player.y;
                        player.standingon=map[player.y][player.x];
                    }
                    break;
                case 'k':
                    if(map[player.y+1][player.x]!=' ' &&  map[player.y-1][player.x]!='#'){
                        map[player.y][player.x]=player.standingon;
                        ++player.y;
                        player.standingon=map[player.y][player.x];
                    }
                    break;
                case 'l':
                    if (map[player.y][player.x+1]!=' ' &&  map[player.y][player.x+1]!='#'){
                        map[player.y][player.x]=player.standingon;
                        ++player.x;
                        player.standingon=map[player.y][player.x];
                    }
                    break;
                case '>':
                    if (player.standingon=='>'){
                        level=true;
                        ++player.level;
                    }
                    break;
                default:
                    break;
            }
            
        }
        
        
    }
    return 0;

}

    
void buildroom(int x, int y){

}

