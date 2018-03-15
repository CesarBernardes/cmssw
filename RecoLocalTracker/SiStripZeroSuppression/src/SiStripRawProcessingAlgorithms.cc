#include "RecoLocalTracker/SiStripZeroSuppression/interface/SiStripRawProcessingAlgorithms.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/SiStripDigi/interface/SiStripDigi.h"
#include "DataFormats/SiStripDigi/interface/SiStripRawDigi.h"
#include "RecoLocalTracker/SiStripZeroSuppression/interface/SiStripRawProcessingFactory.h"
#include <memory>


SiStripRawProcessingAlgorithms::SiStripRawProcessingAlgorithms(std::auto_ptr<SiStripPedestalsSubtractor> ped,
				 std::auto_ptr<SiStripCommonModeNoiseSubtractor> cmn,
				 std::auto_ptr<SiStripFedZeroSuppression> zs,
				 std::auto_ptr<SiStripAPVRestorer> res,
				 bool doAPVRest,
				 bool useCMMap)
    :  subtractorPed(ped),
       subtractorCMN(cmn),
       suppressor(zs),
       restorer(res),
       doAPVRestore(doAPVRest),
       useCMMeanMap(useCMMap)
    {}


void SiStripRawProcessingAlgorithms::initialize(const edm::EventSetup& es) {
    subtractorPed->init(es);
    subtractorCMN->init(es);
    suppressor->init(es);
    if(restorer.get()) restorer->init(es);
} 

void SiStripRawProcessingAlgorithms::initialize(const edm::EventSetup& es, const edm::Event& e){
  this->initialize(es);
  if(restorer.get()&&doAPVRestore&&useCMMeanMap) restorer->LoadMeanCMMap(e);
  
}

//Suppressors 
//--------------------------------------------------------
int16_t SiStripRawProcessingAlgorithms::SuppressVirginRawData(const uint32_t& id, const uint16_t& firstAPV, std::vector<int16_t>& processedRawDigis , edm::DetSet<SiStripDigi>& suppressedDigis ){
      
      subtractorPed->subtract( id, firstAPV*128,processedRawDigis);
      return this->SuppressProcessedRawData(id, firstAPV, processedRawDigis , suppressedDigis );
 
}

int16_t SiStripRawProcessingAlgorithms::SuppressVirginRawData(const edm::DetSet<SiStripRawDigi>& rawDigis, edm::DetSet<SiStripDigi>& suppressedDigis){
   
   std::vector<int16_t> RawDigis;
   RawDigis.clear();
   edm::DetSet<SiStripRawDigi>::const_iterator itrawDigis = rawDigis.begin();
   for(; itrawDigis != rawDigis.end(); ++itrawDigis) RawDigis.push_back(itrawDigis->adc());
   return this->SuppressVirginRawData(rawDigis.id, 0,RawDigis , suppressedDigis);
}
  



int16_t SiStripRawProcessingAlgorithms::SuppressProcessedRawData(const uint32_t& id, const uint16_t& firstAPV, std::vector<int16_t>& processedRawDigis , edm::DetSet<SiStripDigi>& suppressedDigis ){
      std::vector<int16_t>  processedRawDigisPedSubtracted ;
      
      int16_t nAPVFlagged =0;
      if( doAPVRestore ) processedRawDigisPedSubtracted.assign(processedRawDigis.begin(), processedRawDigis.end());
      subtractorCMN->subtract(id, firstAPV,  processedRawDigis);
      if( doAPVRestore ) nAPVFlagged = restorer->InspectAndRestore(id, firstAPV, processedRawDigisPedSubtracted, processedRawDigis, subtractorCMN->getAPVsCM() );
      suppressor->suppress( processedRawDigis, firstAPV,  suppressedDigis ); 
      return nAPVFlagged;
}


int16_t SiStripRawProcessingAlgorithms::SuppressProcessedRawData(const edm::DetSet<SiStripRawDigi>& rawDigis, edm::DetSet<SiStripDigi>& suppressedDigis){
   std::vector<int16_t> RawDigis;
   RawDigis.clear();
   edm::DetSet<SiStripRawDigi>::const_iterator itrawDigis = rawDigis.begin();
   for(; itrawDigis != rawDigis.end(); ++itrawDigis) RawDigis.push_back(itrawDigis->adc());
   return this->SuppressProcessedRawData(rawDigis.id, 0, RawDigis , suppressedDigis );
}


//Convert to Hybrid Format 
//--------------------------------------------------------

int16_t SiStripRawProcessingAlgorithms::ConvertVirginRawToHybrid(const uint32_t& id, const uint16_t& firstAPV, std::vector<int16_t>& processedRawDigis , edm::DetSet<SiStripDigi>& suppressedDigis ){
      
      std::vector<int16_t>  processedRawDigisPedSubtracted;
      std::vector<bool> markedVRAPVs;
     
      for(uint16_t strip=0; strip < processedRawDigis.size(); ++strip) processedRawDigis[strip] += 1024;

      
      subtractorPed->subtract( id, firstAPV*128,processedRawDigis);      
      for(uint16_t strip=0; strip < processedRawDigis.size(); ++strip) processedRawDigis[strip] /= 2;
      
      processedRawDigisPedSubtracted.assign(processedRawDigis.begin(), processedRawDigis.end());
      
      subtractorCMN->subtract(id, firstAPV,  processedRawDigis);      
      int16_t nAPVFlagged = restorer->InspectForHybridFormatEmulation(id, firstAPV, processedRawDigis, subtractorCMN->getAPVsCM(), markedVRAPVs);
      
      for(uint16_t strip=0; strip < processedRawDigis.size(); ++strip){
      	 processedRawDigis[strip] *= 2;
         processedRawDigisPedSubtracted[strip] *= 2;
      }
      
      for(uint16_t APV=firstAPV ; APV< processedRawDigis.size()/128 + firstAPV; ++APV){
      	if(markedVRAPVs[APV]){
      		//GB 23/6/08: truncation should be done at the very beginning
      		for(uint16_t strip = (APV-firstAPV)*128; strip < (APV-firstAPV+1)*128; ++strip) suppressedDigis.push_back(SiStripDigi(strip, ( processedRawDigisPedSubtracted[strip] ? 0 : suppressor->truncate(processedRawDigisPedSubtracted[strip]) )));
      	}else{
      		std::vector<int16_t> singleAPVdigi;
  		 	singleAPVdigi.clear();
  		 	for(int16_t strip = (APV-firstAPV)*128; strip < (APV-firstAPV+1)*128; ++strip) singleAPVdigi.push_back(processedRawDigis[strip]); 
      	  	suppressor->suppress(singleAPVdigi, APV,  suppressedDigis );
      	}
      }
       
      
     
      return nAPVFlagged;

 
}

int16_t SiStripRawProcessingAlgorithms::ConvertVirginRawToHybrid(const edm::DetSet<SiStripRawDigi>& rawDigis, edm::DetSet<SiStripDigi>& suppressedDigis){
   
   std::vector<int16_t> RawDigis;
   RawDigis.clear();
   edm::DetSet<SiStripRawDigi>::const_iterator itrawDigis = rawDigis.begin();
   for(; itrawDigis != rawDigis.end(); ++itrawDigis) RawDigis.push_back(itrawDigis->adc());
   return this->ConvertVirginRawToHybrid(rawDigis.id, 0,RawDigis , suppressedDigis);
}
  



