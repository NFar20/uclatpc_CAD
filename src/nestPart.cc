#include "nestPart.hh"
#include <cmath>

nestPart::nestPart() : VDetector() 
{}

double nestPart::get_ElectricField(double x, double y, double z) const 
{
    double r = std::sqrt(x*x + y*y);
    
    if(r < 4.75 && z <= 10.52 && z >= 4.92) {
        return 100.; // V/cm
    }
    else { //outside xenon
        return 0.;
    }
}
