# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: myMasterRecoFile --conditions 103X_dataRun2_Express_v1 -s RAW2DIGI,L1Reco,RECO --data --era Run2_2018_pp_on_AA --eventcontent AOD --runUnscheduled --datatier RECO --repacked --no_exec --nThreads=8
import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('RECO',eras.Run2_2018_pp_on_AA)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.RawToDigi_DataMapper_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_Data_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')


process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
'file:/afs/cern.ch/work/w/wangj/public/HIMinimumBias2/HIRun2018A-v1/FC3873C2-BDA3-D747-908D-7C29DC301129.root' ###RAW file as input
),
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('myMasterRecoFile nevts:1'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition
process.TFileService = cms.Service("TFileService",
      fileName = cms.string("histo_FEDsize_tracks.root"),
      closeFileFast = cms.untracked.bool(True)
)


# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_hlt_hi', '')


# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.RawToDigi)

# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step)


#Setup FWK for multithreaded
process.options.numberOfThreads=cms.untracked.uint32(1)
process.options.numberOfStreams=cms.untracked.uint32(1)

from Configuration.Applications.ConfigBuilder import MassReplaceInputTag
MassReplaceInputTag(process, new="rawDataRepacker", old="rawDataCollector")
