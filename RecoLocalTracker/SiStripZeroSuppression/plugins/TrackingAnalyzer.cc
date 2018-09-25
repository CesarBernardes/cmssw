// -*- C++ -*-
//
// Package:    TrackingAnalyzer/TrackingAnalyzer
// Class:      TrackingAnalyzer
// 
/**\class TrackingAnalyzer TrackingAnalyzer.cc TrackingAnalyzer/TrackingAnalyzer/plugins/TrackingAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Cesar Bernardes
//         Created:  Tue, 14 Aug 2018 18:34:57 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"


#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "DataFormats/TrackReco/interface/TrackExtra.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/Math/interface/Point3D.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"



//ROOT inclusion
#include "TROOT.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TList.h"
#include "TString.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "THStack.h"


//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class TrackingAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit TrackingAnalyzer(const edm::ParameterSet&);
      ~TrackingAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
      edm::Service<TFileService> fs_;
      edm::EDGetTokenT<reco::TrackCollection > srcTracks_;

      ///histograms
      TH1F *tk_Pt_;
      TH1F *tk_Eta_;
      TH1F *tk_Phi_;
      TH1F *tk_Pt_HP_;
      TH1F *tk_Eta_HP_;
      TH1F *tk_Phi_HP_; 
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
TrackingAnalyzer::TrackingAnalyzer(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed
   usesResource("TFileService");

   srcTracks_ = consumes<reco::TrackCollection>(iConfig.getParameter<edm::InputTag>("srcTracks"));

   tk_Pt_= fs_->make<TH1F>("tk_pt","tk_pt",100,0,50);
   tk_Pt_->SetXTitle("track pt [GeV]");
   tk_Pt_->SetYTitle("Entries"); 

   tk_Eta_= fs_->make<TH1F>("tk_eta","tk_eta",100,-2.5,2.5);
   tk_Eta_->SetXTitle("track eta");
   tk_Eta_->SetYTitle("Entries");

   tk_Phi_= fs_->make<TH1F>("tk_phi","tk_phi",100,-3.2,3.2);
   tk_Phi_->SetXTitle("track phi");
   tk_Phi_->SetYTitle("Entries");


   tk_Pt_HP_= fs_->make<TH1F>("tk_pt_hp_","tk_pt_hp_",100,0,50);
   tk_Pt_HP_->SetXTitle("track pt [GeV]");
   tk_Pt_HP_->SetYTitle("Entries");

   tk_Eta_HP_= fs_->make<TH1F>("tk_eta_hp_","tk_eta_hp_",100,-2.5,2.5);
   tk_Eta_HP_->SetXTitle("track eta");
   tk_Eta_HP_->SetYTitle("Entries");

   tk_Phi_HP_= fs_->make<TH1F>("tk_phi_hp_","tk_phi_hp_",100,-3.2,3.2);
   tk_Phi_HP_->SetXTitle("track phi");
   tk_Phi_HP_->SetYTitle("Entries");




}


TrackingAnalyzer::~TrackingAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
TrackingAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace reco;
   using namespace std; 

   //track collection
   edm::Handle<reco::TrackCollection> generalTracks;
   iEvent.getByToken(srcTracks_,generalTracks);


   for(reco::TrackCollection::const_iterator iter_tk = generalTracks->begin(); iter_tk != generalTracks->end(); iter_tk++){
  
   	tk_Pt_->Fill(iter_tk->pt());	
        tk_Eta_->Fill(iter_tk->eta()); 
        tk_Phi_->Fill(iter_tk->phi());
        if(!iter_tk->quality(reco::TrackBase::highPurity) || iter_tk->ptError()/iter_tk->pt()>0.3)continue; 
        tk_Pt_HP_->Fill(iter_tk->pt());       
        tk_Eta_HP_->Fill(iter_tk->eta());
        tk_Phi_HP_->Fill(iter_tk->phi());
   } 

   
}


// ------------ method called once each job just before starting event loop  ------------
void 
TrackingAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
TrackingAnalyzer::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TrackingAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  //desc.setUnknown();
  desc.add<edm::InputTag>("srcTracks",edm::InputTag("hiGeneralTracks"));
  descriptions.add("trackingAnalyzer", desc); 
  //descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TrackingAnalyzer);
