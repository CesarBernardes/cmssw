# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: step4 --conditions auto:run2_data -s RAW2DIGI,L1Reco,RECO --process reRECO -n -1 --data --era Run2_HI --eventcontent RECO --runUnscheduled --scenario HeavyIons --datatier RECO --no_exec
import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('reRECO',eras.Run2_HI)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContentHeavyIons_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.ReconstructionHeavyIons_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(3)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
         'file:step2_RAW2DIGI_DIGI2RAW_Hybrid.root'
    ),
    secondaryFileNames = cms.untracked.vstring()
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('step4 nevts:-1'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

##process.RECOoutput = cms.OutputModule("PoolOutputModule",
##    dataset = cms.untracked.PSet(
##        dataTier = cms.untracked.string('RECO'),
##        filterName = cms.untracked.string('')
##    ),
##    fileName = cms.untracked.string('step4_RAW2DIGI_L1Reco_RECO.root'),
##    outputCommands = process.RECOEventContent.outputCommands,
##    splitLevel = cms.untracked.int32(0)
##)

# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data', '')

process.load("RecoLocalTracker.SiStripZeroSuppression.SiStripZeroSuppression_cfi")
process.load("RecoLocalTracker.SiStripClusterizer.SiStripClusterizer_cfi")
process.siStripZeroSuppression.Algorithms.APVInspectMode = "Hybrid"
process.siStripZeroSuppression.produceHybridFormat = False
process.siStripZeroSuppression.Algorithms.CommonModeNoiseSubtractionMode = 'Median'
process.siStripZeroSuppression.Algorithms.MeanCM = 0
process.siStripZeroSuppression.Algorithms.DeltaCMThreshold = 20
process.siStripZeroSuppression.Algorithms.Use10bitsTruncation = False
zsInputs = process.siStripZeroSuppression.RawDigiProducersList
clusInputs = process.siStripClusters.DigiProducersList
unpackedZS = cms.InputTag("siStripDigis", "ZeroSuppressed")
zsInputs.append(unpackedZS)
clusInputs.remove(unpackedZS)
clusInputs.append(cms.InputTag("siStripZeroSuppression","ZeroSuppressed"))
# for on-demand clusterizer
from FWCore.ParameterSet.MassReplace import massReplaceParameter
massReplaceParameter(process, "HybridZeroSuppressed", cms.bool(False), cms.bool(True))


process.TFileService = cms.Service("TFileService",
        fileName=cms.string("Clusters_Tracks_Hybrid.root")
)
process.plotTrackAna = cms.EDAnalyzer("TrackingAnalyzer",
        srcTracks = cms.InputTag("hiGeneralTracks"), 
)
process.clusterAna = cms.EDAnalyzer("SiStripClusterAnalyzer",
        srcClusters =  cms.InputTag('siStripClusters','')
)


# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.RawToDigi)
process.L1Reco_step = cms.Path(process.L1Reco)
process.reconstruction_step = cms.Path(process.reconstructionHeavyIons)
process.plot_step = cms.Path(process.clusterAna*process.plotTrackAna)
process.endjob_step = cms.EndPath(process.endOfProcess)
##process.RECOoutput_step = cms.EndPath(process.RECOoutput)

# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.L1Reco_step,process.reconstruction_step,process.plot_step,process.endjob_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

#do not add changes to your config after this point (unless you know what you are doing)
from FWCore.ParameterSet.Utilities import convertToUnscheduled
process=convertToUnscheduled(process)


# Customisation from command line
from Configuration.Applications.ConfigBuilder import MassReplaceInputTag; MassReplaceInputTag(process, new="hybridRawDataRepacker")

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion
