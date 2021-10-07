#include <SDL.h>
#include <iostream>

class Window
{
public:
	SDL_Window* m_window;
	SDL_Renderer* m_rend;

	void clear() { SDL_RenderClear(m_rend); }
	void present() { SDL_RenderPresent(m_rend); }
	Window();
};