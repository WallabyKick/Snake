#include "SDL.h"
#include "SDL_TTF.h"
#include <time.h>
#include <stdlib.h>

struct Field{
	SDL_Rect box;
	int type,																		// 0 - Пустота,  1 - Стены, 2 - Голова змеи, 3 - тело змеи, 4 - фрукт.
		dir;																		// 4 - вправо, 6 - влево, 8 - вверх, 2 - вниз
}Element[32][24];

char string[100], stringNEW[100]="             SCORE: \0", num[5];								// Вывод текста
SDL_Renderer *render;
int Restart = 0,
	GamePause=0,
	direct = 6,																		// Изначально направление головы змеи
	antidirect = 4,																	// АНТИСАМОПОЕДАНИЕ
	body,																			// Длинна тела змеи
	menu=0;																			// Индекс меню


void fruit(){																		// Функция генерации фрукта
	int x,y;
	x=rand()%30+1;
	y=rand()%22+1;
	while(Element[x][y].type==2||Element[x][y].type==3){
	x=rand()%30+1;
	y=rand()%22+1;
	}
	SDL_SetRenderDrawColor(render, 20, 20, 20, 255);
	SDL_RenderFillRect(render,&Element[x][y].box);
	Element[x][y].box.x=x*32+1;
	Element[x][y].box.y=y*32+1;
	Element[x][y].box.h=30;
	Element[x][y].box.w=30;
	Element[x][y].type=4;
}

void assignment(){																	// Функция старта/рестарта игры
	int x,y;
	direct = 6;
	for (x=0; x<32; x++)
		for (y=0; y<24; y++){
			Element[x][y].box.x=x*32;
			Element[x][y].box.y=y*32;
			Element[x][y].box.w=32;
			Element[x][y].box.h=32;
			Element[x][y].type=0;
			Element[x][y].dir=0;
		}
		for (x=0; x<32; x++){
			y=0;
			Element[x][y].type = 1;
			y=23;
			Element[x][y].type = 1;
		}
		for (y=0; y<24; y++){
			x=0;
			Element[x][y].type = 1;
			x=31;
			Element[x][y].type = 1;
		}
		Element[8][3].box.x=8*32;
		Element[8][3].box.y=3*32;
		Element[8][3].type=2;
		body=0;
		fruit();
};

Uint32 callbackrender(Uint32 interval, void *param){								// Функция отрисовки элементов
	int x,y;
	for (x=0; x<32; x++)
		for (y=0; y<24; y++){
			switch(Element[x][y].type){
				case 0:																// Пустота
					Element[x][y].box.x=x*32;
					Element[x][y].box.y=y*32;
					Element[x][y].box.h=32;
					Element[x][y].box.w=32;
					SDL_SetRenderDrawColor(render, 20, 20, 20, 255);
					break;
				case 1:																// Стены
					SDL_SetRenderDrawColor(render, 175, 175, 175, 255);
					break;
				case 2:																// Голова
					Element[x][y].box.x=x*32;
					Element[x][y].box.y=y*32;
					Element[x][y].box.h=32;
					Element[x][y].box.w=32;
					SDL_SetRenderDrawColor(render, 150, 220, 0, 255);
					break;
				case 3:																// Тело
					Element[x][y].box.x=x*32;
					Element[x][y].box.y=y*32;
					Element[x][y].box.h=32;
					Element[x][y].box.w=32;
					SDL_SetRenderDrawColor(render, 150, 220, 0, 255);
					SDL_RenderFillRect(render,&Element[x][y].box);
					Element[x][y].box.x=32*x+2;
					Element[x][y].box.y=32*y+2;
					Element[x][y].box.h=28;
					Element[x][y].box.w=28;
					SDL_SetRenderDrawColor(render, 25, 150, 0, 255);
					break;
				case 4:																// Фрукт
					Element[x][y].box.x=x*32;
					Element[x][y].box.y=y*32;
					Element[x][y].box.h=32;
					Element[x][y].box.w=32;
					SDL_SetRenderDrawColor(render, 20, 20, 20, 255);
					SDL_RenderFillRect(render,&Element[x][y].box);
					Element[x][y].box.x=32*x+2;
					Element[x][y].box.y=32*y+2;
					Element[x][y].box.h=28;
					Element[x][y].box.w=28;
					SDL_SetRenderDrawColor(render, 255, 0, 0, 255);
					break;
			};
			SDL_RenderFillRect(render,&Element[x][y].box);
		};
	WriteText(string);
	SDL_RenderPresent(render);
	return interval;
}

