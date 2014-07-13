#include <cmath>
#include <nlopt.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <set>
#include <array>
#include <algorithm>
<<<<<<< HEAD
#include <boost/lexical_cast.hpp>
=======
#include <memory>
#include <Eigen/Dense>
#include "nlopt.hpp"
#include "Genie/SpinWave.h"
>>>>>>> 1e57674e520ae772f18ec12d7e548313c75c6e58
#include "Genie/SpinWaveBuilder.h"
#include "Initializer.h"
#include "Cell/Cell.h"
#include "Cell/Neighbors.h"
#include "Interactions/InteractionFactory.h"
<<<<<<< HEAD
#include "External/ezRateProgressBar.hpp"

using namespace std;
using namespace Eigen;
=======
#include "SpinWavePlot/OneDimensionalFactory.h"
#include "SpinWavePlot/SpinWavePlot.h"
#include "SpinWavePlot/EnergyResolutionFunction.h"
#include "SpinWavePlot/IntegrateAxes.h"
#include "Containers/Energies.h"
#include "Containers/HKLDirections.h"
#include <unistd.h>
#include "Containers/Energies.h"
#include "Containers/Results.h"

using namespace std;
>>>>>>> 1e57674e520ae772f18ec12d7e548313c75c6e58
using namespace SpinWaveGenie;
typedef Eigen::Matrix <bool, Eigen::Dynamic, Eigen::Dynamic> MatrixXb;

<<<<<<< HEAD
double SA = 1.3;
double SB = 0.33;
Cell cell;
SpinWaveBuilder builder;


void initialize(const std::vector<double> &x, const std::vector<double> &parameters)
{

    
    /*cout << "x[i] = ";
    for (size_t i = 0; i<2;i++)
    {
        cout << x[i] << " ";
    }
    cout << endl;
    */

    double theta0 = x[0];
    double theta1 = x[1];

    cell.setBasisVectors(8.5,8.5,8.5,90.0,90.0,90.0);
    
    Sublattice Mn0;
    string name = "Mn0";
    Mn0.setName(name);
    Mn0.setType("MN2");
    Mn0.setMoment(SA,0.0,0.0);
    cell.addSublattice(Mn0);
    cell.addAtom(name,0.0,0.0,0.0);
    cell.addAtom(name,0.0,0.5,0.5);
    cell.addAtom(name,0.5,0.0,0.5);
    cell.addAtom(name,0.5,0.5,0.0);
    
    Sublattice Mn1;
    name = "Mn1";
    Mn1.setName(name);
    Mn1.setType("MN2");
    Mn1.setMoment(SA,0.0,0.0);
    cell.addSublattice(Mn1);
    cell.addAtom(name,0.75,0.25,0.75);
    cell.addAtom(name,0.75,0.75,0.25);
    cell.addAtom(name,0.25,0.25,0.25);
    cell.addAtom(name,0.25,0.75,0.75);
    
    Sublattice V0;
    name = "V0";
    V0.setName(name);
    V0.setType("V3");
    if (theta1 < M_PI)
    {
        V0.setMoment(SB,theta1,3.0*M_PI/4.0);
    }
    else
    {
        V0.setMoment(SB,2.0*M_PI-theta1,7.0*M_PI/4.0);
    }
    cell.addSublattice(V0);
    cell.addAtom(name,0.875,0.125,0.375);
    cell.addAtom(name,0.875,0.625,0.875);
    cell.addAtom(name,0.375,0.125,0.875);
    cell.addAtom(name,0.375,0.625,0.375);
    
    Sublattice V1;
    name = "V1";
    V1.setName(name);
    V1.setType("V3");
    if (theta1 < M_PI)
    {
        V1.setMoment(SB,theta1,7.0*M_PI/4.0);
    }
    else
    {
        V1.setMoment(SB,2.0*M_PI-theta1,3.0*M_PI/4.0);
    }
    cell.addSublattice(V1);
    cell.addAtom(name,0.125,0.375,0.875);
    cell.addAtom(name,0.125,0.875,0.375);
    cell.addAtom(name,0.625,0.375,0.375);
    cell.addAtom(name,0.625,0.875,0.875);
    
    Sublattice V2;
    name = "V2";
    V2.setName(name);
    V2.setType("V3");
    if (theta0 < M_PI)
    {
        V2.setMoment(SB,theta0,M_PI/4.0);
    }
    else
    {
        V2.setMoment(SB,2.0*M_PI-theta0,5.0*M_PI/4.0);
    }
    cell.addSublattice(V2);
    cell.addAtom(name,0.375,0.875,0.125);
    cell.addAtom(name,0.375,0.375,0.625);
    cell.addAtom(name,0.875,0.875,0.625);
    cell.addAtom(name,0.875,0.375,0.125);
    
    Sublattice V3;
    name = "V3";
    V3.setName(name);
    V3.setType("V3");
    if (theta0 < M_PI)
    {
        V3.setMoment(SB,theta0,5.0*M_PI/4.0);
    }
    else
    {
        V3.setMoment(SB,2.0*M_PI-theta0,M_PI/4.0);
    }
    cell.addSublattice(V3);
    cell.addAtom(name,0.625,0.625,0.625);
    cell.addAtom(name,0.625,0.125,0.125);
    cell.addAtom(name,0.125,0.625,0.125);
    cell.addAtom(name,0.125,0.125,0.625);
    
    builder.updateCell(cell);
    InteractionFactory interactions;

    builder.addInteraction(interactions.getExchange("Jbb",parameters[1],"V0","V1",2.975,3.06));
    builder.addInteraction(interactions.getExchange("Jbb",parameters[1],"V2","V3",2.975,3.06));

    builder.addInteraction(interactions.getExchange("Jbbp",parameters[2],"V0","V2",2.975,3.06));
    builder.addInteraction(interactions.getExchange("Jbbp",parameters[2],"V0","V3",2.975,3.06));
    builder.addInteraction(interactions.getExchange("Jbbp",parameters[2],"V1","V2",2.975,3.06));
    builder.addInteraction(interactions.getExchange("Jbbp",parameters[2],"V1","V3",2.975,3.06));
    
    Vector3 direction(-1.0,1.0,-1.0);
    builder.addInteraction(interactions.getAnisotropy("Db",parameters[3],direction,"V0"));
    direction = Vector3(1.0,-1.0,-1.0);
    builder.addInteraction(interactions.getAnisotropy("Db",parameters[3],direction,"V1"));
    direction = Vector3(1.0,1.0,-1.0);
    builder.addInteraction(interactions.getAnisotropy("Db",parameters[3],direction,"V2"));
    direction = Vector3(-1.0,-1.0,-1.0);
    builder.addInteraction(interactions.getAnisotropy("Db",parameters[3],direction,"V3"));

    builder.addInteraction(interactions.getExchange("Jab",parameters[0],"Mn0","V0",3.48,3.57));
    builder.addInteraction(interactions.getExchange("Jab",parameters[0],"Mn0","V1",3.48,3.57));
    builder.addInteraction(interactions.getExchange("Jab",parameters[0],"Mn0","V2",3.48,3.57));
    builder.addInteraction(interactions.getExchange("Jab",parameters[0],"Mn0","V3",3.48,3.57));
    builder.addInteraction(interactions.getExchange("Jab",parameters[0],"Mn1","V0",3.48,3.57));
    builder.addInteraction(interactions.getExchange("Jab",parameters[0],"Mn1","V1",3.48,3.57));
    builder.addInteraction(interactions.getExchange("Jab",parameters[0],"Mn1","V2",3.48,3.57));
    builder.addInteraction(interactions.getExchange("Jab",parameters[0],"Mn1","V3",3.48,3.57));
    
    Vector3 zhat(0.0,0.0,1.0);
    builder.addInteraction(interactions.getMagneticField("H",parameters[4],zhat,"Mn0"));
    builder.addInteraction(interactions.getMagneticField("H",parameters[4],zhat,"Mn1"));
    builder.addInteraction(interactions.getMagneticField("H",parameters[4],zhat,"V0"));
    builder.addInteraction(interactions.getMagneticField("H",parameters[4],zhat,"V1"));
    builder.addInteraction(interactions.getMagneticField("H",parameters[4],zhat,"V2"));
    builder.addInteraction(interactions.getMagneticField("H",parameters[4],zhat,"V3"));
}

double myfunc(const std::vector<double> &x, const std::vector<double> &parameters)
{
    if (x[0] < M_PI)
    {
        cell.getSublattice("V2").setMoment(SB,x[0],M_PI/4.0);
        cell.getSublattice("V3").setMoment(SB,x[0],5.0*M_PI/4.0);
        
    }
    else
    {
        cell.getSublattice("V2").setMoment(SB,2.0*M_PI-x[0],5.0*M_PI/4.0);
        cell.getSublattice("V3").setMoment(SB,2.0*M_PI-x[0],M_PI/4.0);
    }
    
    if (x[1] < M_PI)
    {
        cell.getSublattice("V0").setMoment(SB,x[1],3.0*M_PI/4.0);
        cell.getSublattice("V1").setMoment(SB,x[1],7.0*M_PI/4.0);

    }
    else
    {
        cell.getSublattice("V0").setMoment(SB,2.0*M_PI-x[1],7.0*M_PI/4.0);
        cell.getSublattice("V1").setMoment(SB,2.0*M_PI-x[1],3.0*M_PI/4.0);
    }
    
    builder.updateCell(cell);
    builder.updateInteraction("H", parameters[4]);
    
    return builder.getEnergy();
}

