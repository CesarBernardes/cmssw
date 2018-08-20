import FWCore.ParameterSet.Config as cms


from Configuration.StandardSequences.Eras import eras

process = cms.Process('TEST1', eras.Run2_HI)


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
siStripZeroSuppression.Algorithms.APVInspectMode = 'HybridEmulation'
siStripZeroSuppression.Algorithms.APVRestoreMode = ''
siStripZeroSuppression.HybridInputDigis = cms.InputTag('','')
siStripZeroSuppression.Algorithms.CommonModeNoiseSubtractionMode = cms.string('Median')
siStripZeroSuppression.Algorithms.MeanCM = cms.int32(512)
siStripZeroSuppression.Algorithms.DeltaCMThreshold = cms.uint32(20)
siStripZeroSuppression.Algorithms.Use10bitsTruncation = cms.bool(True)     
siStripZeroSuppression.RawDigiProducersList = cms.VInputTag(cms.InputTag("siStripDigis","VirginRaw"))


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
								  
# Path and EndPath definitions
process.raw2digi_step = cms.Path(siStripDigis)
process.reconstruction_step = cms.Path(siStripZeroSuppression*process.siStripRepackZS*process.myRawDataCollector)
process.RECOoutput_step = cms.EndPath(process.RECOoutput)


# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.reconstruction_step, process.RECOoutput_step)