Uint32 callbackgame(Uint32 interval, void *param){									// Логика
	int x,y,i,j,count=0;
	for (x=0; x<32; x++)															// Поиск текущего местоположения головы
		for (y=0; y<24; y++)
			if (Element[x][y].type==2)
				goto NexTurn;
	NexTurn:
	switch (direct){																// Перемещение головы
		case 4:
			if (Element[x-1][y].type==3){
				Restart = 1;
				return interval;
			}
			Element[x-1][y].type=2;
			Element[x-1][y].dir=6;
			break;
		case 6:
			if (Element[x+1][y].type==3){
				Restart = 1;
				return interval;
			}
			Element[x+1][y].type=2;
			Element[x+1][y].dir=4;
			break;
		case 8:
			if (Element[x][y-1].type==3){
				Restart = 1;
				return interval;
			}
			Element[x][y-1].type=2;
			Element[x][y-1].dir=2;
			break;
		case 2:
			if (Element[x][y+1].type==3){
				Restart = 1;
				return interval;
			}
			Element[x][y+1].type=2;
			Element[x][y+1].dir=8;
			break;
	};
			Element[x][y].type=0;
	for (i=0; i<32; i++)															// Поиск текущего местоположения фрукта
		for (j=0; j<24; j++)
			if (Element[i][j].type==4)
				goto FindCollisionWithFruit;
	FindCollisionWithFruit:
	if((i==32)&&(j==24)){															// Поедание фрукта
		switch (Element[x][y].dir){
			case 4:
				Element[x][y].type=3;
				break;
			case 6:
				Element[x][y].type=3;
				break;
			case 8:
				Element[x][y].type=3;
				break;
			case 2:
				Element[x][y].type=3;
				break;
		}
	body++;
	fruit();
	}
	else{
		i=x;
		j=y;
		while(count!=body){															// Перемещение тела за змеи за головой
			switch (Element[i][j].dir){
				case 4:
					Element[i][j].type=3;
					i=i-1;
					Element[i][j].type=0;
					break;
				case 6:
					Element[i][j].type=3;
					i=i+1;
					Element[i][j].type=0;
					break;
				case 8:
					Element[i][j].type=3;
					j=j-1;
					Element[i][j].type=0;
					break;
				case 2:
					Element[i][j].type=3;
					j=j+1;
					Element[i][j].type=0;
					break;
			}
		count++;
		}
	};
	for (x=0; x<32; x++)															// Поиск текущего местоположения головы
		for (y=0; y<24; y++)
			if (Element[x][y].type==2)
				goto FindCollisionWithWall;
FindCollisionWithWall:
	antidirect=Element[x][y].dir;
	if (x==0||x==31||y==0||y==23){													// Опредиление столкновения головы с стеной
		Restart = 1;
	}
	return interval;
};	

Uint32 MenuTexture(Uint32 interval, void *param){
	SDL_Surface *mainMenu;
	SDL_Texture *MainTexture;
	switch (menu){
	case 0:
			if (GamePause == 1)
				mainMenu=SDL_LoadBMP("img/count.bmp");
			else
			mainMenu=SDL_LoadBMP("img/main.bmp");
		break;
	case 1:
		mainMenu=SDL_LoadBMP("img/level.bmp");
		break;
	case 2:
		mainMenu=SDL_LoadBMP("img/info.bmp");
		break;
	case 3:
		mainMenu=SDL_LoadBMP("img/switchlevel.bmp");
		break;
	case 4:
		mainMenu=SDL_LoadBMP("img/restart.bmp");
		break;
	}
	MainTexture = SDL_CreateTextureFromSurface(render,mainMenu);
	SDL_RenderCopy(render, MainTexture, NULL, NULL);
	SDL_FreeSurface(mainMenu);
	SDL_DestroyTexture(MainTexture);
	SDL_RenderPresent(render);
	return interval;
}

Uint32 WriteText()																			//Отрисовка текста
{
	SDL_Color clr={255, 0, 0}; 
	SDL_Rect dest={0, 768, 1024, 30};
	SDL_Texture *Texture;
	TTF_Font * fnt = TTF_OpenFont("font.ttf", 30);
	SDL_Surface * TextSurface, *text=SDL_CreateRGBSurface(0, 1024, 30, 32, 0, 0, 0, 255);
	strcpy(string,stringNEW);
	itoa(body,num,10);
	strcat(string,num);
	TextSurface = TTF_RenderText_Blended(fnt, string, clr);
	SDL_BlitSurface(TextSurface, NULL, text, NULL);
	Texture=SDL_CreateTextureFromSurface(render, text);
	SDL_RenderCopy(render, Texture, NULL, &dest);
	SDL_RenderPresent(render);
	SDL_DestroyTexture(Texture);
	SDL_FreeSurface(text);
	SDL_FreeSurface(TextSurface);
	TTF_CloseFont(fnt);
}


