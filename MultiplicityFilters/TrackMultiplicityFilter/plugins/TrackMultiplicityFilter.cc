// -*- C++ -*-
//
// Package:    MultiplicityFilters/TrackMultiplicityFilter
// Class:      TrackMultiplicityFilter
// 
/**\class TrackMultiplicityFilter TrackMultiplicityFilter.cc MultiplicityFilters/TrackMultiplicityFilter/plugins/TrackMultiplicityFilter.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Cesar Bernardes
//         Created:  Fri, 15 May 2020 18:47:02 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

//
// class declaration
//

class TrackMultiplicityFilter : public edm::stream::EDFilter<> {
   public:
      explicit TrackMultiplicityFilter(const edm::ParameterSet&);
      ~TrackMultiplicityFilter();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginStream(edm::StreamID) override;
      virtual bool filter(edm::Event&, const edm::EventSetup&) override;
      virtual void endStream() override;

      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------
      edm::EDGetTokenT<reco::VertexCollection > vertexToken_;
      edm::EDGetTokenT<reco::TrackCollection> trackToken_;
      int nTrackMinCut_;
      int nTrackMaxCut_; 

      static bool vtxSort( const reco::Vertex &  a, const reco::Vertex & b ); 
      static int ntrkoff( edm::Handle<std::vector<reco::Track> > generalTracks, double vtx_x, double vtx_y, double vtx_z, double vtx_xError,double vtx_yError,double vtx_zError );


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
TrackMultiplicityFilter::TrackMultiplicityFilter(const edm::ParameterSet& iConfig):
  vertexToken_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertexCollection"))),
  trackToken_(consumes<reco::TrackCollection>(iConfig.getParameter<edm::InputTag>("generalTrackTag"))),
  nTrackMinCut_(iConfig.getParameter<int>("nTrackMinCut")),
  nTrackMaxCut_(iConfig.getParameter<int>("nTrackMaxCut"))
{
   //now do what ever initialization is needed

}


TrackMultiplicityFilter::~TrackMultiplicityFilter()
{
 
   // do anything here that needs to be done at destruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
TrackMultiplicityFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  bool result = false;
  
  //vertex information
  edm::Handle<std::vector<reco::Vertex> > vertexCollection;
  iEvent.getByToken(vertexToken_,vertexCollection);
  std::vector<reco::Vertex> vtx_sorted = *vertexCollection;
  std::sort( vtx_sorted.begin(), vtx_sorted.end(), TrackMultiplicityFilter::vtxSort );
  if(vtx_sorted.size() == 0) return result; //if no good vertex, filter event  
  double vtx_x = (double)vtx_sorted.begin()->position().x();
  double vtx_y = (double)vtx_sorted.begin()->position().y();
  double vtx_z = (double)vtx_sorted.begin()->position().z();
  double vtx_xError = (double)vtx_sorted.begin()->xError();
  double vtx_yError = (double)vtx_sorted.begin()->yError();
  double vtx_zError = (double)vtx_sorted.begin()->zError();
 
  //track information 
  edm::Handle<reco::TrackCollection> generalTracks;
  iEvent.getByToken(trackToken_,generalTracks);

  //compute track multiplicity 
  int aux_N_tk_offline = ntrkoff(generalTracks,vtx_x,vtx_y,vtx_z,vtx_xError,vtx_yError,vtx_zError);

  if( nTrackMinCut_ < aux_N_tk_offline && aux_N_tk_offline < nTrackMaxCut_ ) result=true; 


  return result; 

}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
TrackMultiplicityFilter::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
TrackMultiplicityFilter::endStream() {
}

 
int 
TrackMultiplicityFilter::ntrkoff(edm::Handle<std::vector<reco::Track> > generalTracks,double vtx_x,double vtx_y,double vtx_z,double vtx_xError,double vtx_yError,double vtx_zError){
   math::XYZPoint vtx(vtx_x,vtx_y,vtx_z);
  int aux_ntrkoffline=0;
   for(reco::TrackCollection::const_iterator iter_tk = generalTracks->begin(); iter_tk != generalTracks->end(); iter_tk++){
      double aux_tk_dz_vtx = (double)iter_tk->dz(vtx);
      double aux_tk_dzError_vtx  = (double)sqrt(iter_tk->dzError()*iter_tk->dzError()+vtx_zError*vtx_zError);
      double aux_tk_dxy_vtx = (double)iter_tk->dxy(vtx);
      double aux_tk_dxyError_vtx  = (double)sqrt(iter_tk->dxyError()*iter_tk->dxyError()+vtx_xError*vtx_yError);
      if(iter_tk->pt()<=0.4)continue;
      if(fabs(iter_tk->eta())>2.4)continue;
      if(!iter_tk->quality(reco::TrackBase::highPurity))continue;
      if(fabs(iter_tk->ptError())/iter_tk->pt()>0.1)continue;
      if(fabs(aux_tk_dz_vtx/aux_tk_dzError_vtx)>3)continue;
      if(fabs(aux_tk_dxy_vtx/aux_tk_dxyError_vtx)>3)continue;
      aux_ntrkoffline++;
   }
   return aux_ntrkoffline;
}




bool
TrackMultiplicityFilter::vtxSort( const reco::Vertex &  a, const reco::Vertex & b ){
   if( a.tracksSize() != b.tracksSize() ) 
      return  a.tracksSize() > b.tracksSize() ? true : false ;
   else
      return  a.chi2() < b.chi2() ? true : false ;
} 

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TrackMultiplicityFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("vertexCollection",edm::InputTag("generalTracks"));
  desc.add<edm::InputTag>("generalTrackTag",edm::InputTag("offlinePrimaryVertices"));
  desc.add<int>("nTrackMinCut",int(0));
  desc.add<int>("nTrackMaxCut",int(100000));
  //desc.setUnknown();
  //descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(TrackMultiplicityFilter);
