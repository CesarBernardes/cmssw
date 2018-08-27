import FWCore.ParameterSet.Config as cms

# This object is used to make changes for different running scenarios. In
# this case for Run 2

from EventFilter.SiPixelRawToDigi.SiPixelDigiToRaw_cfi import *
from EventFilter.SiStripRawToDigi.SiStripDigiToRaw_cfi import *
from SimCalorimetry.EcalTrigPrimProducers.ecalTriggerPrimitiveDigis_cff import *
import EventFilter.EcalDigiToRaw.ecalDigiToRaw_cfi
ecalPacker = EventFilter.EcalDigiToRaw.ecalDigiToRaw_cfi.ecaldigitorawzerosup.clone()
from EventFilter.ESDigiToRaw.esDigiToRaw_cfi import *
from EventFilter.HcalRawToDigi.HcalDigiToRaw_cfi import *
from EventFilter.CSCRawToDigi.cscPacker_cfi import *
from EventFilter.DTRawToDigi.dtPacker_cfi import *
from EventFilter.RPCRawToDigi.rpcPacker_cfi import *
from EventFilter.GEMRawToDigi.gemPacker_cfi import *
from EventFilter.CastorRawToDigi.CastorDigiToRaw_cfi import *
from EventFilter.RawDataCollector.rawDataCollector_cfi import *
from L1Trigger.Configuration.L1TDigiToRaw_cff import *

from Configuration.ProcessModifiers.premix_stage1_cff import premix_stage1

#DigiToRaw = cms.Sequence(L1TDigiToRaw*siPixelRawData*SiStripDigiToRaw*ecalPacker*esDigiToRaw*hcalRawData*cscpacker*dtpacker*rpcpacker*rawDataCollector)
DigiToRaw = cms.Sequence(L1TDigiToRaw*siPixelRawData*SiStripDigiToRaw*ecalPacker*esDigiToRaw*hcalRawData*cscpacker*dtpacker*rpcpacker*castorRawData*rawDataCollector)
# no L1 DigiToRaw in first PreMixing step
premix_stage1.toReplaceWith(DigiToRaw, DigiToRaw.copyAndExclude([L1TDigiToRaw]))

ecalPacker.Label = 'simEcalDigis'
ecalPacker.InstanceEB = 'ebDigis'
ecalPacker.InstanceEE = 'eeDigis'
ecalPacker.labelEBSRFlags = "simEcalDigis:ebSrFlags"
ecalPacker.labelEESRFlags = "simEcalDigis:eeSrFlags"
premix_stage1.toModify(hcalRawDatauHTR, premix = True)

from Configuration.Eras.Modifier_run3_common_cff import run3_common
run3_common.toReplaceWith(DigiToRaw, DigiToRaw.copyAndExclude([castorRawData]))

#if we don't have hcal raw data
from Configuration.Eras.Modifier_hcalSkipPacker_cff import hcalSkipPacker
hcalSkipPacker.toReplaceWith(DigiToRaw, DigiToRaw.copyAndExclude([hcalRawData]))

# Remove siPixelRawData until we have phase1 pixel digis
from Configuration.Eras.Modifier_phase2_tracker_cff import phase2_tracker
phase2_tracker.toReplaceWith(DigiToRaw, DigiToRaw.copyAndExclude([siPixelRawData])) # FIXME

# GEM settings
_gem_DigiToRaw = DigiToRaw.copy()
_gem_DigiToRaw.insert(-2,gemPacker)

from Configuration.Eras.Modifier_run2_GEM_2017_cff import run2_GEM_2017
run2_GEM_2017.toReplaceWith(DigiToRaw, _gem_DigiToRaw)

from Configuration.Eras.Modifier_run3_GEM_cff import run3_GEM
run3_GEM.toReplaceWith(DigiToRaw, _gem_DigiToRaw)

from Configuration.Eras.Modifier_phase2_muon_cff import phase2_muon
phase2_muon.toReplaceWith(DigiToRaw, DigiToRaw.copyAndExclude([rpcpacker]))

from Configuration.Eras.Modifier_fastSim_cff import fastSim
fastSim.toReplaceWith(DigiToRaw, DigiToRaw.copyAndExclude([siPixelRawData,SiStripDigiToRaw,castorRawData]))


# for SiStrip Hybrid format data emulation 
# It runs on 2015 PbPb data in VR mode
from Configuration.Eras.Era_Run2_HI_cff import run2_HI_specific
def _modifyDigiToRawForRun2_HI( process ) :
   
   process.load('RecoLocalTracker/SiStripZeroSuppression/SiStripZeroSuppression_cfi')    
   process.siStripZeroSuppression.produceRawDigis = False
   process.siStripZeroSuppression.produceHybridFormat = True
   process.siStripZeroSuppression.Algorithms.APVInspectMode = 'HybridEmulation'
   process.siStripZeroSuppression.Algorithms.APVRestoreMode = ''
   process.siStripZeroSuppression.Algorithms.CommonModeNoiseSubtractionMode = 'Median'
   process.siStripZeroSuppression.Algorithms.MeanCM = 512
   process.siStripZeroSuppression.Algorithms.DeltaCMThreshold = 20
   process.siStripZeroSuppression.Algorithms.Use10bitsTruncation = True
   process.siStripZeroSuppression.RawDigiProducersList = cms.VInputTag(cms.InputTag("siStripDigis","VirginRaw"))

   process.siStripRepackZS = cms.EDProducer("SiStripDigiToRawModule",
      InputDigis       = cms.InputTag("siStripZeroSuppression", "VirginRaw"),
      FedReadoutMode = cms.string('Zero suppressed'),
      PacketCode = cms.string("ZERO_SUPPRESSED10"),
      UseFedKey = cms.bool(False),
      UseWrongDigiType = cms.bool(False),
      CopyBufferHeader = cms.bool(False),
      RawDataTag = cms.InputTag('rawDataCollector')
   )

   process.rawDataRepacker = cms.EDProducer("RawDataCollectorByLabel",
      verbose = cms.untracked.int32(0),     # 0 = quiet, 1 = collection list, 2 = FED list
      RawCollectionList = cms.VInputTag( 
                                      cms.InputTag('siStripRepackZS'),
                                      cms.InputTag('rawDataCollector')
      ),
   )

   process.DigiToRaw = cms.Sequence(process.siStripZeroSuppression+process.siStripRepackZS+process.rawDataRepacker)

modifyConfigurationStandardSequencesDigiToRawForRun2_HI_ = run2_HI_specific.makeProcessModifier( _modifyDigiToRawForRun2_HI )

