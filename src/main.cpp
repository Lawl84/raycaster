#include <SDL.h>
#include <math.h>
#include <vector>
#include "window.h"
#define PI 3.14159265359
#define ONEDEGREE 0.017453

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

bool collides(const SDL_Rect player_rect, const SDL_Rect& rect)
{
	bool a, b, c, d;
	int x1 = player_rect.x, y1 = player_rect.y;
	int x2 = player_rect.x + player_rect.w, y2 = y1;
	int x3 = x1, y3 = player_rect.y + player_rect.h;
	int x4 = x2, y4 = y1;

	a = (x1 >= rect.x && x1 <= rect.x + rect.w) && (y1 >= rect.y && y1 <= rect.y + rect.h);
	b = (x2 >= rect.x && x2 <= rect.x + rect.w) && (y2 >= rect.y && y2 <= rect.y + rect.h);
	c = (x3 >= rect.x && x3 <= rect.x + rect.w) && (y3 >= rect.y && y3 <= rect.y + rect.h);
	d = (x4 >= rect.x && x4 <= rect.x + rect.w) && (y4 >= rect.y && y4 <= rect.y + rect.h);

	return a || b || c || d;
	
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

std::vector<SDL_Rect> generate_collision_points(int map[])
{
	std::vector<SDL_Rect> areas;
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			if (map[y * 8 + x] == 1)
			{
				areas.emplace_back(SDL_Rect{ x * 64, y * 64, 64, 64 });
			}
		}
	}
	return areas;
}

void draw_rays(float pa, float px, float py, const Window& w, int map[])
{
	//horizontal line check
	float ra = pa;
	float rx, ry;
	float xo, yo;
	int mx, my, mp, mo;
	std::pair<float, std::pair<int, int>> dh, dv;
	float dist;
	int dof;
	ra -= PI / 6;
	for (int r = 0; r < 60; r++)
	{
		std::cout << ra << std::endl;
		dof = 0;
		float cotan = 1.0 / tan(ra);
		if (ra > PI)
		{
			mo = 0;
			ry = round_up(py, 64);
			rx = (py - ry) * cotan + (float)px;
			yo = 64;
			xo = -yo * cotan;
		}
		else if (ra < PI)
		{
			mo = 1;
			ry = round_up(py, 64) - 64;
			rx = (py - ry) * cotan + (float)px;
			yo = -64;
			xo = -yo * cotan;
		}

		else
		{
			mo = 0;
			rx = px; ry = py; dof = 8;
		}

		while (dof < 8)
		{
			mx = (int)(rx) >> 6;
			my = ((int)(ry) >> 6) - mo;
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

			if (dof >= 9)
			{
				std::cout << ra << std::endl;
			}
		}
		
		dh.first = distance_between_points(px, py, rx, ry);
		dh.second = { rx, ry };

		//vertical line check

		dof = 0;
		float ta = tan(ra);
		if (ra > PI / (float)2 && ra < 3 * PI / (float)2)
		{
			rx = round_up(px, 64) - 64;
			ry = (px - rx) * ta + (float)py;
			xo = -64;
			yo = -xo * ta;
			mo = 1;
		}
		else if (ra < PI / (float)2 || ra > 3 * PI / (float)2)
		{
			rx = round_up(px, 64);
			ry = (px - rx) * ta + (float)py;
			xo = 64;
			yo = -xo * ta;
			mo = 0;
		}

		else
		{
			ry = py; rx = px; dof = 8; mo = 1;
		}

		while (dof < 8)
		{
			mx = ((int)(rx) >> 6) - mo;
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

		SDL_SetRenderDrawColor(w.m_rend, 255, 153, 153, 255);

		if (dv.first > dh.first)
		{
			SDL_RenderDrawLine(w.m_rend, px, py, dh.second.first, dh.second.second);
			dist = dh.first;
		}
		else
		{
			SDL_RenderDrawLine(w.m_rend, px, py, dv.second.first, dv.second.second);
			dist = dv.first;
		}

		////walls
		float ca = pa - ra;
		dist *= cos(ca);

		float line_height = 64 * 1000 / dist;
		float line_offset = 300 - line_height / 2;

		if (ca < 0) ca += 2 * PI;
		else if (ca > 2 * PI) ca -= 2 * PI;

		if (line_height > 600) line_height = 600;
		SDL_Rect wall = { r * 16 , line_offset + 4, 16, line_height };

		SDL_RenderFillRect(w.m_rend, &wall);
		SDL_RenderDrawLine(w.m_rend, r * 8, line_offset + 4, r * 8, line_offset + 4 + line_height);


		ra += PI / 180;
		if (ra < 0) ra += 2 * PI;
		else if (ra > 2 * PI) ra -= 2 * PI;
	}
}
int main(int argv, char** argc)
{
	Window w;
	SDL_Event evt;
	bool running = true;
	bool valid_point = true;

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

	std::vector<SDL_Rect> areas = generate_collision_points(map);

	int px = 245;
	int py = 395;

	SDL_Rect player_rect = { px, py, 10, 10 };
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
				case SDLK_d:
					dpx = -5 * sin(pa); dpy = -cos(pa) * 5; break;
				case SDLK_s:
					dpy = 5 * sin(pa); dpx = -cos(pa) * 5;  break;
				case SDLK_a:
					dpx = 5 * sin(pa); dpy = 5 * cos(pa); break;
				case SDLK_RIGHT:
					dpa = 0.1; break;
				case SDLK_LEFT:
					dpa = -0.1; break;
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

		player_rect = { px + dpx, py + dpy, 6, 6 };

		pa += dpa;

		for (auto& r : areas)
		{
			if (collides(player_rect, r)) 
			{
				valid_point = false;
				break;
			}				
		}

		if (valid_point)
		{
			px += dpx;
			py += dpy;
		}

		valid_point = true;

		if (pa < 0)
			pa += 2 * PI;
		else if (pa > 2 * PI)
			pa -= 2 * PI;

		w.clear();

		draw_map(w);
		draw_rays(pa, px + 5, py + 5, w, map);
		draw_player(px, py, pa, w);
		SDL_SetRenderDrawColor(w.m_rend, 50, 50, 50, 255);
		w.present();
	}
	return 0;
}

