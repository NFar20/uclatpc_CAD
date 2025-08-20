#include "electricField.hh"

ElectricField::ElectricField(G4double z_field) {
	Zfield = z_field;
}
ElectricField::~ElectricField() {}

void ElectricField::GetFieldValue(const G4double Point[4], G4double *field) const
{
	G4double x = Point[0];
	G4double y = Point[1];
	G4double z = Point[2];
	
	field[0] = 0.;
	field[1] = 0.;
	field[2] = 0.;
	field[3] = 0.;
	field[4] = 0.;
	field[5] = Zfield*volt/cm;
	
}

G4bool ElectricField::DoesFieldChangeEnergy() const
{
	return true;
}
