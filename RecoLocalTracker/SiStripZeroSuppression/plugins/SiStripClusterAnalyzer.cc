// system include files
#include <memory>
#include <iostream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/DetSet.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"

#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"
#include "DataFormats/SiStripCluster/interface/SiStripClusterCollection.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"


//ROOT inclusion
#include "TH1D.h"
#include "TMath.h"

//
// class decleration
//

class SiStripClusterAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit SiStripClusterAnalyzer(const edm::ParameterSet&);
      ~SiStripClusterAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
 

   private:
      virtual void beginJob() override ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override ;
      

      edm::EDGetTokenT<edmNew::DetSetVector<SiStripCluster> > srcClusters_;
      
      edm::Service<TFileService> fs_;

      TH1D* h1ClusterMult_; 
      TH1D* h1ClusterCharge_;
      TH1D* h1ClusterWidth_; 
      TH1D* h1ClusterMean_; 
      TH1D* h1ClusterSigma_; 
	  
};


SiStripClusterAnalyzer::SiStripClusterAnalyzer(const edm::ParameterSet& conf){
  
  //now do what ever initialization is needed
  usesResource("TFileService");
 
  srcClusters_ = consumes<edmNew::DetSetVector<SiStripCluster>>(conf.getParameter<edm::InputTag>("srcClusters")); 

  h1ClusterMult_ = fs_->make<TH1D>("ClusterMult","Cluster Multiplicity;nClusters;nEvents", 100, 0, 500000);
  h1ClusterCharge_ = fs_->make<TH1D>("ClusterCharge","Cluster Charge;Cluster Charge;nCluster", 100, 0, 5000);
  h1ClusterWidth_ = fs_->make<TH1D>("ClusterWidth","Cluster Width;Cluster Width;nCluster", 128, 0, 128);
  h1ClusterMean_ = fs_->make<TH1D>("ClusterMean","Cluster Mean;Cluster Mean;nCluster", 128, 0, 128);
  h1ClusterSigma_ = fs_->make<TH1D>("ClusterSigma","Cluster Sigma;Cluster Sigma;nCluster", 60, 0, 50);

}


SiStripClusterAnalyzer::~SiStripClusterAnalyzer()
{

}

void
SiStripClusterAnalyzer::analyze(const edm::Event& e, const edm::EventSetup& es)
{

   using namespace edm;

   edm::Handle<edmNew::DetSetVector<SiStripCluster> > clusters;
   e.getByToken(srcClusters_,clusters);

   
   int nclust = 0;
   edmNew::DetSetVector<SiStripCluster>::const_iterator itClusters = clusters->begin();
   for ( ; itClusters != clusters->end(); ++itClusters ){

         for ( edmNew::DetSet<SiStripCluster>::const_iterator clus =	itClusters->begin(); clus != itClusters->end(); ++clus){
                             
           //cluster plots from here on
           nclust++;
           int strip2=0;
           double charge = 0;
           double mean = 0;
           double sigma = 0;
           for( auto itAmpl = clus->amplitudes().begin(); itAmpl != clus->amplitudes().end(); ++itAmpl){
             charge += *itAmpl;
             ++strip2;
             mean += strip2*(*itAmpl);
             sigma += strip2*strip2*(*itAmpl);
           }
           h1ClusterCharge_->Fill(charge);
           h1ClusterWidth_->Fill(strip2);
           mean = mean/charge;
           h1ClusterMean_->Fill(mean);
           sigma = TMath::Power((sigma/charge-mean*mean),0.5);
           h1ClusterSigma_->Fill(sigma);

         }

   }
   h1ClusterMult_->Fill(nclust); 

}

// ------------ method called once each job just before starting event loop  ------------
void SiStripClusterAnalyzer::beginJob()
{
 
}

// ------------ method called once each job just after ending the event loop  ------------
void 
SiStripClusterAnalyzer::endJob() {
     
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
SiStripClusterAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("srcClusters",edm::InputTag("SiStripCluster"));
  descriptions.add("siStripClusterAnalyzer", desc); 
}

//define this as a plug-in
DEFINE_FWK_MODULE(SiStripClusterAnalyzer);

