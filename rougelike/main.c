#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <math.h>
#define lowestroomconstant 10
#define accu_mult 6
#define player_accu_mult 2
#define maximumlevel sizeof(levelup)
int stagesizey;
int stagesizex;
int roomconstant;//how ofte rooms appear
int buildtime; //howlong we build for
int roomlengthmax;
int roomlenghtmin;
int branches;

int d20(int num);
int d12(int num);
int d10(int num);
int d8(int num);
int d6(int num);
int d4(int num);
int dice(int count, int num);


typedef struct enemyattritbutes {
    char *name;
    char type;
    unsigned int x;
    unsigned int y;
    int HP;
    int count;
    int dice;
    int accuracy;
    char standingon;
    int expgain;
} enemyattributes;

struct enemyattritbutes *enemystructs[256]; //pointer to enemy structs

typedef const struct enemydata {
    char *name;
    int HP;
    int count;
    int dice;
    signed int Accuracy;
    int expgain;

} enemydata;

const struct enemydata salamander={"Salamander\0",10,1,4,4,10};
const struct enemydata goblin={"Goblin\0",20,1,10,-1,20};

int enemycount; //lengths of enemy structs



//lets have it make a path randomly
typedef const struct weapondata {
    char *name;
    int count;
    int dice;
    int accuracy;
} weapondata;

const struct weapondata dagger={"dagger\0",1,6,3};
const struct weapondata sword={"sword\0", 3,4,5};
const struct weapondata axe={"axe\0",2,4,2};

typedef const struct potiondata{
    char *name;
    void (*function)(void);
} potiondata;


void healthpotion(void);
const struct potiondata health_potion={"Potion of Healing\0", &(healthpotion)};


typedef struct Inventory {
    char type[128];
    void *ptr[128];
    char quantity[128];
} Inventory;

struct Inventory playerinventory;

typedef struct playerattributes {
    char *name;
    unsigned int x;
    unsigned int y;
    int HP;
    int MaxHP;
    const struct weapondata *weaponequiped;
    int Attack;
    int accuracyStat;
    unsigned int Gold;
    unsigned int exp;
    unsigned int level;
    unsigned int floor;
    char standingon; //this is what block the player is currently standing on;
    struct Inventory *inventory;
} playerattributes;

struct playerattributes player;

const int levelup[7]={200,400,800,1600,3200,6400,12800};

int generateenemy(unsigned int X, unsigned int Y, enemydata *dataptr);
struct enemyattritbutes *whichenemyatcoord(unsigned int x,unsigned int y);
void attack(struct enemyattritbutes *enemyptr);
void printhelpmenu(void);
int displayinventory(void);
int useitem(void);
void clearitemslot(char item);

