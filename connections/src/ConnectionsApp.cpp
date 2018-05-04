#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ConnectionsApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
private:
	vec2 position[30];
	vec2 velocity[30];
	vec2 lineA[10000];
	vec2 lineB[10000];
	int numLines;
	float radius;
};

void ConnectionsApp::setup()
{

	setWindowSize(1024, 768);

	for (int i = 0; i < 30; i++)
	{
		position[i] = getWindowCenter();
		velocity[i] = vec2(Rand::randFloat(-3.0f, 3.0f), Rand::randFloat(-3.0f, 3.0f));
	}

	numLines = 0;

	radius = 5.0f;
}

void ConnectionsApp::mouseDown( MouseEvent event )
{
}

void ConnectionsApp::update()
{
	numLines = 0;
	for (int i = 0; i < 30; i++)
	{
		//move the dots
		position[i].x += velocity[i].x;
		position[i].y += velocity[i].y;

		//Distance from middle
		float dSqrd = abs(position[i].x - getWindowCenter().x) + abs(position[i].y - getWindowCenter().y);
		
		//Loop round
		if (position[i].x > getWindowWidth() || position[i].x < 0)
		{
			velocity[i].x *= -1;
		}

		if (position[i].y > getWindowHeight() || position[i].y < 0)
		{
			velocity[i].y *= -1;
		}

		
		for (int j = 0; j < 30; j++)
		{
			if (i == j)
			{
				continue;
			}
			else if( abs( position[i].x - position[j].x)+ abs(position[i].y - position[j].y) < 10)
			{

				lineA[numLines] = position[i];
				lineB[numLines] = position[j];
				numLines++;
			}
			
		}


	}

	
}

void ConnectionsApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	for (int i = 0; i < 30; i++)
	{
		gl::drawSolidCircle(position[i], radius);
	}

	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 30; j++)
		{
			if (i != j)
			{
				if (abs(position[i].x - position[j].x) + abs(position[i].y - position[j].y) < 150)
				{
					gl::drawLine(position[i], position[j]);
				}
				
			}
			
		}
		
	}




}

CINDER_APP(ConnectionsApp, RendererGl )
