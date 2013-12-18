//
//  SpinWavePlot.cpp
//  spin_wave_genie
//
//  Created by Hahn, Steven E. on 10/30/13.
//
//
#include <cmath>
#include <functional>
#include "SpinWavePlot.h"
#include "SpinWave.h"
#include "extern/cubature.h"

using namespace std;

TwoDimensionResolutionFunction::TwoDimensionResolutionFunction(TwoDimGaussian& info, double min, double max, double points)
{
    MinimumEnergy = min;
    MaximumEnergy = max;
    EnergyPoints = points;
    
    a = info.a;
    b = info.b;
    c = info.c;
    tol = info.tol;
    direction = info.direction;
    SW = info.SW;
}

int TwoDimensionResolutionFunction::calculateIntegrand(unsigned dim, const double *x, unsigned fdim, double *fval)
{
    assert(fdim == EnergyPoints);
    assert(dim == 1);
        
    for(int i=0;i!=EnergyPoints;i++)
    {
        fval[i] = 0.0;
    }

    double u;
    switch (direction)
    {
        case 0:
            SW.createMatrix(x[0],ky,kz);
            u = x[0] - kx;
            break;
        case 1:
            SW.createMatrix(kx,x[0],kz);
            u = x[0] - ky;
            break;
        case 2:
            SW.createMatrix(kx,ky,x[0]);
            u = x[0] - kz;
            break;
    }
    
    //cout << x[0] << " " << ky << " " << kz << " " << u << endl;

    double maximum = 1.0;
    double fraction = 1.0e-5;
    double minEnergy,maxEnergy;
    
    double d = log(maximum/fraction);
    
    if ((b*b - a*c)*u*u +c*d > 0.0)
    {
    
      SW.Calc();
      vector<point> points = SW.getPoints();
    
      double firstSolution = (-b*u + sqrt((b*b - a*c)*u*u +c*d))/c;
      double secondSolution = (a*u*u - d)/(c*firstSolution);
    
      //double checkSecond = (-b*u - sqrt((b*b - a*c)*u*u +c*d))/c;
      //cout << secondSolution << " " << checkSecond << endl;
    
      minEnergy = min(firstSolution,secondSolution);
      maxEnergy = max(firstSolution,secondSolution);
    
      //cout << minEnergy << " " << maxEnergy << endl;
    
      for(size_t k=0;k!=points.size();k++)
      {
        //cout << "calculated frequency & intensity: " << frequencies[k] << " " << intensities[k] << "  " << endl;
        long min_bin = (points[k].frequency+minEnergy - MinimumEnergy)*(EnergyPoints-1)/(MaximumEnergy-MinimumEnergy);
        long max_bin = (points[k].frequency+maxEnergy - MinimumEnergy)*(EnergyPoints-1)/(MaximumEnergy-MinimumEnergy);
        
        if (min_bin < 0)
            min_bin = 0;
        else if (min_bin > EnergyPoints)
            min_bin = EnergyPoints;
        if (max_bin < 0)
            max_bin = 0;
        else if (max_bin > EnergyPoints)
            max_bin = EnergyPoints;
        
        //cout << min_bin << " " << max_bin << endl;
        
        for(int i=min_bin;i!=max_bin;i++)
        {
          double energy = MinimumEnergy + (MaximumEnergy-MinimumEnergy)*(double)i/(double)(EnergyPoints-1);
          fval[i] += points[k].intensity*exp(-1.0*(c*pow(points[k].frequency-energy,2)+2.0*b*(points[k].frequency-energy)*u+a*pow(u,2)));
        }
      }
    
      double tmp = (a*c-b*b)/(M_PI*M_PI);
      for(int i=0;i!=EnergyPoints;i++)
      {
        fval[i] *= tmp;
      }
    }
    
    //cout << endl;
    /*for(int i=0;i!=EnergyPoints;i++)
    {
        double energy = MinimumEnergy + (MaximumEnergy-MinimumEnergy)*(double)i/(double)(EnergyPoints-1);
        cout << energy << " " << fval[i] << " ";
    }
    cout << endl;
    */
    return 0;
}

