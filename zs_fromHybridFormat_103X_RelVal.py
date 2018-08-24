import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('ZeroSuppression',eras.Run2_2018_pp_on_AA)

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
    "file:emulation_hybridFormat_103X_DeltaCMth20_RelVal.root" 
    )
)


# Output definition
process.RECOoutput = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
    fileName = cms.untracked.string('file:zs_clusters.root'),
    dataset = cms.untracked.PSet(
        filterName = cms.untracked.string(''),
        dataTier = cms.untracked.string('RECO')
    ),
)


process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = '75X_dataRun2_HLTHI_v4'


## Offline Silicon Tracker Zero Suppression
from RecoLocalTracker.SiStripZeroSuppression.DefaultAlgorithms_cff import *
process.siStripZeroSuppression.produceRawDigis = cms.bool(False)
process.siStripZeroSuppression.produceCalculatedBaseline = cms.bool(False)
process.siStripZeroSuppression.produceBaselinePoints = cms.bool(False)
process.siStripZeroSuppression.storeCM = cms.bool(True)
process.siStripZeroSuppression.produceHybridFormat = cms.bool(False)
process.siStripZeroSuppression.Algorithms.APVInspectMode = 'BaselineFollower'
process.siStripZeroSuppression.Algorithms.APVRestoreMode = 'BaselineFollower'
process.siStripZeroSuppression.Algorithms.CommonModeNoiseSubtractionMode = cms.string('Median')
process.siStripZeroSuppression.Algorithms.MeanCM = cms.int32(512)
process.siStripZeroSuppression.Algorithms.Use10bitsTruncation = cms.bool(False)
process.siStripZeroSuppression.RawDigiProducersList = cms.VInputTag(cms.InputTag('siStripDigis','ZeroSuppressed'))

process.siStripClusters.DigiProducersList = cms.VInputTag(cms.InputTag("siStripZeroSuppression","ZeroSuppressed"))

								  
# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.siStripDigis)
process.siStripDigis.ProductLabel = cms.InputTag("myRawDataCollector")
process.reconstruction_step = cms.Path(process.siStripZeroSuppression+process.siStripClusters)
process.RECOoutput_step = cms.EndPath(process.RECOoutput)


# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.reconstruction_step, process.RECOoutput_step)
