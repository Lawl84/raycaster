#include "window.h"

Window::Window()
	:m_window(nullptr), m_rend(nullptr)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	
	m_window = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 800, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
	if (!m_window)
	{
		std::cout << SDL_GetError() << "\n";
	}
	m_rend = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	SDL_RenderClear(m_rend);
	SDL_RenderPresent(m_rend);
}