int TwoDimensionResolutionFunction::calc(unsigned dim, const double *x, void *data, unsigned fdim, double *retval)
{
    // Call non-static member function.
    return static_cast<TwoDimensionResolutionFunction*>(data)->calculateIntegrand(dim,x,fdim,retval);
}

std::vector<double> TwoDimensionResolutionFunction::getCut(double kxIn, double kyIn, double kzIn)
{
    kx = kxIn;
    ky = kyIn;
    kz = kzIn;
    
    double xmin, xmax;
    
    double maximum = 1.0;
    double fraction = 1.0e-3;
    
    double d = log(maximum/fraction);

    double diff = sqrt(c*d/(a*c-b*b));

    switch (direction)
    {
        case 0:
            xmin = kx - diff;
            xmax = kx + diff;
            break;
        case 1:
            xmin = ky - diff;
            xmax = ky + diff;
            break;
        case 2:
            xmin = kz - diff;
            xmax = kz + diff;
            break;
    }
    
    vector<double> fval(EnergyPoints);
    vector<double> err(EnergyPoints);
    
    hcubature(EnergyPoints,TwoDimensionResolutionFunction::calc, this, 1, &xmin, &xmax, 0, tol, 0, ERROR_INDIVIDUAL, &fval[0], &err[0]);
    
    return fval;
}

EnergyResolutionFunction::EnergyResolutionFunction(OneDimGaussian& info, double min, double max, double points)
{
    MinimumEnergy = min;
    MaximumEnergy = max;
    EnergyPoints = points;
    
    fwhm = info.fwhm;
    tol = info.tol;
    SW = info.SW;
}

std::vector<double> EnergyResolutionFunction::getCut(double kx, double ky, double kz)
{

    vector<double> fval(EnergyPoints);
    for(int i=0;i!=EnergyPoints;i++)
    {
        fval[i] = 0.0;
    }
    
    //cout << x[0] << " " << ky << " " << kz << " " << u << endl;
    
    SW.createMatrix(kx,ky,kz);
    SW.Calc();
    vector<point> points = SW.getPoints();
    
    double ma = -4.0*log(2.0)/pow(fwhm,2);
    double factor = 2.0*sqrt(log(2.0))/(fwhm*sqrt(M_PI));
    
    double constant = log(tol/factor);
    double minmax = sqrt(constant/ma);
    
    //cout << minmax << endl;
    for(size_t k=0;k!=points.size();k++)
    {
        //cout << "calculated frequency & intensity: " << frequencies[k] << " " << intensities[k] << "  " << endl;
        long min_bin = (points[k].frequency-minmax - MinimumEnergy)*(EnergyPoints-1.0)/(MaximumEnergy-MinimumEnergy);
        //long avg_bin = (frequencies[k] - MinimumEnergy)*(EnergyPoints-1)/(MaximumEnergy-MinimumEnergy);
        long max_bin = (points[k].frequency+minmax - MinimumEnergy)*(EnergyPoints-1.0)/(MaximumEnergy-MinimumEnergy);
        
        if (min_bin < 0)
          min_bin = 0;
        else if (min_bin > EnergyPoints)
          min_bin = EnergyPoints;
        
        if (max_bin < 0)
          max_bin = 0;
        else if (max_bin > EnergyPoints)
          max_bin = EnergyPoints;
        
        //cout << min_bin << " " << avg_bin << " " << max_bin << endl;
        for(int i=0;i<=EnergyPoints;i++)
        {
            double energy = MinimumEnergy + (MaximumEnergy-MinimumEnergy)*(double)i/(double)(EnergyPoints-1);
            fval[i] += points[k].intensity*exp(ma*pow(points[k].frequency-energy,2));
        }
        
    }
    
    
    
    for(int i=0;i!=EnergyPoints;i++)
    {
        fval[i] *= factor;
    }
    
    //cout << endl;
    /*for(int i=0;i!=EnergyPoints;i++)
     {
     double energy = MinimumEnergy + (MaximumEnergy-MinimumEnergy)*(double)i/(double)(EnergyPoints-1);
     cout << energy << " " << fval[i] << " ";
     }
     cout << endl;
     */
    
    return fval;
}


