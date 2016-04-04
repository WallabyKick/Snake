#include "SDL.h"
#include "SDL_TTF.h"
#include <time.h>
#include <stdlib.h>

struct Field{
	SDL_Rect box;
	int type,																		// 0 - Пустота,  1 - Стены, 2 - Голова змеи, 3 - тело змеи, 4 - фрукт.
		dir;																		// 4 - вправо, 6 - влево, 8 - вверх, 2 - вниз
}Element[32][24];

SDL_Rect MenuRect[5];
SDL_Point Point;
SDL_Window *window;
SDL_Renderer *render;

int width=1024,
	height=800,
	Restart = 0,
	GamePause=0,
	SelectMenu=1,
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
	Element[x][y].box.x=x*32;
	Element[x][y].box.y=y*32;
	Element[x][y].box.h=32;
	Element[x][y].box.w=32;
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
	char string[100], num[5];														// Вывод текста
	int x,y;
	SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
	SDL_RenderClear(render);
	for (x=0; x<32; x++)
		for (y=0; y<24; y++){
			switch(Element[x][y].type){
			case 0:																// Пустота
				SDL_SetRenderDrawColor(render, 20, 20, 20, 255);
				SDL_RenderFillRect(render,&Element[x][y].box);
				break;
			case 1:																// Стены
				SDL_SetRenderDrawColor(render, 175, 175, 175, 255);
				SDL_RenderFillRect(render,&Element[x][y].box);
				break;
			case 2:																// Голова
				SDL_SetRenderDrawColor(render, 150, 220, 0, 255);
				SDL_RenderFillRect(render,&Element[x][y].box);
				break;
			case 3:																// Тело
				SDL_SetRenderDrawColor(render, 25, 150, 0, 255);
				SDL_RenderFillRect(render,&Element[x][y].box);
				SDL_SetRenderDrawColor(render, 150, 220, 0, 255);
				SDL_RenderDrawRect(render,&Element[x][y].box);
				break;
			case 4:																// Фрукт
				SDL_SetRenderDrawColor(render, 255, 0, 0, 255);
				SDL_RenderFillRect(render,&Element[x][y].box);
				SDL_SetRenderDrawColor(render, 20, 20, 20, 255);
				SDL_RenderDrawRect(render,&Element[x][y].box);
				break;
			};
		};
	strcpy(string,"SCORE: ");
	_itoa_s(body,num,5,10);
	strcat(string,num);
	WriteText(0, string, 32, 255, 0, 0, TTF_STYLE_NORMAL, 128, 768);
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
	int i;
	for (i=0; i<5;i++){
		MenuRect[i].x=0;
		MenuRect[i].y=0;
		MenuRect[i].w=0;
		MenuRect[i].h=0;
	}
	SDL_SetRenderDrawColor(render, 10, 10, 10, 255);
	SDL_RenderClear(render);
	switch (menu){
	case 0:
		if (GamePause == 1)
			WriteText(1,"Продолжить",  70, 255, 0, 0, TTF_STYLE_UNDERLINE, 270, 240);
		else
			WriteText(1,"Начать игру", 70, 255, 0, 0, TTF_STYLE_UNDERLINE, 260, 240);
		WriteText(0,"Змейка",         150, 0, 190, 0, TTF_STYLE_UNDERLINE, 200, 10);
		WriteText(2,"Сложность",       70, 255, 0, 0, TTF_STYLE_UNDERLINE, 300, 360);
		WriteText(3,"Об игре",         70, 255, 0, 0, TTF_STYLE_UNDERLINE, 350, 480);
		WriteText(4,"Выход",           70, 255, 0, 0, TTF_STYLE_UNDERLINE, 370, 600);
		break;
	case 1:
		WriteText(0,"Сложность",      115, 0, 190, 0, TTF_STYLE_UNDERLINE, 160, 35);
		WriteText(1,"Легкая",          70, 255, 0, 0, TTF_STYLE_UNDERLINE, 370, 240);
		WriteText(2,"Средняя",         70, 255, 0, 0, TTF_STYLE_UNDERLINE, 350, 390);
		WriteText(3,"Сложная",         70, 255, 0, 0, TTF_STYLE_UNDERLINE, 340, 540);
		break;
	case 2:
		WriteText(0,"Об игре",        125, 0, 190, 0, TTF_STYLE_UNDERLINE, 250, 25);
		WriteText(1,"В главное меню",  50, 160, 0, 160, TTF_STYLE_UNDERLINE, 520, 700);
		break;
	case 3:
		WriteText(0,"Внимание!",      110, 0, 190, 0, TTF_STYLE_UNDERLINE, 200, 30);
		WriteText(0,"Изменив сложность во",  60, 255, 0, 0, TTF_STYLE_NORMAL, 100, 210);
		WriteText(0,"время игрового",        60, 255, 0, 0, TTF_STYLE_NORMAL, 100, 280);
		WriteText(0,"процесса вам придется", 60, 255, 0, 0, TTF_STYLE_NORMAL, 100, 350);
		WriteText(0,"начать игру с начала.", 60, 255, 0, 0, TTF_STYLE_NORMAL, 100, 420);
		WriteText(1,"Продолжить",            50, 160, 0, 160, TTF_STYLE_UNDERLINE, 350, 550);
		WriteText(2,"Вернуться в меню",      50, 160, 0, 160, TTF_STYLE_UNDERLINE, 250, 670);
		break;
	case 4:
		WriteText(0, "Вы",             150, 255, 0, 0, TTF_STYLE_UNDERLINE, 380, 140);
		WriteText(0, "проиграли!",     150, 255, 0, 0, TTF_STYLE_UNDERLINE, 40, 350);
		break;
	}
	SDL_SetRenderDrawColor(render, 255, 0, 0, 255);
	for (i=1; i<5; i++)
 	if (SDL_PointInRect(&Point,&MenuRect[i])==SDL_TRUE)
		SelectMenu = i;
 	SDL_RenderDrawRect(render,&MenuRect[SelectMenu]);
	SDL_RenderPresent(render);
	return interval;
}