int main(int argc, const char * argv[]) {
    
    srand(time(0));
    stagesizex=32;
    stagesizey=32;
    player.inventory=&playerinventory;
    for(char i=0; i<0x7f; ++i){
        player.inventory->ptr[i]=NULL;
    }
    int temp=0;
    player.inventory->type[temp]='w';
    player.inventory->ptr[0]=&dagger;
    ++temp;
    player.inventory->type[temp]='p';
    player.inventory->ptr[temp]=&health_potion;
    player.inventory->quantity[temp]=5;
    ++temp;
    player.inventory->type[temp]='q';//this is the exit option to exit the menu;
    player.inventory->ptr[temp]=player.inventory;
    
    player.weaponequiped = &sword;
    player.Attack=0;// we can set this to weapon attack maybe?
    roomconstant=255;//how often rooms appear
    buildtime=1000; //howlong we build for
    roomlengthmax=24;
    roomlenghtmin=10;
    branches=5;
    player.MaxHP=30;
    player.name="Player\0";
    player.accuracyStat=3;
    player.level=0;
    player.floor=0;
    //this reintiizlizes varibles based on the envirometnal variables given by the user
    for (int i=1; i<argc; ++i){
        /*
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
            case 'n':
                ++i;
                strcpy(player.name, argv[i]);
                break;
            */
        if (strcmp(argv[i], "-x")==0){
            
            stagesizex=atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-y")==0){
            
            stagesizey=atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-n")==0){
            //this doesnt work for some reason
            player.name=strdup(argv[++i]);
        }
        else if (strcmp(argv[i], "-h")==0){
            printhelpmenu();
            return 0;
        }
    }
    
    
    
    
     

    
    buildtime=buildtime/branches;
    
    if (roomconstant<lowestroomconstant){
        roomconstant=lowestroomconstant;
    }
    char map[stagesizey][stagesizex];
    
    for (unsigned int y=0; y<stagesizey; ++y){
        for (unsigned int x=0; x<stagesizex; ++x){

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
                int xlength, ylength,ret;
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
                            ret=generateenemy(x, y, &salamander);
                            if(ret==0){++enemycount;}
                        }
                        break;

                    case 3:
                        //map[y][x]='S';
                        if (x!=startingx && y!=startingy){
                            ret=generateenemy(x, y, &salamander);
                            if(ret==0){++enemycount;}
                        }
                        break;
                        
                    case 4:
                        //map[y][x]='G';
                        if (x!=startingx && y!=startingy){
                            ret=generateenemy(x, y, &goblin);
                            if(ret==0){++enemycount;}
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
        while (!level && player.HP>0){
            
            switch (player.standingon){
                case '$':
                    player.standingon='.';
                    player.Gold+=5;
                case 'A'://sometimes it glitches and he drags a enemy on his boot.
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                case 'G':
                case 'H':
                case 'I':
                case 'J':
                case 'K':
                case 'L':
                case 'M':
                case 'N':
                case 'O':
                case 'P':
                case 'Q':
                case 'R':
                case 'S':
                case 'T':
                case 'U':
                case 'V':
                case 'W':
                case 'X':
                case 'Y':
                case 'Z':
                    player.standingon='.';
                    break;
            }
            
            for (int i=0; i<enemycount; ++i){
                if(enemystructs[i]->y==player.y && enemystructs[i]->x==player.x){
                    enemystructs[i]->HP=0;
                }
                if (enemystructs[i]->HP>0 && (sqrt(( (enemystructs[i]->y-player.y)*(enemystructs[i]->y-player.y))+((enemystructs[i]->x-player.x) *(enemystructs[i]->x-player.x))) <=player.accuracyStat*player_accu_mult)){
                    map[enemystructs[i]->y][enemystructs[i]->x]=enemystructs[i]->type;
                }
                else{
                    map[enemystructs[i]->y][enemystructs[i]->x]=enemystructs[i]->standingon;
                }

            }
            
            
            map[player.y][player.x]='@';
            int playeriventorychecks=0;
    
            if (player.exp>=levelup[player.level] && player.level!=maximumlevel){
                player.exp-=levelup[player.level++];
            }

            printf("\nlevel:%d\n", player.level+1);
            printf("exp: %d / %d",player.exp, levelup[player.level]);
            printf("                                           ");
            
            printf("floor %d\n", player.floor+1);
            for(int y=0; y<stagesizey; ++y){
                for(int x=0; x<stagesizex; ++x){
                    printf(" %c",map[y][x]);//the space after is to space it properly so its sqaureish
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
                        case 'A'://sometimes it glitches and he drags a enemy on his boot.
                        case 'B':
                        case 'C':
                        case 'D':
                        case 'E':
                        case 'F':
                        case 'G':
                        case 'H':
                        case 'I':
                        case 'J':
                        case 'K':
                        case 'L':
                        case 'M':
                        case 'N':
                        case 'O':
                        case 'P':
                        case 'Q':
                        case 'R':
                        case 'S':
                        case 'T':
                        case 'U':
                        case 'V':
                        case 'W':
                        case 'X':
                        case 'Y':
                        case 'Z':
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
                        case 'A'://sometimes it glitches and he drags a enemy on his boot.
                        case 'B':
                        case 'C':
                        case 'D':
                        case 'E':
                        case 'F':
                        case 'G':
                        case 'H':
                        case 'I':
                        case 'J':
                        case 'K':
                        case 'L':
                        case 'M':
                        case 'N':
                        case 'O':
                        case 'P':
                        case 'Q':
                        case 'R':
                        case 'S':
                        case 'T':
                        case 'U':
                        case 'V':
                        case 'W':
                        case 'X':
                        case 'Y':
                        case 'Z':
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
                        case 'A'://sometimes it glitches and he drags a enemy on his boot.
                        case 'B':
                        case 'C':
                        case 'D':
                        case 'E':
                        case 'F':
                        case 'G':
                        case 'H':
                        case 'I':
                        case 'J':
                        case 'K':
                        case 'L':
                        case 'M':
                        case 'N':
                        case 'O':
                        case 'P':
                        case 'Q':
                        case 'R':
                        case 'S':
                        case 'T':
                        case 'U':
                        case 'V':
                        case 'W':
                        case 'X':
                        case 'Y':
                        case 'Z':
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
                        case 'A'://sometimes it glitches and he drags a enemy on his boot.
                        case 'B':
                        case 'C':
                        case 'D':
                        case 'E':
                        case 'F':
                        case 'G':
                        case 'H':
                        case 'I':
                        case 'J':
                        case 'K':
                        case 'L':
                        case 'M':
                        case 'N':
                        case 'O':
                        case 'P':
                        case 'Q':
                        case 'R':
                        case 'S':
                        case 'T':
                        case 'U':
                        case 'V':
                        case 'W':
                        case 'X':
                        case 'Y':
                        case 'Z':
                            attack(whichenemyatcoord(player.x+1, player.y));
                            printf("the player attacks:\n");
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
                        ++player.floor;
                    }
                    break;
                case 'i':
                    displayinventory();
                    playeriventorychecks=useitem();
                    if(playeriventorychecks<=1){
                    goto levelloop;
                    }
                    else{
                        break;
                    }
                default:
                    break;
            }
        
            //enemyai
            for (int i=0; i<enemycount; ++i){
                
                if (enemystructs[i]->HP>0){
                    signed int x=enemystructs[i]->x-player.x;
                    signed int y=enemystructs[i]->y-player.y;
                    signed int movechangedir=0;
                    int move=0x00;
                    
                    /*
                     
                     enemy 0,0
                     
                     player 1,1
                     downright= neg neg
                     
                     
                     */

                    if (x>enemystructs[i]->accuracy*accu_mult || y>enemystructs[i]->accuracy*accu_mult){
                        move=rand()&0x7;
                    } else if (x==0 && y>0){
                        move=0;
                    }
                    else if (x<0 && y>0){
                        move=1;
                    }
                    else if (x<0 && y==0){
                        move=2;
                    }
                    else if (x<0 && y<0){
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
                    
                    bool canmove=false;
                    signed int xadd=0;
                    signed int yadd=0;
                    int numberofloops=0;;
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
                                canmove=true;
                                //printf("this shouldnt be happening\n");
                                break;
                        }
                        
                        int enemynewlocationx=enemystructs[i]->x+xadd;
                        int enemynewlocationy=enemystructs[i]->y+yadd;
                        
                        switch(map[enemynewlocationy][enemynewlocationx]){
                            case ' ':
                                //printf("tried to walk on blankspace\n");
                                break;
                            case '#':
                                break;
                            case 'A':
                            case 'B':
                            case 'C':
                            case 'D':
                            case 'E':
                            case 'F':
                            case 'G':
                            case 'H':
                            case 'I':
                            case 'J':
                            case 'K':
                            case 'L':
                            case 'M':
                            case 'N':
                            case 'O':
                            case 'P':
                            case 'Q':
                            case 'R':
                            case 'S':
                            case 'T':
                            case 'U':
                            case 'V':
                            case 'W':
                            case 'X':
                            case 'Y':
                            case 'Z':
                                break;
                            default:
                                canmove=true;
                                break;
                                
                        }
                        if (enemynewlocationx==player.x && enemynewlocationy==player.y){
                            printf("The %s attacks:\n", enemystructs[i]->name);
                            attack(enemystructs[i]);
                            xadd=0;
                            yadd=0;
                            canmove=true;
                            break;
                        } else if (numberofloops>7){
                            canmove=true;
                            xadd=0;
                            yadd=0;
                            //printf("cant move");
                        } else if (!canmove){
                            if(movechangedir==0){
                                movechangedir=( ( (rand()&0x01)<<1 )-1 );//need to make it soemthing cooler
                                //maybe we could fix it by counting 8 tries and when 8 tries fail it jsut styops trying/
                            }
                            move+=movechangedir;
                            ++numberofloops;
                            
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




int generateenemy(unsigned int X, unsigned int Y, enemydata *dataptr){

    
    if(X>stagesizex || Y> stagesizey){
        //for some reason this happens sometimes and could mess stuff up :/
        return 1;
    }
    enemystructs[enemycount]=(enemyattributes*)malloc(sizeof(enemyattributes));
    enemystructs[enemycount]->name=dataptr->name;
    enemystructs[enemycount]->type=dataptr->name[0];
    enemystructs[enemycount]->x=X;
    enemystructs[enemycount]->y=Y;
    enemystructs[enemycount]->HP=dataptr->HP;
    enemystructs[enemycount]->dice=dataptr->dice;
    enemystructs[enemycount]->count=dataptr->count;
    enemystructs[enemycount]->accuracy=dataptr->Accuracy;
    enemystructs[enemycount]->expgain=dataptr->expgain;
    enemystructs[enemycount]->standingon='.';
   
    ++enemycount; //no idea why but I have int incriment this outside it for the map ti wirk and inside here for the poitners to work.
    return 0;
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
    int weaponaccuracy=player.accuracyStat+player.weaponequiped->accuracy;
    if (d20(1)+weaponaccuracy>=10){
        enemyptr->HP-=player.Attack+dice(player.weaponequiped->count, player.weaponequiped->dice);
        printf ("%s hit, ", player.name);
        if(enemyptr->HP<=0){
            player.exp+=enemyptr->expgain;
            printf("%s dies\n", enemyptr->name);
            return;
        }
    }
    else {
        printf ("%s misses, ", player.name);
    }
    if (d20(1)+enemyptr->accuracy>10){
        player.HP-=dice(enemyptr->count,enemyptr->dice);
        
        printf ("%s hit \n", enemyptr->name);
    }
    else {
        printf ("%s misses \n", enemyptr->name);
    }
}

void printhelpmenu(){
    printf("-x <n> width of levels \n-y <n> hieght of levels \n-n <name> enter the name of your player \n-h help\n");
    return;
}

int displayinventory(){
    void *pointer;
    for (char i=0; i<0x7f && playerinventory.ptr[i&0x7f]!=NULL; ++i){
        printf("(%d) ",i+1);
        switch (playerinventory.type[i]){
            case 'p':
                pointer=playerinventory.ptr[i];
                printf("%s x%d", ((const struct potiondata *)pointer)->name, player.inventory->quantity[i]);
                break;
            case 'w':
                pointer=playerinventory.ptr[i];
                printf("%s", ((const struct weapondata *)pointer)->name);
                break;
            case 's':
                break;
            case 'q':
                printf("exit");
                break;
        }
        printf("\n");
            
    }
    printf("\n");
    return 0;
}

int useitem(){
    int item;
    void *pointer;
    char test='a';
    while(test!='\n'){
        test=getchar();
    }
getnewchar:
    scanf("%d", &item);
    --item;
    if (item>0x7f){
        printf("sorry, that number is too large\n");
        goto getnewchar;
    }
    switch (player.inventory->type[item]){
        case 'p':
            pointer=player.inventory->ptr[item];
            ((const struct potiondata *)pointer)->function();
            --player.inventory->quantity[item];
            if (player.inventory->quantity[item]<=0){
                clearitemslot((char)item);
            }
            printf("\n%s drank a %s\n", player.name, ((const struct potiondata *)pointer)->name);
            break;
        case 'w':
            player.weaponequiped=player.inventory->ptr[item];
            printf("\n%s equiped a %s\n", player.name, ((const struct weapondata *)player.weaponequiped)->name);
            break;
        case 'q':
            printf("\n");
            return 0;
        
    }
    return 1;
}

void clearitemslot(char item){
    for (char i=item; i<0x7f && playerinventory.ptr[i&0x7f]!=NULL; ++i){
        playerinventory.type[i]=playerinventory.type[i+1];
        playerinventory.ptr[i]=playerinventory.ptr[i+1];
        playerinventory.quantity[i]=playerinventory.quantity[i+1];
    }
}
/*typedef struct Inventory {
 char type[256];
 const void *ptr[256];
 char quantity[256];
} Inventory;

struct Inventory playerinventory;*/
// potions

void healthpotion(){
    
    player.HP+=(d4(3+(player.level>>2))+(5*(player.level+1)));
    if (player.HP>player.MaxHP){
        player.HP=player.MaxHP;
    }
    player.exp=+5;
}


int d20(int num){
    int ret=0;
    for (int i=0; i<num; ++i){
         ret+=(rand()%20);
    }
    return ret;
}
                                    
int d12(int num){
    int ret=0;
    for (int i=0; i<num; ++i){
         ret+=(rand()%12);
    }
    return ret;
}
int d10(int num){
    int ret=0;
    for (int i=0; i<num; ++i){
         ret+=(rand()%10);
    }
    return ret;
}

int d8(int num){
    int ret=0;
    for (int i=0; i<num; ++i){
         ret+=(rand()%8);
    }
    return ret;
}

int d6(int num){
    int ret=0;
    for (int i=0; i<num; ++i){
         ret+=(rand()%6);
    }
    return ret;
}

int d4(int num){
    int ret=0;
    for (int i=0; i<num; ++i){
         ret+=(rand()%4);
    }
    return ret;
}
int dice(int count,int num){
    switch (num){
        case 4:
            return d4(count);
        case 6:
            return d6(count);
        case 8:
            return d8(count);
        case 10:
            return d10(count);
        case 12:
            return d12(count);
        case 20:
            return d20(count);
        default:
            return 0;
        }
}
