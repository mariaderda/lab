#ifndef B3Sensitive_h
#define B3Sensitive_h 1

#include "G4VSensitiveDetector.hh"
#include "B3Hits.hh"

class G4Step;
class G4HCofThisEvent;

class B3SensitiveDetector : public G4VSensitiveDetector
{
public:
  ///constructor
  B3SensitiveDetector(G4String);

  ///destructor
  ~B3SensitiveDetector();

  ///Method invoked at the beginning of each event
  void Initialize(G4HCofThisEvent*);

  ///Method invoked at each step in a "sensitive" volume
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);

  ///Method invoked at the end of each event
  void EndOfEvent(G4HCofThisEvent*);
  
private:
  B3HitsCollection *Collection;
  

};

#endif