int WriteText(int rect, char text[100], int size, int fontR, int fontG, int fontB, Uint32 style,  int x, int y){
	SDL_Color clr={fontR, fontG, fontB}; 
	SDL_Texture *Texture;
	TTF_Font * fnt = TTF_OpenFont("menu.ttf", size);
	SDL_Surface * TextSurface, *Surface=SDL_GetWindowSurface(window);
	MenuRect[rect].x=x;
	MenuRect[rect].y=y;
	MenuRect[rect].w=0;
	MenuRect[rect].h=0;
	TTF_SetFontStyle(fnt, style);
	TextSurface = TTF_RenderUTF8_Solid(fnt, text, clr);
	SDL_BlitSurface(TextSurface, NULL, Surface, &MenuRect[rect]);
	Texture=SDL_CreateTextureFromSurface(render, TextSurface);
	SDL_RenderCopy(render, Texture, NULL, &MenuRect[rect]);
	SDL_DestroyTexture(Texture);
	SDL_FreeSurface(TextSurface);
	TTF_CloseFont(fnt);
	return 1;
}

Uint32 __stdcall WinMain(int argc, char* argv[]) {
	SDL_TimerID RenderTiks, GameTiks, RenderTiksMenu;
	int buff, GameSpeed = 250;
	Uint32 timeout=0;
	SDL_Event Event;
	TTF_Init();
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("Змейка. Курсовая работа студента группы 15ВВ2 Кичук В.И.", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
	render = SDL_CreateRenderer(window, -1, 0);
MenuStart:
	RenderTiksMenu = SDL_AddTimer(100, MenuTexture, NULL);
	while(1){
		SDL_WaitEvent(&Event);
		SDL_GetMouseState(&Point.x, &Point.y);
		if (((Event.type==SDL_MOUSEBUTTONUP)&&(Event.button.button==SDL_BUTTON_LEFT)&&(SDL_PointInRect(&Point,&MenuRect[SelectMenu])==SDL_TRUE))||(Event.key.keysym.scancode==SDL_SCANCODE_RETURN)&&(SDL_TICKS_PASSED(SDL_GetTicks(), timeout))){
				switch (menu){
				case 0:
					if (SelectMenu==1)
						goto StartGame;
					if (SelectMenu==2)
						menu = 1;
					if (SelectMenu==3)
						menu = 2;
					if (SelectMenu==4)
						exit(0);
					break;
				case 1:
					if (SelectMenu==1)
						buff = 250;
					if (SelectMenu==2)
						buff = 125;
					if (SelectMenu==3)
						buff = 60;
					if (GamePause == 0){
						GameSpeed = buff;
						menu = 0;
					}else
						menu = 3;
					break;
				case 2:
					if (SelectMenu==1)
						menu=0;
					break;
				case 3:
					if (SelectMenu==1){
						GameSpeed = buff;
						menu = 0;
						GamePause = 0;
					}
					if (SelectMenu==2)
						menu = 0;
					break;
				case 4:
					menu = 0;
				default:
						break;
			}	
				SelectMenu = 1;
				timeout = SDL_GetTicks() + 1000;
		}
			if (Event.type==SDL_KEYUP){
			if ((Event.key.keysym.scancode==SDL_SCANCODE_RETURN)&&(menu == 4)){
				menu = 0;
				timeout = SDL_GetTicks() + 500;
			}
			if ((Event.key.keysym.scancode==SDL_SCANCODE_ESCAPE)&&(menu == 0)&&(GamePause==1)&&(SDL_TICKS_PASSED(SDL_GetTicks(), timeout)))
				break;
			if ((Event.key.keysym.scancode==SDL_SCANCODE_UP)&&(SelectMenu>1))
				SelectMenu--;
			else
				if ((Event.key.keysym.scancode==SDL_SCANCODE_DOWN)&&(SelectMenu<4))
					SelectMenu++;
			}
			if (Event.type==SDL_QUIT)
				exit(0);
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
					SDL_RemoveTimer(GameTiks);
					SDL_RemoveTimer(RenderTiks);
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
			SDL_RemoveTimer(GameTiks);
			SDL_RemoveTimer(RenderTiks);
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
