#include "nestPart.hh"
#include <cmath>

nestPart::nestPart() : VDetector() 
{}

double nestPart::get_ElectricField(double x, double y, double z) const 
{
    //transferring electric field info to NEST
    double r = std::sqrt(x*x + z*z);
    
    if(r < 47.5*mm && y <= -44.*mm && y >= -100.*mm) {
        return 100.; // V/cm
    }
    else if(r < 47.5*mm && y <= -28.*mm && z >= -44.*mm) {
        return 250.;
    }
    else { //outside xenon
        return 0.;
    }
}
