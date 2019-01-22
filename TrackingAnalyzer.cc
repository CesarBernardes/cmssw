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
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"


//ROOT inclusion
#include "TROOT.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TProfile.h"
#include "TList.h"
#include "TString.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "THStack.h"
#include "Math/Point3D.h"

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
      edm::EDGetTokenT<int> centralitySrc_; 
      edm::EDGetTokenT<reco::VertexCollection > vertexSrc_;
      edm::EDGetTokenT<reco::TrackCollection > generalTracksSrc_;
      edm::EDGetTokenT<reco::TrackCollection > pixelTracksSrc_;

      bool useGeneralTracks_;

      ///functions
      static bool vtxSort( const reco::Vertex &  a, const reco::Vertex & b );  

      ///histograms
      //1D 
      TH1F *tk_NHits_;
      TH1F *tk_Pt_;
      TH1F *tk_Eta_;
      TH1F *tk_Phi_;
      TH1F *tk_Algo_;
      TH1F *tk_dzOverDzErr_;  
      TH1F *tk_dxyOverDxyErr_;
      TH1F *tk_dptOverPt_;
      TH1F *tk_normChi2_;     

      TH1F *tk_NHits_HP_;
      TH1F *tk_Pt_HP_;
      TH1F *tk_Eta_HP_;
      TH1F *tk_Phi_HP_; 
      TH1F *tk_Algo_HP_;  
      TH1F *tk_dzOverDzErr_HP_;  
      TH1F *tk_dxyOverDxyErr_HP_;
      TH1F *tk_dptOverPt_HP_;
      TH1F *tk_normChi2_HP_; 

      //2D 
      TH2F *tk_centrality_eta_;
      TH2F *tk_centrality_pt_;
      TH2F *tk_eta_pt_;
      TH2F *tk_centrality_eta_HP_;
      TH2F *tk_centrality_pt_HP_;
      TH2F *tk_eta_pt_HP_;
      //3D
      TH3F *tk_eta_centrality_dzOverDzErr_;
      TH3F *tk_eta_centrality_dxyOverDxyErr_;
      TH3F *tk_eta_centrality_dptOverPt_;
      TH3F *tk_eta_centrality_normChi2_;
      TH3F *tk_eta_centrality_NHits_; 
      TH3F *tk_eta_centrality_dzOverDzErr_HP_;
      TH3F *tk_eta_centrality_dxyOverDxyErr_HP_;
      TH3F *tk_eta_centrality_dptOverPt_HP_;
      TH3F *tk_eta_centrality_normChi2_HP_;
      TH3F *tk_eta_centrality_NHits_HP_; 
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

   centralitySrc_ = consumes<int>(iConfig.getParameter<edm::InputTag>("centralitySrc"));
   vertexSrc_ = consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertexSrc"));
   generalTracksSrc_ = consumes<reco::TrackCollection>(iConfig.getParameter<edm::InputTag>("generalTracksSrc"));
   pixelTracksSrc_ = consumes<reco::TrackCollection>(iConfig.getParameter<edm::InputTag>("pixelTracksSrc"));

   useGeneralTracks_  = iConfig.getParameter<bool>("useGeneralTracks");

   //1D histos

   tk_Pt_= fs_->make<TH1F>("tk_pt","tk_pt",100,0,50);
   tk_Pt_->SetXTitle("track pt [GeV]");
   tk_Pt_->SetYTitle("Entries"); 

   tk_Eta_= fs_->make<TH1F>("tk_eta","tk_eta",100,-2.4,2.4);
   tk_Eta_->SetXTitle("track eta");
   tk_Eta_->SetYTitle("Entries");

   tk_Phi_= fs_->make<TH1F>("tk_phi","tk_phi",100,-3.2,3.2);
   tk_Phi_->SetXTitle("track phi");
   tk_Phi_->SetYTitle("Entries");

   tk_dzOverDzErr_= fs_->make<TH1F>("tk_dzOverDzErr","tk_dzOverDzErr",100,-50,50);
   tk_dzOverDzErr_->SetXTitle("tracks dZ/dZerror");
   tk_dzOverDzErr_->SetYTitle("Entries"); 

   tk_dxyOverDxyErr_= fs_->make<TH1F>("tk_dxyOverDxyErr","tk_dxyOverDxyErr",100,-50,50);
   tk_dxyOverDxyErr_->SetXTitle("tracks dXY/dXYerror");
   tk_dxyOverDxyErr_->SetYTitle("Entries");

   tk_dptOverPt_= fs_->make<TH1F>("tk_dptOverPt","tk_dptOverPt",100,0,1);
   tk_dptOverPt_->SetXTitle("tracks pTerror/pT");
   tk_dptOverPt_->SetYTitle("Entries");
    
   tk_normChi2_= fs_->make<TH1F>("tk_normChi2","tk_normChi2",100,0,100);
   tk_normChi2_->SetXTitle("Normalized #Chi^{2}");
   tk_normChi2_->SetYTitle("Entries");

   tk_Pt_HP_= fs_->make<TH1F>("tk_Pt_HP","tk_Pt_HP",100,0,50);
   tk_Pt_HP_->SetXTitle("track pt [GeV]");
   tk_Pt_HP_->SetYTitle("Entries");

   tk_Eta_HP_= fs_->make<TH1F>("tk_Eta_HP","tk_Eta_HP",100,-2.4,2.4);
   tk_Eta_HP_->SetXTitle("track eta");
   tk_Eta_HP_->SetYTitle("Entries");

   tk_Phi_HP_= fs_->make<TH1F>("tk_Phi_HP","tk_Phi_HP",100,-3.2,3.2);
   tk_Phi_HP_->SetXTitle("track phi");
   tk_Phi_HP_->SetYTitle("Entries");



   tk_NHits_= fs_->make<TH1F>("tk_NHits","tk_NHits",60,0,60);
   tk_NHits_->SetXTitle("# of Valid Hits"); 
   tk_NHits_->SetYTitle("Entries");

   tk_NHits_HP_= fs_->make<TH1F>("tk_NHits_HP","tk_NHits_HP",60,0,60);
   tk_NHits_HP_->SetXTitle("# of Valid Hits");
   tk_NHits_HP_->SetYTitle("Entries");
   

   tk_Algo_= fs_->make<TH1F>("tk_Algo","tk_Algo",30,0,30);
   tk_Algo_->SetXTitle("Algo");
   tk_Algo_->SetYTitle("Entries");

   tk_Algo_HP_= fs_->make<TH1F>("tk_Algo_HP","tk_Algo_HP",30,0,30);
   tk_Algo_HP_->SetXTitle("Algo");
   tk_Algo_HP_->SetYTitle("Entries");

   
   tk_dzOverDzErr_HP_= fs_->make<TH1F>("tk_dzOverDzErr_HP","tk_dzOverDzErr_HP",100,-50,50);
   tk_dzOverDzErr_HP_->SetXTitle("tracks dZ/dZerror");
   tk_dzOverDzErr_HP_->SetYTitle("Entries"); 

   tk_dxyOverDxyErr_HP_= fs_->make<TH1F>("tk_dxyOverDxyErr_HP","tk_dxyOverDxyErr_HP",100,-50,50);
   tk_dxyOverDxyErr_HP_->SetXTitle("tracks dXY/dXYerror");
   tk_dxyOverDxyErr_HP_->SetYTitle("Entries");

   tk_dptOverPt_HP_= fs_->make<TH1F>("tk_dptOverPt_HP","tk_dptOverPt_HP",100,0,1);
   tk_dptOverPt_HP_->SetXTitle("tracks pTerror/pT");
   tk_dptOverPt_HP_->SetYTitle("Entries");
    
   tk_normChi2_HP_= fs_->make<TH1F>("tk_normChi2_HP","tk_normChi2_HP",100,0,100);
   tk_normChi2_HP_->SetXTitle("Normalized #Chi^{2}");
   tk_normChi2_HP_->SetYTitle("Entries");


   //2D histos 
   //std::vector<double> x_centrality_bin={0.0, 10.0, 20.0, 60.0, 120.0, 160.0, 200.0};
   //tk_centrality_eta_= fs_->make<TH2F>("tk_centrality_eta","tk_centrality_eta",x_centrality_bin.size()-1,&x_centrality_bin[0],100,-2.4,2.4);
   tk_centrality_eta_= fs_->make<TH2F>("tk_centrality_eta","tk_centrality_eta",20,0,200,100,-2.4,2.4);
   tk_centrality_eta_->SetXTitle("Centrality");
   tk_centrality_eta_->SetYTitle("track #eta"); 

   //tk_centrality_pt_= fs_->make<TH2F>("tk_centrality_pt","tk_centrality_pt",x_centrality_bin.size()-1,&x_centrality_bin[0],100,0,5);
   tk_centrality_pt_= fs_->make<TH2F>("tk_centrality_pt","tk_centrality_pt",20,0,200,200,0,5);
   tk_centrality_pt_->SetXTitle("Centrality");
   tk_centrality_pt_->SetYTitle("track p_{T}"); 

   //std::vector<double> x_eta_bin={-2.4, -2.0, -0.8, 0.0, 0.8, 2.0, 2.4};
   //tk_eta_pt_= fs_->make<TH2F>("tk_eta_pt","tk_eta_pt",x_eta_bin.size()-1,&x_eta_bin[0],100,0,5);
   tk_eta_pt_= fs_->make<TH2F>("tk_eta_pt","tk_eta_pt",12,-2.4,2.4,200,0,5);
   tk_eta_pt_->SetXTitle("track #eta");
   tk_eta_pt_->SetYTitle("track p_{T}");

   tk_centrality_eta_HP_= fs_->make<TH2F>("tk_centrality_eta_HP","tk_centrality_eta_HP",20,0,200,100,-2.4,2.4);
   tk_centrality_eta_HP_->SetXTitle("Centrality");
   tk_centrality_eta_HP_->SetYTitle("track #eta"); 

   tk_centrality_pt_HP_= fs_->make<TH2F>("tk_centrality_pt_HP","tk_centrality_pt_HP",20,0,200,200,0,5);
   tk_centrality_pt_HP_->SetXTitle("Centrality");
   tk_centrality_pt_HP_->SetYTitle("track p_{T}"); 

   tk_eta_pt_HP_= fs_->make<TH2F>("tk_eta_pt_HP","tk_eta_pt_HP",12,-2.4,2.4,200,0,5);
   tk_eta_pt_HP_->SetXTitle("track #eta");
   tk_eta_pt_HP_->SetYTitle("track p_{T}");



  
   //3D histos
   tk_eta_centrality_dzOverDzErr_= fs_->make<TH3F>("tk_eta_centrality_dzOverDzErr","tk_eta_centrality_dzOverDzErr",12,-2.4,2.4,20,0,200,100,-50,50);
   tk_eta_centrality_dzOverDzErr_->SetXTitle("track #eta");
   tk_eta_centrality_dzOverDzErr_->SetYTitle("Centrality"); 
   tk_eta_centrality_dzOverDzErr_->SetZTitle("tracks dZ/dZerror");   
  
   tk_eta_centrality_dxyOverDxyErr_= fs_->make<TH3F>("tk_eta_centrality_dxyOverDxyErr","tk_eta_centrality_dxyOverDxyErr",12,-2.4,2.4,20,0,200,100,-50,50);
   tk_eta_centrality_dxyOverDxyErr_->SetXTitle("track #eta");
   tk_eta_centrality_dxyOverDxyErr_->SetYTitle("Centrality");
   tk_eta_centrality_dxyOverDxyErr_->SetZTitle("tracks dXY/dXYerror");

   tk_eta_centrality_dptOverPt_= fs_->make<TH3F>("tk_eta_centrality_dptOverPt","tk_eta_centrality_dptOverPt",12,-2.4,2.4,20,0,200,100,0,1);
   tk_eta_centrality_dptOverPt_->SetXTitle("track #eta");
   tk_eta_centrality_dptOverPt_->SetYTitle("Centrality");
   tk_eta_centrality_dptOverPt_->SetZTitle("tracks pTerror/pT");  

   tk_eta_centrality_normChi2_= fs_->make<TH3F>("tk_eta_centrality_normChi2","tk_eta_centrality_normChi2",12,-2.4,2.4,20,0,200,100,0,100);
   tk_eta_centrality_normChi2_->SetXTitle("track #eta");
   tk_eta_centrality_normChi2_->SetYTitle("Centrality");
   tk_eta_centrality_normChi2_->SetZTitle("Normalized #Chi^{2}");   

   tk_eta_centrality_NHits_= fs_->make<TH3F>("tk_eta_centrality_NHits","tk_eta_centrality_NHits",12,-2.4,2.4,20,0,200,60,0,60);
   tk_eta_centrality_NHits_->SetXTitle("track #eta");
   tk_eta_centrality_NHits_->SetYTitle("Centrality");
   tk_eta_centrality_NHits_->SetZTitle("# of Valid Hits");


   tk_eta_centrality_dzOverDzErr_HP_= fs_->make<TH3F>("tk_eta_centrality_dzOverDzErr_HP","tk_eta_centrality_dzOverDzErr_HP",12,-2.4,2.4,20,0,200,100,-50,50);
   tk_eta_centrality_dzOverDzErr_HP_->SetXTitle("track #eta");
   tk_eta_centrality_dzOverDzErr_HP_->SetYTitle("Centrality"); 
   tk_eta_centrality_dzOverDzErr_HP_->SetZTitle("tracks dZ/dZerror");   
  
   tk_eta_centrality_dxyOverDxyErr_HP_= fs_->make<TH3F>("tk_eta_centrality_dxyOverDxyErr_HP","tk_eta_centrality_dxyOverDxyErr_HP",12,-2.4,2.4,20,0,200,100,-50,50);
   tk_eta_centrality_dxyOverDxyErr_HP_->SetXTitle("track #eta");
   tk_eta_centrality_dxyOverDxyErr_HP_->SetYTitle("Centrality");
   tk_eta_centrality_dxyOverDxyErr_HP_->SetZTitle("tracks dXY/dXYerror");

   tk_eta_centrality_dptOverPt_HP_= fs_->make<TH3F>("tk_eta_centrality_dptOverPt_HP","tk_eta_centrality_dptOverPt_HP",12,-2.4,2.4,20,0,200,100,0,1);
   tk_eta_centrality_dptOverPt_HP_->SetXTitle("track #eta");
   tk_eta_centrality_dptOverPt_HP_->SetYTitle("Centrality");
   tk_eta_centrality_dptOverPt_HP_->SetZTitle("tracks pTerror/pT");  

   tk_eta_centrality_normChi2_HP_= fs_->make<TH3F>("tk_eta_centrality_normChi2_HP","tk_eta_centrality_normChi2_HP",12,-2.4,2.4,20,0,200,100,0,100);
   tk_eta_centrality_normChi2_HP_->SetXTitle("track #eta");
   tk_eta_centrality_normChi2_HP_->SetYTitle("Centrality");
   tk_eta_centrality_normChi2_HP_->SetZTitle("Normalized #Chi^{2}");   

   tk_eta_centrality_NHits_HP_= fs_->make<TH3F>("tk_eta_centrality_NHits_HP","tk_eta_centrality_NHits_HP",12,-2.4,2.4,20,0,200,60,0,60);
   tk_eta_centrality_NHits_HP_->SetXTitle("track #eta");
   tk_eta_centrality_NHits_HP_->SetYTitle("Centrality");
   tk_eta_centrality_NHits_HP_->SetZTitle("# of Valid Hits");




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

   //centrality information
   edm::Handle<int> centralityBin;
   iEvent.getByToken(centralitySrc_, centralityBin);
   int cbin = *centralityBin;
   //std::cout<<"cbin  :  "<<cbin<<std::endl;

   //vertex information
   edm::Handle<std::vector<reco::Vertex> > vertexCollection;
   iEvent.getByToken(vertexSrc_,vertexCollection);
   std::vector<reco::Vertex> vtx_sorted = *vertexCollection;
   std::sort( vtx_sorted.begin(), vtx_sorted.end(), TrackingAnalyzer::vtxSort );
   if(vtx_sorted.size() == 0) return;
   double vtx_x, vtx_y, vtx_z, vtx_xError, vtx_yError, vtx_zError; 
   vtx_x = (double)vtx_sorted.begin()->position().x(); 
   vtx_y = (double)vtx_sorted.begin()->position().y(); 
   vtx_z = (double)vtx_sorted.begin()->position().z(); 
   vtx_xError = (double)vtx_sorted.begin()->xError();
   vtx_yError = (double)vtx_sorted.begin()->yError();
   vtx_zError = (double)vtx_sorted.begin()->zError();
   /*std::cout<<" vtx_x : "<<vtx_x
            <<" vtx_y : "<<vtx_y
            <<" vtx_z : "<<vtx_z
            <<std::endl; 
   std::cout<<" vtx_err_x : "<<vtx_xError
            <<" vtx_err_y : "<<vtx_yError
            <<" vtx_err_z : "<<vtx_zError
            <<std::endl; 
   */
   ///if(fabs(vtx_sorted.begin()->position().z()) > 15.0) return; //default 
   math::XYZPoint vtx(vtx_x,vtx_y,vtx_z);  


   //track collections
   edm::Handle<reco::TrackCollection> tracks;
   if(useGeneralTracks_){iEvent.getByToken(generalTracksSrc_,tracks);}
   else{ //use pixel tracks
     iEvent.getByToken(pixelTracksSrc_,tracks);
   } 

   for(reco::TrackCollection::const_iterator iter_tk = tracks->begin(); iter_tk != tracks->end(); iter_tk++){

   	tk_Pt_->Fill(iter_tk->pt());	
        tk_Eta_->Fill(iter_tk->eta()); 
        tk_Phi_->Fill(iter_tk->phi());
        tk_NHits_->Fill(iter_tk->numberOfValidHits());
        tk_Algo_->Fill(iter_tk->algo());  
        double aux_tk_dz_vtx = (double)iter_tk->dz(vtx);
        double aux_tk_dzError_vtx  = (double)sqrt(iter_tk->dzError()*iter_tk->dzError()+vtx_zError*vtx_zError);
        double aux_tk_dxy_vtx = (double)iter_tk->dxy(vtx);
        double aux_tk_dxyError_vtx  = (double)sqrt(iter_tk->dxyError()*iter_tk->dxyError()+vtx_xError*vtx_yError);
        tk_dzOverDzErr_->Fill(aux_tk_dz_vtx/aux_tk_dzError_vtx);
        tk_dxyOverDxyErr_->Fill(aux_tk_dxy_vtx/aux_tk_dxyError_vtx);
        tk_dptOverPt_->Fill(fabs(iter_tk->ptError())/iter_tk->pt());
        tk_normChi2_->Fill(iter_tk->normalizedChi2());
         //fill 2d histos
        tk_centrality_eta_->Fill((Double_t)cbin,iter_tk->eta()); 
        tk_centrality_pt_->Fill((Double_t)cbin,iter_tk->pt()); 
        tk_eta_pt_->Fill(iter_tk->eta(),iter_tk->pt());
         //fill 3d histos
        tk_eta_centrality_dzOverDzErr_->Fill(iter_tk->eta(),(Double_t)cbin,aux_tk_dz_vtx/aux_tk_dzError_vtx);
        tk_eta_centrality_dxyOverDxyErr_->Fill(iter_tk->eta(),(Double_t)cbin,aux_tk_dxy_vtx/aux_tk_dxyError_vtx);
        tk_eta_centrality_dptOverPt_->Fill(iter_tk->eta(),(Double_t)cbin,fabs(iter_tk->ptError())/iter_tk->pt());
        tk_eta_centrality_normChi2_->Fill(iter_tk->eta(),(Double_t)cbin,iter_tk->normalizedChi2());
        tk_eta_centrality_NHits_->Fill(iter_tk->eta(),(Double_t)cbin,iter_tk->numberOfValidHits());
         //selection 
        if(useGeneralTracks_){
          if(!iter_tk->quality(reco::TrackBase::highPurity) || fabs(iter_tk->ptError())/iter_tk->pt()>0.1 || fabs(aux_tk_dz_vtx/aux_tk_dzError_vtx)>3 
                                                            || fabs(aux_tk_dxy_vtx/aux_tk_dxyError_vtx)>3)continue; 
        }
        else{
          if(fabs(aux_tk_dz_vtx/aux_tk_dzError_vtx)>20 || fabs(aux_tk_dxy_vtx/aux_tk_dxyError_vtx)>7)continue; 
        } 
        tk_Pt_HP_->Fill(iter_tk->pt());       
        tk_Eta_HP_->Fill(iter_tk->eta());
        tk_Phi_HP_->Fill(iter_tk->phi());
        tk_NHits_HP_->Fill(iter_tk->numberOfValidHits());
        tk_Algo_HP_->Fill(iter_tk->algo()); 
        tk_dzOverDzErr_HP_->Fill(aux_tk_dz_vtx/aux_tk_dzError_vtx);
        tk_dxyOverDxyErr_HP_->Fill(aux_tk_dxy_vtx/aux_tk_dxyError_vtx);
        tk_dptOverPt_HP_->Fill(fabs(iter_tk->ptError())/iter_tk->pt());
        tk_normChi2_HP_->Fill(iter_tk->normalizedChi2());  
         //fill 2d histos
        tk_centrality_eta_HP_->Fill((Double_t)cbin,iter_tk->eta()); 
        tk_centrality_pt_HP_->Fill((Double_t)cbin,iter_tk->pt()); 
        tk_eta_pt_HP_->Fill(iter_tk->eta(),iter_tk->pt());
         //fill 3d histos
        tk_eta_centrality_dzOverDzErr_HP_->Fill(iter_tk->eta(),(Double_t)cbin,aux_tk_dz_vtx/aux_tk_dzError_vtx);
        tk_eta_centrality_dxyOverDxyErr_HP_->Fill(iter_tk->eta(),(Double_t)cbin,aux_tk_dxy_vtx/aux_tk_dxyError_vtx);
        tk_eta_centrality_dptOverPt_HP_->Fill(iter_tk->eta(),(Double_t)cbin,fabs(iter_tk->ptError())/iter_tk->pt());
        tk_eta_centrality_normChi2_HP_->Fill(iter_tk->eta(),(Double_t)cbin,iter_tk->normalizedChi2());
        tk_eta_centrality_NHits_HP_->Fill(iter_tk->eta(),(Double_t)cbin,iter_tk->numberOfValidHits());

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

bool 
TrackingAnalyzer::vtxSort( const reco::Vertex &  a, const reco::Vertex & b ){
   if( a.tracksSize() != b.tracksSize() ) 
      return  a.tracksSize() > b.tracksSize() ? true : false ;
   else
      return  a.chi2() < b.chi2() ? true : false ;
} 

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TrackingAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  //desc.setUnknown();
  desc.add<edm::InputTag>("centralitySrc",edm::InputTag("centralityBin","HFtowers"));
  desc.add<edm::InputTag>("vertexSrc",edm::InputTag("offlinePrimaryVerticesRecovery"));
  desc.add<edm::InputTag>("generalTracksSrc",edm::InputTag("generalTracks"));
  desc.add<edm::InputTag>("pixelTracksSrc",edm::InputTag("hiConformalPixelTracks"));
  desc.add<bool>("useGeneralTracks",true);
  descriptions.add("trackingAnalyzer", desc); 
  //descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TrackingAnalyzer);
