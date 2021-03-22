#pragma once

#include <math.h>

#define DTYPE double

#define PI 3.14159265359

DTYPE radToDeg(DTYPE rad){
    return rad*(180.0/PI);
}

DTYPE degToRad(DTYPE deg){
    return deg*(PI/180.0);
}

class Vec{

    public:
        DTYPE x,y,z;

        Vec(){}
        Vec(DTYPE a, DTYPE b, DTYPE c){
            x = a;
            y = b;
            z = c;
        }

        //cross this vec by another and return
        Vec cross(Vec otherVec){
            Vec outVec;
            outVec.x = y*otherVec.z - otherVec.y*z;
            outVec.y = otherVec.x*z - x*otherVec.z;
            outVec.z = x*otherVec.y - otherVec.x*y;

            return outVec;
        }

        DTYPE dot(Vec otherVec){
            DTYPE a = x*otherVec.x;
            DTYPE b = y*otherVec.y;
            DTYPE c = z*otherVec.z;

            return (a+b+c);

        }

        DTYPE dist(Vec otherVec){
            DTYPE a = (x - otherVec.x)*(x - otherVec.x);
            DTYPE b = (y - otherVec.y)*(y - otherVec.y);
            DTYPE c = (z - otherVec.z)*(z - otherVec.z);

            return sqrt(a+b+c);

        }

        DTYPE mag(){
            DTYPE a = x*x;
            DTYPE b = y*y;
            DTYPE c = z*z;

            return sqrt(a+b+c); 
        }

        // normalize the current vector to length 1
        void normalize(){
            DTYPE mag = this->mag();
            x = x/mag;
            y = y/mag;
            z = z/mag;
        }

        // returns the noraml of a vector of length 1
        Vec norm(){
            DTYPE mag = this->mag();
            DTYPE a = x/mag;
            DTYPE b = y/mag;
            DTYPE c = z/mag;

            return Vec(a,b,c);
        }

        // returns a rotated vector around an axis with inputin degree
        Vec rot(Vec axis, DTYPE theta){
            Vec outVec;
            DTYPE thetaRad = degToRad(theta);
            Vec crossProd = axis.cross(*this);
            DTYPE dotProd = axis.dot(*this);
            DTYPE sinTheta = sin(thetaRad);
            DTYPE cosTheta = cos(thetaRad);
            outVec.x = x*cosTheta+crossProd.x*sinTheta + axis.x*dotProd*(1- cosTheta);
            outVec.y = y*cosTheta+crossProd.y*sinTheta + axis.y*dotProd*(1- cosTheta);
            outVec.z = z*cosTheta+crossProd.z*sinTheta + axis.z*dotProd*(1- cosTheta);
            
            // DTYPE a =cos(thetaRad/2);
            // DTYPE b = axis.x*sin(thetaRad/2);
            // DTYPE c = axis.y*sin(thetaRad/2);
            // DTYPE d = axis.z*sin(thetaRad/2);

            // outVec.x = (a*a+b*b-c*c-d*d)*x + 2*(b*c-a*d)*y + 2*(b*d+a*c)*z;
            // outVec.y = 2*(b*c+a*d)*x + (a*a+c*c-b*b-d*d)*y + 2*(c*d-a*b)*z;
            // outVec.z = 2*(b*d-a*c)*x + 2*(c*d+a*b)*y + (a*a+d*d-b*b-c*c)*z; 

            return outVec;
        }

};