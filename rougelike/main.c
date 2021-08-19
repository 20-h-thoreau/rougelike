#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define lowestroomconstant 10
int stagesizey;
int stagesizex;
int roomconstant;//how ofte rooms appear
int buildtime; //howlong we build for
int roomlengthmax;
int roomlenghtmin;
int branches;



typedef struct enemyattritbutes {
    char type;
    unsigned int x;
    unsigned int y;
    int HP;
    int Attack;
    int accuracy;
    char standingon;
} enemyattributes;

struct enemyattritbutes *enemystructs[256]; //pointer to enemy structs
int enemycount; //lengths of enemy structs

typedef struct playerattributes {
    unsigned int x;
    unsigned int y;
    int HP;
    int MaxHP;
    int Attack;
    unsigned int Gold;
    unsigned int level;
    char standingon; //this is what block the player is currently standing on;
} playerattributes;

struct playerattributes player;
//lets have it make a path randomly

void generateenemy(unsigned int X, unsigned int Y);
struct enemyattritbutes *whichenemyatcoord(unsigned int x,unsigned int y);
void attack(struct enemyattritbutes *enemyptr);

int main(int argc, const char * argv[]) {
    srand(time(0));
    stagesizex=64;
    stagesizey=64;
    player.Attack=10;// we can set this to weapon attack maybe?
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
        int enemycount=0;
        
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
                    case 2:
                        //map[y][x]='S';
                        if (x!=startingx && y!=startingy){
                            generateenemy(x, y);
                            ++enemycount;
                        }
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
                if (buildcounter>=buildtime){
                    break;
                }
                
            }
            ++buildtotal;
            if (buildtotal==branches){
                map [y][x]='%';
            }
        }
        
        
        
        
        
        // player
        bool level=false;
        player.standingon='.';
        while (!level){

            switch (player.standingon){
                case '$':
                    player.standingon='.';
                    player.Gold+=5;
            }
            
            for (int i=0; i<enemycount; ++i){
                if (enemystructs[i]->HP>0){
                    map[enemystructs[i]->y][enemystructs[i]->x]=enemystructs[i]->type;
                }
                else{
                    map[enemystructs[i]->y][enemystructs[i]->x]=enemystructs[i]->standingon;
                }
            }
            
            
            map[player.y][player.x]='@';
            
            
            printf("level:%d \n", player.level+1);
            for(int y=0; y<stagesizey; ++y){
                for(int x=0; x<stagesizex; ++x){
                    printf("%c",map[y][x]);
                }
            }

            printf("\n HP:%d/%d  Gold:%d \n \n", player.HP, player.MaxHP,player.Gold);
            char input;
        levelloop:
            input=getchar(); //player input
            if(input=='\n'){
                goto levelloop;
            }
            //struct enemyattritbutes *enemyptr;
            switch (input){
                case 'h':
                    
                    switch(map[player.y][player.x-1]){
                        case ' ':
                            break;
                        case '#':
                            break;
                        case 'S':
                            attack(whichenemyatcoord(player.x-1, player.y));
                            break;
                        default:
                            map[player.y][player.x]=player.standingon;
                            --player.x;
                            player.standingon=map[player.y][player.x];
                            break;
                    }
                    break;
                case 'j':
                    
                    switch(map[player.y-1][player.x]){
                        case ' ':
                            break;
                        case '#':
                            break;
                        case 'S':
                            attack(whichenemyatcoord(player.x, player.y-1));
                            break;
                        default:
                            map[player.y][player.x]=player.standingon;
                            --player.y;
                            player.standingon=map[player.y][player.x];
                            break;
                    }
                    break;
                case 'k':
                    switch(map[player.y+1][player.x]){
                        case ' ':
                            break;
                        case '#':
                            break;
                        case 'S':
                            attack(whichenemyatcoord(player.x, player.y+1));
                            
                            break;
                        default:
                            map[player.y][player.x]=player.standingon;
                            ++player.y;
                            player.standingon=map[player.y][player.x];
                            break;
                    }
                    break;
                case 'l':
                    switch(map[player.y][player.x+1]){
                        case ' ':
                            break;
                        case '#':
                            break;
                        case 'S':
                            attack(whichenemyatcoord(player.x+1, player.y));
                            
                            break;
                        default:
                            map[player.y][player.x]=player.standingon;
                            ++player.x;
                            player.standingon=map[player.y][player.x];
                            break;
                    }
                    break;
                case '>':
                    if (player.standingon=='%'){
                        level=true;
                        ++player.level;
                    }
                    break;
                default:
                    break;
            }
        
            //enemyai
            for (int i=0; i<enemycount; ++i){
                
                if (enemystructs[i]->HP>0){
                    signed int x=enemystructs[i]->x-player.x;
                    signed int y=enemystructs[i]->y-player.y;
                    signed int movechangedir=-1;
                    int move=0xffff;
                    
                    if (x==0 && y>0){
                        move=0;
                    }
                    else if (x>0 && y>0){
                        move=1;
                    }
                    else if (x>0 && y==0){
                        move=2;
                    }
                    else if (x>0 && y>0){
                        move=3;
                    }
                    else if (x==0 && y<0){
                        move=4;
                    }
                    else if (x>0 && y<0){
                        move=5;
                    }
                    else if (x>0 && y==0){
                        move=6;
                    }
                    else if (x>0 && y>0){
                        move=7;
                    }
                    
                    /*
                     
                     
                     if x neg we need to move right
                     if y neg we need to move down
                     if they cant move there, then we need to find which one is farthest from 0 and have them walk in that direction.
                    000
                 111    001
                110      010
                 101    011
                    100
                     
                     */
                    //bigger y=further down
                    bool attacked=false;
                    bool canmove=false;
                    signed int xadd=0;
                    signed int yadd=0;
                    while (!canmove){
                        move=move&0x07;
                        switch (move){
                            case 0:
                                xadd=0;
                                yadd=-1;
                                break;
                            case 1:
                                xadd=1;
                                yadd=-1;
                                break;
                            case 2:
                                xadd=1;
                                yadd=0;
                            case 3:
                                xadd=1;
                                yadd=1;
                                break;
                            case 4:
                                yadd=1;
                                xadd=0;
                                break;
                            case 5:
                                yadd=1;
                                xadd=-1;
                                break;
                            case 6:
                                xadd=-1;
                                yadd=0;
                                break;
                            case 7:
                                xadd=-1;
                                yadd=-1;
                                break;
                            default:
                                xadd=0;
                                yadd=0;
                                break;
                        }
                        
                        int enemynewlocationx=enemystructs[i]->x+xadd;
                        int enemynewlocationy=enemystructs[i]->y+yadd;
                        
                        switch(map[enemynewlocationy][enemynewlocationx]){
                            case ' ':
                                break;
                            case '#':
                                break;
                            case 'S':
                                break;
                            case '@':
                                attack(enemystructs[i]);
                                attacked=true;
                                break;
                            case '.':
                                canmove=true;
                                break;
                            
                            default:
                                canmove=true;
                                break;
                                
                        }
                        
                        if (!canmove){
                            /*if(movechangedir==0){
                                movechangedir=-1;//need to make it soemthing cooler
                            }*/
                            move+=movechangedir;
                        }
                        else if (attacked){
                            goto endofloop;
                        }
                        
                    }
      
                    //now time to move the enemy
                    map[enemystructs[i]->y][enemystructs[i]->x]=enemystructs[i]->standingon;
                    enemystructs[i]->y+=yadd;
                    enemystructs[i]->x+=xadd;
                    enemystructs[i]->standingon=map[enemystructs[i]->y][enemystructs[i]->x];
                    //map[enemystructs[i]->y][enemystructs[i]->x]=enemystructs[i]->type;
                endofloop:
                    x=x;
                }
            
            }
            
            
        }
        
        
    }
    
    
    
    
    
    return 0;

}