int __stdcall WinMain(int argc, char* argv[]) {
	SDL_Window *window;
	SDL_TimerID RenderTiks, GameTiks, RenderTiksMenu;
	int x,y, buff, GameSpeed = 250;
	Uint32 timeout=0;
	SDL_Event Event;
	TTF_Init();
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("The game \"Snake\". Developed by Kichuk Vasily", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 800, NULL);
	render = SDL_CreateRenderer(window, -1, NULL);
	MenuStart:
	SDL_RemoveTimer(GameTiks);
	SDL_RemoveTimer(RenderTiks);
	RenderTiksMenu = SDL_AddTimer(100, MenuTexture, NULL);
	while(1){
		SDL_WaitEvent(&Event);
		if (Event.type==SDL_MOUSEBUTTONUP)
			if(Event.button.button==SDL_BUTTON_LEFT)
				switch (menu){
				case 0:
					for (x=0; x<1024; x++)
						for (y=0; y<800; y++){
							if ((Event.button.x>250)&&(Event.button.y>240)&&(Event.button.x<770)&&(Event.button.y<305))
								goto StartGame;
							if ((Event.button.x>270)&&(Event.button.y>350)&&(Event.button.x<750)&&(Event.button.y<430)){
								menu = 1;
								goto nextTurn;
							}
							if ((Event.button.x>340)&&(Event.button.y>490)&&(Event.button.x<675)&&(Event.button.y<560)){
								menu = 2;
								goto nextTurn;
							}
							if ((Event.button.x>360)&&(Event.button.y>620)&&(Event.button.x<660)&&(Event.button.y<700))
								exit(0);
						}
				case 1:
					for (x=0; x<1024; x++)
						for (y=0; y<800; y++){
							if ((Event.button.x>345)&&(Event.button.y>225)&&(Event.button.x<700)&&(Event.button.y<300)){
								if (GamePause == 0){
								GameSpeed = 250;
								menu = 0;
								goto nextTurn;
								}
								buff = 250;
								menu = 3;
							}
							if ((Event.button.x>330)&&(Event.button.y>385)&&(Event.button.x<690)&&(Event.button.y<450)){
								if (GamePause == 0){
								GameSpeed = 125;
								menu = 0;
								goto nextTurn;
								}
								buff = 125;
								menu = 3;
							}
							if ((Event.button.x>330)&&(Event.button.y>540)&&(Event.button.x<695)&&(Event.button.y<605)){
								if (GamePause == 0){
								GameSpeed = 60;
								menu = 0;
								goto nextTurn;
								}
								buff = 60;
								menu = 3;
							}
						}
				case 2:
					for (x=0; x<1024; x++)
						for (y=0; y<800; y++)
							if ((Event.button.x>525)&&(Event.button.y>735)&&(Event.button.x<1010)&&(Event.button.y<780)){
								menu = 0;
								goto nextTurn;
							}
							break;
				case 3:
					for (x=0; x<1024; x++)
						for (y=0; y<800; y++)
							if ((Event.button.x>30)&&(Event.button.y>610)&&(Event.button.x<1010)&&(Event.button.y<725))
								menu = 0;
							if ((Event.button.x>560)&&(Event.button.y>610)&&(Event.button.x<975)&&(Event.button.y<660)){
								GameSpeed = buff;
								menu = 0;
								GamePause=0;
							}
							break;
				case 4:
					menu = 0;
					break;
				default:
					break;
			}
			if ((Event.key.keysym.scancode==SDL_SCANCODE_RETURN)&&(menu == 4))
				menu = 0;
			else
				if (menu == 0)
					goto StartGame;
	nextTurn:
			if (Event.type==SDL_QUIT)
				exit(0);
			if ((Event.key.keysym.scancode==SDL_SCANCODE_ESCAPE)&&(GamePause==1)&&(SDL_TICKS_PASSED(SDL_GetTicks(), timeout)))
				break;
	}
	StartGame:
	Restart = 0;
	SDL_RemoveTimer(RenderTiksMenu);
	if (GamePause==0){
	srand(time(NULL));
	assignment();
	}
	else
		GamePause = 0;
	GameunPause:
	RenderTiks = SDL_AddTimer(15, callbackrender, NULL);
	GameTiks = SDL_AddTimer(GameSpeed, callbackgame, NULL);
	while(1){
		SDL_WaitEventTimeout(&Event,1);
		if (Event.type==SDL_KEYDOWN){
			switch( Event.key.keysym.scancode){
			case SDL_SCANCODE_LEFT:
				if (antidirect!=4)
					direct=4;
				break;
			case SDL_SCANCODE_A:
				if (antidirect!=4)
					direct=4;
				break;
			case SDL_SCANCODE_RIGHT:
				if (antidirect!=6)
					direct=6;
				break;
			case SDL_SCANCODE_D:
				if (antidirect!=6)
					direct=6;
				break;
			case SDL_SCANCODE_UP:
				if (antidirect!=8)
					direct=8;
				break;
			case SDL_SCANCODE_W:
				if (antidirect!=8)
					direct=8;
				break;
			case SDL_SCANCODE_DOWN:
				if (antidirect!=2)
					direct=2;
				break;
			case SDL_SCANCODE_S:
				if (antidirect!=2)
					direct=2;
				break;
			case SDL_SCANCODE_ESCAPE:
				if (!GamePause){
					GamePause = 1;
					timeout = SDL_GetTicks() + 1000;
					goto MenuStart;
				}
				break;
			default:
				break;
			}
		}
		if(Event.type == SDL_QUIT)
			exit(0);
		if (Restart==1){
			menu = 4;
			goto MenuStart;
		}
	}
	TTF_Quit();
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();
	exit(0);
	return 0;
}