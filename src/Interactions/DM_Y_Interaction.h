#include <string>
#include <vector>
#include <Eigen/Dense>
#include <boost/shared_ptr.hpp>
#include "Cell.h"
#include "Interaction.h"

class DM_Y_Interaction: public Interaction
{
public:
    void Add_Interaction(double value_in, std::string sl_r_in,std::string sl_s_in, double min_in, double max_in);
    void Update_Matrix(Eigen::Vector3d K, boost::shared_ptr<Cell> cell, Eigen::MatrixXcd &LN);
private:
    std::string sl_r,sl_s;
    double value,min,max;
};