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
#include <boost/shared_ptr.hpp>
#include <random>
#include <boost/thread.hpp>
#include <boost/version.hpp>
#include <boost/program_options.hpp>
#include "SpinWave.h"
#include "Initializer.h"
#include "progressbar.h"
#include <Eigen/Core>
using namespace Eigen;
using namespace boost;
namespace po = boost::program_options;
#include <iostream>
#include <iterator>
using namespace std;

struct mc_params
{
    double x0,y0,z0;
    double E_min,E_max;
    int E_points;
    boost::shared_ptr<SW_Builder> builder;
};

double f(double x,double y,double z)
{
    Vector4d big_F,little_f,eval_points,weights;
    big_F << 0.3972,0.6295,-0.0314,0.0044;
    little_f << 13.2442,4.9034,0.3496,0.0;

    double f_Q = 0.0;
    for(int k=0;k<4;k++)
    {
        double s = 2.0*M_PI/5.4*sqrt(pow(x,2) + pow(y,2) + 0.5*pow(z,2))/(4.0*M_PI);
        f_Q += big_F[k]*exp(-1.0*little_f[k]*pow(s,2));
    }
    return f_Q;
}

VectorXd g(void *params)
{
    mc_params *exp_data = reinterpret_cast<mc_params*>(params);
    double x = exp_data->x0;
    double z = exp_data->z0;
    double E_min = exp_data->E_min;
    double E_max = exp_data->E_max;
    int E_points = exp_data->E_points;
    
    VectorXd integral;
    integral.setZero(E_points);
    
    double a,b,c;
    a = 1109.0;
    b = 0.0;
    c = 0.48;
    double norm = sqrt((a*c-b*b)/(a*M_PI*M_PI));
    
    double sigma = 1.0/sqrt(2.0*a);
    double sigma_energy = 1.0/sqrt(2.0*c);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> d(0.0,sigma);
 
    int npoints = 201;
    double sum = 0.0;
    for(int j=0;j!=npoints;j++)
    {
        double u = d(gen);
        sum += 1.0/exp(-a*pow(u,2));
        double y = exp_data->y0 + u;
        //cout << x << '\t' << y << '\t' << z << endl;
        double f_Q = f(x,y,z);
        SpinWave test = exp_data->builder->Create_Element(x,y,z);
        test.Calc();
        vector<double> frequencies = test.Get_Frequencies();
        vector<double> intensities = test.Get_Intensities();
        for(size_t k=0;k!=frequencies.size();k++)
        {
            int min_bin = (int) (frequencies[k]-10.0*sigma_energy - E_min)*(E_points-1)/(E_max-E_min);
            int max_bin = (int) (frequencies[k]+10.0*sigma_energy - E_min)*(E_points-1)/(E_max-E_min);

            if (min_bin < 0)
                min_bin = 0;
            if (max_bin > E_points)
                max_bin = E_points;
            
            for(int i=min_bin;i!=max_bin;i++)
            {
                double energy = E_min + (E_max-E_min)*(double)i/(double)(E_points-1);
                
                //cout << energy << endl;
                //cout << frequencies[k] << endl;
                integral[i] += f_Q*f_Q*intensities[k]*exp(-c*pow((frequencies[k]-energy),2))*
                                                      exp(-2.0*b*(frequencies[k]-energy)*u);
                                                      //*exp(-a*pow(u,2));
            }
        }
    }
    integral = integral*norm*M_PI/sum;
    return integral;
}

VectorXd h(void *params)
{
    mc_params *exp_data = reinterpret_cast<mc_params*>(params);
    int E_points = exp_data->E_points;
    VectorXd integral,integral_sq;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist_x(1.8,2.2);
    std::uniform_real_distribution<double> dist_z(-3.2,-2.8);
        
    int npoints = 201;
    integral.setZero(E_points);
    for(int j=0;j!=npoints;j++)
    {
        exp_data->x0 = dist_x(gen);
        exp_data->z0 = dist_z(gen);
        //cout << x << " " << y << " " << z << endl;
        VectorXd result = g(params);
        integral += result;
        //integral_sq += result*result;
    }
    integral = integral/(double)npoints;
    /*double sigma = 0.0;
    for(int i=0;i!=integral.size();i++)
    {
        integral[i] = integral[i]/100001.0;
        integral_sq[i] = integral_sq[i]/100001.0;
        sigma += sqrt((integral_sq[i] - integral[i]*integral[i])/100000.0/100000.0);
        
    }
    cout << "sigma = " << sigma << endl;;
    */
    //cout << A << endl;
    return integral;
}

