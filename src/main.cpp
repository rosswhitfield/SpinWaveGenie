//
//  main.cpp
//  Spin Wave Fit
//
//  Created by Hahn, Steven E. on 1/7/13.
//  Copyright (c) 2013 Oak Ridge National Laboratory. All rights reserved.
//
#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "SpinWave.h"
#include "Initializer.h"
#include "Cell/Neighbors.h"
#include "Positions.h"
#include "PointsAlongLine.h"

using namespace boost;
using namespace std;

int main(int argc, char * argv[])
{
    
    //Init four_sl;
    Init four_sl("/Users/svh/Documents/spin_wave_genie/examples/MnV2O4_cubic.xml");

    //cout << check << endl;
    
    /*Cell cell = four_sl.get_cell();
    
    string sl_r = "Mn0";
    string sl_s = "Mn0";
    double min = 8.4;
    double max = 8.6;
    
    Neighbors neighborList;
    neighborList.findNeighbors(cell,sl_r,sl_s,min,max);
    size_t z_rs = neighborList.getNumberNeighbors();
    
    cout << "z_rs= " << z_rs << endl;
    for(Neighbors::Iterator nbr=neighborList.begin();nbr!=neighborList.end();++nbr)
    {
        cout << nbr->get<0>() << " " << nbr->get<1>() << " " << nbr->get<2>() << endl;
    }*/
    
    
    SW_Builder builder = four_sl.get_builder();
    SpinWave test = builder.Create_Element();
    
    PointsAlongLine Line;
    Line.setFirstPoint(1.0,1.0,0.0);
    Line.setFinalPoint(3.0,3.0,0.0);
    Line.setNumberPoints(1000001);
    Positions KPoints = Line.getPoints();
        
    for(Positions::Iterator it = KPoints.begin(); it != KPoints.end(); it++)
    {
        double x = it->get<0>();
        double y = it->get<1>();
        double z = it->get<2>();

        //cout << "Pos." << endl;
        //cout << x << " " << y << " " << z << " ";// << endl;
        test.createMatrix(x,y,z);
        test.Calc();
        vector<point> pts = test.getPoints();
        //cout << "Freq.  Int." << endl;
        /*for(vector<point>::iterator it2 = pts.begin();it2!=pts.end();it2++)
        {
            cout << (*it2).frequency << "  " << (*it2).intensity*10.0 << " " ;//<< endl;
        }*/
        cout << endl;
    }
    return 0;
}
