/*
Input file is black and white sketch through command line
output is instructions for servos
Uncomment lines 246 and 251 to see that pen up and pen down instructions are properly executed
Uncomment line 267 to see the visited array being updated
*/

#include <iostream>
#include <cstdlib>
#include "EasyBMP.h"
#include <math.h>
//#include<vector>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
using namespace boost::numeric::ublas;
using namespace std;
const double pi=3.14159265358979323846 ;

class Point {
 
  public:
  int x,y;
  Point();
  Point(int,int);
};
Point::Point(){
  x=0;
  y=0;
}
Point::Point(int a,int b){
  x=a;
  y=b;
}
double degtorad(double degrees)
{
    return (pi/180)*degrees;
}
 
double radtodeg(double radians)
{
    return (180/pi)*radians;
} 
template <class T>
T sqr(T a){
  return a*a;
}
template <class T>
void print(matrix <T> a){
  for(int i=0;i<a.size1();i++){
    for(int j=0;j<a.size2();j++){
      cout<<a(i,j)<<" ";
    }
    cout<<endl;
  }
}
Point firstCord(matrix<double> a){
  for(int i=0;i<a.size1();i++){
    for(int j=0;j<a.size2();j++){
      if(a(i,j)!=-1){
	Point first (i,j);
	return first;
      }
    }
  }
}
Point nextCord(Point a,matrix<double>b){
  for(int j=a.y;j<b.size2();j++){
    if(b(a.x,j)!=-1&&j!=a.y){	
	Point next (a.x,j);
	return next;
      }
    else {continue;}
  }
  if(a.x!=b.size1()-1){
    for(int i=(a.x)+1;i<b.size1();i++){
      for(int j=0;j<b.size2();j++){
	if(b(i,j)!=-1){
	  Point next (i,j);
	  return next;
	}
      }
    }
  }
  else{ 
    for(int j=0;j<b.size2();j++){
      if(b(a.x,j)!=-1&&j!=a.y){	
	Point next (a.x,j);
	return next;
      }
      else {continue;}
    }
  }
 
}
// Point closestCord(Point a,matrix<double>b){
//   int width=b.size1();
//   int height=b.size2();
//   if(a.x!=width-1 && a.y !=height-1 && a.y!=0){
//   if(b((a.x)+1,a.y)!=-1){
//     Point closest (a.x+1,a.y);
//     return closest;}
//   else if (b(a.x,(a.y)+1)!=-1){
//     Point closest (a.x,(a.y)+1);
//     return closest;}
//   else if (b((a.x)+1,(a.y)+1)!=-1){
//     Point closest (a.x+1,(a.y)+1);
//     return closest;}
//   else if (b((a.x)+1,(a.y)-1)!=-1){
//     Point closest (a.x+1,(a.y)-1);
//     return closest;}
//  else return nextCord(a,b);
//   }
//   else if (a.x==width-1 && a.y!=height-1){
//    if (b(a.x,(a.y)+1)!=-1){
//     Point closest (a.x,(a.y)+1);
//     return closest;}
//  else return nextCord(a,b);
//   }
//   else if (a.x!=width-1 && a.y==height-1){
//     if(b((a.x)+1,a.y)!=-1){
//       Point closest (a.x+1,a.y);
//       return closest;}
//     else if (b((a.x)+1,(a.y)-1)!=-1){
//       Point closest (a.x+1,(a.y)-1);
//       return closest;}
//  else return nextCord(a,b);
//   }
//     else if (a.x!=width-1 && a.y==0){
//     if(b((a.x)+1,a.y)!=-1){
//       Point closest (a.x+1,a.y);
//       return closest;}
//     else if (b((a.x)+1,(a.y)+1)!=-1){
//       Point closest (a.x+1,(a.y)+1);
//       return closest;}
//     else if (b(a.x,(a.y)+1)!=-1){
//       Point closest (a.x,(a.y)+1);
//       return closest;}
//      else return nextCord(a,b);
//     }
  