IntegrateAxes::IntegrateAxes(axes_info info, TwoDimensionResolutionFunction resFunction, double min, double max, double points)
{
    MinimumEnergy = min;
    MaximumEnergy = max;
    EnergyPoints = points;
    resolutionFunction = resFunction;

    x = info.x;
    y = info.y;
    z = info.z;
    dx = info.dx;
    dy = info.dy;
    dz = info.dz;
    tol = info.tol;
}

int IntegrateAxes::calculateIntegrand(unsigned dim, const double *x, unsigned fdim, double *retval)
{
    double tmpx,tmpy,tmpz;
    
    size_t placeholder = 0;
    if(x)
    {
        tmpx = x[placeholder];
        placeholder++;
    }
    else
        tmpx = kx;
        
    if(y)
    {
        tmpy = x[placeholder];
        placeholder++;
    }
    else
        tmpy = ky;
    if(z)
    {
        tmpz = x[placeholder];
        placeholder++;
    }
    else
        tmpz = kz;
    
    //cout << "** " << x[0] << " " << x[1] << " " << x[2] << endl;
    //cout << " " << tmpx << " " << tmpy << " " << tmpz << endl;
    
    vector<double> val = resolutionFunction.getCut(tmpx,tmpy,tmpz);

    for(int i=0;i!=EnergyPoints;i++)
     {
     retval[i] = val[i]/volume;
     //double energy = MinimumEnergy + (MaximumEnergy-MinimumEnergy)*(double)i/(double)(EnergyPoints-1);
     //cout << energy << " " << val[i] << " ";
     }
     //cout << endl;
    return 0;
}

int IntegrateAxes::calc(unsigned dim, const double *x, void *data, unsigned fdim, double *retval)
{
    // Call non-static member function.
    return static_cast<IntegrateAxes*>(data)->calculateIntegrand(dim,x,fdim,retval);
}

std::vector<double> IntegrateAxes::getCut(double kxIn, double kyIn, double kzIn)
{
    kx = kxIn;
    ky = kyIn;
    kz = kzIn;
    
    std::vector<double> xmin,xmax;
    
    int dim = 0;
    volume = 1.0;
    if (x)
    {
        dim++;
        xmin.push_back(kx - dx);
        xmax.push_back(kx + dx);
        volume *= 2.0*dx;
    }
    if (y)
    {
        dim++;
        xmin.push_back(ky - dy);
        xmax.push_back(ky + dy);
        volume *= 2.0*dy;

    }
    if (z)
    {
        dim++;
        xmin.push_back(kz - dz);
        xmax.push_back(kz + dz);
        volume *= 2.0*dz;

    }
    
    //cout << "** " << kx << " " << ky << " " << kz << endl;
    //cout << xmin[0] << " " << xmin[1] << " " << xmin[2] << endl;
    //cout << xmax[0] << " " << xmax[1] << " " << xmax[2] << endl;

    vector<double> fval(EnergyPoints);
    vector<double> err(EnergyPoints);
    
    hcubature(EnergyPoints,IntegrateAxes::calc, this, dim, &xmin[0], &xmax[0], 0, tol, 0, ERROR_INDIVIDUAL, &fval[0], &err[0]);
    
    /*for(int i=0;i!=EnergyPoints;i++)
    {
        double energy = MinimumEnergy + (MaximumEnergy-MinimumEnergy)*(double)i/(double)(EnergyPoints-1);
        cout << energy << " " << fval[i] << " ";
    }
    cout << endl;
    */
    return fval;
    
}


/*NoResolutionFunction::NoResolutionFunction()
 {
 
 }
 
 std::vector<double> NoResolutionFunction::getCut(double kx, double ky, double kz)
 {
 }
 
 
 
 TwoDimensionResolutionFunction::TwoDimensionResolutionFunction()
 {
 
 }*/

/*FourDimensionResolutionFunction::FourDimensionResolutionFunction()
 {
 
 }
 
 std::vector<double> FourDimensionResolutionFunction::getCut(double kx, double ky, double kz)
 {
 }
 
 IntegrateAxes::IntegrateAxes()
 {
 
 }*/