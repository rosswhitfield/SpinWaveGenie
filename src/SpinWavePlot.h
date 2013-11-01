#include <iostream>
#include <string>
#include <vector>
#include "SW_Builder.h"

/* Abstract base class */
// The abstract Coffee class defines the functionality of Coffee implemented by decorator
class SpinWavePlot
{
public:
    virtual std::vector<double> getCut(double kx, double ky, double kz) = 0; // returns constant-Q cut
    virtual ~SpinWavePlot(){};
};

class TwoDimensionResolutionFunction : SpinWavePlot{
public:
    TwoDimensionResolutionFunction(SW_Builder& builderInput, double min, double max, double points);
    int calculateIntegrand(unsigned dim, const double *x, unsigned fdim, double *retval);
    static int calc(unsigned dim, const double *x, void *data, unsigned fdim, double *retval);
    std::vector<double> getCut(double kxIn, double kyIn, double kzIn);
    ~TwoDimensionResolutionFunction(){};
private:
    double MinimumEnergy,MaximumEnergy;
    double kx,ky,kz;
    unsigned EnergyPoints;
    SW_Builder builder;
};

class IntegrateAxes : SpinWavePlot {
public:
    IntegrateAxes(SW_Builder& builderInput, double min, double max, double points);
    int calculateIntegrand(unsigned dim, const double *x, unsigned fdim, double *retval);
    static int calc(unsigned dim, const double *x, void *data, unsigned fdim, double *retval);
    std::vector<double> getCut(double kx, double ky, double kz);
    ~IntegrateAxes(){};
private:
    double MinimumEnergy,MaximumEnergy;
    double kx,ky,kz;
    unsigned EnergyPoints;
    SW_Builder builder;
};

/*
 // extension of SpinWaveGenie with no resolution function
 class NoResolutionFunction : SpinWavePlot {
 NoResolutionFunction();
 std::vector<double> getCut(double kx, double ky, double kz);
 ~NoResolutionFunction() = 0;
 };
 
 
 
 class EnergyResolutionFunction : SpinWavePlot {
 EnergyResolutionFunction();
 std::vector<double> getCut(double kx, double ky, double kz);
 ~EnergyResolutionFunction() = 0;
 };
 */

/*
 class FourDimensionResolutionFunction : SpinWavePlot {
 FourDimensionResolutionFunction();
 std::vector<double> getCut(double kx, double ky, double kz);
 ~FourDimensionResolutionFunction() = 0;
 };
 */