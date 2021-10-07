#include <SDL.h>
#include <math.h>
#include "window.h"
#define PI 3.14159265359

void draw_player(int px, int py, float pa, const Window& w)
{
	SDL_SetRenderDrawColor(w.m_rend, 255, 0, 0, 255);
	SDL_Rect rect = { px, py, 10, 10 };
	SDL_RenderFillRect(w.m_rend, &rect);
	int ax = 20 * cos(pa);
	int ay = -20 * sin(pa);

	SDL_RenderDrawLine(w.m_rend, px + 5, py + 5, px + 5 + ax, py + 5 + ay);
}

float distance_between_points(int x1, int y1, int x2, int y2)
{
	return ((float)sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));
}

int round_up(float numToRound, int multiple)
{
	if (multiple == 0)
		return numToRound;

	int remainder = (int)numToRound % multiple;
	if (remainder == 0)
		return numToRound;

	return numToRound + multiple - remainder;
}

void draw_map(const Window& w)
{
	SDL_Color current_color;
	int mapx = 8, mapy = 8, mapsize = 64;
	int map[] =
	{
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 1, 0, 0, 1,
		1, 0, 1, 0, 1, 0, 0, 1,
		1, 0, 1, 0, 1, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 1, 0, 0, 1,
		1, 1, 1, 1, 1, 1, 1, 1
	};

	int xf, yf;

	for (int y = 0; y < mapy; y++)
	{
		for (int x = 0; x < mapx; x++)
		{
			if (map[y * mapx + x] == 1) current_color = { 100, 100, 100 };
			else current_color = { 0, 0, 0 };
			xf = x * mapsize;
			yf = y * mapsize;
			SDL_Rect rect = { xf + 1, yf + 1, mapsize - 1, mapsize - 1 };
			SDL_SetRenderDrawColor(w.m_rend, current_color.r, current_color.g, current_color.b, current_color.a);
			SDL_RenderFillRect(w.m_rend, &rect);
		}
	}
}

void draw_rays(float pa, int px, int py, const Window& w, int map[])
{
	//horizontal line check
	float ra = pa;
	float rx, ry;
	int xo, yo;
	int mx, my, mp;
	std::pair<float, std::pair<int, int>> dh, dv;
	int dof;
	for (int r = 0; r < 1; r++)
	{
		dof = 0;
		float cotan = 1.0 / tan(ra);
		if (ra > PI)
		{
			ry = round_up(py, 64);
			rx = (py - ry) * cotan + px;
			yo = 64;
			xo = -yo * cotan;
		}
		else if (ra < PI)
		{
			ry = round_up(py, 64) - 64;
			rx = (py - ry) * cotan + px;
			yo = -64;
			xo = -yo * cotan;
		}

		else
		{
			rx = px; ry = py; dof = 8;
		}

		while (dof < 8)
		{
			mx = (int)(rx) >> 6;
			my = (int)(ry) >> 6;
			mp = my * 8 + mx;
			if (mp < 64 && mp >= 0 && map[mp] == 1) // wall has been hit
			{
				dof = 8;
			}
			else 
			{
				rx += xo;
				ry += yo;
				dof++;
			}
		}
		
		dh.first = distance_between_points(px, py, rx, ry);
		dh.second = { rx, ry };

		//vertical line check

		dof = 0;
		float ta = tan(ra);
		if (ra > PI / 2 && ra < 3 * PI / 2)
		{
			rx = round_up(px, 64) - 64;
			ry = (px - rx) * ta + py;
			xo = -64;
			yo = -xo * ta;
		}
		else if (ra < PI / 2 || ra > 3 * PI / 2)
		{
			rx = round_up(px, 64);
			ry = (px - rx) * ta + py;
			xo = 64;
			yo = -xo * ta;
		}

		else
		{
			ry = py; rx = px; dof = 8;
		}

		while (dof < 8)
		{
			mx = (int)(rx) >> 6;
			my = (int)(ry) >> 6;
			mp = my * 8 + mx;
			if (mp < 64 && mp >= 0 && map[mp] == 1) // wall has been hit
			{
				dof = 8;
			}
			else
			{
				rx += xo;
				ry += yo;
				dof++;
			}
		}
		 
		dv.first = distance_between_points(px, py, rx, ry);
		dv.second = { rx, ry };

		SDL_SetRenderDrawColor(w.m_rend, 0, 255, 255, 255);

		if (dv.first > dh.first)
		{
			SDL_RenderDrawLine(w.m_rend, px, py, dh.second.first, dh.second.second);
		}
		else
		{
			SDL_RenderDrawLine(w.m_rend, px, py, dv.second.first, dv.second.second);
		}

		std::cout << dv.first << " " <<  dh.first << std::endl;
	}
}
int main(int argv, char** argc)
{
	Window w;
	SDL_Event evt;
	bool running = true;

	int map[] =
	{
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 1, 0, 0, 1,
		1, 0, 1, 0, 1, 0, 0, 1,
		1, 0, 1, 0, 1, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 1, 0, 0, 1,
		1, 1, 1, 1, 1, 1, 1, 1
	};

	int px = 245;
	int py = 395;

	float pa = PI / 2, dpa = 0;

	int dpx = 0, dpy = 0;

	while (running)
	{
		int mxi, myi;
		SDL_GetMouseState(&mxi, &myi);

		while (SDL_PollEvent(&evt))
		{
			switch (evt.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			
			case SDL_KEYDOWN:
				switch (evt.key.keysym.sym)
				{
				case SDLK_w:
					dpy = -5 * sin(pa); dpx = cos(pa) * 5; break;
				case SDLK_a:
					dpx = -5 * sin(pa); dpy = -cos(pa) * 5; break;
				case SDLK_s:
					dpy = 5 * sin(pa); dpx = -cos(pa) * 5;  break;
				case SDLK_d:
					dpx = 5 * sin(pa); dpy = 5 * cos(pa); break;
				case SDLK_RIGHT:
					dpa = -0.1; break;
				case SDLK_LEFT:
					dpa = 0.1; break;
				}
				break;

			case SDL_KEYUP:
				switch (evt.key.keysym.sym)
				{
				case SDLK_w:
					dpy = 0; dpx = 0; break;
				case SDLK_a:
					dpy = 0; dpx = 0; break;
				case SDLK_s:
					dpy = 0; dpx = 0; break;
				case SDLK_d:
					dpy = 0; dpx = 0; break;
				case SDLK_RIGHT:
					dpa = 0; break;
				case SDLK_LEFT:
					dpa = 0; break;
				}

			}
		}
		pa += dpa;

		px += dpx;
		py += dpy;

		if (pa < 0)
			pa += 2 * PI;
		else if (pa > 2 * PI)
			pa -= 2 * PI;

		w.clear();

		draw_map(w);
		draw_player(px, py, pa, w);
		draw_rays(pa, px + 5, py + 5, w, map);
		SDL_SetRenderDrawColor(w.m_rend, 50, 50, 50, 255);
		w.present();
	}
	return 0;
}

