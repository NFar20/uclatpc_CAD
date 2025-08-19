#include "nestPart.hh"
#include <cmath>

nestPart::nestPart() : VDetector() 
{}

double nestPart::get_ElectricField(double x, double y, double z) const 
{
    double r = std::sqrt(x*x + y*y);
    
    if(r < 47.5 && z <= 105.2 && z >= 49.2) {
        return 100.; // V/cm
    }
    else { //outside xenon
        return 0.;
    }
}
