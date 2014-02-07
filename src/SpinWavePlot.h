#ifndef __SpinWavePlot__
#define __SpinWavePlot__


#include <iostream>
#include <string>
#include <vector>
#include "Genie/SpinWave.h"
#include "OneDimensionalGaussian.h"

/* Abstract base class */
class SpinWavePlot
{
public:
    virtual std::vector<double> getCut(double kx, double ky, double kz) = 0; // returns constant-Q cut
    virtual ~SpinWavePlot(){};
};

struct TwoDimGaussian
{
    double a,b,c;
    unsigned direction;
    double tol;
    SpinWave SW;
};

class TwoDimensionResolutionFunction : SpinWavePlot{
public:
    TwoDimensionResolutionFunction(){};
    TwoDimensionResolutionFunction(TwoDimGaussian& info, double min, double max, double points);
    int calculateIntegrand(unsigned dim, const double *x, unsigned fdim, double *retval);
    static int calc(unsigned dim, const double *x, void *data, unsigned fdim, double *retval);
    std::vector<double> getCut(double kxIn, double kyIn, double kzIn);
    ~TwoDimensionResolutionFunction(){};
private:
    double MinimumEnergy,MaximumEnergy,tol;
    double a,b,c;
    double kx,ky,kz;
    unsigned EnergyPoints,direction;
    SpinWave SW;
};

struct axes_info
{
    bool x,y,z;
    double dx,dy,dz;
    double tol;
};

class IntegrateAxes : SpinWavePlot {
public:
    IntegrateAxes(axes_info info, TwoDimensionResolutionFunction resFunction, double min, double max, double points);
    int calculateIntegrand(unsigned dim, const double *x, unsigned fdim, double *retval);
    static int calc(unsigned dim, const double *x, void *data, unsigned fdim, double *retval);
    std::vector<double> getCut(double kx, double ky, double kz);
    ~IntegrateAxes(){};
private:
    double MinimumEnergy,MaximumEnergy;
    double kx,ky,kz;
    unsigned EnergyPoints;
    bool x,y,z;
    double dx,dy,dz;
    double tol,volume;
    TwoDimensionResolutionFunction ResolutionFunction;
};

/*
 class FourDimensionResolutionFunction : SpinWavePlot {
 FourDimensionResolutionFunction();
 std::vector<double> getCut(double kx, double ky, double kz);
 ~FourDimensionResolutionFunction() = 0;
 };
 */

#endif /* defined(__SpinWavePlot__) */