int main()
{
    int samples(801);

    std::vector<double> angles(2.0*M_PI,2);
    vector<double> parameters = {-2.5,-12.3,-12.3,0.0,0.0};
    initialize(angles,parameters);
    Eigen::MatrixXd results(samples,samples);
    for (size_t frame = 0;frame<501;frame++)
    {
        ez::ezRateProgressBar<int> p(samples*samples);
        p.units = "angles";
        p.start();
        for(int i = 0;i<samples;i++)
        {
            for(int j = 0;j<samples;j++)
            {
                angles[0] = 2.0*M_PI*(double)i/(double)(samples-1);
                angles[1] = 2.0*M_PI*(double)j/(double)(samples-1);
                parameters[4] = frame/10.0;
                results(i,j) = myfunc(angles,parameters);
                p.update(i*samples+j);
            }
        }
        p.update(samples*samples);
        std::ofstream file("BField_"+boost::lexical_cast<string>(frame)+".txt");
        file << results << endl;
    }
    return 0;
=======
class LoadXYZ
{
public:
    void readFile(string filename);
    typedef vector<vector<double> >::iterator Iterator;
    Iterator begin();
    Iterator end();
protected:
    vector<vector<double> > inputData;
    MatrixXb mask;
};

void LoadXYZ::readFile(string filename)
{
    ifstream file;
    string output;
    file.open(filename.c_str());
    while(getline(file,output))
    {
        istringstream parser;
        parser.str(output);
        vector<double> tmp(9);
        parser >> tmp[0] >> tmp[1] >> tmp[2] >> tmp[3] >> tmp[4] >> tmp[5] >> tmp[6] >> tmp[7] >> tmp[8];
        inputData.push_back(tmp);
    }
    file.close();
}

LoadXYZ::Iterator LoadXYZ::begin()
{
    return inputData.begin();
}

LoadXYZ::Iterator LoadXYZ::end()
{
    return inputData.end();
}

LoadXYZ xyzfile;

double myfunc(const std::vector<double> &x, std::vector<double> &grad, void *my_func_data)
{
    if (!grad.empty())
    {
        cout << "error: no gradient available" << endl;
    }
    
    cout << "parameters: ";
    cout << x[0] << " " << x[1] << " " << x[2] << " " << x[3] << " " << x[4] << " " << x[5] << " " << x[6] << " " << x[7]  << " " <<  x[8] << " " << x[9] << " " << x[10] << endl;

    
    double J,J1,J2;
    J = x[1];
    J2 = x[2];
    J1 = x[3];
    
    double SA = 2.0;
    double theta = M_PI/2.0;

    Cell cell;
    cell.setBasisVectors(4.2827,4.2827,6.1103,90.0,90.0,120.0);
    
    Sublattice Spin0;
    string name = "Spin0";
    Spin0.setName(name);
    Spin0.setType("MN2");
    Spin0.setMoment(SA,theta,0.0);
    cell.addSublattice(Spin0);
    cell.addAtom(name,0.0,0.0,0.0);
    cell.addAtom(name,0.0,0.0,0.5);
    
    SpinWaveBuilder builder(cell);
    InteractionFactory interactionFactory;
    
    builder.addInteraction(interactionFactory.getExchange("J1",x[0],name,name,3.0,3.1));
    builder.addInteraction(interactionFactory.getExchange("J2",x[1],name,name,4.2,4.3));
    builder.addInteraction(interactionFactory.getExchange("J3",x[2],name,name,5.2,5.3));
    builder.addInteraction(interactionFactory.getExchange("J4",x[3],name,name,6.0,6.2));
    builder.addInteraction(interactionFactory.getExchange("J5",x[4],name,name,7.40,7.43));
    builder.addInteraction(interactionFactory.getExchange("J6",x[5],name,name,7.44,7.49));
    builder.addInteraction(interactionFactory.getExchange("J7",x[6],name,name,9.0,9.1));
    builder.addInteraction(interactionFactory.getExchange("J8",x[7],name,name,9.1,9.2));
    builder.addInteraction(interactionFactory.getExchange("J9",x[8],name,name,9.55,9.65));
    builder.addInteraction(interactionFactory.getExchange("J10",x[9],name,name,10.05,10.15));
    builder.addInteraction(interactionFactory.getExchange("J11",x[10],name,name,10.5,10.6));

    
    cout << cell.getBasisVectors() << endl;
    cout << cell.getReciprocalVectors() << endl;
    
    SpinWave SW = builder.createElement();
    OneDimensionalFactory factory;
    auto gauss = factory.getGaussian(7.5,1.0e-8);
    auto energies = Energies(0.0, 120.0, 1001);
    
    unique_ptr<SpinWavePlot> res(new EnergyResolutionFunction(move(gauss), SW, energies));
    
    HKLDirections H01;
    H01.addDirection(1,0.1);
    H01.addDirection(2, 0.1);
    
    HKLDirections H0L;
    H0L.addDirection(1.0,-0.5,0.0,0.1);
    H0L.addDirection(1,0.1);
    
    unique_ptr<SpinWavePlot> cut(new IntegrateAxes(move(res),H01,1.0e-7));
    
    double chisq = 0.0;
    
    for (auto it = xyzfile.begin();it!=xyzfile.begin()+16;++it)
    {
        cout << "position1: " << (*it)[0] << "," << (*it)[1] << "," <<(*it)[2] << endl;
        //SW.createMatrix((*it)[0],(*it)[1],(*it)[2]);
        //SW.calculate();
        //Results results = SW.getPoints();
        //double frequency  = results.begin()->frequency;
        vector<double> values = cut->getCut((*it)[0],(*it)[1],(*it)[2]);

        size_t result = std::distance(values.begin(),std::max_element(values.begin(), values.end()));
        double frequency = energies[result];
        chisq += pow((frequency - (*it)[3])/(*it)[4],2);
        cout << "frequency: " << frequency << "," << (*it)[3] << "," <<(*it)[4] << endl;
        cout << pow((frequency - (*it)[3])/(*it)[4],2) << endl;
    }
    cout << endl;
    
    gauss = factory.getGaussian(7.5,1.0e-8);
    res = unique_ptr<SpinWavePlot>(new EnergyResolutionFunction(move(gauss), SW, energies));
    cut = unique_ptr<SpinWavePlot>(new IntegrateAxes(move(res),H0L,1.0e-7));
    
    for (auto it = xyzfile.begin()+16;it!=xyzfile.end();++it)
    {
        cout << "position2: " << (*it)[0] << "," << (*it)[1] << "," <<(*it)[2] << endl;
        //SW.createMatrix((*it)[0],(*it)[1],(*it)[2]);
        //SW.calculate();
        //Results results = SW.getPoints();
        //double frequency  = results.begin()->frequency;
        vector<double> values = cut->getCut((*it)[0],(*it)[1],(*it)[2]);
        
        size_t result = std::distance(values.begin(),std::max_element(values.begin(), values.end()));
        double frequency = energies[result];
        chisq += pow((frequency - (*it)[3])/(*it)[4],2);
        cout << "frequency: " << frequency << "," << (*it)[3] << "," <<(*it)[4] << endl;
        cout << pow((frequency - (*it)[3])/(*it)[4],2) << endl;
    }
    cout << endl;
    
    cout << "chisq = " <<  chisq << endl;
    return chisq;
}

int main()
{
    string filename = "/Users/svh/Documents/spin_wave_genie/MnBi/MnBi.txt";
    xyzfile.readFile(filename);
    
    std::vector<double> ub(11);
    ub[0] =  8.0;
    ub[1] =  8.0;
    ub[2] =  8.0;
    ub[3] =  8.0;
    ub[4] =  8.0;
    ub[5] =  8.0;
    ub[6] =  8.0;
    ub[7] =  8.0;
    ub[8] =  8.0;
    ub[9] =  8.0;
    ub[10] =  8.0;


    std::vector<double> lb(11);
    lb[0] = -8.0;
    lb[1] = -8.0;
    lb[2] = -8.0;
    lb[3] = -8.0;
    lb[4] = -8.0;
    lb[5] = -8.0;
    lb[6] = -8.0;
    lb[7] = -8.0;
    lb[8] = -8.0;
    lb[9] = -8.0;
    lb[10] = -8.0;

    
    std::vector<double> x(11);
    
    double minf = 0.0;
    
    //vector<double> gradient;
    
    x[0] = -6.6508;
    x[1] =  1.16762;
    x[2] =  1.78181;
    x[3] =  -1.02878;
    x[4] =  0.948171;
    x[5] =  0.672291;
    x[6] =  0.152505;
    x[7] =  0.145495;
    x[8] =  0.0802911;
    x[9] =  0.12095;
    x[10] = 0.0999863;
    nlopt::opt opt(nlopt::LN_SBPLX,11);
    opt.set_min_objective(myfunc, NULL);
    opt.set_upper_bounds(ub);
    opt.set_lower_bounds(lb);
    opt.set_ftol_abs(5.0e-5);
    opt.set_maxeval(1000);
    opt.optimize(x, minf);
    
    cout << "minf = " << minf << endl;

>>>>>>> 1e57674e520ae772f18ec12d7e548313c75c6e58
}
