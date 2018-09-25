# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: step2 --eventcontent RAW --datatier RAW --conditions auto:run2_hlt_hi --step RAW2DIGI,DIGI2RAW --scenario HeavyIons --data --era Run2_HI --no_exec
import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('DIGI2RAW',eras.Run2_HI)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContentHeavyIons_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.DigiToRaw_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(200)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
            '/store/hidata/HIRun2015/HITrackerVirginRaw/RAW/v1/000/263/400/00000/40322926-4AA3-E511-95F7-02163E0146A8.root'
    ),
    secondaryFileNames = cms.untracked.vstring()
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('step2 nevts:1'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.RAWoutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('RAW'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('step2_RAW2DIGI_DIGI2RAW_Hybrid.root'),
    outputCommands = process.RAWEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

# Additional output definition
## Offline Silicon Tracker Zero Suppression
process.load('RecoLocalTracker/SiStripZeroSuppression/SiStripZeroSuppression_cfi')
process.siStripZeroSuppression.produceHybridFormat = True
process.siStripZeroSuppression.Algorithms.APVInspectMode = 'HybridEmulation'
process.siStripZeroSuppression.Algorithms.APVRestoreMode = ''
process.siStripZeroSuppression.Algorithms.CommonModeNoiseSubtractionMode = 'Median'
process.siStripZeroSuppression.Algorithms.MeanCM = 0
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

process.hybridRawDataRepacker = cms.EDProducer("RawDataCollectorByLabel",
    verbose = cms.untracked.int32(0),     # 0 = quiet, 1 = collection list, 2 = FED list
    RawCollectionList = cms.VInputTag( 
                                       cms.InputTag('siStripRepackZS'),
                                       cms.InputTag('rawDataCollector')
    ),
)

process.RAWoutput.outputCommands.append("drop *_*_*_HLT*")
process.RAWoutput.outputCommands.append("drop *_*_*_LHC*")

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_hlt_hi', '')


# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.RawToDigi)
process.DigiToRaw2 = cms.Sequence(process.siStripZeroSuppression+process.siStripRepackZS+process.hybridRawDataRepacker)
process.digi2raw_step = cms.Path(process.DigiToRaw2)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.RAWoutput_step = cms.EndPath(process.RAWoutput)

# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.digi2raw_step,process.endjob_step,process.RAWoutput_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)


# Customisation from command line

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion
