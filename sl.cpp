/*========================================
 *    sl.c: SL version 5.03
 *        Copyright 1993,1998,2014-2015
 *                  Toyoda Masashi
 *                  (mtoyoda@acm.org)
 *        Last Modified: 2021/02/06
 *========================================
 */
/* sl version 5.04 : SL now shows random art. Added several flag options     */
/*                                              by Jacob French   2021/02/06 */
/* sl version 5.03 : Fix some more compiler warnings.                        */
/*                                              by Ryan Jacobs    2015/01/19 */
/* sl version 5.02 : Fix compiler warnings.                                  */
/*                                              by Jeff Schwab    2014/06/03 */
/* sl version 5.01 : removed cursor and handling of IO                       */
/*                                              by Chris Seymour  2014/01/03 */
/* sl version 5.00 : add -c option                                           */
/*                                              by Toyoda Masashi 2013/05/05 */
/* sl version 4.00 : add C51, usleep(40000)                                  */
/*                                              by Toyoda Masashi 2002/12/31 */
/* sl version 3.03 : add usleep(20000)                                       */
/*                                              by Toyoda Masashi 1998/07/22 */
/* sl version 3.02 : D51 flies! Change options.                              */
/*                                              by Toyoda Masashi 1993/01/19 */
/* sl version 3.01 : Wheel turns smoother                                    */
/*                                              by Toyoda Masashi 1992/12/25 */
/* sl version 3.00 : Add d(D51) option                                       */
/*                                              by Toyoda Masashi 1992/12/24 */
/* sl version 2.02 : Bug fixed.(dust remains in screen)                      */
/*                                              by Toyoda Masashi 1992/12/17 */
/* sl version 2.01 : Smoke run and disappear.                                */
/*                   Change '-a' to accident option.                         */
/*                                              by Toyoda Masashi 1992/12/16 */
/* sl version 2.00 : Add a(all),l(long),F(Fly!) options.                     */
/*                                              by Toyoda Masashi 1992/12/15 */
/* sl version 1.02 : Add turning wheel.                                      */
/*                                              by Toyoda Masashi 1992/12/14 */
/* sl version 1.01 : Add more complex smoke.                                 */
/*                                              by Toyoda Masashi 1992/12/14 */
/* sl version 1.00 : SL runs vomiting out smoke.                             */
/*                                              by Toyoda Masashi 1992/12/11 */


#include <curses.h>
#include <signal.h>
#include <unistd.h>
#include "sl.h"
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>

using namespace std;

void add_smoke(int y, int x);
void add_man(int y, int x);
int add_C51(int x);
int add_D51(int x);
int add_sl(int x);
int add_ss(int x);
int add_rnd(int x, std::vector<std::string> rows);
void option(char *str, char *num);
int my_mvaddstr(int y, int x, char *str);
void print_usage();

int ACCIDENT  = 0;
int LOGO      = 0;
int FLY       = 0;
int C51       = 0; // show smaller train
int D51       = 0; // show original train
int SS        = 0; // show animated space ship
int NTH       = 0; // show nth ascii animation

int num_in = 0;


int my_mvaddstr(int y, int x, char *str)
{
    for ( ; x < 0; ++x, ++str)
        if (*str == '\0')  return ERR;
    for ( ; *str != '\0'; ++str, ++x)
        if (mvaddch(y, x, *str) == ERR)  return ERR;
    return OK;
}

void option(char *str, char *num)
{
    extern int ACCIDENT, LOGO, FLY, C51, D51, SS, NTH, num_in;
    while (*str != '\0') {
        switch (*str++) {
            case 'a': ACCIDENT = 1; break;
            case 'F': FLY      = 1; break;
            case 'l': LOGO     = 1; break;
            case 'c': C51      = 1; break;
			case 's': SS       = 1; break;
			case 't': D51	   = 1; break;
			case 'h':
				print_usage();
				exit(0);
				break;
			case 'n': 
				NTH = 1;
				if(isdigit(*num)) {
					num_in = atoi(num);
				} else {
					print_usage();
					exit(0);
				}
				break;
            default:
				print_usage();
				exit(0);
				break;
        }
    }
}

void print_usage()
{
	printf("usage:\n");
	printf("\tno params \t\t\tdisplays random vehicle\n");
	printf("\t-t\t\t\t\tdisplays D51 train\n");
	printf("\t-a\t\t\t\tdisplays D51 train with accident\n");
	printf("\t-c\t\t\t\tdisplays C51 train\n");
    printf("\t-l\t\t\t\tdisplays LOGO train\n");
	printf("\t-n <0-7>\t\t\tdisplays vehicle by index\n");
	printf("\t-s\t\t\t\tdisplays animated space ship\n");
	printf("\t-F\t\t\t\tvehicle will fly\n");
	printf("\t-h\t\t\t\tshow usage\n"); 
}

void signal_callback_handler(int signum) 
{
	mvcur(0, COLS - 1, LINES - 1, 0);
    endwin();
	exit(signum);
}

int main(int argc, char *argv[])
{
    int x, i;

    for (i = 1; i < argc; ++i) {
        if (*argv[i] == '-') {
		    option(argv[i] + 1, argv[argc-1]);
        }
    }

    initscr(); 
    signal(SIGINT, SIG_IGN);
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    leaveok(stdscr, TRUE);
    scrollok(stdscr, FALSE);

    srand (time(NULL));


	int select = 0;
	if(NTH == 1) {
		select = num_in;
	} 
	else if(D51 == 1) {
		select = 4;
	}
	else if(LOGO == 1) {
		select = 5;
	}
	else if(C51 == 1) {
		select = 6;
	}
	else if(SS == 1) {
		select = 7;
	} else {
		select = rand() % 7;
	}

    char* file_name = NULL;
    switch(select) {
        case 0:
            file_name = (char*)"ascii/bus1.txt";
            break;
        case 1:
            file_name = (char*)"ascii/plane1.txt";
            break;
		case 2:
            file_name = (char*)"ascii/sub1.txt";
            break;
		case 3:
			file_name = (char*)"ascii/ship1.txt";
			break;
        case 4:
            D51	= 1;
			LOGO = 0;
			C51 = 0;
			SS = 0;
            break;
        case 5:
            D51	= 0;
			LOGO = 1;
			C51 = 0;
			SS = 0;
            break;
        case 6:
            D51	= 0;
			LOGO = 0;
			C51 = 1;
			SS = 0;
            break;
        case 7:
            D51	= 0;
			LOGO = 0;
			C51 = 0;
			SS = 1;
            break;
		default:
            mvcur(0, COLS - 1, LINES - 1, 0);
            endwin();
            print_usage();
            exit(0);
			break;
    }

    std::vector<std::string> rows;
    if(file_name != NULL){
        std::ifstream in(file_name);
        std::string line;
        while(std::getline(in, line)) {
            rows.push_back(line);
        }
        in.close();
    }

    for (x = COLS - 1; ; --x) {
		signal(SIGINT, signal_callback_handler);
        if (LOGO == 1) {
            if (add_sl(x) == ERR) break;
        }
        else if (C51 == 1) {
            if (add_C51(x) == ERR) break;
        }
		else if(SS == 1) {
			if(add_ss(x) == ERR) break;
		}
		else if(D51 == 1) {
			if (add_D51(x) == ERR) break;
		} else {
            if(add_rnd(x, rows) == ERR) break;
        }
        getch();
        refresh();
        usleep(40000);
    }
    mvcur(0, COLS - 1, LINES - 1, 0);
    endwin();

    return 0;
}

int add_rnd(int x, std::vector<std::string> rows) 
{
    std::string str(rows[0]);
    int row_length = (int) str.size();

    if (x < -row_length)  return ERR;

    int yoffset = -8;
    int y = (LINES / 2) + yoffset;

    if (FLY) {
        y = (x / 6) + LINES - (COLS / 8) - rows.size();
    }

    for (unsigned int i = 0; i < rows.size(); i++) {
        my_mvaddstr(y + i, x, (char*)rows.at(i).c_str());
    }

  return OK;
}

int add_ss(int x)
{
    static std::string ss[SSPATTERNS][SSHEIGHT]
        = {{SS1, SS2, SS3, SS4, SS5, SS6, RKT1, RKT2, RKT3, RKT4, SS7, SS8, SS9, SS10, SS11},
           {SS1, SS2, SS3, SS4, SS5, SS6, RKT5, RKT6, RKT7, RKT8, SS7, SS8, SS9, SS10, SS11}};

    if (x < -SSLENGTH)  return ERR;

    int yoffset = -8;
    int y = (LINES / 2) + yoffset;

    if (FLY) {
        y = (x / 6) + LINES - (COLS / 8) - SSHEIGHT;
    }

    int frame_delay = 5;
    for (int i = 0; i <= SSHEIGHT-1; i++) {
        my_mvaddstr(y + i, x, (char*)ss[(SSLENGTH + x) / frame_delay % SSPATTERNS][i].c_str());
    }

    return OK;
}


int add_sl(int x)
{
    static std::string sl[LOGOPATTERNS][LOGOHEIGHT + 1]
        = {{LOGO1, LOGO2, LOGO3, LOGO4, LWHL11, LWHL12, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL21, LWHL22, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL31, LWHL32, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL41, LWHL42, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL51, LWHL52, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL61, LWHL62, DELLN}};

    static std::string coal[LOGOHEIGHT + 1]
        = {LCOAL1, LCOAL2, LCOAL3, LCOAL4, LCOAL5, LCOAL6, DELLN};

    static std::string car[LOGOHEIGHT + 1]
        = {LCAR1, LCAR2, LCAR3, LCAR4, LCAR5, LCAR6, DELLN};

    int i, y, py1 = 0, py2 = 0, py3 = 0;

    if (x < - LOGOLENGTH)  return ERR;
    y = LINES / 2 - 3;

    if (FLY == 1) {
        y = (x / 6) + LINES - (COLS / 6) - LOGOHEIGHT;
        py1 = 2;  py2 = 4;  py3 = 6;
    }
    for (i = 0; i <= LOGOHEIGHT; ++i) {
        my_mvaddstr(y + i, x, (char*)sl[(LOGOLENGTH + x) / 3 % LOGOPATTERNS][i].c_str());
        my_mvaddstr(y + i + py1, x + 21, (char*)coal[i].c_str());
        my_mvaddstr(y + i + py2, x + 42, (char*)car[i].c_str());
        my_mvaddstr(y + i + py3, x + 63, (char*)car[i].c_str());
    }
    if (ACCIDENT == 1) {
        add_man(y + 1, x + 14);
        add_man(y + 1 + py2, x + 45);  add_man(y + 1 + py2, x + 53);
        add_man(y + 1 + py3, x + 66);  add_man(y + 1 + py3, x + 74);
    }
    add_smoke(y - 1, x + LOGOFUNNEL);
    return OK;
}


int add_D51(int x)
{
    static std::string d51[D51PATTERNS][D51HEIGHT + 1]
        = {{D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL11, D51WHL12, D51WHL13, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL21, D51WHL22, D51WHL23, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL31, D51WHL32, D51WHL33, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL41, D51WHL42, D51WHL43, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL51, D51WHL52, D51WHL53, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL61, D51WHL62, D51WHL63, D51DEL}};
    static std::string coal[D51HEIGHT + 1]
        = {COAL01, COAL02, COAL03, COAL04, COAL05,
           COAL06, COAL07, COAL08, COAL09, COAL10, COALDEL};

    int y, i, dy = 0;

    if (x < - D51LENGTH)  return ERR;
    y = LINES / 2 - 5;

    if (FLY == 1) {
        y = (x / 7) + LINES - (COLS / 7) - D51HEIGHT;
        dy = 1;
    }
    for (i = 0; i <= D51HEIGHT; ++i) {
        my_mvaddstr(y + i, x, (char*)d51[(D51LENGTH + x) % D51PATTERNS][i].c_str());
        my_mvaddstr(y + i + dy, x + 53, (char*)coal[i].c_str());
    }
    if (ACCIDENT == 1) {
        add_man(y + 2, x + 43);
        add_man(y + 2, x + 47);
    }
    add_smoke(y - 1, x + D51FUNNEL);
    return OK;
}

int add_C51(int x)
{
    static std::string c51[C51PATTERNS][C51HEIGHT + 1]
        = {{C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH11, C51WH12, C51WH13, C51WH14, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH21, C51WH22, C51WH23, C51WH24, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH31, C51WH32, C51WH33, C51WH34, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH41, C51WH42, C51WH43, C51WH44, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH51, C51WH52, C51WH53, C51WH54, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH61, C51WH62, C51WH63, C51WH64, C51DEL}};
    static std::string coal[C51HEIGHT + 1]
        = {COALDEL, COAL01, COAL02, COAL03, COAL04, COAL05,
           COAL06, COAL07, COAL08, COAL09, COAL10, COALDEL};

    int y, i, dy = 0;

    if (x < - C51LENGTH)  return ERR;
    y = LINES / 2 - 5;

    if (FLY == 1) {
        y = (x / 7) + LINES - (COLS / 7) - C51HEIGHT;
        dy = 1;
    }
    for (i = 0; i <= C51HEIGHT; ++i) {
        my_mvaddstr(y + i, x, (char*)c51[(C51LENGTH + x) % C51PATTERNS][i].c_str());
        my_mvaddstr(y + i + dy, x + 55, (char*)coal[i].c_str());
    }
    if (ACCIDENT == 1) {
        add_man(y + 3, x + 45);
        add_man(y + 3, x + 49);
    }
    add_smoke(y - 1, x + C51FUNNEL);
    return OK;
}


void add_man(int y, int x)
{
    static std::string man[2][2] = {{"", "(O)"}, {"Help!", "\\O/"}};
    int i;

    for (i = 0; i < 2; ++i) {
        my_mvaddstr(y + i, x, (char*)man[(LOGOLENGTH + x) / 12 % 2][i].c_str());
    }
}


void add_smoke(int y, int x)
#define SMOKEPTNS        16
{
    static struct smokes {
        int y, x;
        int ptrn, kind;
    } S[1000];
    static int sum = 0;
    static std::string Smoke[2][SMOKEPTNS]
        = {{"(   )", "(    )", "(    )", "(   )", "(  )",
            "(  )" , "( )"   , "( )"   , "()"   , "()"  ,
            "O"    , "O"     , "O"     , "O"    , "O"   ,
            " "                                          },
           {"(@@@)", "(@@@@)", "(@@@@)", "(@@@)", "(@@)",
            "(@@)" , "(@)"   , "(@)"   , "@@"   , "@@"  ,
            "@"    , "@"     , "@"     , "@"    , "@"   ,
            " "                                          }};
    static std::string Eraser[SMOKEPTNS]
        =  {"     ", "      ", "      ", "     ", "    ",
            "    " , "   "   , "   "   , "  "   , "  "  ,
            " "    , " "     , " "     , " "    , " "   ,
            " "                                          };
    static int dy[SMOKEPTNS] = { 2,  1, 1, 1, 0, 0, 0, 0, 0, 0,
                                 0,  0, 0, 0, 0, 0             };
    static int dx[SMOKEPTNS] = {-2, -1, 0, 1, 1, 1, 1, 1, 2, 2,
                                 2,  2, 2, 3, 3, 3             };
    int i;

    if (x % 4 == 0) {
        for (i = 0; i < sum; ++i) {
            my_mvaddstr(S[i].y, S[i].x, (char*)Eraser[S[i].ptrn].c_str());
            S[i].y    -= dy[S[i].ptrn];
            S[i].x    += dx[S[i].ptrn];
            S[i].ptrn += (S[i].ptrn < SMOKEPTNS - 1) ? 1 : 0;
            my_mvaddstr(S[i].y, S[i].x, (char*)Smoke[S[i].kind][S[i].ptrn].c_str());
        }
        my_mvaddstr(y, x, (char*)Smoke[sum % 2][0].c_str());
        S[sum].y = y;    S[sum].x = x;
        S[sum].ptrn = 0; S[sum].kind = sum % 2;
        sum ++;
    }
}