void generateenemy(unsigned int X, unsigned int Y){

    enemystructs[enemycount]=(enemyattributes*)malloc(sizeof(enemyattributes));
    
    enemystructs[enemycount]->type='S';
    enemystructs[enemycount]->x=X;
    enemystructs[enemycount]->y=Y;
    enemystructs[enemycount]->HP=10;
    enemystructs[enemycount]->Attack=3;
    enemystructs[enemycount]->accuracy=3;
    enemystructs[enemycount]->standingon='.';
    ++enemycount; //no idea why but I have int incriment this outside it for the map ti wirk and inside here for the poitners to work.
    
}

struct enemyattritbutes *whichenemyatcoord(unsigned int x,unsigned int y){
    for (int i=0; i<enemycount; ++i){
        if (enemystructs[i]->x==x && enemystructs[i]->y==y){
            return enemystructs[i];
        }
    }
    return (struct enemyattritbutes *)&player;
}

void attack(struct enemyattritbutes *enemyptr){
    if (rand()%10!=0){
        player.HP-=enemyptr->Attack;
        printf ("Player hit, ");
    }
    else {
        printf ("Player misses, ");
    }
    if (rand()%enemyptr->accuracy!=0){
        enemyptr->HP-=player.Attack;
        
        printf ("enemy hit \n");
    }
    else {
        printf ("enemy misses \n");
    }
}
