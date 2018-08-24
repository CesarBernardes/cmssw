import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('reRECO',eras.Run2_2018_pp_on_AA)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_Data_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')


process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('hiRecoDM nevts:2'),
    name = cms.untracked.string('PyReleaseValidation')
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(20)
)

process.options = cms.untracked.PSet(

)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
    '/store/hidata/HIRun2015/HITrackerVirginRaw/RAW/v1/000/263/400/00000/40322926-4AA3-E511-95F7-02163E0146A8.root'
   )
)


# Output definition
process.RECOoutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('RECO'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('file:emulation_hybridFormat_103X_RelVal.root'),
    splitLevel = cms.untracked.int32(0),
    outputCommands = cms.untracked.vstring('keep FEDRawDataCollection_myRawDataCollector_*_*')
)


process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = '75X_dataRun2_HLTHI_v4'


## Offline Silicon Tracker Zero Suppression
from RecoLocalTracker.SiStripZeroSuppression.DefaultAlgorithms_cff import *
process.siStripZeroSuppression.produceRawDigis = cms.bool(False)
process.siStripZeroSuppression.produceCalculatedBaseline = cms.bool(False)
process.siStripZeroSuppression.produceBaselinePoints = cms.bool(False)
process.siStripZeroSuppression.storeCM = cms.bool(True)
process.siStripZeroSuppression.produceHybridFormat = cms.bool(True)
process.siStripZeroSuppression.Algorithms.APVInspectMode = 'HybridEmulation'
process.siStripZeroSuppression.Algorithms.APVRestoreMode = ''
process.siStripZeroSuppression.Algorithms.CommonModeNoiseSubtractionMode = cms.string('Median')
process.siStripZeroSuppression.Algorithms.MeanCM = cms.int32(512)
process.siStripZeroSuppression.Algorithms.DeltaCMThreshold = cms.uint32(5)
process.siStripZeroSuppression.Algorithms.Use10bitsTruncation = cms.bool(True)     
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

process.myRawDataCollector = cms.EDProducer("RawDataCollectorByLabel",
    verbose = cms.untracked.int32(0),     # 0 = quiet, 1 = collection list, 2 = FED list
    RawCollectionList = cms.VInputTag( 
                                       cms.InputTag('siStripRepackZS'),
                                       cms.InputTag('rawDataCollector')
    ),
)

								  
# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.siStripDigis)
process.reconstruction_step = cms.Path(process.siStripZeroSuppression*process.siStripRepackZS*process.myRawDataCollector)
process.RECOoutput_step = cms.EndPath(process.RECOoutput)


# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.reconstruction_step, process.RECOoutput_step)
