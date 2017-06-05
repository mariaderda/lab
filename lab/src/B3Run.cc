//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B3Run.cc 68058 2013-03-13 14:47:43Z gcosmo $
//
/// \file B3Run.cc
/// \brief Implementation of the B3Run class

#include "B3Run.hh"
#include "B3Hits.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"

#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4THitsMap.hh"
#include "G4SystemOfUnits.hh"

#include "B3Analysis.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B3Run::B3Run()
 : G4Run(), 
   fCollID_cryst(-1),
   fPrintModulo(10000)
{ }


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B3Run::~B3Run()
{
  delete G4AnalysisManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/// Called at the end of each event
void B3Run::RecordEvent(const G4Event* event)
{
  // Retrieve the collectionID corresponding to hits in the crystal
  // and in the patient. This is done only at the first event
  if ( fCollID_cryst < 0 ) {
   fCollID_cryst 
     = G4SDManager::GetSDMpointer()->GetCollectionID("crystal/edep");
   G4cout << " fCollID_cryst: " << fCollID_cryst << G4endl;   
  }

  G4int evtNb = event->GetEventID();
  
  if (evtNb%fPrintModulo == 0) { 
    G4cout << "\n---> end of event: " << evtNb << G4endl;
  }      
  
  //Hits collections
  //  
  // Get all hits-collections available for this events: there should be two 
  // hits-collection, one of hits in the patient and one of hits in the 
  // crystals. They are created in the UserGeometry.
  G4HCofThisEvent* HCE = event->GetHCofThisEvent();
  if(!HCE) return;
               
  //Energy in the crystal
  
   
  //ok, let's start the game: retrieve the hits-collection in the crystals.
  //This comes from a Geant4 multiscorer of type "G4PSEnergyDeposit", which scores 
  //energy deposit.
  G4THitsMap<G4double>* evtMap = 
    static_cast<G4THitsMap<G4double>*>(HCE->GetHC(fCollID_cryst));
               
  std::map<G4int,G4double*>::iterator itr;

  //Store the total energy in a variable
  G4double totEdep = 0.;
  G4AnalysisManager* man = G4AnalysisManager::Instance();

  G4double edep_arr[9];
  
  for (G4int i = 0 ; i < 9 ; i++){
    edep_arr[i] = 0.0; }
  
  for (itr = evtMap->GetMap()->begin(); itr != evtMap->GetMap()->end(); itr++) {
    G4double edep = *(itr->second);

    //Sum the energy deposited in all crystals, irrespectively of threshold.
    totEdep += edep;

    //these are the ID's of the detectors fired. 
    G4int copyNb  = (itr->first);

    edep_arr[copyNb] = edep;

    //G4cout << "\n  cryst" << copyNb << ": " << edep/keV << " keV ";
  }

  for (G4int i = 0 ; i < 9 ; i++){
    man->FillNtupleDColumn(i, edep_arr[i]/keV);}
  
  man->AddNtupleRow();
  man->FillH1(1,totEdep/keV);
  G4Run::RecordEvent(event);   
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B3Run::Merge(const G4Run* aRun)
{
  const B3Run* localRun = static_cast<const B3Run*>(aRun);

  G4Run::Merge(aRun); 
} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
