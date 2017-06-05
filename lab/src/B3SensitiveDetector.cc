#include "B3SensitiveDetector.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B3SensitiveDetector::B3SensitiveDetector(G4String name) : 
  G4VSensitiveDetector(name)
{
  G4String HCname; //name of the hits collection
  collectionName.insert(HCname="B3HitsCollection");
} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B3SensitiveDetector::~B3SensitiveDetector()
{;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B3SensitiveDetector::Initialize(G4HCofThisEvent *HCE)
{
  //initialize the hit collection. This gets its own ID. Other collections 
  //are created by the Geant4 scorers, so many HC will be available at the 
  //end of the event, partly custom-made and partly created by the Geant4 
  //scorers.
  Collection = new B3HitsCollection(SensitiveDetectorName,collectionName[0]);
 
  //retrieve the ID associated to this hits collection. This is done only 
  //once.
  static G4int HCID= -1;
  if (HCID<0){
    HCID=G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  }

  //Add the hits collection in the container, for the present event
  HCE->AddHitsCollection(HCID,Collection);
  
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool B3SensitiveDetector::ProcessHits (G4Step *aStep,G4TouchableHistory*)
{
  G4double edep = aStep->GetTotalEnergyDeposit();

  //skip, if there is no energy deposit: i.e. create hit only if there 
  //is actually an energy deposit in the sensitive detector.
  if(edep==0.) return false;

  //Create and fill a new B3Hits object
  B3Hits* newHit = new B3Hits();

  //fill:
  newHit->SetEnergyDeposit(edep);
  newHit->SetZ(aStep->GetPostStepPoint()->GetPosition().z());

  //insert the new hit in the collection
  Collection->insert( newHit ); 

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B3SensitiveDetector::EndOfEvent(G4HCofThisEvent* )
{;}
