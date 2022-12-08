#define SDL_MAIN_HANDLED

#include <math.h>
#include <vector>
#include <iostream>
#include <thread>
#include <Windows.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

using namespace std;

int x,y;

double roundTo(double t,int p) {
	return round(t*p)/p;
}

struct returnTypeBounces {
	vector<double> heights;
	vector<double> times;
	void set(vector<double> h,vector<double> t) {
		heights = h; times = t;
	}
};

returnTypeBounces calculateBounces(double startingPoint, double gravity, double timePrecision, double coefficientOfRestitution, double contactTimeForBounce, double minimumHeight, bool freeFalling) {
	double h0 = startingPoint;
	double dt = timePrecision;
	double rho = coefficientOfRestitution;
	double tau = contactTimeForBounce;
	double g = gravity;
	double hstop = minimumHeight;
	bool freefall = freeFalling;
	double v = 0.0;
	double t = 0.0;
	double hmax = h0;
	double h = h0;
	double t_last = -sqrt(2.0 * h0 / g);
	double vmax = sqrt(2.0 * hmax * g);
	vector<double> H;
	vector<double> T;
	while(hmax > hstop) {
		if(freefall) {
			double hnew = h + v * dt - 0.5 * g * dt * dt;
			if(hnew < 0.0) {
				t = t_last + 2.0 * sqrt(2.0 * hmax / g);
				freefall = false;
				t_last = t + tau;
				h = 0.0;
			}
			else
			{
				t = t + dt;
				v = v - g * dt;
				h = hnew;
			}
		}
		else {
			t = t + tau;
			vmax = vmax * rho;
			v = vmax;
			freefall = true;
			h = 0.0;
		}
		hmax = 0.5 * vmax * vmax / g;
		H.push_back(h);
		T.push_back(t);
	}
	returnTypeBounces ret;
	ret.set(H,T);
	return ret;
}

void drawQuadrilateralShape(SDL_Renderer* renderer, int x,int y,int base,int height) {
	for(int i = 0; i < base; i++) {
		SDL_RenderDrawLine(renderer,x + i,y,x + i,y + height);
	}
}

int main() {
	x = 1920; y = 1080;
	int newX = 1920/2;
	bool useTestValues = true;
	double sp = 9.0; double g = 10; double tp = .001; double cor = .75; double ctfb = .10; double mh = .001; bool ff = true;
	if(!useTestValues) {
		cout << "Starting point (meters): "; cin >> sp;
		cout << "Gravity (m/s2): "; cin >> g;
		cout << "Time precision (seconds): "; cin >> tp;
		cout << "Coefficient of restitution (ratio of velocity differences, double): "; cin >> cor;
		cout << "contant time for bounce (seconds): "; cin >> ctfb;
		cout << "Minimum height (meters): "; cin >> mh;
		cout << "Is the object free falling (boolean): "; cin >> ff;
	}
	
	//ShowWindow(GetConsoleWindow(),SW_HIDE);
	if(!SDL_Init(SDL_INIT_EVERYTHING)) {
		SDL_Window* win = nullptr;
		SDL_Renderer* rend = nullptr;
		if(!SDL_CreateWindowAndRenderer(x,y,0,&win, &rend)) {
			bool done = false;
			double time_passed=0.0;
			start:
			returnTypeBounces calc = calculateBounces(sp,g,tp,cor,ctfb,mh,true);
			for(int i = 0; i < calc.times.size(); i++) {
				SDL_SetRenderDrawColor(rend,255,255,255,1);
				SDL_RenderClear(rend);
				cout << "drawing object at height: " << roundTo(calc.heights[i],100) * 100 << endl;
				SDL_SetRenderDrawColor(rend,255,0,0,1);
				drawQuadrilateralShape(rend,newX,y-100.0-(roundTo(calc.heights[i],100) * 100), 100, 100);
				SDL_RenderPresent(rend);
				this_thread::sleep_for(chrono::milliseconds(int((roundTo(calc.times[i], 1000))-time_passed)));
				time_passed += calc.times[i];
			}
			Uint32 buttons; int mouseX,mouseY;
			int st_Y = 0;
			while(1) {
				SDL_PumpEvents();
				buttons = SDL_GetMouseState(&mouseX,&mouseY);
				//cout << buttons << endl;
				if(buttons == SDL_BUTTON_LEFT) {
					SDL_SetRenderDrawColor(rend,255,255,255,1);
					SDL_RenderClear(rend);
					SDL_SetRenderDrawColor(rend,0,0,0,1);
					drawQuadrilateralShape(rend,mouseX-100/2, mouseY-100/2,100,100);
					newX = mouseX-100/2;
					st_Y = mouseY-100/2;
					SDL_RenderPresent(rend);
				}
				else if(buttons == 4) {
					sp = (9-st_Y / 100);
					goto start;
				}
			};
		}
	}
	return 0;
}