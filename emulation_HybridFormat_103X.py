import FWCore.ParameterSet.Config as cms

###process = cms.Process('TEST1')

from Configuration.StandardSequences.Eras import eras

process = cms.Process('TestSiStripPacker', eras.Run2_HI)

# import of standard configurations
###process.load('Configuration.StandardSequences.Services_cff')
###process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
###process.load('FWCore.MessageService.MessageLogger_cfi')
###process.load('Configuration.StandardSequences.GeometryDB_cff')
###process.load('Configuration.StandardSequences.MagneticField_38T_cff')
###process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
###process.load('Configuration.StandardSequences.L1Reco_cff')
###process.load('Configuration.StandardSequences.ReconstructionHeavyIons_cff')
###process.load('Configuration.StandardSequences.EndOfProcess_cff')
###process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
###process.load('Configuration.EventContent.EventContentHeavyIons_cff')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')


process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(20)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
    '/store/hidata/HIRun2015/HITrackerVirginRaw/RAW/v1/000/263/400/00000/40322926-4AA3-E511-95F7-02163E0146A8.root'
   )
)

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data', '')


# Output definition
process.RECOoutput = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
    fileName = cms.untracked.string('file:emulation_hybridFormat.root'),
    dataset = cms.untracked.PSet(
        filterName = cms.untracked.string(''),
        dataTier = cms.untracked.string('GEN-SIM-RECO')
    ),
    outputCommands = cms.untracked.vstring('keep FEDRawDataCollection_myRawDataCollector_*_*') 
)


from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data', '')

process.load('EventFilter.SiStripRawToDigi.SiStripDigis_cfi')
process.load("RecoLocalTracker.SiStripZeroSuppression.SiStripZeroSuppression_cfi")

from EventFilter.SiStripRawToDigi.SiStripDigis_cfi import siStripDigis
from RecoLocalTracker.SiStripZeroSuppression.SiStripZeroSuppression_cfi import siStripZeroSuppression

## Offline Silicon Tracker Zero Suppression
from RecoLocalTracker.SiStripZeroSuppression.DefaultAlgorithms_cff import *
siStripZeroSuppression.produceRawDigis = cms.bool(False)
siStripZeroSuppression.produceCalculatedBaseline = cms.bool(False)
siStripZeroSuppression.produceBaselinePoints = cms.bool(False)
siStripZeroSuppression.storeCM = cms.bool(True)
siStripZeroSuppression.produceHybridFormat = cms.bool(True)
siStripZeroSuppression.Algorithms.APVInspectMode = 'HybridEmulation' ###from Pieter
siStripZeroSuppression.Algorithms.APVRestoreMode = '' ###from cesar
siStripZeroSuppression.HybridInputDigis = cms.InputTag('','')
siStripZeroSuppression.Algorithms.CommonModeNoiseSubtractionMode = cms.string('Median')
siStripZeroSuppression.Algorithms.MeanCM = cms.int32(512)
siStripZeroSuppression.Algorithms.DeltaCMThreshold = cms.uint32(20)
siStripZeroSuppression.Algorithms.Use10bitsTruncation = cms.bool(True)     
siStripZeroSuppression.RawDigiProducersList = cms.VInputTag(cms.InputTag("siStripDigis","VirginRaw"))###Hybrid only run on VR




process.siStripRepackZS = cms.EDProducer("SiStripDigiToRawModule",
    InputDigis       = cms.InputTag("siStripZeroSuppression", "VirginRaw"),
    FedReadoutMode = cms.string('Zero suppressed'),
    PacketCode = cms.string("ZERO_SUPPRESSED10"),
    UseFedKey = cms.bool(False),
    UseWrongDigiType = cms.bool(False),
    CopyBufferHeader = cms.bool(False),
    RawDataTag = cms.InputTag('rawDataCollector')
)

process.myRawDataCollector = cms.EDProducer("RawDataCollectorByLabel",
    verbose = cms.untracked.int32(0),     # 0 = quiet, 1 = collection list, 2 = FED list
    RawCollectionList = cms.VInputTag( 
                                       cms.InputTag('siStripRepackZS'),
                                       cms.InputTag('rawDataCollector')
    ),
)

#this is just to check if is using the Hybrid Format properly
process.siStripUnpackRepackedZS = process.siStripDigis.clone(
        ProductLabel     = cms.InputTag('siStripRepackZS'),
        )

process.TFileService = cms.Service("TFileService",
        fileName=cms.string("Baselines.root"))

###for this module below should pass only the type : edm::DetSetVector<SiStripDigi>
process.hybridAna = cms.EDAnalyzer("SiStripHybridFormatAnalyzer",
    ###srcDigis =  cms.InputTag('siStripZeroSuppression','VirginRaw'), ###this was used to compare the realistic
    srcDigis =  cms.InputTag("siStripUnpackRepackedZS","ZeroSuppressed"), ###this was used to check each format
    srcAPVCM =  cms.InputTag('siStripZeroSuppression','APVCMVirginRaw'),
    ##nModuletoDisplay = cms.uint32(10000),
    nModuletoDisplay = cms.uint32(100),
    plotAPVCM   = cms.bool(True)
)

								  
# Path and EndPath definitions
process.raw2digi_step = cms.Path(siStripDigis)
###process.reconstruction_step = cms.Path(process.siStripZeroSuppression*process.siStripRepackZS*process.siStripUnpackRepackedZS*process.hybridAna)
process.reconstruction_step = cms.Path(siStripZeroSuppression*process.siStripRepackZS*process.myRawDataCollector*process.siStripUnpackRepackedZS*process.hybridAna)
process.RECOoutput_step = cms.EndPath(process.RECOoutput)


# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.reconstruction_step, process.RECOoutput_step)
