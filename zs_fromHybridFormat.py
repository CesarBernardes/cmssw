import FWCore.ParameterSet.Config as cms


from Configuration.StandardSequences.Eras import eras

process = cms.Process('TEST2', eras.Run2_HI)


# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)


# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
    "file:emulation_hybridFormat.root" 
   )
)


# Output definition
process.RECOoutput = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
    fileName = cms.untracked.string('file:zs.root'),
    dataset = cms.untracked.PSet(
        filterName = cms.untracked.string(''),
        dataTier = cms.untracked.string('GEN-SIM-RECO')
    ),
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
siStripZeroSuppression.produceHybridFormat = cms.bool(False)
siStripZeroSuppression.Algorithms.APVInspectMode = 'BaselineFollower'
siStripZeroSuppression.Algorithms.APVRestoreMode = 'BaselineFollower'
siStripZeroSuppression.HybridInputDigis = cms.InputTag('','')
siStripZeroSuppression.Algorithms.CommonModeNoiseSubtractionMode = cms.string('Median')
siStripZeroSuppression.Algorithms.MeanCM = cms.int32(512)
siStripZeroSuppression.Algorithms.Use10bitsTruncation = cms.bool(False)
siStripZeroSuppression.RawDigiProducersList = cms.VInputTag(cms.InputTag('siStripDigis','ZeroSuppressed'))


###clusterizer
process.load('RecoLocalTracker.SiStripClusterizer.SiStripClusterizer_cfi')
from RecoLocalTracker.SiStripClusterizer.SiStripClusterizer_cfi import siStripClusters
siStripClusters.DigiProducersList = cms.VInputTag(cms.InputTag("siStripZeroSuppression","ZeroSuppressed"))

								  
# Path and EndPath definitions
process.raw2digi_step = cms.Path(siStripDigis)
process.siStripDigis.ProductLabel = cms.InputTag("myRawDataCollector")
process.reconstruction_step = cms.Path(siStripZeroSuppression*siStripClusters)
process.RECOoutput_step = cms.EndPath(process.RECOoutput)


# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.reconstruction_step, process.RECOoutput_step)
