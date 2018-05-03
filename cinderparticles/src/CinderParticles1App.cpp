#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct Dot
{
	vec3 pos;
	ColorA color;
	float speed;
	float direction;
	
};

struct Boundary
{
	vec3 pos;
	float radius;
	float minRadius;
	float maxRadius;
};

const int NUM_DOTS = 100000;

class CinderParticles1App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

private:

	//Shaders
	gl::GlslProgRef mUpdateProg;
	gl::GlslProgRef mRenderProg;

	//Vertex attribute object
	gl::VaoRef mAttributes[2];
	//Dot vertex buffer
	gl::VboRef mDotBuffer[2]; //2 so that we can do frame swap

	//Buffer ID's for source and destination buffers
	//These are swapped after each update
	uint32_t mSourceIndex = 0;
	uint32_t mDestinationIndex = 1;

	Boundary boundary;

	float elapsed;
};

void CinderParticles1App::setup()
{
	boundary = Boundary();
	boundary.pos = vec3(getWindowCenter(), 0.0f);
	boundary.minRadius = boundary.radius = 50.0f;
	boundary.maxRadius = 400.0f;

	vector<Dot> dots;
	dots.assign(NUM_DOTS, Dot());

	vec3 center = vec3(getWindowCenter(), 0.0f);
	srand(time(NULL)); //seed our random

	//Initialise our dots
	for (int i = 0; i < dots.size(); ++i)
	{
		//Random angle in radians
		float angle = (float)rand() / (float)RAND_MAX * (float)M_PI * 2.0f;
		float speed = 2.0f* (float)rand() / (float)RAND_MAX;

		//Set this dots values
		auto &d = dots.at(i);
		d.pos = center;
		d.color = ColorA(1, 1, 1, 1.0f);
		d.direction = angle;
		d.speed = speed;
		
	}

	//Create buffers to hold dots on GPU and copy data into the first buffer
	mDotBuffer[mSourceIndex] = gl::Vbo::create(
		GL_ARRAY_BUFFER,
		dots.size() * sizeof(Dot),
		dots.data(),
		GL_STATIC_DRAW // we use static because cpu doesn't need to modify this after this point
	);

	//Same again for the other buffer, but dont need to copy data as will flipflop
	mDotBuffer[mDestinationIndex] = gl::Vbo::create(GL_ARRAY_BUFFER, dots.size() * sizeof(Dot), nullptr, GL_STATIC_DRAW);

	
	//Now setup attribute arrays and pointers because OpenGL is procedural
	for (int i = 0; i < 2; ++i)
	{
		//Create attributes
		mAttributes[i] = gl::Vao::create(); //Create an attribute buffer object
		gl::ScopedVao vao(mAttributes[i]);

		//Define attributes using offsets into the bound dot buffer
		gl::ScopedBuffer buffer(mDotBuffer[i]);//bind the dot buffer
	 		
		//Create attribute array - one for each property of Dot
		gl::enableVertexAttribArray(0);
		gl::enableVertexAttribArray(1);
		gl::enableVertexAttribArray(2);
		gl::enableVertexAttribArray(3);

		
		//Copy across pointers to attributes of dots
		gl::vertexAttribPointer(
			0, //start index
			3, //size of data - 3 because this is a vec3
			GL_FLOAT, //type of data
			GL_FALSE, //whether it's normalised
			sizeof(Dot), //stride (how much space to leave for the next datas
			(const GLvoid *)offsetof(Dot, pos) //pointer to the data
		);
		gl::vertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Dot), (const GLvoid *)offsetof(Dot, color));
		gl::vertexAttribPointer(2,1,GL_FLOAT,GL_FALSE,sizeof(Dot),(const GLvoid *)offsetof(Dot, speed));
		gl::vertexAttribPointer(3,1,GL_FLOAT,GL_FALSE,sizeof(Dot),(const GLvoid *)offsetof(Dot, direction));
		

		//Create stock shader and update program (which is written in GLSL)
		mRenderProg = gl::getStockShader(gl::ShaderDef().color());
		mUpdateProg = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("dotUpdate.vs"))
			.feedbackFormat(GL_INTERLEAVED_ATTRIBS)
			.feedbackVaryings({ "position", "color", "speed", "direction"})
			.attribLocation("iPosition", 0)
			.attribLocation("iColor", 1)
			.attribLocation("iSpeed",2)
			.attribLocation("iDirection", 3)
			
		);
		

	}
	

}

void CinderParticles1App::mouseDown( MouseEvent event )
{
}

void CinderParticles1App::update()
{
	
	gl::ScopedGlslProg prog(mUpdateProg);
	gl::ScopedState rasteriser(GL_RASTERIZER_DISCARD, true); //turn off fragment stage


	mUpdateProg->uniform("uBoundaryPos", boundary.pos);
	float boundaryRadiusSquared = boundary.radius*boundary.radius;
	mUpdateProg->uniform("uBoundaryRadiusSquared", boundaryRadiusSquared);
	boundary.radius = boundary.minRadius + ((1.0f+sinf((float)getElapsedSeconds()*0.3))*(boundary.maxRadius-boundary.minRadius))/2.0f;
	//Bind the source data (Attributes refer to specific buffers)
	gl::ScopedVao source(mAttributes[mSourceIndex]);
	//Bind destination as buffer base
	gl::bindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mDotBuffer[mDestinationIndex]);
	gl::beginTransformFeedback(GL_POINTS);

	//Draw source into destination
	gl::drawArrays(GL_POINTS, 0, NUM_DOTS);

	gl::endTransformFeedback();

	//Swap buffers
	swap(mSourceIndex, mDestinationIndex);
	

}

void CinderParticles1App::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 

	
	gl::ScopedGlslProg render(mRenderProg);
	gl::ScopedVao vao(mAttributes[mSourceIndex]);
	gl::context()->setDefaultShaderVars();
	gl::drawArrays(GL_POINTS, 0, NUM_DOTS);
	
}

CINDER_APP( CinderParticles1App, RendererGl, [](App::Settings *settings) {
	settings->setMultiTouchEnabled(false);
	settings->setFullScreen(true);
})