//     else return nextCord(a,b);
// }
double sqrDistance (Point a,Point b){
  double dist= sqr(a.x-b.x)+sqr(a.y-b.y);
  return dist;
}
bool isClose(Point a,Point b){ //returns true if Points in neighborhood of one pixel;
  if(sqrDistance(a,b)>2){return false;}
  else {return true;}
}

 main(int argc,char* argv[]){
	 BMFH bmfh = GetBMFH(argv[1]);
  if (bmfh.bfType != BMP::BMP_FILE_TYPE) { return -1; }  //Check if input is valid bmp file
  BMP inImage;
  inImage.ReadFromFile(argv[1]);       //Input Image (After Edge Detection)
  const int width=inImage.TellWidth();
  const int height=inImage.TellHeight();
  BMP outImage; // Image corresponding to paper
  outImage.SetSize(width,height);
  const double paperSize=30;  //Size in cm
  const double  k= paperSize/double(width);   // size of 1 pixel on paper
  matrix <int> visited(width,height); //Array storing locations of visited pixels
  const double dist= 17.5/sqrt(double(2));
  const double armLength1=35;
  const double armLength2=30;
  const double sqrArmLength1=sqr(armLength1);
  const double sqrArmLength2=sqr(armLength2);
  matrix <double> alpha(width,height);   // Storing  angles of first arm
  matrix <double> theta(width,height);   // Storing  angles of second arm
  for(int i=0;i<width;i++){
    for(int j=0;j<height;j++){
      alpha(i,j)=-1;
      theta(i,j)=-1;
      visited(i,j)=0;
    }
  }// Initialize to illegal value

  for(int i=0; i<width; i++){
	for (int j=0; j<height; j++){
		RGBApixel temp=inImage.GetPixel(i,j);
		if(temp.Red==0 && temp.Green==0 && temp.Blue==0) {     
		  double x= (i==width-1)?(k*(i+1))-30:(k*i)-30;          //x cord on paper in cm
		  double y= (j==height-1)?30-(k*(j+1)):30-(k*j);          //y cord on paper in cm
		  double slope=double(y+dist)/double(-x+dist);
		  double phi=atan(slope);
		  double radius=sqrt(double(sqr(x-dist)+sqr(y+dist)));
		  alpha(i,j)=radtodeg(acos((sqrArmLength1+sqrArmLength2-sqr(radius))/(2*armLength1*armLength2)));  //cos rule to find angle alpha; 
		   double thetaphi=acos(((sqrArmLength1+sqr(radius)-sqrArmLength2)/(2*armLength1*radius)));//cos rule to find theta-phi
		   theta(i,j)=90-radtodeg(thetaphi- phi);
		  // cout<<"phi is "<< phi<<endl;
		   // cout<<"radius for "<<"("<<i<<","<<j<<") is "<<radius<<endl;
		    //cout<<"x is "<<x<<"y is"<<y<<endl;
		  }//end if
		
	}
  }
  Point first = firstCord(theta);
 
  // Point third(599,599);
  int count=0;
  std::vector<int>motor1;   //lower motor 
  std::vector<int>motor2;   // upper motor
  cout <<"first is "<< first.x<<"  "<<first.y<<endl;
  cout <<"next is " << nextCord(first,theta).x<<" "<< nextCord(first,theta).y<<endl;
  // cout << "closest is "<<closestCord(next,theta).x<<" "<<closestCord(next,theta).y<<endl;
  for(int i=0;i<width;i++){
    for(int j=0;j<height;j++){
       if(theta(i,j)!=-1){
	 count++;
	 int intgr1=floor(alpha(i,j));
	 int intgr2= floor(theta(i,j));
	 double frctn1=alpha(i,j)-intgr1;
	 double frctn2=theta(i,j)-intgr2;
	 if(frctn1>=0.5){
	   alpha(i,j)=intgr1+1;
	   	     motor1.push_back(alpha(i,j));
	 }
	 else{
	   alpha(i,j)=intgr1;
	   motor1.push_back(alpha(i,j));}
	 //cout<<"alpha ("<<i<<","<<j<<")="<<alpha(i,j)<<endl;
       
	 if(frctn2>=0.5){
	   theta(i,j)=intgr2+1;
	   motor2.push_back(theta(i,j));
	 }
	 else{
	   theta(i,j)=intgr2;
	 motor2.push_back(theta(i,j)); }
	//  cout<<"theta ("<<i<<","<<j<<")="<<theta(i,j)<<endl;
       }
    }  
  }
  cout<<"Number of black pixels in the image is "<<count<<endl;
  //Move pen to first Point 
  //Pen Down
   Point next=first;
   Point next2=nextCord(next,theta);
  
  for(int i=0;i<count;i++){

    if(isClose(next,next2)){//pen down
  	  //move to next position
       visited(next.x,next.y)=1;
      // cout<<"pen down "<<endl;
    }
    else {//pen up 
      //move to next position
       visited(next.x,next.y)=1;
     //  cout<<"pen up "<<endl;
    }
  cout<< "next is ("<< next.x<<", "<<next.y<<")"<<endl;
    next=nextCord(next,theta);
    next2=nextCord(next2,theta);
  }
  int count2=0;
  RGBApixel black;
  black.Red=0;
  black.Blue=0;
  black.Green=0;
  for(int i=0;i<width;i++){
    for(int j=0;j<height;j++){
       if(visited(i,j)==1){
  	 count2++;
  	// cout<<"visited ("<<i<<","<<j<<")="<<visited(i,j)<<endl;
  	 outImage.SetPixel(i,j,black);
       }
    }
  }	 
  cout<< count2<<endl;
//  outImage.WriteToFile(argv[2]);
  cout<<"motor 1 "<<motor1.size()<<endl;
 }
