// Daniel Shiffman
// http://codingtra.in
// http://patreon.com/codingtrain
// Code for: https://youtu.be/QLHMtE5XsMs

import processing.video.*;

Capture video;
PImage prev;

float threshold = 25;

float motionX = 0;
float motionY = 0;

float lerpX = 0;
float lerpY = 0;

PVector position[];
PVector velocity[];
float speed[];
float ang[];
float turnSpeed[];
int numDots = 1024;


void setup() {
  size(1280, 960);
  String[] cameras = Capture.list();
  printArray(cameras);
  video = new Capture(this, cameras[1]);
  video.start();
  prev = createImage(640, 480, RGB);
  
  position = new PVector[numDots];
  velocity = new PVector[numDots];
  speed = new float[numDots];
  ang = new float[numDots];
  turnSpeed = new float[numDots];
  for(int i = 0; i < numDots; i++)
  {
    position[i] = new PVector(random(width),random(height));
    velocity[i] = new PVector(0,0);
    speed[i] = random(2);
    ang[i] = random(2*PI);
    turnSpeed[i] = random(0.1);
  }
}


void mousePressed() {
}

void captureEvent(Capture video) {
  prev.copy(video, 0, 0, video.width, video.height, 0, 0, prev.width, prev.height);
  prev.updatePixels();
  video.read();
}

void draw() {
  
  background(0);
  

  
  video.loadPixels();
  prev.loadPixels();
  //image(video, 0, 0);

  //threshold = map(mouseX, 0, width, 0, 100);
  threshold = 50;


  int count = 0;
  
  float avgX = 0;
  float avgY = 0;

  //loadPixels();
  // Begin loop to walk through every pixel
  for (int x = 0; x < video.width; x++ ) {
    for (int y = 0; y < video.height; y++ ) {
      
      //Get this pixel
      int loc = x + y * video.width;
      
      // What is current color
      color currentColor = video.pixels[loc];
      float r1 = red(currentColor);
      float g1 = green(currentColor);
      float b1 = blue(currentColor);
      
      //What was previous color
      color prevColor = prev.pixels[loc];
      float r2 = red(prevColor);
      float g2 = green(prevColor);
      float b2 = blue(prevColor);

      //What's the difference between the two?
      float d = distSq(r1, g1, b1, r2, g2, b2); 

      //If the difference is greater than our t
      if (d > threshold*threshold) {
        avgX += x;
        avgY += y;
        count++;
        //pixels[screenLoc] = color(255);
        
        fill(255);
        stroke(0);
        //ellipse(width - x*2, y*2, 10, 10);
        
        
      } else {
        //pixels[screenLoc] = color(0);
      }
    }
  }
  //updatePixels();

  // We only consider the color found if its color distance is less than 10. 
  // This threshold of 10 is arbitrary and you can adjust this number depending on how accurate you require the tracking to be.
  if (count > 50) { 
    motionX = width - avgX / count * 2;
    motionY = avgY / count * 2;
    // Draw a circle at the tracked pixel

  }
  
  lerpX = lerp(lerpX, motionX, 0.1); 
  lerpY = lerp(lerpY, motionY, 0.1); 
  
  
  fill(20, 20, 20);
  strokeWeight(2.0);
  stroke(0);
  ellipse(lerpX, lerpY, 36, 36);


    //Dot physics
  
  for(int i = 0; i < numDots; i++)
  {
    
    float distSq = abs(position[i].y - lerpY) + abs(position[i].x - lerpX);
    if(distSq > 100)
    {
      ang[i] = lerp(ang[i], (atan2((position[i].y - lerpY),(position[i].x - lerpX)) + PI), turnSpeed[i]);
    velocity[i].x = cos(ang[i])*speed[i];
    velocity[i].y = sin(ang[i])*speed[i];
    
    
    }
    position[i] = position[i].add(velocity[i]);
    
    float c = map(distSq,0,1000,255,40);
    
    fill(c,c,c);
    ellipse(position[i].x, position[i].y, 10, 10);
    
  }

  //image(video, 0, 0, 100, 100);
  //image(prev, 100, 0, 100, 100);

  //println(mouseX, threshold);
}

float distSq(float x1, float y1, float z1, float x2, float y2, float z2) {
  float d = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) +(z2-z1)*(z2-z1);
  return d;
}