class ThreadClass {
public:
    int npoints,nproc,Epoints;
    MatrixXd figure;
    boost::mutex io_mutex; // The iostreams are not guaranteed to be thread-safe!
    ThreadClass(int n) // Constructor
    {
#if EIGEN_WORLD_VERSION >= 3 && EIGEN_MAJOR_VERSION >= 1
        Eigen::initParallel();
#endif
        nproc = n;
        npoints = 201;
        Epoints = 161;
        figure.setZero(Epoints,npoints);
    }
    ~ThreadClass()
    {
        
    }
    // Destructor
    void Run(int i,string filename)
    {
        Init four_sl;
        boost::unique_lock<boost::mutex> scoped_lock(io_mutex);
        four_sl.read_input(filename);
        scoped_lock.unlock();
        mc_params data;
        data.E_min = 0.0;
        data.E_max = 80.0;
        data.E_points = Epoints;
        data.builder = four_sl.get_builder();
        
        double x0,y0,z0,x1,y1,z1;
        x0=2.0;x1=2.0;
        y0=-1.5;y1=1.5;
        z0=-3.0;z1=-3.0;
        for(int m=i;m<npoints;m=m+nproc)
        {
            //cout << n << endl;
            data.x0 = x0 + (x1-x0)*m/(npoints-1);
            data.y0 = y0 + (y1-y0)*m/(npoints-1);
            data.z0 = z0 + (z1-z0)*m/(npoints-1);
            scoped_lock.lock();
            cout << data.x0 << " " << data.y0 << " " << data.z0 << endl;
            scoped_lock.unlock();
            figure.col(m) = h((void *)&data);
            //cout << figure.col(m) << endl;
        }
    }
};

int main(int argc, char * argv[])
{
    /*Init four_sl;

    four_sl.read_input("4sl_cell.xml");
    //shared_ptr<SW_Builder> asdf = four_sl.get_builder();
    mc_params data;
    data.E_min = 0.0;
    data.E_max = 80.0;
    data.E_points = 51;
    data.builder = four_sl.get_builder();
    
    vector<double> E_array(data.E_points);
    for(int i=0;i<data.E_points;i++)
    {
        E_array[i] = data.E_min + (data.E_max-data.E_min)*(double)i/(double)(data.E_points-1);
    }
    
    double x0,y0,z0,x1,y1,z1;
    x0=2.0;x1=2.0;
    y0=0.0;y1=1.5;
    z0=-3.0;z1=-3.0;
    

    MatrixXd figure(data.E_points,npoints);
    ProgressBar pbar(npoints);
    pbar.start();
    for(int n=0;n!=npoints;n++)
    {
        pbar.update(n);
        //cout << n << endl;
        data.x0 = x0 + (x1-x0)*n/(npoints-1);
        data.y0 = y0 + (y1-y0)*n/(npoints-1);
        data.z0 = z0 + (z1-z0)*n/(npoints-1);
        //cout << x << " " << y << " " << z << endl;
        figure.col(n) = h((void *)&data);
    }
    pbar.finish();
    */
    
    string filename;
    int n_threads;
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
        ("help", "produce help message")
        ("input", po::value<string>(), "set input filename")
        ("threads", po::value<int>(), "set number of threads")
        ;
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        if (vm.count("help")) {
            cout << desc << "\n";
            return 1;
        }
        if (vm.count("input")) {
            cout << "input filename was set to "
            << vm["input"].as<string>() << ".\n";
            filename = vm["input"].as<string>();
        } else {
            cout << "input filename was not set.\n";
            return 1;
        }
        if (vm.count("threads")) {
            cout << "Using "
            << vm["threads"].as<int>() << " processors.\n";
            n_threads = vm["threads"].as<int>();
        } else {
            cout << "number of threads was not set. Using 1 processor.\n";
            n_threads = 1;

        }
    }
    catch(std::exception& e) {
        cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
    }
    
    boost::thread_group g;

    ThreadClass tc(n_threads);
    for (int i=0;i<n_threads;i++)
    {
        boost::thread *t = new boost::thread(&ThreadClass::Run, &tc, i, filename);
        g.add_thread(t);
    }
    g.join_all();
    
    std::ofstream file("test.txt");
    if (file.is_open())
    {
        file << tc.figure << '\n';
    }

    /*for(int i=0;i!=E_array.size();i++)
    {
        cout << E_array[i] << '\t' << intensities[i] << endl;
    }*/
   
    return 0;
}
