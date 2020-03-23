// -*- C++ -*-
//
// Package:    PFCandAnalyzer/PFCandAnalyzer
// Class:      PFCandAnalyzer
// 
/**\class PFCandAnalyzer PFCandAnalyzer.cc PFCandAnalyzer/PFCandAnalyzer/plugins/PFCandAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Cesar Bernardes


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
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"


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

class PFCandAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit PFCandAnalyzer(const edm::ParameterSet&);
      ~PFCandAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
      edm::Service<TFileService> fs_;
      edm::EDGetTokenT<pat::PackedCandidateCollection  > srcPFcand_;
      edm::EDGetTokenT<reco::VertexCollection> tok_offlinePV_;

      ///histograms
      TH1F *tk_NHits_;
      TH1F *tk_Pt_;
      TH1F *tk_Eta_;
      TH1F *tk_Phi_;
      TH1F *tk_Algo_;
      TH1F *tk_dzSig_;
      TH1F *tk_dxySig_;
      TH1F *tk_pTres_;
      TH1F *tk_chi2_; 

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
PFCandAnalyzer::PFCandAnalyzer(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed
   usesResource("TFileService");

   srcPFcand_ = consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("srcPFcand"));
   tok_offlinePV_ = consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("srcVertices"));

   tk_Pt_= fs_->make<TH1F>("tk_pt","tk_pt",100,0,5);
   tk_Pt_->SetXTitle("track pt [GeV]");
   tk_Pt_->SetYTitle("Entries"); 

   tk_Eta_= fs_->make<TH1F>("tk_eta","tk_eta",100,-2.5,2.5);
   tk_Eta_->SetXTitle("track eta");
   tk_Eta_->SetYTitle("Entries");

   tk_Phi_= fs_->make<TH1F>("tk_phi","tk_phi",100,-3.2,3.2);
   tk_Phi_->SetXTitle("track phi");
   tk_Phi_->SetYTitle("Entries");

   tk_NHits_= fs_->make<TH1F>("tk_nhits_","tk_nhits_",60,0,60);
   tk_NHits_->SetXTitle("# of Valid Hits"); 
   tk_NHits_->SetYTitle("Entries");

   tk_Algo_= fs_->make<TH1F>("tk_algo_","tk_algo_",30,0,30);
   tk_Algo_->SetXTitle("Algo");
   tk_Algo_->SetYTitle("Entries");
 
   tk_dzSig_= fs_->make<TH1F>("tk_dzSig_","tk_dzSig_",100,-20,20);
   tk_dzSig_->SetXTitle("dz/#sigma(dz)");
   tk_dzSig_->SetYTitle("Entries");

   tk_dxySig_= fs_->make<TH1F>("tk_dxySig_","tk_dxySig_",100,-20,20);
   tk_dxySig_->SetXTitle("dxy/#sigma(dxy)");
   tk_dxySig_->SetYTitle("Entries"); 
  
   tk_pTres_= fs_->make<TH1F>("tk_pTres_","tk_pTres_",100,0,1);
   tk_pTres_->SetXTitle("#sigma_{pT}/pT");
   tk_pTres_->SetYTitle("Entries");    

   tk_chi2_= fs_->make<TH1F>("tk_chi2_","tk_chi2_",100,0,2);
   tk_chi2_->SetXTitle("#chi^{2}/NDF/NLayers");
   tk_chi2_->SetYTitle("Entries"); 


}


PFCandAnalyzer::~PFCandAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
PFCandAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace reco;
   using namespace std; 


   //packed PF candidates
   edm::Handle<pat::PackedCandidateCollection> pfCandidates;
   iEvent.getByToken(srcPFcand_,pfCandidates);

   //vtx collection
   edm::Handle<reco::VertexCollection> vertices;
   iEvent.getByToken(tok_offlinePV_,vertices);

   //best vertex
   double bestvz=-999.9, bestvx=-999.9, bestvy=-999.9;
   double bestvzError=-999.9, bestvxError=-999.9, bestvyError=-999.9;
   const reco::Vertex & vtx = (*vertices)[0];
   bestvz = vtx.z(); bestvx = vtx.x(); bestvy = vtx.y();
   bestvzError = vtx.zError(); bestvxError = vtx.xError(); bestvyError = vtx.yError();
   math::XYZPoint bestvtx(bestvx,bestvy,bestvz);


   //Loop in pf candidates
   for(unsigned int i = 0, n = pfCandidates->size(); i < n; ++i){
     const pat::PackedCandidate &pf = (*pfCandidates)[i];
     
     ///get track reco info
     if(!(pf.hasTrackDetails()))continue;
 
     const reco::Track &trk = pf.pseudoTrack();

     tk_Pt_->Fill(trk.pt());    
     ///if(trk.pt()<0.95)continue;//just for test
     tk_Eta_->Fill(trk.eta()); 
     tk_Phi_->Fill(trk.phi());
     tk_NHits_->Fill(trk.numberOfValidHits());
     tk_Algo_->Fill(trk.algo()); 
     double dzvtx = trk.dz(bestvtx);
     double dxyvtx = trk.dxy(bestvtx);
     double dzerror = sqrt(trk.dzError()*trk.dzError()+bestvzError*bestvzError);
     double dxyerror = sqrt(trk.dxyError()*trk.dxyError()+bestvxError*bestvyError);
     tk_dzSig_->Fill(dzvtx/dzerror);
     tk_dxySig_->Fill(dxyvtx/dxyerror);
     tk_pTres_->Fill(fabs(trk.ptError())/trk.pt());
     const reco::HitPattern& hit_pattern = trk.hitPattern();
     tk_chi2_->Fill(trk.normalizedChi2()/hit_pattern.trackerLayersWithMeasurement());

   }  
   
}


// ------------ method called once each job just before starting event loop  ------------
void 
PFCandAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
PFCandAnalyzer::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
PFCandAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  //desc.setUnknown();
  desc.add<edm::InputTag>("srcPFcand",edm::InputTag("packedPFCandidates"));
  desc.add<edm::InputTag>("srcVertices",edm::InputTag("offlineSlimmedPrimaryVertices")); 
  descriptions.add("pfcandAnalyzer", desc); 
  //descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(PFCandAnalyzer);
